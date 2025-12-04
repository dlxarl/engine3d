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

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
    return 0.0;

    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);

    float shadowVisibility = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            shadowVisibility += texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, projCoords.z - bias));
        }
    }
    shadowVisibility /= 9.0;

    return 1.0 - shadowVisibility;
}

void main()
{
    vec3 albedo = objectColor;
    if (material.hasAlbedo) albedo = texture(material.albedoMap, TexCoords).rgb;

    vec3 norm = normalize(Normal);
    if (material.hasNormal) {
        vec3 normalMapValue = texture(material.normalMap, TexCoords).rgb;
        norm = normalize(normalMapValue * 2.0 - 1.0);
    }

    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * lightColor * albedo;

    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * albedo;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = 0.5 * spec * lightColor;

    float shadow = ShadowCalculation(FragPosLightSpace, norm, lightDir);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));

    FragColor = vec4(lighting, 1.0);
}