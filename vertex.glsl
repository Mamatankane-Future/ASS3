#version 330 core
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec4 vertexCol;
out vec4 fragmentCol;

uniform mat4 projectionMatrix;

void main() {
    gl_Position = projectionMatrix * vec4(vertexPos, 1.0);
   
    fragmentCol = vertexCol;
}
