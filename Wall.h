#ifndef WALL_H
#define WALL_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include "Matrix.h"
class Wall {
public:
    Wall(const char* texturePath, float verts[]);
    ~Wall();
    void draw();
    void rotate(double cosValue, double sinValue, int index1, int index2, bool reverse);
    void move(float x, float y, float z);
    void scale(float x, float y, float z);
    void moveScene(Matrix &transform);
    GLuint getShaderProgram();

private:
    GLuint VBO, VAO, EBO;
    GLuint texture;

    static const char* vertexShaderSource;
    static const char* fragmentShaderSource;
    GLuint shaderProgram;

    static GLuint compileShader(GLenum type, const char* source);
    static GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource);
    // Define the wall's vertices and texture coordinates
    float vertices[20];

    unsigned int indices[6] = {
        0, 1, 2,
        1, 2, 3
    };
   
};

#endif /* WALL_H */
