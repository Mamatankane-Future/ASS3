#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <chrono> 

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "Wall.h"

#include "Matrix.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

const char *getError()
{
    const char *errorDescription;
    glfwGetError(&errorDescription);
    return errorDescription;
}

inline void startUpGLFW()
{
    glewExperimental = true; // Needed for core profile
    if (!glfwInit())
    {
        throw getError();
    }
}

inline void startUpGLEW()
{
    glewExperimental = true; // Needed in core profile
    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        throw getError();
    }
}

inline GLFWwindow *setUp()
{
    startUpGLFW();
    glfwWindowHint(GLFW_SAMPLES, 4);               // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
    GLFWwindow *window;                                            // (In the accompanying source code, this variable is global for simplicity)
    window = glfwCreateWindow(1920, 1080, "CGG", NULL, NULL);
    if (window == NULL)
    {
        cout << getError() << endl;
        glfwTerminate();
        throw "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n";
    }
    glfwMakeContextCurrent(window); // Initialize GLEW
    startUpGLEW();
    return window;
}



inline void wire_frame(bool type, unsigned int &VAO, unsigned int &VBO, unsigned int &EBO, float origins[], unsigned indices[], long originsSize, long indicesSize, unsigned int &CBO, float color[], long colorSize){
    
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, colorSize, color, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, originsSize, origins, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    if (type) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float) , (void*)0);
        glEnableVertexAttribArray(0);
        glDrawElements(GL_TRIANGLES, indicesSize / sizeof(int), GL_UNSIGNED_INT, 0);
    } else {
        for (int a = 0; a < indicesSize/ sizeof(int); a += 3) {
            GLuint temp[] = {
                indices[a], indices[a + 1],
                indices[a + 1], indices[a + 2],
                indices[a + 2], indices[a]
            };
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(temp), temp, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glEnableVertexAttribArray(0);
            glDrawElements(GL_LINES, sizeof(temp) / sizeof(int), GL_UNSIGNED_INT, 0);
        }
    }
}


inline void moveScene(float * vertices, int size, Matrix &transform){
    for (int j = 0; j < size; j+=3){
        double ** arr = new double*[4];
        arr[0] = new double[1]{vertices[j]};
        arr[1] = new double[1]{vertices[j + 1]};
        arr[2] = new double[1]{vertices[j + 2]};
        arr[3] = new double[1]{1};
        Matrix point(4, 1, arr);
        Matrix res = transform * point;
        vertices[j]  = res[0][0];
        vertices[j + 1] = res[1][0];
        vertices[j + 2] = res[2][0];
    }

}


float *ORIGIN = new float[3]{-0.4, -0.8, 0.8};


class Glass{
private:
    float * vertices;
    unsigned int * indices;
    unsigned int VBO, VAO, EBO, CBO;
    float * color;

    int currentColor = 0;

    const float colors[14][4] = {
        {1.0f, 1.0f, 1.0f, 1.0f},
        {0.0f, 1.0f, 0.0f, 1.0f},
        {1.0f, 0.7f, 0.7f, 1.0f},
        {0.88f, 0.92f, 0.89f, 1.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},

        {1.0f, 0.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 1.0f, 1.0f},

        {0.5f, 0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f},
        {0.5f, 0.0f, 0.5f, 1.0f},
        {0.133f, 0.133f, 0.133f, 1.0f},
        {0.906f, 0.624f, 0.192f, 1.0f},
        {0.663f, 0.663f, 0.663f, 1.0f}
    };
     
public:
    float origin[3];

    Glass(float width, float height, float depth, float o[], int incoming_color=0, float incomming_alpha=1){
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &CBO);

        for(int i = 0; i < 3; i++){
            origin[i] = o[i];
        }

        color = new float[8 * 4];
        currentColor = incoming_color;

        for (size_t i = 0; i < 8; i++)
        {
            color[i * 4] = colors[currentColor][0];
            color[i * 4 + 1] = colors[currentColor][1];
            color[i * 4 + 2] = colors[currentColor][2];
            color[i * 4 + 3] = incomming_alpha;
        }

        initialise(width, height, depth, origin);

    }

    void initialise(float width, float height, float depth, float origin[3]){
        vertices = new float[8*3]{
            origin[0], origin[1], origin[2],
            origin[0] + width, origin[1], origin[2],
            origin[0] + width, origin[1] + height, origin[2],
            origin[0], origin[1] + height, origin[2],

            origin[0], origin[1], origin[2] + depth,
            origin[0] + width, origin[1], origin[2] + depth,
            origin[0] + width, origin[1] + height, origin[2] + depth,
            origin[0], origin[1] + height, origin[2] + depth,

        };

        indices = new unsigned int[12*3]{
            0, 1, 2,
            2, 3, 0,

            4, 5, 6,
            6, 7, 4,

            3, 7, 2,
            2, 6, 7,

            0, 4, 1,
            1, 5, 4,

            0, 3, 4,
            4, 7, 3,

            1, 2, 5,
            5, 6, 2

            
        };
    }

    void changeColor(int by){
        currentColor += by;
        if (currentColor > 10){
            currentColor = 0;
        } else if (currentColor < 0){
            currentColor = 10;
        }

        for (size_t i = 0; i < 8*4; i)
        {
            color[i++] = colors[currentColor][0];
            color[i++] = colors[currentColor][1];
            color[i++] = colors[currentColor][2];
            color[i++] = colors[currentColor][3];
        }
    }



    void draw(bool type=true){
        wire_frame(type, VAO, VBO, EBO, vertices, indices, 8*3*sizeof(float), 12*3*sizeof(unsigned int), CBO, color, 8*4*sizeof(float));
    }

    void rotate(double cosValue, double sinValue, int index1, int index2, bool reverse = false){
        IdentityMatrix toOrigin(4);
        toOrigin[0][3] = -ORIGIN[0];
        toOrigin[1][3] = -ORIGIN[1];
        toOrigin[2][3] = -ORIGIN[2];
        IdentityMatrix toOriginal(4);
        toOriginal[0][3] = ORIGIN[0];
        toOriginal[1][3] = ORIGIN[1];
        toOriginal[2][3] = ORIGIN[2];

        IdentityMatrix rotate(4);
        rotate[index1][index1] = rotate[index2][index2] = cosValue;
        if (reverse){
            rotate[index1][index2] = sinValue;
            rotate[index2][index1] = -sinValue;
        } else {
            rotate[index1][index2] = -sinValue;
            rotate[index2][index1] = sinValue;
        }

        Matrix transform = toOriginal * rotate * toOrigin;
        moveScene(vertices, 8*3, transform);
        moveScene(origin, 3, transform);
    }

    void changeTranslucency(float a){
        float alpha = color[3] + a;
        cout << alpha << endl;  
        if (alpha > 1){
            alpha = 1;
        } else if (alpha < 0){
            alpha = 0;
        }

        for (size_t i = 3; i < 12*4; i+=4)
        {
            color[i] = alpha;
        }
    }

    void move(float x, float y, float z){
        IdentityMatrix transform(4);
        transform[0][3] = x;
        transform[1][3] = y;
        transform[2][3] = z;
    
        moveScene(vertices, 8*3, transform);
        moveScene(origin, 3, transform);
    }

    void scale(float x, float y, float z){
        IdentityMatrix transform(4);
        transform[0][0] = x;
        transform[1][1] = y;
        transform[2][2] = z;
    
        moveScene(vertices, 8*3, transform);
        moveScene(origin, 3, transform);
    }

    ~Glass(){
        delete[] vertices;
        delete[] indices;
        delete[] color;
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &CBO);
    }
};



class Object{
public:
    virtual void draw(bool type=true) = 0;
    virtual void rotate(double cosValue, double sinValue, int index1, int index2, bool reverse = false) = 0;

    virtual void move(float x, float y, float z) = 0;
    virtual void scale(float x, float y, float z) = 0;

    virtual ~Object(){};

    virtual float* returnORIGIN(){
        return ORIGIN;
    }
};


class NorthWall: public Object{
public:
    static const int size = 26;

    Glass * glasses[size];
    float originsY[5] = {-0.75, -0.6, 0, 0.2, 0.8};
    float originsX[5] = {-0.4, -0.2, 0, 0.2, 0.4};
    float z = 0.95;

    NorthWall(){
        glasses[0] = new Glass(0.015, 1.55, 0.02, new float[3]{originsX[0], originsY[0], z});
        glasses[1] = new Glass(0.015, 1.55, 0.02, new float[3]{originsX[1], originsY[0], z});
        glasses[2] = new Glass(0.015, 1.55, 0.02, new float[3]{originsX[2], originsY[0], z});
        glasses[3] = new Glass(0.015, 1.55, 0.02, new float[3]{originsX[3], originsY[0], z});
        glasses[4] = new Glass(0.015, 1.55, 0.02, new float[3]{originsX[4], originsY[0], z});

        glasses[5] = new Glass(0.8, 0.015, 0.02, new float[3]{originsX[0], originsY[0], z});
        glasses[6] = new Glass(0.8, 0.015, 0.02, new float[3]{originsX[0], originsY[1], z});
        glasses[7] = new Glass(0.8, 0.015, 0.02, new float[3]{originsX[0], originsY[2], z});
        glasses[8] = new Glass(0.8, 0.015, 0.02, new float[3]{originsX[0], originsY[3], z});
        glasses[9] = new Glass(0.8, 0.015, 0.02, new float[3]{originsX[0], originsY[4], z});

        int k = 10;
        for (size_t i = 0; i < 4; i++)
        {
            float height = 0.6;
            if (i % 2 == 0){
                height = 0.2;
            }
            for (size_t j = 0; j < 4; j++)
            {
                glasses[k++] = new Glass(0.2, height, 0.02, new float[3]{originsX[j] + 0.015, originsY[i]+ 0.015, z}, 3, 0.1);
            }

        }



    

    }

    float* returnORIGIN(){
        return new float[3]{glasses[0]->origin[0], glasses[0]->origin[1], glasses[0]->origin[2]};
    }

    void draw(bool type=true){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->draw(type);
        }
    }

    void move(float x, float y, float z){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->move(x, y, z);
        }
    }

    void scale(float x, float y, float z){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->scale(x, y, z);
        }
    }


    void rotate(double cosValue, double sinValue, int index1, int index2, bool reverse = false){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->rotate(cosValue, sinValue, index1, index2, reverse);
        }
    }

    ~NorthWall(){
        for (size_t i = 0; i < size; i++)
        {
            delete glasses[i];
        }
    }
};



class EastWall: public Object{
public:
    static const int size = 46;


    Glass * glasses[size];
    Object * windows;
    float z[9] = {0.95, 0.75, 0.55, 0.35, 0.15, -0.05, -0.25, -0.45, -0.65};

    float y = -0.75;
    float x= -0.5;
    
    EastWall(){
        glasses[0] = new Glass(0.1, 0.75, -1.9, new float[3]{x, y, z[0]}, 13);
        glasses[1] = new Glass(0.15, 0.3, -1.8, new float[3]{x + 0.1, y+0.45, z[0]}, 13);
        glasses[2] = new Glass(0.15, 0.3, -1.8, new float[3]{x + 0.1, y+0.9, z[0]}, 13);
        glasses[3] = new Glass(0.15, 0.2, -1.8, new float[3]{x + 0.1, y+1.35, z[0]}, 11);
        glasses[4] = new Glass(0.01, 0.01, -1.8, new float[3]{x + 0.1, 0, z[0]});
        glasses[5] = new Glass(0.01, 0.01, -1.8, new float[3]{x + 0.1, 0.14, z[0]});
        glasses[6] = new Glass(0.01, 0.01, -1.8, new float[3]{x + 0.1, 0.45, z[0]});
        glasses[7] = new Glass(0.01, 0.01, -1.8, new float[3]{x + 0.1, 0.59, z[0]});

        glasses[8] = new Glass(0.1, 0.3, -1.9, new float[3]{x, y+0.9, z[0]}, 13);
        glasses[9] = new Glass(0.1, 0.2, -1.9, new float[3]{x, y+1.35, z[0]}, 13);
 
        int k = 10;
        for (size_t i = 0; i < 9; i++)
        {
            glasses[k++] = new Glass(0.02, 0.15, -0.02, new float[3]{x + 0.1, 0, z[i]});
            glasses[k++] = new Glass(0.02, 0.15, -0.02, new float[3]{x + 0.1, 0.45, z[i]});
            glasses[k++] = new Glass(0.01, 0.14, -0.19, new float[3]{x + 0.11, 0.46, z[i]+0.01}, 3, 0.1);
            glasses[k++] = new Glass(0.01, 0.14, -0.19, new float[3]{x + 0.11, 0.01, z[i]+0.01}, 3, 0.1);
        }

        // glasses[k++] = new Glass(0.2, height, 0.02, new float[3]{originsX[j] + 0.015, originsY[i]+ 0.015, z}, 3, 0.1);
    }


    void draw(bool type=true){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->draw(type);
        }
    }

    void move(float x, float y, float z){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->move(x, y, z);

        }
    }

    void scale(float x, float y, float z){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->scale(x, y, z);
        }
    }

    void rotate(double cosValue, double sinValue, int index1, int index2, bool reverse = false){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->rotate(cosValue, sinValue, index1, index2, reverse);
        }
    }

    ~EastWall(){
        for (size_t i = 0; i < size; i++)
        {
            delete glasses[i];
        }
    }

};

class WestWall: public Object{
public:
    static const int size = 46;


    Glass * glasses[size];
    Object * windows;
    float z[9] = {0.95, 0.75, 0.55, 0.35, 0.15, -0.05, -0.25, -0.45, -0.65};

    float y = -0.75;
    float x= 0.5;
    
    WestWall(){
        glasses[0] = new Glass(-0.1, 0.75, -1.9, new float[3]{x, y, z[0]}, 13);
        glasses[1] = new Glass(-0.15, 0.3, -1.8, new float[3]{x - 0.1, y+0.45, z[0]}, 13);
        glasses[2] = new Glass(-0.15, 0.3, -1.8, new float[3]{x - 0.1, y+0.9, z[0]}, 13);
        glasses[3] = new Glass(-0.15, 0.2, -1.8, new float[3]{x - 0.1, y+1.35, z[0]}, 11);
        glasses[4] = new Glass(-0.01, 0.01, -1.8, new float[3]{x - 0.1, 0, z[0]});
        glasses[5] = new Glass(-0.01, 0.01, -1.8, new float[3]{x - 0.1, 0.14, z[0]});
        glasses[6] = new Glass(-0.01, 0.01, -1.8, new float[3]{x - 0.1, 0.45, z[0]});
        glasses[7] = new Glass(-0.01, 0.01, -1.8, new float[3]{x - 0.1, 0.59, z[0]});

        glasses[8] = new Glass(-0.1, 0.3, -1.9, new float[3]{x, y+0.9, z[0]}, 13);
        glasses[9] = new Glass(-0.1, 0.2, -1.9, new float[3]{x, y+1.35, z[0]}, 13);
 
        int k = 10;
        for (size_t i = 0; i < 9; i++)
        {
            glasses[k++] = new Glass(-0.02, 0.15, -0.02, new float[3]{x - 0.1, 0, z[i]});
            glasses[k++] = new Glass(-0.02, 0.15, -0.02, new float[3]{x - 0.1, 0.45, z[i]});
            glasses[k++] = new Glass(-0.01, 0.14, -0.19, new float[3]{x - 0.11, 0.46, z[i]+0.01}, 3, 0.1);
            glasses[k++] = new Glass(-0.01, 0.14, -0.19, new float[3]{x - 0.11, 0.01, z[i]+0.01}, 3, 0.1);
        }

        // glasses[k++] = new Glass(0.2, height, 0.02, new float[3]{originsX[j] + 0.015, originsY[i]+ 0.015, z}, 3, 0.1);
    }


    void draw(bool type=true){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->draw(type);
        }
    }

    void move(float x, float y, float z){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->move(x, y, z);

        }
    }

    void scale(float x, float y, float z){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->scale(x, y, z);
        }
    }

    void rotate(double cosValue, double sinValue, int index1, int index2, bool reverse = false){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->rotate(cosValue, sinValue, index1, index2, reverse);
        }
    }

    ~WestWall(){
        for (size_t i = 0; i < size; i++)
        {
            delete glasses[i];
        }
    }

};

class SouthWall: public Object{
public:
    static const int size = 27;

    Glass * glasses[size];
    float originsY[6] = {0, 0.2, 0.8, -0.75, -0.3, -0.31};
    float originsX[6] = {-0.4, -0.2, 0.2, 0.4, -0.305, 0.305};
    float z = -0.95;

    SouthWall(){
        glasses[0] = new Glass(0.015, 0.8, 0.02, new float[3]{originsX[0], originsY[0], z});
        glasses[1] = new Glass(0.015, 0.8, 0.02, new float[3]{originsX[1], originsY[0], z});
        glasses[2] = new Glass(0.015, 0.8, 0.02, new float[3]{originsX[2], originsY[0], z});
        glasses[3] = new Glass(0.015, 0.8, 0.02, new float[3]{originsX[3], originsY[0], z});

        glasses[4] = new Glass(0.2, 0.015, 0.02, new float[3]{originsX[0], originsY[0], z});
        glasses[5] = new Glass(0.2, 0.015, 0.02, new float[3]{originsX[0], originsY[1], z});
        glasses[6] = new Glass(0.2, 0.015, 0.02, new float[3]{originsX[0], originsY[2], z});
        glasses[7] = new Glass(0.2, 0.015, 0.02, new float[3]{originsX[2], originsY[0], z});
        glasses[8] = new Glass(0.2, 0.015, 0.02, new float[3]{originsX[2], originsY[1], z});
        glasses[9] = new Glass(0.2, 0.015, 0.02, new float[3]{originsX[2], originsY[2], z});

        glasses[10] = new Glass(0.385, 1.55, 0.35, new float[3]{originsX[1] + 0.015f, originsY[3], z}, 11);
        glasses[11] = new Glass(0.215, 0.3, 0.02, new float[3]{originsX[0], originsY[4], z}, 11);
        glasses[12] = new Glass(0.215, 0.3, 0.02, new float[3]{originsX[2], originsY[4], z}, 11);
   
        glasses[13] = new Glass(0.2, 0.2, 0.02, new float[3]{originsX[0] + 0.015, originsY[0]+ 0.015, z}, 3, 0.1);
        glasses[14] = new Glass(0.2, 0.6, 0.02, new float[3]{originsX[0] + 0.015, originsY[1]+ 0.015, z}, 3, 0.1);
        glasses[15] = new Glass(0.2, 0.2, 0.02, new float[3]{originsX[2] + 0.015, originsY[0]+ 0.015, z}, 3, 0.1);
        glasses[16] = new Glass(0.2, 0.6, 0.02, new float[3]{originsX[2] + 0.015, originsY[1]+ 0.015, z}, 3, 0.1);


        glasses[17] = new Glass(0.01, 0.45, 0.02, new float[3]{originsX[0], originsY[3], z});
        glasses[18] = new Glass(0.01, 0.45, 0.02, new float[3]{originsX[2], originsY[3], z});


        glasses[19] = new Glass(0.01, 0.45, 0.02, new float[3]{originsX[4], originsY[3], z});
        glasses[20] = new Glass(0.01, 0.45, 0.02, new float[3]{originsX[1], originsY[3], z});

        glasses[21] = new Glass(0.01, 0.45, 0.02, new float[3]{originsX[5], originsY[3], z});
        glasses[22] = new Glass(0.01, 0.45, 0.02, new float[3]{originsX[3], originsY[3], z});

        glasses[23] = new Glass(0.2, 0.01, 0.02, new float[3]{originsX[0], originsY[3], z});
        glasses[24] = new Glass(0.2, 0.01, 0.02, new float[3]{originsX[2], originsY[3], z});

        glasses[25] = new Glass(0.2, 0.01, 0.02, new float[3]{originsX[0], originsY[5], z});
        glasses[26] = new Glass(0.2, 0.01, 0.02, new float[3]{originsX[2], originsY[5], z});


        glasses[27] = new Glass(0.095, 0.44, 0.02, new float[3]{originsX[0] + 0.01, originsY[3] +0.01, z}, 3, 0.1);

        glasses[28] = new Glass(0.095, 0.44, 0.02, new float[3]{originsX[2] + 0.01, originsY[3] +0.01, z}, 3, 0.1);

        glasses[29] = new Glass(0.095, 0.44, 0.02, new float[3]{originsX[4] + 0.01, originsY[3] +0.01, z}, 3, 0.1);
        glasses[30] = new Glass(0.095, 0.44, 0.02, new float[3]{originsX[5]+ 0.01, originsY[3]+ 0.01, z}, 3, 0.1);

        // glasses[19] = new Glass(0.015, 0.015, 0.02, new float[3]{originsX[0], originsY[3], z});

    

    }

    void draw(bool type=true){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->draw(type);
        }
    }

    void move(float x, float y, float z){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->move(x, y, z);
        }
    }

    void scale(float x, float y, float z){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->scale(x, y, z);
        }
    }

    void rotate(double cosValue, double sinValue, int index1, int index2, bool reverse = false){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->rotate(cosValue, sinValue, index1, index2, reverse);
        }
    }

    void move(){
        
    }

    ~SouthWall(){
        for (size_t i = 0; i < size; i++)
        {
            delete glasses[i];
        }
    }

};

class Floor: public Object{
public:
    static const int size = 5;
    Glass * glasses[size];
    float y = -0.8;
    float x = -0.4;
    float z[6]= {-0.95, -0.3, -0.25, 0.35, 0.40, 0.95};

    Floor(){
        glasses[0] = new Glass(0.8, 0.05, 0.65, new float[3]{x, y, z[0]}, 11);
        glasses[1] = new Glass(0.8, 0.05, 0.05, new float[3]{x, y, z[1]}, 12);
        glasses[2] = new Glass(0.8, 0.05, 0.6, new float[3]{x, y, z[2]}, 11);
        glasses[3] = new Glass(0.8, 0.05, 0.05, new float[3]{x, y, z[3]}, 12);
        glasses[4] = new Glass(0.8, 0.05, 0.55, new float[3]{x, y, z[4]}, 11);
    }

    void draw(bool type=true){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->draw(type);
        }
    }

    void rotate(double cosValue, double sinValue, int index1, int index2, bool reverse = false){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->rotate(cosValue, sinValue, index1, index2, reverse);
        }
    }

    void move(float x, float y, float z){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->move(x, y, z);
        }
    }

    void scale(float x, float y, float z){
        for (size_t i = 0; i < size; i++)
        {
            glasses[i]->scale(x, y, z);
        }
    }

};

class Scene{
private:
    static const int size = 5;
    Object * objects[size];

public:
    Scene(){
        objects[0] = new NorthWall();
        objects[1] = new Floor();
        objects[2] = new SouthWall();
        objects[3] = new EastWall();
        objects[4] = new WestWall();
        
        
    }

    void draw(bool type=true){
        for (size_t i = 0; i < size; i++)
        {
            objects[i]->draw(type);
        }
    }

    void rotate(double cosValue, double sinValue, int index1, int index2, bool reverse = false){
        delete ORIGIN;
        ORIGIN = objects[0]->returnORIGIN();
        
        for (size_t i = 0; i < size; i++)
        {
            objects[i]->rotate(cosValue, sinValue, index1, index2, reverse);
        }
    }

    void move(float x, float y, float z){
        for (size_t i = 0; i < size; i++)
        {
            objects[i]->move(x, y, z);
        }
    }

    void scale(float x, float y, float z){
        for (size_t i = 0; i < size; i++)
        {
            objects[i]->scale(x, y, z);
        }
    }

    ~Scene(){
        for (size_t i = 0; i < size; i++)
        {
            delete objects[i];
        }
    }

};


enum ProjectionType {
    PERSPECTIVE,
    ORTHOGRAPHIC
};

ProjectionType currentProjection = PERSPECTIVE; 
// ProjectionType currentColor = ORTHOGRAPHIC;

void toggleProjection() {
    if (currentProjection == PERSPECTIVE) {
        currentProjection = ORTHOGRAPHIC;
    } else {
        currentProjection = PERSPECTIVE;
    }
}


int main()
{
    GLFWwindow *window;
    try
    {
        window = setUp();
    }
    catch (const char *e)
    {
        cout << e << endl;
        throw;
    }

    //Add code here

    unsigned int programID = LoadShaders("vertex.glsl", "fragment.glsl");


    Scene scene;
    float vertices[20] = {
                -0.4f, -0.75f, 0.93f,  0.0f, 0.0f,
                -0.4f,  0.8f, 0.93f,  1.0f, 0.0f,
                0.4f, -0.75f, 0.93f,  1.0f, 1.0f,
                0.4f,  0.8f, 0.93f,  0.0f, 1.0f};
    Wall wall("table.jpg", vertices);

    glm::mat4 perspectiveProjection;
    glm::mat4 orthoProjection;

    // Set up the initial perspective projection matrix
    float fov = 45.0f;
    float aspectRatio = 1920.0f / 1080.0f;
    float near = 0.1f;
    float far = 100.0f;
    perspectiveProjection = glm::perspective(glm::radians(fov), aspectRatio, near, far);

    // Set up the initial orthographic projection matrix
    float left = -3.0f;
    float right = 3.0f;
    float bottom = -2.0f;
    float top = 2.0f;
    orthoProjection = glm::ortho(left, right, bottom, top, near, far);

    GLuint projectionMatrixLocation = glGetUniformLocation(programID, "projectionMatrix");
    GLuint projectionMatrixLocationW = glGetUniformLocation(wall.getShaderProgram(), "projectionMatrix");


    const float TOGGLE_DELAY = 0.2f;
    double lastToggleTime = 0.0;
    bool type = true;
    const float xrotationAngle = 0.02f;
    const float yrotationAngle = 0.02f;
    const float zrotationAngle = 0.02f;


    while (!glfwWindowShouldClose(window)) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_BLEND);  
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

        // Choose the current projection matrix
        glm::mat4 projectionMatrix;
        if (currentProjection == PERSPECTIVE) {
            projectionMatrix = perspectiveProjection;
        } else {
            projectionMatrix = orthoProjection;
        }

        // Set the projection matrix uniform in your shader program
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(projectionMatrixLocationW, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        

        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        {

            double currentTime = glfwGetTime();

            if (currentTime - lastToggleTime >= TOGGLE_DELAY) {
                type = !type;
                lastToggleTime = currentTime;
            }

        }

        glUseProgram(programID);
        scene.draw(type);
        // glUseProgram(wall.getShaderProgram());
        wall.draw();

        

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            
            scene.rotate(cos(zrotationAngle), sin(zrotationAngle), 0, 1);
            wall.rotate(cos(zrotationAngle), sin(zrotationAngle), 0, 1, false);
        }
        else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            scene.rotate(cos(-zrotationAngle), sin(-zrotationAngle), 0, 1);
            wall.rotate(cos(-zrotationAngle), sin(-zrotationAngle), 0, 1, false);

        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            scene.rotate(cos(xrotationAngle), sin(xrotationAngle), 1, 2);
            wall.rotate(cos(xrotationAngle), sin(xrotationAngle), 1, 2, false);
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            scene.rotate(cos(-xrotationAngle), sin(-xrotationAngle), 1, 2);
            wall.rotate(cos(-xrotationAngle), sin(-xrotationAngle), 1, 2, false);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            scene.rotate(cos(yrotationAngle), sin(yrotationAngle), 0, 2, true);
            wall.rotate(cos(yrotationAngle), sin(yrotationAngle), 0, 2, true);
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            scene.rotate(cos(-yrotationAngle), sin(-yrotationAngle), 0, 2, true);
            wall.rotate(cos(-yrotationAngle), sin(-yrotationAngle), 0, 2, true);
    
        }
        else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            scene.move(0, 0.03, 0);
            wall.move(0, 0.03, 0);
        }
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            scene.move(0, -0.03, 0);
            wall.move(0, -0.03, 0);
        }
        else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            scene.move(-0.03, 0, 0);
            wall.move(-0.03, 0, 0);
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            scene.move(0.03, 0, 0);
            wall.move(0.03, 0, 0);
        }
        else if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
        {
            if (currentProjection == PERSPECTIVE) {
                scene.move(0, 0, 0.03);
                wall.move(0, 0, 0.03);
            } else {
                scene.scale(1.03, 1.03, 1.03);
                wall.scale(1.03, 1.03, 1.03);
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
        {
            if (currentProjection == PERSPECTIVE) {
                scene.move(0, 0, -0.03);
                wall.move(0, 0, -0.03);
            } else {
                scene.scale(0.97, 0.97, 0.97);
                wall.scale(0.97, 0.97, 0.97);
            }
        }

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            toggleProjection();
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(programID); 

    glfwTerminate();
}
