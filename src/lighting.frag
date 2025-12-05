#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

struct Material {
    sampler2D albedoMap;
    bool hasAlbedo;
    sampler2D normalMap;
    bool hasNormal;
    sampler2D metallicMap;
    bool hasMetallic;
    sampler2D roughnessMap;
    bool hasRoughness;
    sampler2D aoMap;
    bool hasAO;
};

uniform Material material;
uniform sampler2DShadow shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float time;

const float PI = 3.14159265359;

// Sunrise/sunset colors
const vec3 sunColor = vec3(1.0, 0.7, 0.4);        // Warm orange-yellow sun
const vec3 skyColorZenith = vec3(0.4, 0.6, 0.9);  // Blue sky above
const vec3 skyColorHorizon = vec3(1.0, 0.6, 0.3); // Orange at horizon
const vec3 ambientGround = vec3(0.15, 0.1, 0.08); // Warm ground bounce

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
        return 0.0;

    float bias = max(0.003 * (1.0 - dot(normal, lightDir)), 0.0005);

    float shadowVisibility = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for(int x = -2; x <= 2; ++x)
    {
        for(int y = -2; y <= 2; ++y)
        {
            shadowVisibility += texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, projCoords.z - bias));
        }
    }
    shadowVisibility /= 25.0;

    return 1.0 - shadowVisibility;
}

void main()
{
    vec3 albedo = objectColor;
    if (material.hasAlbedo) {
        albedo = pow(texture(material.albedoMap, TexCoords).rgb, vec3(2.2));
    }

    float roughness = 0.8;
    if (material.hasRoughness) {
        roughness = texture(material.roughnessMap, TexCoords).r;
    }

    float metallic = 0.0;
    if (material.hasMetallic) {
        metallic = texture(material.metallicMap, TexCoords).r;
    }

    float ao = 1.0;
    if (material.hasAO) {
        ao = texture(material.aoMap, TexCoords).r;
    }

    vec3 N = normalize(Normal);
    if (material.hasNormal) {
        vec3 normalMapValue = texture(material.normalMap, TexCoords).rgb;
        N = normalize(normalMapValue * 2.0 - 1.0);
    }

    vec3 V = normalize(viewPos - FragPos);
    vec3 L = normalize(lightPos - FragPos);
    vec3 H = normalize(V + L);

    // Calculate sun height for atmospheric effects (0 = horizon, 1 = zenith)
    vec3 sunDir = normalize(lightPos);
    float sunHeight = clamp(sunDir.y, 0.0, 1.0);
    
    // Sunrise lighting - warmer when sun is low
    vec3 sunlightColor = mix(sunColor, vec3(1.0, 0.95, 0.9), sunHeight);
    
    // Atmospheric scattering - more intense at sunrise
    float scatterFactor = 1.0 - sunHeight * 0.5;
    
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0;
    vec3 radiance = sunlightColor * lightColor * attenuation * (1.5 + sunHeight);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

    float shadow = ShadowCalculation(FragPosLightSpace, N, L);

    // Realistic ambient lighting for sunrise
    // Sky light from above (blue-ish)
    float skyInfluence = max(dot(N, vec3(0.0, 1.0, 0.0)), 0.0);
    vec3 skyAmbient = mix(skyColorHorizon, skyColorZenith, skyInfluence) * 0.15;
    
    // Ground bounce (warm, from below)
    float groundInfluence = max(dot(N, vec3(0.0, -1.0, 0.0)), 0.0);
    vec3 groundAmbient = ambientGround * groundInfluence * 0.3;
    
    // Horizon glow effect
    vec3 toLight = normalize(lightPos - FragPos);
    float horizonGlow = pow(max(1.0 - abs(toLight.y), 0.0), 2.0) * 0.1;
    vec3 glowColor = sunColor * horizonGlow;
    
    // Combined ambient
    vec3 ambient = (skyAmbient + groundAmbient + glowColor) * albedo * ao;
    
    // Softer shadows at sunrise (atmospheric scattering fills shadows)
    float shadowSoftness = mix(0.4, 0.0, sunHeight); // More fill light when sun is low
    float effectiveShadow = shadow * (1.0 - shadowSoftness);
    
    vec3 color = ambient + (1.0 - effectiveShadow) * Lo;
    
    // Subtle warm color grading for sunrise atmosphere
    color = mix(color, color * sunColor, 0.1 * scatterFactor);

    // Tone mapping
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}