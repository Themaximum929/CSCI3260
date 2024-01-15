#version 330 core

struct FS_IN {
    vec2 TexCoords;
    vec3 TangentFragPos;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
};

in FS_IN fs_in;

out vec4 FragColor;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

// Add the point light structure
struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};
uniform PointLight pointLight;

uniform vec3 diffuseLightColorNormal;
uniform vec3 lightPositionWorldNormal;
uniform float directionalLightBrightness;
uniform float ambientLight;

void main()
{   
    // Get normal from normal map
    vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;

    // Ambient lighting
    vec3 ambient = 0.1 * color;

    // Diffuse lighting
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

    // Specular lighting
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos); 
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(normal, reflectDir), 0.0), 32.0);
    vec3 specular = 0.3 * spec * vec3(1.0, 1.0, 1.0); // specular color is white (1.0, 1.0, 1.0)

    // Add the point light calculations
    vec3 pointLightDir = normalize(pointLight.position - fs_in.TangentFragPos);
    float distance = length(pointLight.position - fs_in.TangentFragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));
    float pointDiff = max(dot(normal, pointLightDir), 0.0);
    vec3 pointDiffuse = attenuation * pointDiff * pointLight.color;
    vec3 pointReflectDir = reflect(-pointLightDir, normal);
    float pointSpec = pow(max(dot(viewDir, pointReflectDir), 0.0), 32.0);
    vec3 pointSpecular = attenuation * pointSpec * pointLight.color;

    // Combine all the lighting contributions
    vec3 finalColor = ambient + diffuse + specular + pointDiffuse + pointSpecular;
    FragColor = vec4(finalColor, 1.0);
}