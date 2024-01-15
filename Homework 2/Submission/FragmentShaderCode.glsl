#version 430

// Interpolated values from the vertex shader.
in vec2 UV;
in vec2 UV2;
in vec3 theColor;
in vec3 Normal; // Added
in vec3 FragPos; // Added

// Ouput data
out vec4 Color;

// Values that stay constant for the whole mesh.

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 lightDir; // Added
uniform vec3 lightColor; // Added
uniform float lightBrightness; // Added

// Point Light
uniform vec3 pointLightPos;
uniform vec3 pointLightColor;
uniform float pointLightBrightness;

void main()
{
    // Ambient
    vec3 ambient = 0.5 * vec3(texture(texture1, UV));
    vec3 ambient2 = 0.5 * vec3(texture(texture2, UV2));

    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDirN = normalize(lightDir);
    float diff = max(dot(norm, lightDirN), 0.0);
    vec3 diffuse = diff * vec3(texture(texture2, UV));

    // Point Light
    vec3 lightDirPoint = normalize(pointLightPos - FragPos);
    float diffPoint = max(dot(norm, lightDirPoint), 0.0);
    vec3 diffusePoint = diffPoint * pointLightColor;

    vec3 result = (ambient + diffuse + diffusePoint) * theColor * (lightBrightness + pointLightBrightness);
    Color = vec4(result, 1.0);
}