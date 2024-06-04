 #ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);


    glm::mat4 GetViewMatrix() const;
    void ProcessKeyboard(const int direction, float deltaTime);

    glm::vec3 getPosition() const { return Position; }

    void RotateZ(float angle);
    void RotateX(float angle);
    void RotateY(float angle);
    void drawFilter();

    glm::vec3 Front;
    

private:
    void updateCameraVectors();

    glm::vec3 Position;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
};

#endif // CAMERA_H
