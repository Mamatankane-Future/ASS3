#include "FloorPlan.h"
#include "CompositeShape.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>


FloorPlan::FloorPlan(const std::string& filename)
    : shapes(nullptr), shapeCount(0), shapeCapacity(0) {
    readFile(filename);
}

FloorPlan::~FloorPlan() {
    for (int i = 0; i < shapeCount; ++i) {
        delete shapes[i];
    }
    delete[] shapes;
}

void FloorPlan::draw(bool type) {
    for (int i = 0; i < shapeCount; ++i) {
        shapes[i]->draw(type);
    }
}

void FloorPlan::readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        cout << line << endl;
        char delimiter = '#';
        std::vector<std::string> parts = split(line, delimiter);
        int type = std::stoi(parts[0]);
        std::vector<std::string> positionParts = split(parts[1], ',');
        std::vector<std::string> sizeParts = split(parts[2], ',');
        glm::vec3 position(std::stof(positionParts[0]), std::stof(positionParts[1]), std::stof(positionParts[2]));
        glm::vec3 size(std::stof(sizeParts[0]), std::stof(sizeParts[1]), std::stof(sizeParts[2]));
        createShape(type, position, size);
        cout << "Type: " << type << " Position: " << position.x << " " << position.y << " " << position.z << " Size: " << size.x << " " << size.y << " " << size.z << endl;
    }

    file.close();
}

std::vector<std::string> FloorPlan::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}


void FloorPlan::createShape(char type, const glm::vec3& position, const glm::vec3& size) {
    if (shapeCount == shapeCapacity) {
        expandShapeArray();
    }
    switch (type) {
        case 1:
            shapes[shapeCount++] = new roundedTable(position, size);
            break;
        default:
            std::cerr << "Unknown shape type: " << type << std::endl;
    }
}

void FloorPlan::expandShapeArray() {
    int newCapacity = shapeCapacity == 0 ? 1 : shapeCapacity * 2;
    Shape** newShapes = new Shape*[newCapacity];
    if (shapes) {
        std::memcpy(newShapes, shapes, shapeCount * sizeof(Shape*));
        delete[] shapes;
    }
    shapes = newShapes;
    shapeCapacity = newCapacity;
}
