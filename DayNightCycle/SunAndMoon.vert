#version 460 core

layout (location = 0) in vec4 inPos;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec2 inTexCoords;

out vec4 outColor;
out vec2 texCoords;

uniform mat4 myMatrix;

void main() {
    gl_Position = myMatrix * inPos;
    outColor = inColor;
    texCoords = vec2(inTexCoords.x, 1-inTexCoords.y);
    
}