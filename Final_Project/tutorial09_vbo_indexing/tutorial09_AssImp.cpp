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
#include <glm/gtc/type_ptr.hpp>
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
    GLuint &TextureID, GLuint &LightID, GLuint &justGreenID, GLuint &applyInternalLightID) {
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
    applyInternalLightID = glGetUniformLocation(programID, "applyInternalLight");
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
std::vector<glm::mat4> calculateMVPs(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, std::vector<objectTransformation>& objs) {
    std::vector<glm::mat4> MVPs(objs.size());
    for (size_t i = 0; i < objs.size(); i++) {
        MVPs[i] = projectionMatrix * viewMatrix * objs[i].getModelMatrix();
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
void setupSquareBuffers(GLuint &vertexbuffer, GLuint &uvbufferSquare,  GLuint &elementbufferSquare , const float size) {
    
    // Define the vertices for a square with only positions using 6 vertices
        std::vector<glm::vec3> squareVertices = {
        glm::vec3(-size, -size, 0.0f), // Bottom-left
        glm::vec3(size, -size, 0.0f),  // Bottom-right
        glm::vec3(size, size, 0.0f),   // Top-right
        glm::vec3(-size, size, 0.0f)   // Top-left
    };

    static const GLfloat g_uv_buffer_data[] = {
        0.0f, 0.0f, // Bottom-left corner
        1.0f, 0.0f, // Bottom-right corner
        1.0f, 1.0f, // Top-right corner
        0.0f, 1.0f  // Top-left corner
    };

    static const GLuint indices[] = {
        0, 1, 2, // First triangle (bottom-left, bottom-right, top-right)
        2, 3, 0  // Second triangle (top-right, top-left, bottom-left)
    };

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, squareVertices.size() * sizeof(glm::vec3), &squareVertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbufferSquare);
    glBindBuffer(GL_ARRAY_BUFFER, uvbufferSquare);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &elementbufferSquare);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbufferSquare);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

}


void setupCubeBuffers(GLuint &cubeVBO, GLuint &cubeColorBuffer) {
   // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	static const GLfloat g_vertex_buffer_data[] = { 
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f
	};

	// One color for each vertex. They were generated randomly.
	static const GLfloat g_color_buffer_data[] = { 
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f,
		0.822f,  0.569f,  0.201f,
		0.435f,  0.602f,  0.223f,
		0.310f,  0.747f,  0.185f,
		0.597f,  0.770f,  0.761f,
		0.559f,  0.436f,  0.730f,
		0.359f,  0.583f,  0.152f,
		0.483f,  0.596f,  0.789f,
		0.559f,  0.861f,  0.639f,
		0.195f,  0.548f,  0.859f,
		0.014f,  0.184f,  0.576f,
		0.771f,  0.328f,  0.970f,
		0.406f,  0.615f,  0.116f,
		0.676f,  0.977f,  0.133f,
		0.971f,  0.572f,  0.833f,
		0.140f,  0.616f,  0.489f,
		0.997f,  0.513f,  0.064f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.055f,  0.953f,  0.042f,
		0.714f,  0.505f,  0.345f,
		0.783f,  0.290f,  0.734f,
		0.722f,  0.645f,  0.174f,
		0.302f,  0.455f,  0.848f,
		0.225f,  0.587f,  0.040f,
		0.517f,  0.713f,  0.338f,
		0.053f,  0.959f,  0.120f,
		0.393f,  0.621f,  0.362f,
		0.673f,  0.211f,  0.457f,
		0.820f,  0.883f,  0.371f,
		0.982f,  0.099f,  0.879f
	};

    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // Generate a buffer for the colors
    glGenBuffers(1, &cubeColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, cubeColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
}


float calculateMinSeparation(const std::vector<glm::vec3>& vertices, 
                            glm::vec3& minBound, glm::vec3& maxBound)
{

    // Initialize minBound and maxBound with the first vertex
    minBound = vertices[0];
    maxBound = vertices[0];

    // Iterate through the vertices to find the min and max coordinates
    for (const glm::vec3& vertex : vertices) {
        minBound.x = std::min(minBound.x, vertex.x);
        maxBound.x = std::max(maxBound.x, vertex.x);
        minBound.y = std::min(minBound.y, vertex.y);
        maxBound.y = std::max(maxBound.y, vertex.y);
        minBound.z = std::min(minBound.z, vertex.z);
        maxBound.z = std::max(maxBound.z, vertex.z);
    }

    // Calculate the dimensions of the bounding box
    glm::vec3 boundingBoxDimensions = maxBound - minBound;
    
    // Determine the longest dimension
    float maxLength = boundingBoxDimensions.x;
    if (boundingBoxDimensions.y > maxLength) maxLength = boundingBoxDimensions.y;
    if (boundingBoxDimensions.z > maxLength) maxLength = boundingBoxDimensions.z;

    // Calculate the center of the bounding box
    glm::vec3 center = (minBound + maxBound) * 0.5f;

    // Calculate the half-length of the cube
    float halfLength = maxLength / 2.0f;


    return halfLength;

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
void cleanup(GLuint programID, GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer, GLuint elementbuffer, GLuint Texture , GLuint cubeVertexBuffer , GLuint cubeColorBuffer ) {
    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &Texture);
    glDeleteBuffers(1, &cubeVertexBuffer);
    glDeleteBuffers(1, &cubeColorBuffer);

    glfwTerminate();
}

void renderCubes(GLuint &cubeVertexBuffer, GLuint &cubeColorBuffer, GLuint programID, GLuint MatrixID, std::vector<glm::mat4> &cubeMVPs) {
    for (auto &mvp : cubeMVPs) {

        glUseProgram(programID);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, cubeColorBuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLES, 0, 36); // Assuming 36 vertices (6 faces * 2 triangles * 3 vertices)

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    }
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
    
    //Use Shader Cube
    GLuint programIDCube = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");

    // Variables for uniform and attribute locations
    GLuint MatrixID, ViewMatrixID, ModelMatrixID, vertexPosition_modelspaceID, vertexUVID, vertexNormal_modelspaceID,
        TextureID, LightID, justGreenID, applyInternalLightID;

    // Retrieve locations for uniforms and attributes
    getUniformAttributeLocations(programID, MatrixID, ViewMatrixID, ModelMatrixID, vertexPosition_modelspaceID, vertexUVID,
                                 vertexNormal_modelspaceID, TextureID, LightID, justGreenID, applyInternalLightID);


    // Load a texture
    GLuint Texture = loadDDS("uvmap.DDS");
    GLuint Texture_Square = loadBMP_custom("ssc2003-06d2.bmp");

    // Variables for model loading and lighting
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
    glm::vec3 lightPos = glm::vec3(4,4,4);
    bool res = loadAssImp("suzanne.obj", indices, indexed_vertices, indexed_uvs, indexed_normals);



    // Buffers for object rendering
    GLuint vertexbuffer, uvbuffer, normalbuffer, elementbuffer, vertexbufferSquare, uvbufferSquare , elementbufferSquare , cubeVBO, cubeColorBuffer;
    setupBuffers(vertexbuffer, uvbuffer, normalbuffer, elementbuffer, indexed_vertices, indexed_uvs, indexed_normals, indices);
    
    glm::vec3 minBound , maxBound;
    float minSep = calculateMinSeparation(indexed_vertices , minBound , maxBound);
    setupSquareBuffers(vertexbufferSquare, uvbufferSquare,  elementbufferSquare , 15.0f);
    setupCubeBuffers(cubeVBO, cubeColorBuffer); // for static objects

    glUseProgram(programID);

    double lastTime = glfwGetTime();
    int nbFrames = 0;
    bool lightingState = false;
    
    glm::vec3 axis1(1.0f, 0.0f, 0.0f); // Rotation around the x-axis
    glm::vec3 axis2(0.0f, 1.0f, 0.0f); // Rotation around the y-axis
    glm::vec3 axis3(0.0f, 0.0f, 1.0f); // Rotation around the z-axis
    glm::vec3 axis4(1.0f, 1.0f, 0.0f); // Rotation around a diagonal axis

    // Create objectTransformation objects with different rotation axes
    objectTransformation obj1(glm::mat4(1.0f), glm::vec3(0.0f), axis1 , minSep);
    objectTransformation obj2(glm::mat4(1.0f), glm::vec3(0.0f), axis2 , minSep);
    objectTransformation obj3(glm::mat4(1.0f), glm::vec3(0.0f), axis3 , minSep);
    objectTransformation obj4(glm::mat4(1.0f), glm::vec3(0.0f), axis4 , minSep);
    
    
    std::vector<objectTransformation> objects;
    objects.push_back(obj1);
    objects.push_back(obj2);
    objects.push_back(obj3);
    objects.push_back(obj4);

    // During object initialization
    for (auto& obj : objects) {
        float vx = static_cast<float>(rand()) / RAND_MAX * 10.0f - 5.0f; // Velocity range -5 to 5
        float vy = static_cast<float>(rand()) / RAND_MAX * 10.0f - 5.0f;
        float vz = static_cast<float>(rand()) / RAND_MAX * 10.0f; // Upward velocity 0 to 10

        obj.setVelocity(glm::vec3(vx, vy, vz));
    }



    do {
        // Measure speed
        double currentTime = glfwGetTime();
        nbFrames++;
        double deltaTime = currentTime - lastTime;
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
                    
        // Get projection and view matrices and set justGreen to 1
        glm::mat4 projectionMatrix = getProjectionMatrix();
        glm::mat4 viewMatrix = getViewMatrix();
        

        // Set the square's position and orientation
        glm::mat4 squareModelMatrix = glm::mat4(1.0f);
        squareModelMatrix = glm::translate(squareModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 squareMVP = projectionMatrix * viewMatrix * squareModelMatrix;
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &squareModelMatrix[0][0]);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &squareMVP[0][0]);

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture_Square);
        glUniform1i(TextureID, 0);

        // Vertex buffer for position
        glEnableVertexAttribArray(vertexPosition_modelspaceID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferSquare);
        glVertexAttribPointer(
            vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0
        );

        // UV buffer
        glEnableVertexAttribArray(vertexUVID);
        glBindBuffer(GL_ARRAY_BUFFER, uvbufferSquare); // Assuming uvbuffer is the name of your UV buffer
        glVertexAttribPointer(
            vertexUVID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0
        );

        // Bind the element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbufferSquare); // Assuming elementbuffer is your element buffer ID

        // Draw the square using the element buffer
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0); // 6 indices (2 triangles)

        // Disable attribute arrays
        glDisableVertexAttribArray(vertexPosition_modelspaceID);
        glDisableVertexAttribArray(vertexUVID);


        glm::vec3 cubePositions[] = {
            glm::vec3(-11.0f, -11.0f, 1.0f),
            glm::vec3(11.0f, 11.0f, 1.0f),
            glm::vec3(-11.0f, 11.0f, 1.0f),
            glm::vec3(11.0f, -11.0f, 1.0f)
        };

        std::vector<glm::mat4> cubeMVPs;
        for (const auto &position : cubePositions) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
            cubeMVPs.push_back(projectionMatrix * viewMatrix * model);
        }

        renderCubes(cubeVBO, cubeColorBuffer, programIDCube, MatrixID, cubeMVPs);


        bool status = checkAndUpdateModelMatrix(1.8f , 1.0f ,objects , deltaTime);
        glUniform1i(applyInternalLightID, status);
        std::vector<glm::mat4> MVPs = calculateMVPs(projectionMatrix, viewMatrix, objects);
        for (int i = 0; i < MVPs.size(); i++) {
            // Use the specified shader program
            glUseProgram(programID);
            // Set uniform values
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPs[i][0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &objects[i].getModelMatrix()[0][0]);
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

        lastTime = currentTime;
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);

    // Cleanup VBO, shader, and exit
    cleanup(programID, vertexbuffer, uvbuffer, normalbuffer, elementbuffer, Texture , cubeVBO, cubeColorBuffer);

    return 0;
}
