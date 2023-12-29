// Include GLFW
#include <glfw3.h>
extern GLFWwindow* window; 
#include <iostream>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

// Matrices for view and projection
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix() {
    return ViewMatrix;
}

glm::mat4 getProjectionMatrix() {
    return ProjectionMatrix;
}

// Initial camera position and orientation
glm::vec3 position = glm::vec3(10, 10, 0);
glm::vec3 origin = glm::vec3(0, 0, 0);
glm::vec3 up = glm::vec3(0, 0, 1);

// Spherical coordinates for camera orientation
float phi = 0.0f;  // Azimuth angle
float deltaphi = 0.1f;
float theta = 1.5708f;  // Polar angle (90 degrees in radians)
float deltatheta = 0.1f;

// Initial Field of View
float initialFoV = 45.0f;

// Camera radius and speed
float radius = 10.0f;
float deltar = 1.0f;
float speed = 5.0f;

void computeMatricesFromInputs() {
    // Time-based camera movement
    static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Adjust azimuth angle (phi) based on user input
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        phi -= deltaphi * deltaTime * speed;
        // Wrap phi to stay within [0, 2π]
        if (phi < 0.0f) {
            phi = 2 * 3.14159265f;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        phi += deltaphi * deltaTime * speed;
        // Wrap phi to stay within [0, 2π]
        if (phi >= 2 * 3.14159265f) {
            phi = 0;
        }
    }

    // Adjust polar angle (theta) based on user input
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        theta -= deltatheta * deltaTime * speed;
        // Ensure theta stays within [0.01, π] to avoid camera flipping
        if (theta < 0.01f) {
            theta = 0.01f;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        theta += deltatheta * deltaTime * speed;
        // Ensure theta stays within [0.01, π] to avoid camera flipping
        if (theta > 3.1315f) {
            theta = 3.1315f;
        }
    }

    // Adjust camera radius based on user input
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        radius -= deltar * deltaTime * speed;
        // Ensure radius is non-negative
        if (radius < 0.0f) {
            radius = 0.0f;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        radius += deltar * deltaTime * speed;
    }

    // Update camera position based on spherical coordinates
    position.x = radius * sin(theta) * cos(phi);
    position.y = radius * sin(theta) * sin(phi);
    position.z = radius * cos(theta);

    // Calculate the Field of View
    float FoV = initialFoV;

    // Calculate the projection matrix
    ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);

    // Calculate the view matrix (camera orientation)
    ViewMatrix = glm::lookAt(position, origin, up);

    lastTime = currentTime;  // Update the last frame time
}


bool checkLightingToggle() {
    static bool lastState = false; // Static variable to store the last state
    bool currentState = (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS); // Check the current state

    // If the key was not pressed in the last frame and is pressed now, return true
    if (!lastState && currentState) {
        lastState = currentState; // Update the last state
        return true;
    }

    lastState = currentState; // Update the last state
    return false;
}
