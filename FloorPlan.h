#ifndef FLOORPLAN_H
#define FLOORPLAN_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

class Shape;

class FloorPlan {
public:
    FloorPlan(const std::string& filename);
    ~FloorPlan();

    void draw(bool type);
    std::vector<std::string> split(const std::string& s, char delimiter);

private:
    Shape** shapes;
    int shapeCount;
    int shapeCapacity;

    void readFile(const std::string& filename);
    void createShape(int type, const glm::vec3& position, const glm::vec3& size);
    void expandShapeArray();
};

#endif // FLOORPLAN_H
