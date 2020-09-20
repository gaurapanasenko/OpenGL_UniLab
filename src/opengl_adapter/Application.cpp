#include "Application.h"

void LAM::AppInit() {
    glfwInit();

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    glewExperimental = true;
    glewInit();
}

void LAM::AppClose() {
    glfwTerminate();
}