#version 430
out vec3 outColor;

in vec2 uv;
in vec3 normalObj;
in vec3 vertexPositionWorld;

uniform sampler2D ourTexture;
uniform vec3 diffuseLightColorNormal;
uniform vec3 lightPositionWorldNormal;

uniform float directionalLightBrightness;
uniform float ambientLight;

// Add the point light structure
struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};
uniform PointLight pointLight;

void main()
{
    vec3 Texture = texture(ourTexture, uv).rgb;

    vec3 lightVectorWorldNormal = normalize(lightPositionWorldNormal - vertexPositionWorld);
    float brightnessNormal =max( dot(lightVectorWorldNormal, normalize(normalObj)),0) * directionalLightBrightness;
    vec3 diffuseLightNormal = vec3(brightnessNormal);

    vec3 eyeVector;

    vec3 reflectedLightVectorWorld1 = reflect(-lightVectorWorldNormal, normalObj);
    eyeVector = normalize(vertexPositionWorld);
    float s1 = pow(clamp(max(dot(reflectedLightVectorWorld1, eyeVector),0), 0, 1), 32);
    vec3 specularLight1 = vec3(s1);

    // Add the point light calculations
    vec3 lightDir = normalize(pointLight.position - vertexPositionWorld);
    float distance = length(pointLight.position - vertexPositionWorld);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

    float diff = max(dot(normalObj, lightDir), 0.0);
    vec3 diffuse = attenuation * diff * pointLight.color;

    vec3 reflectDir = reflect(-lightDir, normalObj);
    float spec = pow(max(dot(eyeVector, reflectDir), 0.0), 32);
    vec3 specular = attenuation * spec * pointLight.color;

    //Directional light
    outColor = vec3(ambientLight); 
    outColor = outColor + clamp(diffuseLightNormal, 0, 1) * diffuseLightColorNormal;
    outColor = outColor + specularLight1 * diffuseLightColorNormal;

    // Add the point light diffuse and specular contributions
    outColor = outColor + diffuse + specular;

    outColor = outColor * Texture;
}