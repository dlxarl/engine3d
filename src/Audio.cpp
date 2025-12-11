#include "Audio.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <cmath>

// Include stb_vorbis for OGG support
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.h"

// Include dr_mp3 for MP3 support
#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

// ==================== AudioSystem ====================

AudioSystem& AudioSystem::getInstance() {
    static AudioSystem instance;
    return instance;
}

AudioSystem::~AudioSystem() {
    shutdown();
}

bool AudioSystem::init() {
    if (initialized) return true;
    
    // Open default device
    device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "ERROR::AUDIO: Failed to open audio device" << std::endl;
        return false;
    }
    
    // Print device name
    const ALCchar* deviceName = alcGetString(device, ALC_DEVICE_SPECIFIER);
    std::cout << "Audio device: " << (deviceName ? deviceName : "unknown") << std::endl;
    
    // Create context
    context = alcCreateContext(device, nullptr);
    if (!context) {
        std::cerr << "ERROR::AUDIO: Failed to create audio context" << std::endl;
        alcCloseDevice(device);
        device = nullptr;
        return false;
    }
    
    if (!alcMakeContextCurrent(context)) {
        std::cerr << "ERROR::AUDIO: Failed to make audio context current" << std::endl;
        alcDestroyContext(context);
        alcCloseDevice(device);
        context = nullptr;
        device = nullptr;
        return false;
    }
    
    // Set default listener properties
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    ALfloat orientation[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
    alListenerfv(AL_ORIENTATION, orientation);
    
    // Set distance model for 3D audio
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
    
    // Set master volume
    alListenerf(AL_GAIN, 1.0f);
    
    // Clear any errors
    alGetError();
    
    initialized = true;
    std::cout << "Audio system initialized successfully" << std::endl;
    return true;
}

void AudioSystem::shutdown() {
    if (!initialized) return;
    
    // Clear sources
    sources.clear();
    sources3D.clear();
    
    // Delete all buffers
    for (auto& [name, buffer] : loadedBuffers) {
        alDeleteBuffers(1, &buffer);
    }
    loadedBuffers.clear();
    
    // Destroy context and close device
    alcMakeContextCurrent(nullptr);
    if (context) {
        alcDestroyContext(context);
        context = nullptr;
    }
    if (device) {
        alcCloseDevice(device);
        device = nullptr;
    }
    
    initialized = false;
}

// WAV file header structure
struct WAVHeader {
    char riff[4];           // "RIFF"
    uint32_t fileSize;      // File size - 8
    char wave[4];           // "WAVE"
    char fmt[4];            // "fmt "
    uint32_t fmtSize;       // Format chunk size
    uint16_t audioFormat;   // Audio format (1 = PCM)
    uint16_t numChannels;   // Number of channels
    uint32_t sampleRate;    // Sample rate
    uint32_t byteRate;      // Byte rate
    uint16_t blockAlign;    // Block align
    uint16_t bitsPerSample; // Bits per sample
};

bool AudioSystem::loadWAV(const std::string& filename, ALuint buffer) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "ERROR::AUDIO: Failed to open WAV file: " << filename << std::endl;
        return false;
    }
    
    WAVHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
    
    // Verify RIFF header
    if (strncmp(header.riff, "RIFF", 4) != 0 || strncmp(header.wave, "WAVE", 4) != 0) {
        std::cerr << "ERROR::AUDIO: Invalid WAV file format: " << filename << std::endl;
        return false;
    }
    
    // Skip to data chunk
    char chunkId[4];
    uint32_t chunkSize;
    
    // Skip any extra fmt data
    if (header.fmtSize > 16) {
        file.seekg(header.fmtSize - 16, std::ios::cur);
    }
    
    // Find data chunk
    while (file.read(chunkId, 4)) {
        file.read(reinterpret_cast<char*>(&chunkSize), 4);
        if (strncmp(chunkId, "data", 4) == 0) {
            break;
        }
        file.seekg(chunkSize, std::ios::cur);
    }
    
    if (strncmp(chunkId, "data", 4) != 0) {
        std::cerr << "ERROR::AUDIO: No data chunk found in WAV file: " << filename << std::endl;
        return false;
    }
    
    // Read audio data
    std::vector<char> data(chunkSize);
    file.read(data.data(), chunkSize);
    
    // Determine format
    ALenum format;
    if (header.numChannels == 1) {
        format = (header.bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    } else {
        format = (header.bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
    }
    
    // Upload to OpenAL buffer
    alBufferData(buffer, format, data.data(), chunkSize, header.sampleRate);
    
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "ERROR::AUDIO: OpenAL error loading buffer: " << error << std::endl;
        return false;
    }
    
    return true;
}

ALuint AudioSystem::generateTestTone(float frequency, float duration) {
    const int sampleRate = 44100;
    const int numSamples = static_cast<int>(sampleRate * duration);
    
    std::vector<short> samples(numSamples);
    for (int i = 0; i < numSamples; i++) {
        float t = static_cast<float>(i) / sampleRate;
        float value = std::sin(2.0f * 3.14159f * frequency * t);
        samples[i] = static_cast<short>(value * 32767.0f * 0.5f); // 50% volume
    }
    
    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, AL_FORMAT_MONO16, samples.data(), numSamples * sizeof(short), sampleRate);
    
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "ERROR::AUDIO: Failed to create test tone buffer: " << error << std::endl;
        alDeleteBuffers(1, &buffer);
        return 0;
    }
    
    std::cout << "Generated test tone: " << frequency << " Hz, " << duration << " sec" << std::endl;
    return buffer;
}

ALuint AudioSystem::loadSound(const std::string& filename) {
    // Check if already loaded
    auto it = loadedBuffers.find(filename);
    if (it != loadedBuffers.end()) {
        return it->second;
    }
    
    // Generate new buffer
    ALuint buffer;
    alGenBuffers(1, &buffer);
    
    // Determine file type and load accordingly
    std::string ext = getFileExtension(filename);
    bool success = false;
    
    if (ext == "wav") {
        success = loadWAV(filename, buffer);
    } else if (ext == "ogg") {
        success = loadOGG(filename, buffer);
    } else if (ext == "mp3") {
        success = loadMP3(filename, buffer);
    } else {
        std::cerr << "ERROR::AUDIO: Unsupported audio format: " << ext << std::endl;
    }
    
    if (!success) {
        alDeleteBuffers(1, &buffer);
        return 0;
    }
    
    loadedBuffers[filename] = buffer;
    return buffer;
}

std::string AudioSystem::getFileExtension(const std::string& filename) {
    size_t pos = filename.rfind('.');
    if (pos == std::string::npos) return "";
    std::string ext = filename.substr(pos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

bool AudioSystem::loadOGG(const std::string& filename, ALuint buffer) {
    int channels, sampleRate;
    short* data;
    int samples = stb_vorbis_decode_filename(filename.c_str(), &channels, &sampleRate, &data);
    
    if (samples <= 0) {
        std::cerr << "ERROR::AUDIO: Failed to load OGG file: " << filename << std::endl;
        return false;
    }
    
    short* finalData = data;
    ALenum format;
    ALsizei dataSize;
    
    // Convert stereo to mono for 3D audio positioning
    if (channels == 2) {
        short* monoData = (short*)malloc(samples * sizeof(short));
        for (int i = 0; i < samples; i++) {
            int32_t left = data[i * 2];
            int32_t right = data[i * 2 + 1];
            monoData[i] = (short)((left + right) / 2);
        }
        finalData = monoData;
        format = AL_FORMAT_MONO16;
        dataSize = samples * sizeof(short);
        std::cout << "Converted stereo to mono for 3D audio" << std::endl;
    } else {
        format = AL_FORMAT_MONO16;
        dataSize = samples * sizeof(short);
    }
    
    alBufferData(buffer, format, finalData, dataSize, sampleRate);
    
    if (finalData != data) {
        free(finalData);
    }
    free(data);
    
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "ERROR::AUDIO: OpenAL error loading OGG buffer: " << error << std::endl;
        return false;
    }
    
    std::cout << "Loaded OGG: " << filename << " (mono, " << sampleRate << " Hz)" << std::endl;
    return true;
}

bool AudioSystem::loadMP3(const std::string& filename, ALuint buffer) {
    drmp3_config config;
    drmp3_uint64 totalFrameCount;
    
    drmp3_int16* data = drmp3_open_file_and_read_pcm_frames_s16(filename.c_str(), &config, &totalFrameCount, NULL);
    
    if (!data) {
        std::cerr << "ERROR::AUDIO: Failed to load MP3 file: " << filename << std::endl;
        return false;
    }
    
    std::cout << "MP3 raw data: " << totalFrameCount << " frames, " << config.channels << " channels, " << config.sampleRate << " Hz" << std::endl;
    
    drmp3_int16* finalData = data;
    drmp3_uint64 finalFrameCount = totalFrameCount;
    ALenum format;
    
    // Convert stereo to mono for 3D audio positioning
    if (config.channels == 2) {
        drmp3_int16* monoData = (drmp3_int16*)malloc(totalFrameCount * sizeof(drmp3_int16));
        for (drmp3_uint64 i = 0; i < totalFrameCount; i++) {
            // Average left and right channels
            int32_t left = data[i * 2];
            int32_t right = data[i * 2 + 1];
            monoData[i] = (drmp3_int16)((left + right) / 2);
        }
        finalData = monoData;
        format = AL_FORMAT_MONO16;
        std::cout << "Converted stereo to mono for 3D audio" << std::endl;
    } else {
        format = AL_FORMAT_MONO16;
    }
    
    ALsizei dataSize = (ALsizei)(finalFrameCount * sizeof(drmp3_int16));
    std::cout << "Uploading " << dataSize << " bytes to OpenAL buffer" << std::endl;
    
    alGetError(); // Clear errors
    alBufferData(buffer, format, finalData, dataSize, config.sampleRate);
    
    ALenum bufferError = alGetError();
    if (bufferError != AL_NO_ERROR) {
        std::cerr << "ERROR::AUDIO: alBufferData failed with error: " << bufferError << std::endl;
    }
    
    // Free memory
    if (finalData != data) {
        free(finalData);
    }
    drmp3_free(data, NULL);
    
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "ERROR::AUDIO: OpenAL error loading MP3 buffer: " << error << std::endl;
        return false;
    }
    
    std::cout << "Loaded MP3: " << filename << " (mono, " << config.sampleRate << " Hz)" << std::endl;
    return true;
}

void AudioSystem::setListenerPosition(const glm::vec3& position) {
    alListener3f(AL_POSITION, position.x, position.y, position.z);
}

void AudioSystem::setListenerOrientation(const glm::vec3& forward, const glm::vec3& up) {
    ALfloat orientation[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
    alListenerfv(AL_ORIENTATION, orientation);
}

void AudioSystem::setListenerVelocity(const glm::vec3& velocity) {
    alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}

std::shared_ptr<AudioSource> AudioSystem::createSource() {
    auto source = std::make_shared<AudioSource>();
    sources.push_back(source);
    return source;
}

std::shared_ptr<AudioSource3D> AudioSystem::createSource3D() {
    auto source = std::make_shared<AudioSource3D>();
    sources3D.push_back(source);
    return source;
}

void AudioSystem::update() {
    // Clean up expired sources
    sources.erase(
        std::remove_if(sources.begin(), sources.end(),
            [](const std::weak_ptr<AudioSource>& wp) { return wp.expired(); }),
        sources.end());
    
    sources3D.erase(
        std::remove_if(sources3D.begin(), sources3D.end(),
            [](const std::weak_ptr<AudioSource3D>& wp) { return wp.expired(); }),
        sources3D.end());
}

void AudioSystem::setMasterVolume(float volume) {
    masterVolume = volume;
    alListenerf(AL_GAIN, volume);
}

// ==================== AudioSource ====================

AudioSource::AudioSource() {
    alGenSources(1, &source);
    // Disable 3D positioning for 2D audio
    alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
}

AudioSource::~AudioSource() {
    if (source) {
        alDeleteSources(1, &source);
    }
}

void AudioSource::setBuffer(ALuint buffer) {
    alSourcei(source, AL_BUFFER, buffer);
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "ERROR::AUDIO: Failed to set buffer on source: " << error << std::endl;
    }
}

void AudioSource::play() {
    alSourcePlay(source);
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "ERROR::AUDIO: Failed to play source: " << error << std::endl;
    }
}

void AudioSource::stop() {
    alSourceStop(source);
}

void AudioSource::pause() {
    alSourcePause(source);
}

void AudioSource::setVolume(float vol) {
    volume = vol;
    alSourcef(source, AL_GAIN, vol);
}

void AudioSource::setPitch(float p) {
    pitch = p;
    alSourcef(source, AL_PITCH, p);
}

void AudioSource::setLooping(bool loop) {
    looping = loop;
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

bool AudioSource::isPlaying() const {
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

// ==================== AudioSource3D ====================

AudioSource3D::AudioSource3D() : AudioSource() {
    // Enable 3D positioning
    alSourcei(source, AL_SOURCE_RELATIVE, AL_FALSE);
    
    // Set default attenuation model
    alSourcef(source, AL_REFERENCE_DISTANCE, referenceDistance);
    alSourcef(source, AL_MAX_DISTANCE, maxDistance);
    alSourcef(source, AL_ROLLOFF_FACTOR, rolloffFactor);
}

void AudioSource3D::setPosition(const glm::vec3& pos) {
    position = pos;
    alSource3f(source, AL_POSITION, pos.x, pos.y, pos.z);
}

void AudioSource3D::setVelocity(const glm::vec3& vel) {
    velocity = vel;
    alSource3f(source, AL_VELOCITY, vel.x, vel.y, vel.z);
}

void AudioSource3D::setDirection(const glm::vec3& dir) {
    direction = dir;
    alSource3f(source, AL_DIRECTION, dir.x, dir.y, dir.z);
}

void AudioSource3D::setReferenceDistance(float distance) {
    referenceDistance = distance;
    alSourcef(source, AL_REFERENCE_DISTANCE, distance);
}

void AudioSource3D::setMaxDistance(float distance) {
    maxDistance = distance;
    alSourcef(source, AL_MAX_DISTANCE, distance);
}

void AudioSource3D::setRolloffFactor(float factor) {
    rolloffFactor = factor;
    alSourcef(source, AL_ROLLOFF_FACTOR, factor);
}

void AudioSource3D::setConeAngles(float innerAngle, float outerAngle) {
    alSourcef(source, AL_CONE_INNER_ANGLE, innerAngle);
    alSourcef(source, AL_CONE_OUTER_ANGLE, outerAngle);
}

void AudioSource3D::setConeOuterGain(float gain) {
    alSourcef(source, AL_CONE_OUTER_GAIN, gain);
}
