/*
Type your name and student ID here
    - Name:         Chan Chun Ming     
    - Student ID:   1155163257
*/

#include "Dependencies/glew/glew.h"
#include "Dependencies/GLFW/glfw3.h"

#include "Dependencies/glm/glm/glm.hpp"
#include "Dependencies/glm/glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <fstream>


GLuint programID;
GLuint vao[2]; // vertex array object

float x_delta = 0.1f;
int x_press_num = 0;

float rotationAngle = 0.0f;
float rotationIncrement = 5.0f;

float scaleFactor = 1.0f;
float scaleIncrement = 0.1f;

// Define variables to control the scene
float translationX = 0.0f;
float translationY = 0.0f;

void get_OpenGL_info() {
    // OpenGL information
    const GLubyte* name = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* glversion = glGetString(GL_VERSION);
    std::cout << "OpenGL company: " << name << std::endl;
    std::cout << "Renderer name: " << renderer << std::endl;
    std::cout << "OpenGL version: " << glversion << std::endl;
}


bool checkStatus(
    GLuint objectID,
    PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
    PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
    GLenum statusType)
{
    GLint status;
    objectPropertyGetterFunc(objectID, statusType, &status);
    if (status != GL_TRUE)
    {
        GLint infoLogLength;
        objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* buffer = new GLchar[infoLogLength];

        GLsizei bufferSize;
        getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
        std::cout << buffer << std::endl;

        delete[] buffer;
        return false;
    }
    return true;
}

bool checkShaderStatus(GLuint shaderID) {
    return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID) {
    return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

std::string readShaderCode(const char* fileName) {
    std::ifstream meInput(fileName);
    if (!meInput.good()) {
        std::cout << "File failed to load ... " << fileName << std::endl;
        exit(1);
    }
    return std::string(
        std::istreambuf_iterator<char>(meInput),
        std::istreambuf_iterator<char>()
    );
}

void installShaders() {
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* adapter[1];
    //adapter[0] = vertexShaderCode;
    std::string temp = readShaderCode("VertexShaderCode.glsl");
    adapter[0] = temp.c_str();
    glShaderSource(vertexShaderID, 1, adapter, 0);
    //adapter[0] = fragmentShaderCode;
    temp = readShaderCode("FragmentShaderCode.glsl");
    adapter[0] = temp.c_str();
    glShaderSource(fragmentShaderID, 1, adapter, 0);

    glCompileShader(vertexShaderID);
    glCompileShader(fragmentShaderID);

    if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
        return;

    programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    if (!checkProgramStatus(programID))
        return;

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    glUseProgram(programID);
}

void sendDataToOpenGL() {
    // TODO:
    GLfloat triangle[] = {
        // Vertex 1: Position (x, y, z) and Color (r, g, b)
        0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f, // Top vertex (red)

        // Front face
        -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom left vertex (green)
        1.0f, -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, // Bottom right vertex (blue)

        // Right face
        1.0f, -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, // Bottom left vertex (blue)
        1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // Bottom right vertex (green)

        // Back face
        1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // Bottom left vertex (green)
        -1.0f, -1.0f, -1.0f,0.0f, 0.0f, 1.0f, // Bottom right vertex (blue)

        // Left face
        -1.0f, -1.0f, -1.0f,0.0f, 0.0f, 1.0f, // Bottom left vertex (blue)
        -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f  // Bottom right vertex (green)
    };


    // Triangle
    glGenVertexArrays(1, &vao[0]);
    glBindVertexArray(vao[0]);

    GLuint vboID;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);


    // vertex position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    // vertex color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));


    // with indexing
    GLuint indices[] = { 0, 1, 2 };
    // index buffer
    GLuint indexBufferID;
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    
    const GLfloat cube[] = {
        // Front face (1)
        -0.5f + 2.0f, -0.5f, 0.5f,        1.0f, 0.0f, 0.0f,  // bottom left (red)
        0.5f + 2.0f, -0.5f, 0.5f,         1.0f, 0.0f, 0.0f,  // bottom right (red)
        0.5f + 2.0f, 0.5f, 0.5f,          1.0f, 0.0f, 0.0f,  // top right (red)
        -0.5f + 2.0f, 0.5f, 0.5f,         1.0f, 0.0f, 0.0f,  // top left (red)

        // Back face (6)
        -0.5f + 2.0f, -0.5f, -0.5f,       0.0f, 1.0f, 0.0f,  // bottom left (green)
        -0.5f + 2.0f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,  // top left (green)
        0.5f + 2.0f, 0.5f, -0.5f,         0.0f, 1.0f, 0.0f,  // top right (green)
        0.5f + 2.0f, -0.5f, -0.5f,        0.0f, 1.0f, 0.0f,  // bottom right (green)

        // Left face (4)
        -0.5f + 2.0f, -0.5f, -0.5f,       0.0f, 0.0f, 1.0f,  // bottom front (blue)
        -0.5f + 2.0f, -0.5f, 0.5f,        0.0f, 0.0f, 1.0f,  // bottom back (blue)
        -0.5f + 2.0f, 0.5f, 0.5f,         0.0f, 0.0f, 1.0f,  // top back (blue)
        -0.5f + 2.0f, 0.5f, -0.5f,        0.0f, 0.0f, 1.0f,  // top front (blue)

        // Right face (3)
        0.5f + 2.0f, -0.5f, -0.5f,        1.0f, 1.0f, 0.0f,  // bottom front (yellow)
        0.5f + 2.0f, 0.5f, -0.5f,         1.0f, 1.0f, 0.0f,  // top front (yellow)
        0.5f + 2.0f, 0.5f, 0.5f,          1.0f, 1.0f, 0.0f,  // top back (yellow)
        0.5f + 2.0f, -0.5f, 0.5f,         1.0f, 1.0f, 0.0f,  // bottom back (yellow)

        // Top face (2)
        -0.5f + 2.0f, 0.5f, -0.5f,        1.0f, 0.0f, 1.0f,  // front left (purple)
        -0.5f + 2.0f, 0.5f, 0.5f,         1.0f, 0.0f, 1.0f,  // back left (purple)
        0.5f + 2.0f, 0.5f, 0.5f,          1.0f, 0.0f, 1.0f,  // back right (purple)
        0.5f + 2.0f, 0.5f, -0.5f,         1.0f, 0.0f, 1.0f,  // front right (purple)

        // Bottom face (5)
        -0.5f + 2.0f, -0.5f, -0.5f,       0.0f, 1.0f, 1.0f,  // front left (cyan)
        0.5f + 2.0f, -0.5f, -0.5f,        0.0f, 1.0f, 1.0f,  // front right (cyan)
        0.5f + 2.0f, -0.5f, 0.5f,         0.0f, 1.0f, 1.0f,  // back right (cyan)
        -0.5f + 2.0f, -0.5f, 0.5f,        0.0f, 1.0f, 1.0f   // back left (cyan)
    };

  
    // cube
    glGenVertexArrays(1, &vao[1]);
    glBindVertexArray(vao[1]);

    GLuint vboID2;
    glGenBuffers(1, &vboID2);
    glBindBuffer(GL_ARRAY_BUFFER, vboID2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    // vertex position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    // vertex color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
       
    // with indexing
    GLuint indices2[] = { 0, 1, 2, 0, 2, 3 };
    // index buffer
    GLuint indexBufferID2;
    glGenBuffers(1, &indexBufferID2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);
}


void initializedGL(void) {
    // run only once 
    sendDataToOpenGL();
    installShaders();
    glEnable(GL_DEPTH_TEST);
    
}

void paintGL(void) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  //specify the background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // View matrix
    glm::mat4 viewMatrix = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 5.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
    GLint viewMatrixUniformLocation =
        glGetUniformLocation(programID, "viewMatrix");
    glUniformMatrix4fv(viewMatrixUniformLocation, 1,
        GL_FALSE, &viewMatrix[0][0]);

    // Set up the projection matrix
    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(45.0f), 1.0f, 1.0f, 100.0f);
    GLint projectionMatrixUniformLocation =
        glGetUniformLocation(programID, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixUniformLocation, 1,
        GL_FALSE, &projectionMatrix[0][0]);

    // always run
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  //specify the background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Translate
    glm::mat4 modelTransformMatrix = glm::mat4(1.0f);
    modelTransformMatrix = glm::translate(glm::mat4(1.0f),
        glm::vec3(x_delta * x_press_num, 0.0f, 0.0f));
    GLint modelTransformMatrixUniformLocation =
        glGetUniformLocation(programID, "modelTransformMatrix");
    glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1,
        GL_FALSE, &modelTransformMatrix[0][0]);

    // Scaling
    glm::mat4 scaleTransformMatrix = glm::mat4(1.0f);
    scaleTransformMatrix = glm::scale(scaleTransformMatrix, 
        glm::vec3(scaleFactor, scaleFactor, scaleFactor));
    GLint scaleTransformMatrixUniformLocation = glGetUniformLocation(programID, "scaleTransformMatrix");
    glUniformMatrix4fv(scaleTransformMatrixUniformLocation, 1, GL_FALSE, &scaleTransformMatrix[0][0]);

    // Rotate
    glm::mat4 rotateTransformMatrix = glm::mat4(1.0f);
    rotateTransformMatrix = glm::rotate(rotateTransformMatrix, 
        glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    GLint rotateTransformMatrixUniformLocation =
        glGetUniformLocation(programID, "rotateTransformMatrix");
    glUniformMatrix4fv(rotateTransformMatrixUniformLocation, 1,
        GL_FALSE, &rotateTransformMatrix[0][0]);

    // Perspective Translate
    glm::mat4 perspectiveTransformMatrix = glm::mat4(1.0f);
    perspectiveTransformMatrix = glm::translate(glm::mat4(1.0f),
        		glm::vec3(translationX, translationY, 0.0f));
    GLint perspectiveTransformMatrixUniformLocation =
        glGetUniformLocation(programID, "perspectiveTransformMatrix");
    glUniformMatrix4fv(perspectiveTransformMatrixUniformLocation, 1,
        GL_FALSE, &perspectiveTransformMatrix[0][0]);
    

    glBindVertexArray(vao[0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(vao[1]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);    
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    //translate 
    if (key == GLFW_KEY_A) {
        x_press_num -= 1;
    }
    if (key == GLFW_KEY_D) {
        x_press_num += 1;
    }
    //scaling
    if (key == GLFW_KEY_W) {
        scaleFactor += scaleIncrement;
    }
    if (key == GLFW_KEY_S) {
        scaleFactor -= scaleIncrement;
    }
    //rotate
    if (key == GLFW_KEY_Q) {
        rotationAngle -= rotationIncrement;
	}
    if (key == GLFW_KEY_E) {
        rotationAngle += rotationIncrement;
	}

    float rotationSpeed = 0.1f;
    float translationSpeed = 0.1f;
    if (key == GLFW_KEY_LEFT) {
		translationX -= translationSpeed;
	}
    if (key == GLFW_KEY_RIGHT) {
		translationX += translationSpeed;
	}
    if (key == GLFW_KEY_UP) {
		translationY += translationSpeed;
	}
	if (key == GLFW_KEY_DOWN) {
		translationY -= translationSpeed;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // Cursor to control rotation
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		rotationAngle += rotationIncrement;
	}
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		rotationAngle -= rotationIncrement;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	// Scroll to control scaling
    if (yoffset > 0) {
		scaleFactor += scaleIncrement;
	}
    else if (yoffset < 0) {
		scaleFactor -= scaleIncrement;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main(int argc, char* argv[]) {
    GLFWwindow* window;

    /* Initialize the glfw */
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    /* glfw: configure; necessary for MAC */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    /* do not allow resizing */
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(512, 512, "Hello Triangle", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    /* Initialize the glew */
    if (GLEW_OK != glewInit()) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    get_OpenGL_info();
    initializedGL();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        paintGL();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
