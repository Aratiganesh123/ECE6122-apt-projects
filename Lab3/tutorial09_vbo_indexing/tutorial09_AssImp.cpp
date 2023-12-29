// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include<iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

/**
 * Initializes the GLFW library, sets window hints, and creates a GLFW window.
 * This function should be called at the beginning of your program to set up
 * the GLFW environment.
 *
 * @return True if initialization is successful, false otherwise.
 */
bool initialize() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    window = glfwCreateWindow(1024, 768, "Tutorial 09 - Loading with AssImp", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        return false;
    }

    glfwMakeContextCurrent(window);
    return true;
}



/**
 * Initializes the GLEW library to access OpenGL extensions and features.
 * This function should be called after the GLFW context has been created to
 * set up GLEW for OpenGL rendering.
 *
 * @return True if GLEW initialization is successful, false otherwise.
 */
bool initializeGLEW() {
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return false;
    }
    return true;
}


/**
 * Setup the OpenGL environment and GLFW input modes.
 * This function configures GLFW settings, including key input and cursor control,
 * and sets up OpenGL features like depth testing.
 */
void setupOpenGL() {
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // glEnable(GL_CULL_FACE);
}


/**
 * Retrieve uniform and attribute locations in an OpenGL shader program.
 *
 * @param programID OpenGL shader program ID.
 * @param MatrixID Location of the MVP matrix uniform.
 * @param ViewMatrixID Location of the view matrix uniform.
 * @param ModelMatrixID Location of the model matrix uniform.
 * @param vertexPosition_modelspaceID Location of position attribute.
 * @param vertexUVID Location of UV attribute.
 * @param vertexNormal_modelspaceID Location of normal attribute.
 * @param TextureID Location of texture sampler uniform.
 * @param LightID Location of light position uniform.
 * @param justGreenID Location of "justGreen" uniform (for shader toggling).
 * @param lightingToggleID Location of "toggleLighting" uniform.
 */

void getUniformAttributeLocations(GLuint programID, GLuint &MatrixID, GLuint &ViewMatrixID, GLuint &ModelMatrixID,
    GLuint &vertexPosition_modelspaceID, GLuint &vertexUVID, GLuint &vertexNormal_modelspaceID,
    GLuint &TextureID, GLuint &LightID, GLuint &justGreenID, GLuint &lightingToggleID) {
    // Retrieve the locations of various uniform and attribute variables
    MatrixID = glGetUniformLocation(programID, "MVP");
    ViewMatrixID = glGetUniformLocation(programID, "V");
    ModelMatrixID = glGetUniformLocation(programID, "M");
    vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    vertexUVID = glGetAttribLocation(programID, "vertexUV");
    vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");
    TextureID = glGetUniformLocation(programID, "myTextureSampler");
    LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    justGreenID = glGetUniformLocation(programID, "justGreen");
    lightingToggleID = glGetUniformLocation(programID, "toggleLighting");
}


/**
 * Set up Vertex Buffer Objects (VBOs) for an OBJ file model.
 *
 * This function creates and initializes VBOs for vertex positions, UV coordinates, normals, and element indices.
 *
 * @param vertexbuffer OpenGL VBO ID for vertex positions.
 * @param uvbuffer OpenGL VBO ID for UV coordinates.
 * @param normalbuffer OpenGL VBO ID for normal vectors.
 * @param elementbuffer OpenGL VBO ID for element indices.
 * @param indexed_vertices A vector of vertex positions.
 * @param indexed_uvs A vector of UV coordinates.
 * @param indexed_normals A vector of normal vectors.
 * @param indices A vector of element indices.
 */
void setupBuffers(GLuint &vertexbuffer, GLuint &uvbuffer, GLuint &normalbuffer, GLuint &elementbuffer,
    const std::vector<glm::vec3> &indexed_vertices, const std::vector<glm::vec2> &indexed_uvs,
    const std::vector<glm::vec3> &indexed_normals, const std::vector<unsigned short> &indices)
 {
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
}

/**
 * Update model matrices for a set of objects with translations and rotations.
 *
 * This function calculates and returns a vector of model matrices, each representing a transformation of an object.
 * Each object is translated along the X and Z axes and rotated in different ways.
 *
 * @param translateDistx The distance of translation along the X-axis for each object.
 * @param translateDistz The distance of translation along the Z-axis for each object.
 * @return A vector of glm::mat4 matrices representing model transformations.
 */
std::vector<glm::mat4> updateModelMatrices(float translateDistx , float translateDistz) {
    
    std::vector<glm::mat4> modelMatrices(4);

    modelMatrices[0] = glm::mat4(1.0);
    modelMatrices[1] = glm::mat4(1.0);
    modelMatrices[2] = glm::mat4(1.0);
    modelMatrices[3] = glm::mat4(1.0);

    modelMatrices[0] = glm::translate(modelMatrices[0], glm::vec3(translateDistx, 0.0f, translateDistz));
    modelMatrices[0] = glm::rotate(modelMatrices[0], glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrices[0] = glm::rotate(modelMatrices[0], glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    modelMatrices[1] = glm::translate(modelMatrices[1], glm::vec3(-translateDistx, 0.0f, translateDistz));
    modelMatrices[1] = glm::rotate(modelMatrices[1], glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrices[1] = glm::rotate(modelMatrices[1], glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    modelMatrices[2] = glm::translate(modelMatrices[2], glm::vec3(0.0f, translateDistx, translateDistz));
    modelMatrices[2] = glm::rotate(modelMatrices[2], glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrices[2] = glm::rotate(modelMatrices[2], glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    modelMatrices[3] = glm::translate(modelMatrices[3], glm::vec3(0.0f, -translateDistx, translateDistz));
    modelMatrices[3] = glm::rotate(modelMatrices[3], glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    return modelMatrices;
}

/**
 * Calculate Model-View-Projection (MVP) matrices for a set of objects.
 *
 * This function takes the projection matrix, view matrix, and a vector of model matrices as input.
 * It calculates the MVP matrix for each object using these matrices and returns them in a vector.
 *
 * @param projectionMatrix The projection matrix for the scene.
 * @param viewMatrix The view matrix representing the camera's position and orientation.
 * @param modelMatrices A vector of model matrices for the objects.
 * @return A vector of glm::mat4 matrices representing the MVP transformations.
 */
std::vector<glm::mat4> calculateMVPs(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const std::vector<glm::mat4>& modelMatrices) {
    std::vector<glm::mat4> MVPs(4);
    for (int i = 0; i < 4; i++) {
        MVPs[i] = projectionMatrix * viewMatrix * modelMatrices[i];
    }
    return MVPs;
}


/**
 * Set up vertex buffer for a square with specified size.
 *
 * This function generates vertex data for a square with positions and stores it in a vertex buffer.
 *
 * @param vertexbuffer Reference to the vertex buffer object (VBO).
 * @param size The size of the square.
 */
void setupSquareBuffers(GLuint &vertexbuffer, const float size) {
    // Define the vertices for a square with only positions using 6 vertices
    std::vector<glm::vec3> squareVertices = {
        glm::vec3(-size, -size, 0.0f),
        glm::vec3(size, -size, 0.0f),
        glm::vec3(size, size, 0.0f),
        glm::vec3(-size, -size, 0.0f),
        glm::vec3(size, size, 0.0f),
        glm::vec3(-size, size, 0.0f)
    };

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, squareVertices.size() * sizeof(glm::vec3), &squareVertices[0], GL_STATIC_DRAW);
}


/**
 * Clean up OpenGL resources and terminate GLFW.
 *
 * @param programID The shader program ID.
 * @param vertexbuffer Vertex buffer ID.
 * @param uvbuffer Texture UV buffer ID.
 * @param normalbuffer Normal buffer ID.
 * @param elementbuffer Element buffer ID.
 * @param Texture Texture ID.
 */
void cleanup(GLuint programID, GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer, GLuint elementbuffer, GLuint Texture) {
    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &Texture);
    glfwTerminate();
}

/**
 * The main rendering loop.
 *
 * Initializes GLFW, GLEW, OpenGL settings, shaders, and objects, then enters the rendering loop.
 * Handles camera movement, lighting toggling, and rendering objects.
 *
 * @return An integer status code.
 */
int main(void) {
    // Initialize GLFW and GLEW, exit if failed
    if (!initialize() || !initializeGLEW()) {
        std::cout << "Exit > " << std::endl;
        return -1;
    }

    // Setup OpenGL settings and shaders
    setupOpenGL();

    // Create and compile the GLSL program from vertex and fragment shaders
    GLuint programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");

    // Variables for uniform and attribute locations
    GLuint MatrixID, ViewMatrixID, ModelMatrixID, vertexPosition_modelspaceID, vertexUVID, vertexNormal_modelspaceID,
        TextureID, LightID, justGreenID, lightingToggleID;

    // Retrieve locations for uniforms and attributes
    getUniformAttributeLocations(programID, MatrixID, ViewMatrixID, ModelMatrixID, vertexPosition_modelspaceID, vertexUVID,
                                 vertexNormal_modelspaceID, TextureID, LightID, justGreenID, lightingToggleID);

    // Load a texture
    GLuint Texture = loadDDS("uvmap.DDS");

    // Variables for model loading and lighting
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
    glm::vec3 lightPos = glm::vec3(4,4,4);
    bool res = loadAssImp("suzanne.obj", indices, indexed_vertices, indexed_uvs, indexed_normals);

    // Buffers for object rendering
    GLuint vertexbuffer, uvbuffer, normalbuffer, elementbuffer, vertexbufferSquare;
    setupBuffers(vertexbuffer, uvbuffer, normalbuffer, elementbuffer, indexed_vertices, indexed_uvs, indexed_normals, indices);
    setupSquareBuffers(vertexbufferSquare, 5.0f);
    glUseProgram(programID);

    double lastTime = glfwGetTime();
    int nbFrames = 0;
    bool lightingState = false;

    do {
        // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0) {
            // If more than 1 second has passed, print frame time
            printf("%f ms/frame\n", 1000.0 / double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use the shader program
        glUseProgram(programID);

        // Compute camera matrices and check for lighting toggle
        computeMatricesFromInputs();
        // Inside the main loop
        // Before rendering objects
        bool check = checkLightingToggle();
        if (check) {
            lightingState = !lightingState;
            std::cout << "Toggled lighting " << (lightingState ? "on" : "off") << std::endl;
        }

        // Inside the rendering loop
        glUniform1i(lightingToggleID, lightingState);

                    
        // Get projection and view matrices and set justGreen to 1
        glm::mat4 projectionMatrix = getProjectionMatrix();
        glm::mat4 viewMatrix = getViewMatrix();
        glUniform1i(justGreenID, 1);

        // Set the square's position and orientation
        glm::mat4 squareModelMatrix = glm::mat4(1.0f);
        squareModelMatrix = glm::translate(squareModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 squareMVP = projectionMatrix * viewMatrix * squareModelMatrix;
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &squareModelMatrix[0][0]);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &squareMVP[0][0]);
        glEnableVertexAttribArray(vertexPosition_modelspaceID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferSquare);
        glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(vertexPosition_modelspaceID);

        // Reset justGreen to 0
        glUniform1i(justGreenID, 0);

        // Update model matrices and calculate MVPs
        std::vector<glm::mat4> modelMatrices = updateModelMatrices(1.8f, 1.0f);
        std::vector<glm::mat4> MVPs = calculateMVPs(projectionMatrix, viewMatrix, modelMatrices);

        for (int i = 0; i < MVPs.size(); i++) {
            // Use the specified shader program
            glUseProgram(programID);

            // Set uniform values
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPs[i][0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &modelMatrices[i][0][0]);
            glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);
            glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

            // Bind texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Texture);
            glUniform1i(TextureID, 0);

            // Bind vertex and attribute buffers
            glEnableVertexAttribArray(vertexPosition_modelspaceID);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glEnableVertexAttribArray(vertexUVID);
            glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
            glVertexAttribPointer(vertexUVID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

            glEnableVertexAttribArray(vertexNormal_modelspaceID);
            glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
            glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

            // Bind element buffer and draw
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);

            // Disable attribute arrays
            glDisableVertexAttribArray(vertexPosition_modelspaceID);
            glDisableVertexAttribArray(vertexUVID);
            glDisableVertexAttribArray(vertexNormal_modelspaceID);
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);

    // Cleanup VBO, shader, and exit
    cleanup(programID, vertexbuffer, uvbuffer, normalbuffer, elementbuffer, Texture);

    return 0;
}
