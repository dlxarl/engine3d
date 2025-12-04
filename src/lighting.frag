#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

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

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    vec3 albedo;
    if (material.hasAlbedo) {
        albedo = texture(material.albedoMap, TexCoords).rgb;
    } else {
        albedo = objectColor;
    }

    vec3 norm = normalize(Normal);
    if (material.hasNormal) {
        vec3 normalMapValue = texture(material.normalMap, TexCoords).rgb;
        normalMapValue = normalize(normalMapValue * 2.0 - 1.0);
    }

    float roughness = 0.5;
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

    float ambientStrength = 0.1 * ao;
    vec3 ambient = ambientStrength * lightColor * albedo;

    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * albedo;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float shininess = (1.0 - roughness) * 128.0;
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);

    vec3 specularColor = mix(vec3(1.0), albedo, metallic);
    vec3 specular = spec * specularColor * lightColor;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}