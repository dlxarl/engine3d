#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 ClipSpace;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 waterColor;
uniform float time;
uniform float transparency;
uniform float waterDepth;

// Ripple sources - positions and times of object interactions
#define MAX_RIPPLES 8
uniform vec3 ripplePositions[MAX_RIPPLES];
uniform float rippleTimes[MAX_RIPPLES];
uniform int activeRipples;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Calculate ripple displacement from object interactions
float calculateRipples(vec3 pos)
{
    float totalRipple = 0.0;
    
    for (int i = 0; i < MAX_RIPPLES; i++) {
        if (i >= activeRipples) break;
        
        float age = time - rippleTimes[i];
        if (age < 0.0 || age > 5.0) continue;
        
        vec2 delta = pos.xz - ripplePositions[i].xz;
        float dist = length(delta);
        
        // Expanding ring wave
        float waveSpeed = 3.0;
        float waveRadius = age * waveSpeed;
        float ringDist = abs(dist - waveRadius);
        
        // Decay based on distance and age
        float decay = exp(-age * 0.8) * exp(-ringDist * 2.0);
        float frequency = 8.0;
        float ripple = sin(dist * frequency - age * 10.0) * decay;
        
        totalRipple += ripple * 0.1;
    }
    
    return totalRipple;
}

// Perlin-like noise for surface detail
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

float fbm(vec2 p) {
    float f = 0.0;
    f += 0.5000 * noise(p); p *= 2.02;
    f += 0.2500 * noise(p); p *= 2.03;
    f += 0.1250 * noise(p); p *= 2.01;
    f += 0.0625 * noise(p);
    return f / 0.9375;
}

void main()
{
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);
    vec3 L = normalize(lightPos - FragPos);
    vec3 H = normalize(V + L);
    
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    
    // Water refraction index for fresnel
    vec3 F0 = vec3(0.02);
    vec3 fresnel = fresnelSchlick(NdotV, F0);
    
    // Dark, murky reflection
    vec3 R = reflect(-V, N);
    vec3 nightSkyColor = vec3(0.05, 0.08, 0.12);
    vec3 horizonColor = vec3(0.1, 0.12, 0.15);
    vec3 reflection = mix(horizonColor, nightSkyColor, pow(max(R.y, 0.0), 0.3));
    
    // Dark, realistic water colors - deep ocean/lake style
    vec3 deepColor = vec3(0.01, 0.03, 0.05);  // Nearly black deep water
    vec3 midColor = vec3(0.02, 0.06, 0.1);    // Dark blue-green
    vec3 shallowColor = vec3(0.03, 0.08, 0.12); // Slightly lighter at edges
    
    // Depth-based color with absorption
    float viewDepth = pow(NdotV, 0.3);
    vec3 waterTint = mix(deepColor, mix(midColor, shallowColor, viewDepth * 0.5), viewDepth);
    
    // Add subtle color variation from noise
    float noiseVal = fbm(FragPos.xz * 0.5 + time * 0.1);
    waterTint *= (0.85 + noiseVal * 0.3);
    
    // Subtle specular highlights - moonlight/starlight effect
    float specPower = 512.0;
    float spec = pow(NdotH, specPower);
    vec3 specular = spec * lightColor * 0.4;
    
    // Secondary broader highlight
    float spec2 = pow(NdotH, 64.0);
    specular += spec2 * lightColor * 0.1;
    
    // Subtle underwater caustics (darker, less prominent)
    float caustics = sin(FragPos.x * 8.0 + time * 1.5) * sin(FragPos.z * 8.0 + time * 1.2);
    caustics += sin(FragPos.x * 12.0 - time * 2.0) * sin(FragPos.z * 10.0 + time * 1.8) * 0.5;
    caustics = caustics * 0.25 + 0.5;
    caustics = pow(caustics, 3.0) * 0.05; // Much subtler caustics
    
    // Surface foam and turbulence
    float foam = fbm(FragPos.xz * 3.0 + time * 0.3);
    foam = smoothstep(0.6, 0.8, foam) * 0.1;
    
    // Very dim diffuse lighting
    vec3 diffuse = NdotL * lightColor * waterTint * 0.2;
    
    // Subsurface scattering approximation
    vec3 subsurface = vec3(0.0, 0.02, 0.04) * pow(max(dot(-L, V), 0.0), 4.0) * 0.3;
    
    // Fresnel-based mixing
    float fresnelFactor = fresnel.r;
    // Darker reflection for murky water
    vec3 darkReflection = reflection * 0.3;
    vec3 color = mix(waterTint + diffuse + subsurface, darkReflection, fresnelFactor * 0.6);
    
    color += specular;
    color += caustics * vec3(0.1, 0.15, 0.2);
    color += foam * vec3(0.2, 0.25, 0.3);
    
    // Very dark ambient
    vec3 ambient = vec3(0.02, 0.03, 0.04) * waterTint;
    color += ambient;
    
    // Film grain effect for realism
    float grain = hash(FragPos.xz * 100.0 + time) * 0.02;
    color += grain;
    
    // Tone mapping
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    
    // Higher base opacity for darker water
    float alpha = mix(transparency + 0.2, 1.0, fresnelFactor * 0.3);
    
    FragColor = vec4(color, alpha);
}
