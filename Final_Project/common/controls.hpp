#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <vector>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Global or static variable to track the random movement state
static bool isRandomMovementActive = false;

class objectTransformation{

    private: 
        glm::mat4 modelMatrix;
        glm::vec3 lastTranslation;
        glm::vec3 rotationAxis;
        glm::vec3 velocity;
        float minSep;

    public: 

        objectTransformation();
        objectTransformation(const glm::mat4& initModelMatrix, const glm::vec3& initLastTranslation,const glm::vec3& initRotationAxis , float minSep);
        void setModelMatrix(const glm::mat4& newModelMatrix);
        glm::mat4 getModelMatrix();
        
        void applyRotation(float angle, const glm::vec3& axis);
        void applyTranslation(const glm::vec3& translation);
        
        
        glm::vec3 getCenter();
        glm::vec3 getLastTranslation() const;
        glm::vec3 getRotAxis() const;
        void setVelocity(const glm::vec3& newVelocity);
        glm::vec3 getVelocity() const;
        void update(float deltaTime);
        void checkAndRespondToCollision(objectTransformation& otherObj);     
};


bool checkAndUpdateModelMatrix(float translateDistx , float translateDistz , std::vector<objectTransformation>& objs , float deltaTime);
void applyDefaultTransformation(float translateDistx , float translateDistz , std::vector<objectTransformation>& objs);
void applyRandomTransformations(std::vector<objectTransformation>& objs , float deltaTime);


void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
bool checkLightingToggle();
bool isgKeyPressed();

#endif // CONTROLS_HPP
