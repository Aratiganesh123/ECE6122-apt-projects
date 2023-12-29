// Include GLFW
#include <glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.
#include <iostream>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
#include <thread>

#include "controls.hpp"


// Matrices for view and projection
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

// Global or static variable
static double lastFrameTime = glfwGetTime();

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
static double lastTime = glfwGetTime();
static float deltaTime = 0.0f;


void computeMatricesFromInputs() {

    double currentTime = glfwGetTime();
    deltaTime = float(currentTime - lastTime);

    // Adjust azimuth angle (phi) based on user input
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { //left
        phi -= deltaphi * deltaTime * speed;
        // Wrap phi to stay within [0, 2π]
        if (phi < 0.0f) {
            phi = 2 * 3.14159265f;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { //right
        phi += deltaphi * deltaTime * speed;
        // Wrap phi to stay within [0, 2π]
        if (phi >= 2 * 3.14159265f) {
            phi = 0;
        }
    }

    // Adjust polar angle (theta) based on user input
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) { // u
        theta -= deltatheta * deltaTime * speed;
        // Ensure theta stays within [0.01, π] to avoid camera flipping
        if (theta < 0.01f) {
            theta = 0.01f;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { // d
        theta += deltatheta * deltaTime * speed;
        // Ensure theta stays within [0.01, π] to avoid camera flipping
        if (theta > 3.1315f) {
            theta = 3.1315f;
        }
    }

    // Adjust camera radius based on user input
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { //up 
        radius -= deltar * deltaTime * speed;
        // Ensure radius is non-negative
        if (radius < 0.0f) {
            radius = 0.0f;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { //down
        radius += deltar * deltaTime * speed;
    }

    if (isgKeyPressed()) {
        isRandomMovementActive = true;
        std::cout << "Random Movement Activated" << std::endl;
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

bool isgKeyPressed() {
    return glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS;
}


/*********************************************** Methods related to object Transformation **********************************************************/

objectTransformation::objectTransformation() 
        : modelMatrix(1.0f), 
          lastTranslation(0.0f, 0.0f, 0.0f), 
          rotationAxis(0.0f, 0.0f, 1.0f) ,
          minSep(0.0f)
{

}

objectTransformation::objectTransformation(const glm::mat4& initModelMatrix, const glm::vec3& initLastTranslation,const glm::vec3& initRotationAxis , float minSep)
        : modelMatrix(initModelMatrix),
          lastTranslation(initLastTranslation),
          rotationAxis(initRotationAxis),
          minSep(minSep)
{

}
glm::mat4 objectTransformation::getModelMatrix() {
    return modelMatrix;
}
void objectTransformation::setModelMatrix(const glm::mat4& newModelMatrix) {
    modelMatrix = newModelMatrix;
}

void objectTransformation::applyRotation(float angle, const glm::vec3& axis) {
    modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), axis);
}

void objectTransformation::applyTranslation(const glm::vec3& translation) {
    modelMatrix = glm::translate(modelMatrix, translation);
    lastTranslation = translation;
}

glm::vec3 objectTransformation::getLastTranslation() const {
    return lastTranslation;
}


glm::vec3 objectTransformation::getCenter() {
    // The last column of the model matrix contains the translation
    return glm::vec3(modelMatrix[3]);
}


glm::vec3 objectTransformation::getRotAxis() const
{
    return rotationAxis;
}


void objectTransformation::setVelocity(const glm::vec3& newVelocity) {
    velocity = newVelocity;
}

glm::vec3 objectTransformation::getVelocity() const {
    return velocity;
}

void objectTransformation::update(float deltaTime) {
    glm::vec3 newPosition = glm::vec3(modelMatrix[3]) + velocity * deltaTime;
    // X-axis
    if (newPosition.x < -10.0f + minSep) { 
        newPosition.x = -10.0f + minSep; 
        velocity.x = -velocity.x * 0.9f; 
    } else if (newPosition.x > 10.0f - minSep) { 
        newPosition.x = 10.0f - minSep;
        velocity.x = -velocity.x * 0.9f; 
    }

    // Y-axis
    if (newPosition.y < -10.0f + minSep) {
        newPosition.y = -10.0f + minSep;
        velocity.y = -velocity.y * 0.9f; 
    } else if (newPosition.y > 10.0f - minSep) { 
        newPosition.y = 10.0f - minSep; 
        velocity.y = -velocity.y * 0.9f; 
    }

    // Z-axis (Floor and Ceiling)
    if (newPosition.z < 0.0f + minSep) { 
        newPosition.z = 0.0f + minSep; 
        velocity.z = -velocity.z * 0.8f; 
    } else if (newPosition.z > 10.0f - minSep) { 
        newPosition.z = 10.0f - minSep; 
        velocity.z = -velocity.z * 0.8f; 
    }

    float rotationSpeed = 30.0f; 
    float angle = rotationSpeed * deltaTime;

    // Apply rotation around the object's own axis
    modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), rotationAxis) * modelMatrix;

    // Update the model matrix with the new position
    modelMatrix[3] = glm::vec4(newPosition, 1.0f);
}

void objectTransformation::checkAndRespondToCollision(objectTransformation& otherObj) {
    glm::vec3 pos1 = glm::vec3(this->modelMatrix[3]);
    glm::vec3 pos2 = glm::vec3(otherObj.getModelMatrix()[3]);
    glm::vec3 diff = pos1 - pos2;
    float dist = glm::length(diff);

    if (dist < 2 * this->minSep) { // Collision detected
        // Reverse and dampen the velocities
        this->velocity = -this->velocity * 1.0f;
        otherObj.setVelocity(-otherObj.getVelocity() * 1.0f);
    }
}

/*********************************************** Methods related to perform Transformation **********************************************************/

void applyDefaultTransformation(float translateDistx, float translateDistz, std::vector<objectTransformation>& objs) {
    // Ensure there are at least 4 objects
    if (objs.size() < 4) {
        return; // or handle the error as appropriate
    }
    // Transformation for object 0
    glm::mat4 modelMatrix0 = glm::mat4(1.0f);
    modelMatrix0 = glm::translate(modelMatrix0, glm::vec3(translateDistx, 0.0f, translateDistz));
    modelMatrix0 = glm::rotate(modelMatrix0, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix0 = glm::rotate(modelMatrix0, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    objs[0].setModelMatrix(modelMatrix0);

    // Transformation for object 1
    glm::mat4 modelMatrix1 = glm::mat4(1.0f);
    modelMatrix1 = glm::translate(modelMatrix1, glm::vec3(-translateDistx, 0.0f, translateDistz));
    modelMatrix1 = glm::rotate(modelMatrix1, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix1 = glm::rotate(modelMatrix1, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    objs[1].setModelMatrix(modelMatrix1);

    // Transformation for object 2
    glm::mat4 modelMatrix2 = glm::mat4(1.0f);
    modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0.0f, translateDistx, translateDistz));
    modelMatrix2 = glm::rotate(modelMatrix2, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix2 = glm::rotate(modelMatrix2, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    objs[2].setModelMatrix(modelMatrix2);

    // Transformation for object 3
    glm::mat4 modelMatrix3 = glm::mat4(1.0f);
    modelMatrix3 = glm::translate(modelMatrix3, glm::vec3(0.0f, -translateDistx, translateDistz));
    modelMatrix3 = glm::rotate(modelMatrix3, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    objs[3].setModelMatrix(modelMatrix3);
}


void updateObjectTransformation(objectTransformation& obj, float deltaTime) {
    // Individualized update logic for each object
    obj.update(deltaTime);
}

void applyRandomTransformations(std::vector<objectTransformation>& objs, float deltaTime) {
    std::vector<std::thread> threads;

    // Create a thread for each object
    for (auto& obj : objs) {
        threads.emplace_back(updateObjectTransformation, std::ref(obj), deltaTime);
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    // Handle collisions (this part should remain single-threaded to avoid complexity)
    for (auto& obj1 : objs) {
        for (auto& obj2 : objs) {
            if (&obj1 != &obj2) {
                obj1.checkAndRespondToCollision(obj2);
            }
        }
    }
}

bool checkAndUpdateModelMatrix(float translateDistx, float translateDistz, std::vector<objectTransformation>& objs , float deltaTime) {
    if (isRandomMovementActive) {
        applyRandomTransformations(objs , deltaTime);
        return true;
    } else {
        applyDefaultTransformation(translateDistx, translateDistz, objs);
    }

    return false;
}

