#ifndef SHAPES_H
#define SHAPES_H

#include <iostream>
#include <GL/glew.h>
#include <vector>
#include "Matrix.h"
using namespace std;

class Shape{
    protected:
        unsigned int * indices;
        unsigned int indicesSize;
        unsigned int colorsSize;

        unsigned int VBO, VAO, EBO, CBO;
        float origin[3];
        float * color;
    public:
        float * vertices; //does not include origin matrix
        unsigned int verticesSize;
        
        static void wire_frame(bool type, unsigned int &VAO, unsigned int &VBO, unsigned int &EBO, float origins[], unsigned indices[], long originsSize, long indicesSize, unsigned int &CBO, float color[], long colorSize){
    
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

    static void moveScene(float * vertices, int size, Matrix &transform){
    //loop through, and apply transformation to, all vertices
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

    
    virtual void rotate(double cosValue, double sinValue, int index1, int index2, bool reverse = false){

        IdentityMatrix toOrigin(4);
        toOrigin[0][3] = -origin[0];
        toOrigin[1][3] = -origin[1];
        toOrigin[2][3] = -origin[2];
        IdentityMatrix toOriginal(4);
        toOriginal[0][3] = origin[0];
        toOriginal[1][3] = origin[1];
        toOriginal[2][3] = origin[2];

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
        moveScene(vertices, verticesSize, transform);
        moveScene(origin, 3, transform);
    }

    virtual void draw(bool) = 0;

    virtual void print(){
        for (int i = 0; i < verticesSize; i+=3){
            cout << vertices[i] << " " << vertices[i + 1] << " " << vertices[i + 2] << endl;
        }
    }

    virtual ~Shape(){}

    friend class CompositeShape;
};


//////////////////////////////////////////////Glass class///////////////////////////////////////////////////

/*class Glass : public Shape{
private:

    int currentColor = 0;

    const float colors[11][4] = {
        {0.0f, 1.0f, 0.0f, 1.0f},
        {1.0f, 0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f, 1.0f},

        {1.0f, 1.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},

        {1.0f, 0.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 1.0f, 1.0f},

        {0.5f, 0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f},
        {0.5f, 0.0f, 0.5f, 1.0f}
    };
     
public:
    Glass(float width, float height, float p[], float c[], float depth = 0.05){
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &CBO);

        for(int i = 0; i < 3; i++){
            origin[i] = p[i];
        }

        color = new float[8 * 4];
        for (size_t i = 0; i < 8; i++)
        {
            color[i * 4] = c[0];
            color[i * 4 + 1] = c[1];
            color[i * 4 + 2] = c[2];
            color[i * 4 + 3] = c[3];
        }

        initialise(width, height, depth, origin);

    }

    Glass(float width, float height, float p[], float depth = 0.05){
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &CBO);

        for(int i = 0; i < 3; i++){
            origin[i] = p[i];
        }

        color = new float[8 * 4];
        for (size_t i = 0; i < 8; i++)
        {
            color[i * 4] = colors[currentColor][0];
            color[i * 4 + 1] = colors[currentColor][1];
            color[i * 4 + 2] = colors[currentColor][2];
            color[i * 4 + 3] = colors[currentColor][3];
        }

        initialise(width, height, depth, origin);

    }

    void initialise(float width, float height, float depth, float origin[3]){
        // creates array of vertices for cuboid 
        verticesSize = 8 * 3;
        vertices = new float[verticesSize]{
            origin[0], origin[1], origin[2],
            origin[0] + width, origin[1], origin[2],
            origin[0] + width, origin[1] + height, origin[2],
            origin[0], origin[1] + height, origin[2],

            origin[0], origin[1], origin[2] + depth,
            origin[0] + width, origin[1], origin[2] + depth,
            origin[0] + width, origin[1] + height, origin[2] + depth,
            origin[0], origin[1] + height, origin[2] + depth,

        };

        // 
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
*/

//////////////////////////////////////////////Wall class///////////////////////////////////////////////////
// class Wall : public Shape{
// private:

//     int currentColor = 1;

//     const float colors[11][4] = {
//         {0.0f, 1.0f, 0.0f, 1.0f},
//         {1.0f, 0.0f, 0.0f, 1.0f},
//         {0.0f, 0.0f, 1.0f, 1.0f},

//         {1.0f, 1.0f, 1.0f, 1.0f},
//         {0.0f, 0.0f, 0.0f, 1.0f},

//         {1.0f, 0.0f, 1.0f, 1.0f},
//         {1.0f, 1.0f, 0.0f, 1.0f},
//         {0.0f, 1.0f, 1.0f, 1.0f},

//         {0.0f, 0.5f, 0.5f, 1.0f},
//         {0.5f, 0.5f, 0.0f, 1.0f},
//         {0.5f, 0.0f, 0.5f, 1.0f}
//     };
     
// public:
//     Wall(float width, float height, int x, int y, float p[]){
//         glGenVertexArrays(1, &VAO);
//         glBindVertexArray(VAO);
//         glGenBuffers(1, &VBO);
//         glGenBuffers(1, &EBO);
//         glGenBuffers(1, &CBO);

//         for(int i = 0; i < 3; i++){
//             origin[i] = p[i];
//         }


//         float incX = width / x;
//         float incY = height / y;

//         x++;
//         y++;

//         verticesSize = x * y * 3;
        

//         vertices = new float[verticesSize]{};
//         int k = 0;

//         for (int i = 0; i < x; i++){
//             for (int j = 0; j < y; j++){
//                 vertices[k++] = origin[0] + i * incX;
//                 vertices[k++] = origin[1] + j * incY;
//                 vertices[k++] = origin[2];
//             }
//         }


//         indicesSize = (x - 1) * (y - 1) * 6;

    
//         indices = new unsigned int[indicesSize]{};

//         k = 0;
//         for (int i = 0; i < x - 1; i++){
//             editIndices(y, k, i * y);
//         }

//         for (int i = 0; i < x - 1; i++){
//             editIndicesReverse(y, k, i * y);
//         } 

//         colorsSize = x * y * 4;

//         color = new float[colorsSize];
//         for (size_t i = 0; i < colorsSize; i)
//         {
//             color[i++] = colors[currentColor][0];
//             color[i++] = colors[currentColor][1];
//             color[i++] = colors[currentColor][2];
//             color[i++] = colors[currentColor][3];
//         }

//     }

//     void editIndices(int rects, int &k, int offset){
//         for (int i = 0; i < rects - 1; i++){
//             indices[k++] = i + offset; 
//             indices[k++] = i + 1 + offset;
//             indices[k++] = rects + i + offset;
//         }
//     }

//     void editIndicesReverse(int rects, int &k, int offset){
//         for (int i = 0; i < rects - 1; i++){
//             indices[k++] = rects + i + offset + 1;
//             indices[k++] = i + 1 + offset;
//             indices[k++] = rects + i + offset;
//         }
//     }




//     void draw(bool type=true){
//         wire_frame(type, VAO, VBO, EBO, vertices, indices, verticesSize*sizeof(float), indicesSize*sizeof(unsigned int), CBO, color, colorsSize*sizeof(float));
//     }

//     void changeColor(int by){
//         currentColor += by;
//         if (currentColor > 10){
//             currentColor = 0;
//         } else if (currentColor < 0){
//             currentColor = 10;
//         }

//         for (size_t i = 0; i < colorsSize; i)
//         {
//             color[i++] = colors[currentColor][0];
//             color[i++] = colors[currentColor][1];
//             color[i++] = colors[currentColor][2];
//             color[i++] = colors[currentColor][3];
//         }
//     }

//     ~Wall(){
//         delete[] vertices;
//         delete[] indices;
//         delete[] color;
//         glDeleteVertexArrays(1, &VAO);
//         glDeleteBuffers(1, &VBO);
//         glDeleteBuffers(1, &EBO);
//         glDeleteBuffers(1, &CBO);
//     }
// };


////////////////////////////////////Cuboid////////////////////////////////////////

class Cuboid : public Shape {
private: 
    int currentColor = 2;
public:    
    const float colors[11][4] = {
    {0.0f, 1.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 1.0f, 1.0f},
    {0.5f, 0.5f, 0.5f, 1.0f},
    {0.5f, 0.5f, 0.0f, 1.0f},
    {0.5f, 0.0f, 0.5f, 1.0f}
    };

    Cuboid(float width, float height, float depth, float p[], float c[]) {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &CBO);

        for (int i = 0; i < 3; i++) {
            origin[i] = p[i];
        }

        colorsSize = 8 * 4;
        color = new float[colorsSize];
        for (size_t i = 0; i < 8; i++) {
            color[i * 4] = c[0];
            color[i * 4 + 1] = c[1];
            color[i * 4 + 2] = c[2];
            color[i * 4 + 3] = c[3];
        }

        initialise(width, height, depth, origin);
    }

    Cuboid(float width, float height, float depth, float p[]) {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &CBO);

        for (int i = 0; i < 3; i++) {
            origin[i] = p[i];
        }

        colorsSize = 8 * 4;
        color = new float[colorsSize];
        for (size_t i = 0; i < 8; i++) {
            color[i * 4] = colors[currentColor][0];
            color[i * 4 + 1] = colors[currentColor][1];
            color[i * 4 + 2] = colors[currentColor][2];
            color[i * 4 + 3] = colors[currentColor][3];
        }

        initialise(width, height, depth, origin);
    }

    void initialise(float width, float height, float depth, float origin[3]) {
        verticesSize = 8 * 3;
        vertices = new float[verticesSize]{
            origin[0] - width / 2, origin[1] - height / 2, origin[2] - depth / 2,
            origin[0] + width / 2, origin[1] - height / 2, origin[2] - depth / 2,
            origin[0] + width / 2, origin[1] + height / 2, origin[2] - depth / 2,
            origin[0] - width / 2, origin[1] + height / 2, origin[2] - depth / 2,

            origin[0] - width / 2, origin[1] - height / 2, origin[2] + depth / 2,
            origin[0] + width / 2, origin[1] - height / 2, origin[2] + depth / 2,
            origin[0] + width / 2, origin[1] + height / 2, origin[2] + depth / 2,
            origin[0] - width / 2, origin[1] + height / 2, origin[2] + depth / 2,
        };

        indicesSize = 12 * 3;
        indices = new unsigned int[indicesSize]{
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            3, 7, 2, 2, 6, 7,
            0, 4, 1, 1, 5, 4,
            0, 3, 4, 4, 7, 3,
            1, 2, 5, 5, 6, 2
        };

        // Bind and set buffer data for vertices
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(float), vertices, GL_STATIC_DRAW);
        // Bind and set buffer data for indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(unsigned int), indices, GL_STATIC_DRAW);
        // Bind and set buffer data for colors
        glBindBuffer(GL_ARRAY_BUFFER, CBO);
        glBufferData(GL_ARRAY_BUFFER, colorsSize * sizeof(float), color, GL_STATIC_DRAW);

        // Enable and set vertex attribute pointers (you might need to adjust these based on your shader)
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, CBO);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // Unbind the VAO for now
        glBindVertexArray(0);
    }

    void draw(bool type=true){
        wire_frame(type, VAO, VBO, EBO, vertices, indices, verticesSize*sizeof(float), indicesSize*sizeof(unsigned int), CBO, color, colorsSize*sizeof(float));
    }

    ~Cuboid() {
        delete[] vertices;
        delete[] indices;
        delete[] color;
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &CBO);
        glDeleteVertexArrays(1, &VAO);
    }
};

//////////////////////////////////////////Cylinder class///////////////////////////////////////////

class Cylinder : public Shape {
public:

    const float colors[11][4] = {
        {0.0f, 1.0f, 0.0f, 1.0f},
        {1.0f, 0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        {1.0f, 0.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 1.0f, 1.0f},
        {0.5f, 0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f},
        {0.5f, 0.0f, 0.5f, 1.0f}
    };

    int currentColor = 1;

    Cylinder(float radius, float height, int points, float p[], float c[]) {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &CBO);

        for (int i = 0; i < 3; i++) {
            origin[i] = p[i];
        }

        colorsSize = (points + 1) * 2 * 4;
        color = new float[colorsSize];
        for (size_t i = 0; i < (points + 1) * 2; i++) {
            color[i * 4] = c[0];
            color[i * 4 + 1] = c[1];
            color[i * 4 + 2] = c[2];
            color[i * 4 + 3] = c[3];
        }

        initialise(radius, height, points, origin);
    }

    Cylinder(float radius, float height, int points, float p[]) {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &CBO);

        for (int i = 0; i < 3; i++) {
            origin[i] = p[i];
        }

        colorsSize = (points + 1) * 2 * 4;
        color = new float[colorsSize];
        for (size_t i = 0; i < (points + 1) * 2; i++) {
            color[i * 4] = colors[currentColor][0];
            color[i * 4 + 1] = colors[currentColor][1];
            color[i * 4 + 2] = colors[currentColor][2];
            color[i * 4 + 3] = colors[currentColor][3];
        }

        initialise(radius, height, points, origin);
    }

    void initialise(float radius, float height, int points, float origin[3]) {
        std::vector<float> vertexData;
        std::vector<unsigned int> indexData;

        // Top and bottom center vertices
        vertexData.push_back(origin[0]);
        vertexData.push_back(origin[1] + height / 2);
        vertexData.push_back(origin[2]);

        vertexData.push_back(origin[0]);
        vertexData.push_back(origin[1] - height / 2);
        vertexData.push_back(origin[2]);

        float angleIncrement = 2.0f * M_PI / points;
        for (int i = 0; i <= points; ++i) {
            float angle = i * angleIncrement;
            float x = radius * cos(angle);
            float z = radius * sin(angle);

            // Top ring vertices
            vertexData.push_back(origin[0] + x);
            vertexData.push_back(origin[1] + height / 2);
            vertexData.push_back(origin[2] + z);

            // Bottom ring vertices
            vertexData.push_back(origin[0] + x);
            vertexData.push_back(origin[1] - height / 2);
            vertexData.push_back(origin[2] + z);
        }

        // Create indices for top and bottom faces
        for (int i = 1; i <= points; ++i) {
            indexData.push_back(0);
            indexData.push_back(i * 2);
            indexData.push_back((i % points) * 2 + 2);

            indexData.push_back(1);
            indexData.push_back((i % points) * 2 + 3);
            indexData.push_back(i * 2 + 1);
        }

        // Create indices for side faces
        for (int i = 1; i <= points; ++i) {
            indexData.push_back(i * 2);
            indexData.push_back(i * 2 + 1);
            indexData.push_back((i % points) * 2 + 2);

            indexData.push_back((i % points) * 2 + 2);
            indexData.push_back(i * 2 + 1);
            indexData.push_back((i % points) * 2 + 3);
        }

        verticesSize = vertexData.size();
        vertices = new float[verticesSize];
        std::copy(vertexData.begin(), vertexData.end(), vertices);

        indicesSize = indexData.size();
        indices = new unsigned int[indicesSize];
        std::copy(indexData.begin(), indexData.end(), indices);

        // Bind and set buffer data for vertices
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(float), vertices, GL_STATIC_DRAW);
        // Bind and set buffer data for indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(unsigned int), indices, GL_STATIC_DRAW);
        // Bind and set buffer data for colors
        glBindBuffer(GL_ARRAY_BUFFER, CBO);
        glBufferData(GL_ARRAY_BUFFER, colorsSize * sizeof(float), color, GL_STATIC_DRAW);

        // Enable and set vertex attribute pointers (you might need to adjust these based on your shader)
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, CBO);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // Unbind the VAO for now
        glBindVertexArray(0);
    }

    ~Cylinder() {
        delete[] vertices;
        delete[] indices;
        delete[] color;
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &CBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void draw(bool type=true) {
        wire_frame(type, VAO, VBO, EBO, vertices, indices, verticesSize*sizeof(float), indicesSize*sizeof(unsigned int), CBO, color, colorsSize*sizeof(float));
    }
};

#endif /*SHAPES_H*/