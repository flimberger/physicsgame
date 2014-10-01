#version 330 core

in vec2 uvCoord;
in vec3 vertexPosition_worldspace;
in vec3 vertexNormal_cameraspace;
in vec3 eyeDirection_cameraspace;
in vec3 lightDirection_cameraspace;

out vec3 color;

uniform sampler2D myTextureSampler;
uniform vec3 lightPosition_worldspace;
uniform vec3 lightColor;
uniform float lightPower;

void main() {
    vec3 n = normalize(vertexNormal_cameraspace);
    vec3 l = normalize(lightDirection_cameraspace);
    vec3 e = normalize(eyeDirection_cameraspace);
    vec3 r = reflect(-l, n);
    vec3 materialDiffuseColor = texture2D(myTextureSampler, uvCoord).rgb;
    vec3 materialAmbientColor = vec3(0.1, 0.1, 0.1) * materialDiffuseColor;
    vec3 materialSpecularColor = vec3(0.3, 0.3, 0.3);
    float cosTheta = clamp(dot(n, l), 0, 1);
    float cosAlpha = clamp(dot(e, r), 0, 1);
    float lightDist = length(lightPosition_worldspace - vertexPosition_worldspace);

    color = materialAmbientColor
        + materialDiffuseColor * lightColor * lightPower * cosTheta/(lightDist*lightDist)
        + materialSpecularColor * lightColor * lightPower * pow(cosAlpha, 5)/(lightDist*lightDist);
}
