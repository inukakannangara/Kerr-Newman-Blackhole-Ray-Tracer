#include "Camera.h"

Camera::Camera(vec3 origin, vec3 forward, vec3 right, vec3 up)
{
    this->origin = vec3(origin.x, origin.y, origin.z);
    this->forward = normalize(vec3(forward.x, forward.y, forward.z));
    this->right = normalize(vec3(right.x, right.y, right.z));
    this->up = normalize(vec3(up.x, up.y, up.z));
    velocity = vec3(0.0);
    acceleration = vec3(0.0);
}

void Camera::moveForward(float t)
{
    origin = origin + t * forward;
}

void Camera::moveRight(float t)
{
    origin = origin + t * right;
}

void Camera::moveUp(float t)
{
    origin = origin + t * up;
}

void Camera::orbit(float dx, float dy)
{
    float angleX = glm::radians(dx);
    float angleY = glm::radians(dy);

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angleX, up) * glm::rotate(glm::mat4(1.0f), angleY, right);

    forward = glm::vec3(rotationMatrix * glm::vec4(forward, 0.0f));
    right = glm::vec3(rotationMatrix * glm::vec4(right, 0.0f));
    up = glm::vec3(rotationMatrix * glm::vec4(up, 0.0f));
}

void Camera::setAcceleration(vec3 acceleration)
{
    this->acceleration = acceleration;
}

void Camera::accelerate(vec3 acceleration)
{
    velocity = velocity + acceleration;
}

void Camera::update(float dt)
{
    velocity = velocity + acceleration * dt;
    origin = origin + velocity * dt;
}

vec3 Camera::getOrigin()
{
    return origin;
}

vec3 Camera::getForward()
{
    return forward;
}

vec3 Camera::getRight()
{
    return right;
}

vec3 Camera::getUp()
{
    return up;
}

vec3 Camera::getVelocity()
{
    return velocity;
}

void Camera::toShader(Shader* shaderPtr)
{
    Shader shader = *shaderPtr;
    shader.setUniformVec3("cameraOrigin", origin);
    shader.setUniformVec3("cameraForward", forward);
    shader.setUniformVec3("cameraRight", right);
    shader.setUniformVec3("cameraUp", up);
}
