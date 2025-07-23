#pragma once
#include "glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/transform.hpp"
#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace glm;

class Camera
{
private:
	vec3 origin;
	vec3 forward;
	vec3 right;
	vec3 up;

	vec3 velocity;
	vec3 acceleration;

public:
	Camera(vec3 origin, vec3 forward, vec3 right, vec3 up);

	void moveForward(float t);
	void moveRight(float t);
	void moveUp(float t);

	void orbit(float dx, float dy);

	void setAcceleration(vec3 acceleration);
	void accelerate(vec3 acceleration);
	void update(float dt);

	vec3 getOrigin();
	vec3 getForward();
	vec3 getRight();
	vec3 getUp();
	vec3 getVelocity();

	void toShader(Shader* shader);
};

