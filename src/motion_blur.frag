#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;
uniform mat4 inverseViewProjection;
uniform mat4 previousViewProjection;
uniform bool useMotionBlur;
uniform float fps;

void main()
{
    vec3 color = texture(screenTexture, TexCoords).rgb;

    if (!useMotionBlur) {
        FragColor = vec4(color, 1.0);
        return;
    }

    float z = texture(depthTexture, TexCoords).r * 2.0 - 1.0;
    vec4 clipSpacePosition = vec4(TexCoords * 2.0 - 1.0, z, 1.0);
    vec4 worldPosition = inverseViewProjection * clipSpacePosition;
    worldPosition /= worldPosition.w;

    vec4 previousClipSpacePosition = previousViewProjection * worldPosition;
    previousClipSpacePosition /= previousClipSpacePosition.w;

    vec2 velocity = (clipSpacePosition.xy - previousClipSpacePosition.xy) / 2.0;

    float currentFPS = fps > 0.0 ? fps : 60.0;
    float targetFPS = 60.0;

    float intensity = 0.05 * (targetFPS / currentFPS);
    velocity *= intensity;

    if (length(velocity) < 0.0005) {
        velocity = vec2(0.0);
    }

    float maxVelocity = 0.005;
    if (length(velocity) > maxVelocity) {
        velocity = normalize(velocity) * maxVelocity;
    }

    int numSamples = 8;
    vec2 texCoord = TexCoords;

    for(int i = 1; i < numSamples; ++i)
    {
        texCoord += velocity;

        if (texCoord.x >= 0.0 && texCoord.x <= 1.0 && texCoord.y >= 0.0 && texCoord.y <= 1.0) {
            color += texture(screenTexture, texCoord).rgb;
        } else {
            color += texture(screenTexture, TexCoords).rgb;
        }
    }

    FragColor = vec4(color / float(numSamples), 1.0);
}