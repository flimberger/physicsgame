#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUvCoord;
layout(location = 2) in vec3 vertexNormal_modelspace;

out vec2 uvCoord;
out vec3 vertexPosition_worldspace;
out vec3 vertexNormal_cameraspace;
out vec3 eyeDirection_cameraspace;
out vec3 lightDirection_cameraspace;

uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform vec3 lightPosition_worldspace;

void main() {
    // vertex in clip space
	gl_Position = mvpMatrix * vec4(vertexPosition_modelspace, 1);
    vertexPosition_worldspace = (modelMatrix * vec4(vertexPosition_modelspace, 1)).xyz;

    vec3 vertexPosition_cameraspace =
        (viewMatrix * modelMatrix * vec4(vertexPosition_modelspace, 1)).xyz;
    eyeDirection_cameraspace = vec3(0, 0, 0) - vertexPosition_cameraspace;

    vec3 lightPosition_cameraspace =
        (viewMatrix * modelMatrix * vec4(lightPosition_worldspace, 1)).xyz;
    lightDirection_cameraspace = lightPosition_cameraspace + eyeDirection_cameraspace;

    vertexNormal_cameraspace = (viewMatrix * modelMatrix * vec4(vertexNormal_modelspace, 0)).xyz;

	uvCoord = vertexUvCoord;
}
