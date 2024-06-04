#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Position(position), WorldUp(up), Yaw(yaw), Pitch(pitch), MovementSpeed(2.5f) {
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(const int direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime/10.0f;
    if (direction == 1) {
        Position += Front * velocity; 
    }
    if (direction == 2) {
        Position -= Front * velocity; 
    }
    if (direction == 3) {
        Position -= Right * velocity;
    }
    if (direction == 4) {
        Position += Right * velocity; 
    }
    if (direction == 5) {
        Position += Up * velocity; 
    }
    if (direction == 6) {
        Position -= Up * velocity; 
    }

    
    float rotationSpeed = 1.0f;

    if (direction == 7) {
        RotateX(rotationSpeed);
    }
    if (direction == 8) {
        RotateY(rotationSpeed);
    }
    if (direction == 9) {
        RotateZ(rotationSpeed);
    }


    if (direction == -7) {
        RotateX(-rotationSpeed); 
    }
    if (direction == -8) {
        RotateY(-rotationSpeed); 
    }
    if (direction == -9) {
        RotateZ(-rotationSpeed); 
    }
}

void Camera::RotateX(float angle) {
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
    Front = glm::normalize(glm::vec3(rotation * glm::vec4(Front, 1.0f)));
    Up = glm::normalize(glm::vec3(rotation * glm::vec4(Up, 1.0f)));
    Right = glm::normalize(glm::cross(Front, WorldUp));
}

void Camera::RotateY(float angle) {
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    Front = glm::normalize(glm::vec3(rotation * glm::vec4(Front, 1.0f)));
    Right = glm::normalize(glm::vec3(rotation * glm::vec4(Right, 1.0f)));
    Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::RotateZ(float angle) {
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
    Right = glm::normalize(glm::vec3(rotation * glm::vec4(Right, 1.0f)));
    Up = glm::normalize(glm::vec3(rotation * glm::vec4(Up, 1.0f)));
    Front = glm::normalize(glm::cross(Up, Right));
}


void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}


