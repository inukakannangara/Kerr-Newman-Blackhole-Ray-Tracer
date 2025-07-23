#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <string>
#include <iostream>

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

int windowWidth = 1920;
int windowHeight = 1080;

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

bool isMousePressed = false;
float prevXPos = 0.0;
float prevYPos = 0.0;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos);

void handleInput(GLFWwindow* window);

Camera camera = Camera(vec3(0.0, -30.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, -1.0));
float cameraSensitivity = 0.5;

float a = 0.1f;
float m = 1.0f;
float Q = 0.5f;

const float G = 6.67430e-11;

int main(void)
{
    if (!glfwInit())
        return -1;

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Spacetime", NULL, NULL);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    if (!window){glfwTerminate(); return -1;}
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){return -1;}

    Shader shader = Shader("Shaders/VertexShader.glsl", "Shaders/FragmentShader.glsl"); 
    Texture texture2 = Texture("space_texture_3.jpeg", 2);

    float vertices[] = {
        -1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
    };
    unsigned int indices[] = { 0, 1, 2,2, 3, 0 };

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;      
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    float prevTime = 0.0;
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        handleInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Spacetime");
        ImGui::Text((std::to_string(int(io.Framerate)) +   " fps (" + std::to_string(int(1000.0/io.Framerate)) + " ms per frame)").c_str());
        ImGui::SliderFloat("camera sensitivity", &cameraSensitivity, 0.0, 1.0, NULL, NULL);

        float speed = length(camera.getVelocity());
        std::string speedString = std::to_string(speed);

        ImGui::Text(("Speed: " + speedString + "c").c_str());
        ImGui::SliderFloat("a", &a, -2.0, 2.0, NULL, NULL);
        ImGui::SliderFloat("m", &m, 0.0, 5.0, NULL, NULL);
        ImGui::SliderFloat("Q", &Q, -5.0, 5.0, NULL, NULL);
        ImGui::End();

        ImGui::Render();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.setUniformFloat("windowWidth", float(windowWidth));
        shader.setUniformFloat("windowHeight", float(windowHeight));

        shader.setUniformVec3("cameraVelocity", camera.getVelocity());

        texture2.toShader(&shader);

        shader.setUniformFloat("a", a);
        shader.setUniformFloat("m", m);
        shader.setUniformFloat("Q", Q);

        camera.toShader(&shader);

        float glfwTime = float (glfwGetTime());
        float dt = glfwTime - prevTime;
        prevTime = glfwTime;

        camera.update(dt);

        shader.use();

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    shader.deleteShader();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        isMousePressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        isMousePressed = false;
    }
}

void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (isMousePressed)
    {
        float dx = float(xPos) - prevXPos;
        float dy = float(yPos) - prevYPos;

        camera.orbit(dx/10.0f, dy/10.0f);
    }
    prevXPos = float(xPos);
    prevYPos = float(yPos);
}

void handleInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.moveForward(0.02f * cameraSensitivity);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.moveForward(-0.02f * cameraSensitivity);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.moveRight(0.02f * cameraSensitivity);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.moveRight(-0.02f * cameraSensitivity);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera.moveUp(0.02f * cameraSensitivity);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.moveUp(-0.02f * cameraSensitivity); 
    }
}
