#pragma once
// Use OpenAL-Soft instead of macOS OpenAL
#include <AL/al.h>
#include <AL/alc.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

// Forward declarations
class AudioSource;
class AudioSource3D;

class AudioSystem {
public:
    static AudioSystem& getInstance();
    
    bool init();
    void shutdown();
    
    // Load audio file (WAV, OGG, MP3)
    ALuint loadSound(const std::string& filename);
    
    // Generate a test tone (for debugging)
    ALuint generateTestTone(float frequency = 440.0f, float duration = 1.0f);
    
    // Update listener position for 3D audio
    void setListenerPosition(const glm::vec3& position);
    void setListenerOrientation(const glm::vec3& forward, const glm::vec3& up);
    void setListenerVelocity(const glm::vec3& velocity);
    
    // Create audio sources
    std::shared_ptr<AudioSource> createSource();
    std::shared_ptr<AudioSource3D> createSource3D();
    
    // Update all sources
    void update();
    
    // Master volume
    void setMasterVolume(float volume);
    float getMasterVolume() const { return masterVolume; }
    
private:
    AudioSystem() = default;
    ~AudioSystem();
    AudioSystem(const AudioSystem&) = delete;
    AudioSystem& operator=(const AudioSystem&) = delete;
    
    ALCdevice* device = nullptr;
    ALCcontext* context = nullptr;
    
    std::unordered_map<std::string, ALuint> loadedBuffers;
    std::vector<std::weak_ptr<AudioSource>> sources;
    std::vector<std::weak_ptr<AudioSource3D>> sources3D;
    
    float masterVolume = 1.0f;
    bool initialized = false;
    
    // File loading helpers
    bool loadWAV(const std::string& filename, ALuint buffer);
    bool loadOGG(const std::string& filename, ALuint buffer);
    bool loadMP3(const std::string& filename, ALuint buffer);
    
    // Get file extension
    std::string getFileExtension(const std::string& filename);
};

// 2D Audio Source (no spatial positioning)
class AudioSource {
public:
    AudioSource();
    ~AudioSource();
    
    void setBuffer(ALuint buffer);
    void play();
    void stop();
    void pause();
    
    void setVolume(float volume);
    void setPitch(float pitch);
    void setLooping(bool loop);
    
    bool isPlaying() const;
    float getVolume() const { return volume; }
    float getPitch() const { return pitch; }
    bool isLooping() const { return looping; }
    
protected:
    ALuint source = 0;
    float volume = 1.0f;
    float pitch = 1.0f;
    bool looping = false;
};

// 3D Audio Source (with spatial positioning)
class AudioSource3D : public AudioSource {
public:
    AudioSource3D();
    
    void setPosition(const glm::vec3& pos);
    void setVelocity(const glm::vec3& vel);
    void setDirection(const glm::vec3& dir);
    
    // Distance attenuation
    void setReferenceDistance(float distance);
    void setMaxDistance(float distance);
    void setRolloffFactor(float factor);
    
    // Cone settings for directional sound
    void setConeAngles(float innerAngle, float outerAngle);
    void setConeOuterGain(float gain);
    
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getVelocity() const { return velocity; }
    
private:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
    
    float referenceDistance = 1.0f;
    float maxDistance = 100.0f;
    float rolloffFactor = 1.0f;
};
