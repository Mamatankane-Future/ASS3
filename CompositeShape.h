#ifndef COMPSHAPE_H
#define COMPSHAPE_H

#include "Shape.h"
#include "Matrix.h"
#include <math.h>
#include <glm/glm.hpp>


//Create compositeShape interface
class CompositeShape : public Shape{
    public:
        const static int CYLINDER_VERTICES = 15;
    protected:
        Shape** shapes;
        int numShapes;

        void draw(bool type=true){
            for(int i=0; i<numShapes; i++){
                shapes[i]->draw(type);
            }

        }

        CompositeShape(int numShapes) : numShapes(numShapes){}

        void scaleShape(glm::vec3 scale){
            IdentityMatrix t(4);
            t[0][0] = scale.x;
            t[1][1] = scale.y;
            t[2][2] = scale.z;

            this->transform(t);
        }

        void rotate(double cosValue, double sinValue, int index1, int index2, bool reverse = false) override {
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
            if (reverse) {
                rotate[index1][index2] = sinValue;
                rotate[index2][index1] = -sinValue;
            } else {
                rotate[index1][index2] = -sinValue;
                rotate[index2][index1] = sinValue;
            }

            Matrix transform = toOriginal * rotate * toOrigin;

            for(int i=0; i<numShapes; i++){
                moveScene(shapes[i]->vertices, shapes[i]->verticesSize, transform);
                moveScene(shapes[i]->origin, 3, transform);
            }

            moveScene(origin, 3, transform);
        }

        void transform(Matrix& scaleMatrix) {
            // Apply scaling transformation to each individual shape in the composite shape
            for (int i = 0; i < numShapes; ++i) {
                // Apply scaling transformation to vertices of the current shape
                moveScene(shapes[i]->vertices, shapes[i]->verticesSize, scaleMatrix);
                // Apply scaling transformation to origin of the current shape
                moveScene(shapes[i]->origin, 3, scaleMatrix);
            }

            // Apply scaling transformation to the origin of the composite shape itself
            moveScene(origin, 3, scaleMatrix);
        }


        void print(){
            for(int i=0; i<numShapes; i++){
                shapes[i]->print();
            }
        }

        virtual ~CompositeShape(){
            for(int i=0; i<numShapes;i++){
                delete shapes[i];
            }
            delete [] shapes;
        }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////small rounded flat-based table/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////


class roundedTable : public CompositeShape{

    public:
        roundedTable(glm::vec3 center, glm::vec3 scale): CompositeShape(3){

            origin[0] = center.x;
            origin[1] = center.y;
            origin[2] = center.z;

            float baseHeight = 0.05, neckHeight = 0.4, topHeight = 0.08;

            float tableBaseCenter[3] = {center[0],center[1],center[2]};
            float tableNeckCenter[3] = {center[0],center[1] + 0.2,center[2]};
            float tabletopCenter[3] = {center[0],center[1] + 0.5,center[2]};

            //apply scaling
           

            float tabletopColor[4] = {0.0f, 1.0f, 0.0f, 1.0f};
            float tableBaseColor[4] = {1.0f, 0.0f, 0.0f, 1.0f};
            float tableNeckColor[4] = {0.0f, 0.0f, 1.0f, 1.0f};

            //shape creation
            Shape *tableTop = new Cylinder(0.3,0.2,CompositeShape::CYLINDER_VERTICES,tabletopCenter,tabletopColor);
            Shape *tableNeck = new Cylinder(0.05,0.4,CompositeShape::CYLINDER_VERTICES,tableNeckCenter,tableNeckColor);
            Shape *tableBase = new Cylinder(0.2,0.1,CompositeShape::CYLINDER_VERTICES,tableBaseCenter,tableBaseColor);

            //add shapes to array
            shapes = new Shape * [numShapes]{tableTop, tableNeck, tableBase}; 

            scaleShape(scale);

        }

        virtual ~roundedTable(){}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////Hexagonal chair/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

class HexagonalChair : public CompositeShape{
    private:
        float height = 0.3, radius = 0.3;
    public:
        HexagonalChair(glm::vec3 center, glm::vec3 scale) : CompositeShape(1){
            origin[0] = center.x;
            origin[1] = center.y + height/2;
            origin[2] = center.z;

            float yellow[4] = {1,1,0,1};

            shapes = new Shape * [1]{new Cylinder(radius,height,6,origin,yellow)};

            origin[1] -= height/2;

            scaleShape(scale);
        }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////Dustbin/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
class Dustbin : public CompositeShape{
    private:
        float binHeight = 0.2, lidHeight = 0.15, binWidth = 0.2, lidWidth = 0.23;
        float binColor[4] = {0.5,0.5,0.5,1}, lidColor[4] = {0.2,0.2,0.2,1}, flapColor[4] = {0.7,0.7,0.7,1};
        float flapHeight = 0.1, flapWidth = 0.18, flapDepth = 0.01;
    
    public:
        Dustbin(glm::vec3 center, glm::vec3 scale) : CompositeShape(3){
            origin[0] = center.x;
            origin[1] = center.y + binHeight/2;
            origin[2] = center.z;

            float lidCenter[3] = {origin[0], origin[1] + lidWidth/2, origin[2]};
            float flapCenter[3] = {origin[0], origin[1] + lidWidth/2, origin[2] - lidWidth/2};

            Cuboid *bin = new Cuboid(binWidth, binHeight, binWidth, origin, binColor);
            Cuboid *lid = new Cuboid(lidWidth, lidHeight, lidWidth, lidCenter, lidColor);
            Cuboid *flap = new Cuboid(flapWidth, flapHeight, flapDepth, flapCenter, flapColor);

            origin[1] -= binHeight/2;

            shapes = new Shape * [numShapes]{bin,lid,flap};

            scaleShape(scale);

        }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////Sign/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

class Sign : public CompositeShape{
    private:
        float poleWidth = 0.05, signWidth = 0.15, depth = 0.05, poleLength = 0.5, signLength = 0.65;
        float poleColor[4] = {0,0,0,1}, signColor[4] = {1,1,1,1};
    public:
        Sign(glm::vec3 center, glm::vec3 scale) : CompositeShape(2){
            origin[0] = center.x;
            origin[1] = center.y + poleLength/2;
            origin[2] = center.z;

            float yellow[4] = {1,1,0,1};

            float signCenter[3] = {origin[0] + poleWidth/2, origin[1] + poleLength/2 + signLength/2, origin[2]};

            Shape *pole = new Cuboid(poleWidth, poleLength, depth, origin, poleColor);
            Shape *sign = new Cuboid(signWidth, signLength, depth, signCenter, signColor);

            shapes = new Shape * [numShapes]{pole,sign};

            origin[1] -= poleLength/2;

            scaleShape(scale);
        }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////Wall divider/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

class Divider : public CompositeShape{
    private:
        float width = 0.8, height = 1, depth = 0.1;
    public:
        Divider(glm::vec3 center, glm::vec3 scale) : CompositeShape(1){
            origin[0] = center.x;
            origin[1] = center.y + height/2;
            origin[2] = center.z;

            float white[4] = {1,1,1,1};

            shapes = new Shape * [1]{new Cuboid(width, height, depth, origin, white)};

            origin[1] -= height/2;

            scaleShape(scale);
        }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////Two-legged rounded table/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

class TwoLeggedRoundedTable : public CompositeShape{
    private:
        float neckHeight = 0.4, tableTopHeight = 0.1, baseHeight = 0.05;
        float neckRadius = 0.05, tableTopRadius = 0.3, baseWidth = neckRadius * 2;
        float baseDepth = 0.3;
        float black[4] = {0,0,0,1}, tableTopColor[4] = {0.96,0.87,0.7,1}; 
    public:
        TwoLeggedRoundedTable(glm::vec3 center, glm::vec3 scale) : CompositeShape(4){
            origin[0] = center.x;
            origin[1] = center.y;
            origin[2] = center.z;

            //centers
            float neckCenter[3] = {center.x, center.y + neckHeight/2 + baseHeight,center.z}; 
            float tableTopCenter[3] = {center.x, center.y + tableTopHeight/2 + baseHeight + neckHeight,center.z}; 
            float baseCenter[3] = {center.x, center.y + baseHeight/2, center.z}; //same centers but rotated about the y axis

            //shape creation
            Shape *base1 = new Cuboid(baseWidth, baseHeight, baseDepth, baseCenter, black);
            Shape *base2 = new Cuboid(baseWidth, baseHeight, baseDepth, baseCenter, black);
            Shape *neck = new Cylinder(neckRadius, neckHeight, CompositeShape::CYLINDER_VERTICES, neckCenter, black);
            Shape *tableTop = new Cylinder(tableTopRadius, tableTopHeight, CompositeShape::CYLINDER_VERTICES, tableTopCenter, tableTopColor);

            //shape rotation
            float theta = 45 * M_PI/180.0;
            base1->rotate(cos(theta), sin(theta), 0, 2);
            base2->rotate(cos(-theta), sin(-theta), 0, 2);

            // //shape translation
            // IdentityMatrix translateZ(4);
            // translateZ[2][3] = (center.z - baseDepth/2);
            // base1->moveScene(base1->vertices, base1->verticesSize, translateZ);


            //shape population
            shapes = new Shape * [numShapes]{base1, base2, neck, tableTop};

            scaleShape(scale);
        }
};
#endif