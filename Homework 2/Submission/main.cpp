/*
Student Information
Student ID:		1155163257
Student Name:	Chan Chun Ming
*/

#include "Dependencies/glew/glew.h"
#include "Dependencies/GLFW/glfw3.h"
#include "Dependencies/glm/glm/glm.hpp"
#include "Dependencies/glm/glm/gtc/matrix_transform.hpp"
#include "Dependencies/glm/glm/gtc/type_ptr.hpp"

//#include "Shader.h"
#include "Texture.h"
#include <string>


#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cstdlib>

#include "Dependencies/stb_image/stb_image.h"

GLint programID;

float x_delta = 0.1f;
int x_press_num = 0;

float rotationAngle = 0.0f;
float rotationIncrement = 5.0f;

float scaleFactor = 1.0f;
float scaleIncrement = 0.1f;

// Define variables to control the scene
float translationX = 0.0f;
float translationY = 0.0f;

// screen setting
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

// dog movement
float dogPositionY = 0.0f;
float DogMovementIncrement = 0.1f;

int lightBrightness = 1.0f;
int lightBrightnessIncrement = 1.0f;

int dog_texture = 1;
int ground_texture = 1;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
float pointLightBrightness = 1.0f;
float pointLightBrightnessIncrement = 1.0f;

float cameraVerticalAngle = 0.0f;

bool mouseLeftButtonPressed = false;

double lastY = 0.0;

// struct for storing the obj file
struct Vertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
};

struct Model {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

Model loadOBJ(const char* objPath)
{
	// function to load the obj file
	// Note: this simple function cannot load all obj files.

	struct V {
		// struct for identify if a vertex has showed up
		unsigned int index_position, index_uv, index_normal;
		bool operator == (const V& v) const {
			return index_position == v.index_position && index_uv == v.index_uv && index_normal == v.index_normal;
		}
		bool operator < (const V& v) const {
			return (index_position < v.index_position) ||
				(index_position == v.index_position && index_uv < v.index_uv) ||
				(index_position == v.index_position && index_uv == v.index_uv && index_normal < v.index_normal);
		}
	};

	std::vector<glm::vec3> temp_positions;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	std::map<V, unsigned int> temp_vertices;

	Model model;
	unsigned int num_vertices = 0;

	std::cout << "\nLoading OBJ file " << objPath << "..." << std::endl;

	std::ifstream file;
	file.open(objPath);

	// Check for Error
	if (file.fail()) {
		std::cerr << "Impossible to open the file! Do you use the right path? See Tutorial 6 for details" << std::endl;
		exit(1);
	}

	while (!file.eof()) {
		// process the object file
		char lineHeader[128];
		file >> lineHeader;

		if (strcmp(lineHeader, "v") == 0) {
			// geometric vertices
			glm::vec3 position;
			file >> position.x >> position.y >> position.z;
			temp_positions.push_back(position);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			// texture coordinates
			glm::vec2 uv;
			file >> uv.x >> uv.y;
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			// vertex normals
			glm::vec3 normal;
			file >> normal.x >> normal.y >> normal.z;
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			// Face elements
			V vertices[3];
			for (int i = 0; i < 3; i++) {
				char ch;
				file >> vertices[i].index_position >> ch >> vertices[i].index_uv >> ch >> vertices[i].index_normal;
			}

			// Check if there are more than three vertices in one face.
			std::string redundency;
			std::getline(file, redundency);
			if (redundency.length() >= 5) {
				std::cerr << "There may exist some errors while load the obj file. Error content: [" << redundency << " ]" << std::endl;
				std::cerr << "Please note that we only support the faces drawing with triangles. There are more than three vertices in one face." << std::endl;
				std::cerr << "Your obj file can't be read properly by our simple parser :-( Try exporting with other options." << std::endl;
				exit(1);
			}

			for (int i = 0; i < 3; i++) {
				if (temp_vertices.find(vertices[i]) == temp_vertices.end()) {
					// the vertex never shows before
					Vertex vertex;
					vertex.position = temp_positions[vertices[i].index_position - 1];
					vertex.uv = temp_uvs[vertices[i].index_uv - 1];
					vertex.normal = temp_normals[vertices[i].index_normal - 1];

					model.vertices.push_back(vertex);
					model.indices.push_back(num_vertices);
					temp_vertices[vertices[i]] = num_vertices;
					num_vertices += 1;
				}
				else {
					// reuse the existing vertex
					unsigned int index = temp_vertices[vertices[i]];
					model.indices.push_back(index);
				}
			} // for
		} // else if
		else {
			// it's not a vertex, texture coordinate, normal or face
			char stupidBuffer[1024];
			file.getline(stupidBuffer, 1024);
		}
	}
	file.close();

	std::cout << "There are " << num_vertices << " vertices in the obj file.\n" << std::endl;
	return model;
}

void get_OpenGL_info()
{
	// OpenGL information
	const GLubyte* name = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* glversion = glGetString(GL_VERSION);
	std::cout << "OpenGL company: " << name << std::endl;
	std::cout << "Renderer name: " << renderer << std::endl;
	std::cout << "OpenGL version: " << glversion << std::endl;
}

GLuint loadTexture(const char* texturePath)
{
	printf("Loading %s\n", texturePath);
	// tell stb_image.h to flip loaded texture's on the y-axis.
	stbi_set_flip_vertically_on_load(true);
	// load the texture data into "data"
	int Width, Height, BPP;
	unsigned char* data = stbi_load(texturePath, &Width, &Height, &BPP, 0);
	// Please pay attention to the format when sending the data to GPU
	GLenum format = 3;
	switch (BPP) {
	case 1: format = GL_RED; break;
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	}
	if (!data) {
		std::cout << "Failed to load texture: " << texturePath << std::endl;
		exit(1);
	}

	//GLuint textureID = 0;
	//TODO: Create one OpenGL texture and set the texture parameter 
	GLuint textureID;
	glGenTextures(1, &textureID);
	// "Bind" the newly created texture :
	// to indicate all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, data);
	// OpenGL has now copied the data. Free our own version
	stbi_image_free(data);

	std::cout << "Load " << texturePath << " successfully!" << std::endl;


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

// senddatatoopengl
GLuint dogVAO, dogVBO, dogEBO;
Model dogobj;
GLuint dogTexture0;
GLuint dogTexture1;

GLuint groundVAO, groundVBO, groundEBO;
Model groundobj;
GLuint groundTexture0;
GLuint groundTexture1;


void sendDataToOpenGL()
{
	//TODO
	//Load objects and bind to VAO and VBO
	//Load textures
	// Dog object (Fixed)
	// Dog Texture
	dogTexture0 = loadTexture("resources/dog/dog_01.jpg");
	dogTexture1 = loadTexture("resources/dog/dog_02.jpg");
	printf("dogTexture0 %d\n", dogTexture0);
	printf("dogTexture1 %d\n", dogTexture1);

	dogobj = loadOBJ("resources/dog/dog.obj");
	glGenVertexArrays(1, &dogVAO);
	glBindVertexArray(dogVAO);
	// Create VBO
	glGenBuffers(1, &dogVBO);
	glBindBuffer(GL_ARRAY_BUFFER, dogVBO);
	glBufferData(GL_ARRAY_BUFFER, dogobj.vertices.size() * sizeof(Vertex), &dogobj.vertices[0], GL_STATIC_DRAW);
	// Create EBO
	glGenBuffers(1, &dogEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dogEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dogobj.indices.size() * sizeof(unsigned int), &dogobj.indices[0], GL_STATIC_DRAW);
	// Vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	// Vertex uv
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	// Vertex normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	// Ground object (Fixed)
	// Ground Texture
	groundTexture0 = loadTexture("resources/ground/ground_01.jpg");
	groundTexture1 = loadTexture("resources/ground/ground_02.jpg");
	printf("groundTexture0 %d\n", groundTexture0);
	printf("groundTexture1 %d\n", groundTexture1);

	groundobj = loadOBJ("resources/ground/ground.obj");
	glGenVertexArrays(1, &groundVAO);
	glBindVertexArray(groundVAO);
	// Create VBO
	glGenBuffers(1, &groundVBO);
	glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
	glBufferData(GL_ARRAY_BUFFER, groundobj.vertices.size() * sizeof(Vertex), &groundobj.vertices[0], GL_STATIC_DRAW);
	// Create EBO
	glGenBuffers(1, &groundEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, groundobj.indices.size() * sizeof(unsigned int), &groundobj.indices[0], GL_STATIC_DRAW);
	// Vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	// Vertex uv
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	// Vertex normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
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

bool checkProgramStatus(GLuint ProgramID) {
	return checkStatus(ProgramID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
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
	std::string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
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

void use_2()
{
	glUseProgram(programID);
}

void setMat4_2(const std::string& name, glm::mat4& value)
{
	unsigned int transformLoc = glGetUniformLocation(programID, name.c_str());
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(value));
}

void setVec4_2(const std::string& name, glm::vec4 value)
{
	glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}

void setVec3_2(const std::string& name, glm::vec3 value)
{
	glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}

void setVec3_2(const std::string& name, float v1, float v2, float v3)
{
	glUniform3f(glGetUniformLocation(programID, name.c_str()), v1, v2, v3);
}

void setFloat_2(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void setInt_2(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

std::string readShaderCode_2(const char* fileName)
{
	std::ifstream myInput(fileName);
	if (!myInput.good())
	{
		std::cout << "File failed to load..." << fileName << std::endl;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(myInput),
		std::istreambuf_iterator<char>()
	);
}

bool checkShaderStatus_2(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus_2(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

bool checkStatus_2(GLuint objectID, PFNGLGETSHADERIVPROC objectPropertyGetterFunc, PFNGLGETSHADERINFOLOGPROC getInfoLogFunc, GLenum statusType)
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

void initializedGL(void) //run only once
{
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW not OK." << std::endl;
	}

	get_OpenGL_info();
	sendDataToOpenGL();

	//TODO: set up the camera parameters	
	//TODO: set up the vertex shader and fragment shader
	installShaders();
	//Shader shader;
	//shader.setupShader("VertexShaderCode.glsl", "FragmentShaderCode.glsl");
	//shader.use();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void Matrix(std::string obj) {
	glm::mat4 TransformMatrix = glm::mat4(1.0f);
	glm::mat4 RotationMatrix = glm::mat4(1.0f);
	glm::mat4 ScalingMatrix = glm::mat4(1.0f);
	//unsigned int slot = 0;

	if (obj == "dog") {
		TransformMatrix = glm::translate(TransformMatrix, glm::vec3(translationX, translationY+ dogPositionY, 0.0f));
		RotationMatrix = glm::rotate(RotationMatrix, glm::radians(rotationAngle), glm::vec3(1.0f, 0.0f, 1.0f));
		ScalingMatrix = glm::scale(ScalingMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
	}
	else if (obj == "ground") {
		TransformMatrix = glm::translate(TransformMatrix, glm::vec3(0.0f, -0.9f, 0.0f));
		RotationMatrix = glm::rotate(RotationMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		ScalingMatrix = glm::scale(ScalingMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
	}
	GLint ModelTransformMatrixUniformLocation =
		glGetUniformLocation(programID, "TransformMatrix");
	GLint ModelScalingMatrixUniformLocation =
		glGetUniformLocation(programID, "ScalingMatrix");
	GLint ModelRotationMatrixUniformLocation =
		glGetUniformLocation(programID, "RotationMatrix");

	glUniformMatrix4fv(ModelTransformMatrixUniformLocation, 1, GL_FALSE, &TransformMatrix[0][0]);
	glUniformMatrix4fv(ModelRotationMatrixUniformLocation, 1, GL_FALSE, &RotationMatrix[0][0]);
	glUniformMatrix4fv(ModelScalingMatrixUniformLocation, 1, GL_FALSE, &ScalingMatrix[0][0]);

	// View matrix
	glm::mat4 viewMatrix = glm::lookAt(
		cameraPos, 
		cameraPos + glm::vec3(0.0f, sin(glm::radians(cameraVerticalAngle)), 
			-cos(glm::radians(cameraVerticalAngle))), 
		glm::vec3(0.0f, 1.0f, 0.0f));
	GLint viewMatrixUniformLocation =
		glGetUniformLocation(programID, "viewMatrix");
	glUniformMatrix4fv(viewMatrixUniformLocation, 1,
		GL_FALSE, glm::value_ptr(viewMatrix));

	// Set up the projection matrix
	glm::mat4 projectionMatrix = glm::perspective(
		glm::radians(45.0f), 1.0f, 1.0f, 100.0f);
	GLint projectionMatrixUniformLocation =
		glGetUniformLocation(programID, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixUniformLocation, 1,
		GL_FALSE, &projectionMatrix[0][0]);


	// Lighting
	// GLint TextLoc = glGetUniformLocation(programID, "Texture");
	//glUniform1i(TextLoc, 0);
	//glActiveTexture(GL_TEXTURE0 + slot);

	// Set light properties (Directional Light)
	glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, 0.0f, 0.0f));
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
	glm::vec3 diffuseColor = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);

	setVec3_2("lightDir", lightDir);
	setVec3_2("lightColor", lightColor);
	setVec3_2("ambientColor", ambientColor);
	setVec3_2("diffuseColor", diffuseColor);
	setVec3_2("specularColor", specularColor);

	GLint lightDirUniformLocation =
		glGetUniformLocation(programID, "lightDir");
	GLint lightColorUniformLocation =
		glGetUniformLocation(programID, "lightColor");
	GLuint lightBrightnessUniformLocation =
		glGetUniformLocation(programID, "lightBrightness");

	glUniform3f(lightDirUniformLocation, lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(lightColorUniformLocation, lightColor.x, lightColor.y, lightColor.z);
	glUniform1f(lightBrightnessUniformLocation, lightBrightness);

	// Point light 

	glm::vec3 pointLightColor = glm::vec3(0.0f, 1.0f, 1.0f);
	glm::vec3 pointLightPos = glm::vec3(1.0f, 0.0f, 0.0f);


	GLint pointLightPosUniformLocation =
		glGetUniformLocation(programID, "pointLightPos");
	GLint pointLightColorUniformLocation =
		glGetUniformLocation(programID, "pointLightColor");
	GLint pointLightBrightnessUniformLocation =
		glGetUniformLocation(programID, "pointLightBrightness");

	glUniform3f(pointLightColorUniformLocation, pointLightPos.x, pointLightPos.y, pointLightPos.z);
	glUniform3f(pointLightColorUniformLocation, pointLightColor.x, pointLightColor.y, pointLightColor.z);
	glUniform1f(pointLightBrightnessUniformLocation, pointLightBrightness);

}

void paintGL(void)  //always run
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //specify the background color, this is just an example
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//TODO:
	//Set lighting information, such as position and color of lighting source
	//glm::vec3 lightPos(1.0f, 1.0f, 1.0f);
	//glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

	glClearDepth(1.0f);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	Matrix("dog");
	glBindVertexArray(dogVAO);
	glActiveTexture(GL_TEXTURE0);
	if (dog_texture == 1) 
		glBindTexture(GL_TEXTURE_2D, dogTexture0);
	if (dog_texture == 2)
		glBindTexture(GL_TEXTURE_2D, dogTexture1);
	
	glDrawElements(GL_TRIANGLES, dogobj.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	Matrix("ground");
	glBindVertexArray(groundVAO);
	glActiveTexture(GL_TEXTURE0);
	if (ground_texture == 1)
		glBindTexture(GL_TEXTURE_2D, groundTexture0);
	if (ground_texture == 2)
		glBindTexture(GL_TEXTURE_2D, groundTexture1);
	glDrawElements(GL_TRIANGLES, groundobj.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glFlush();


	/*
	// Old code
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


	// Bind texture
	glBindVertexArray(dogVAO);
	glActiveTexture(GL_TEXTURE0);
	texture[0].bind(0);
	GLuint TextureID = glGetUniformLocation(programID, "dogTexture");
	glUniform1i(TextureID, 0);
	glDrawElements(GL_TRIANGLES, dogobj.indices.size(), GL_UNSIGNED_INT, 0);
	texture[0].unbind();

	glBindVertexArray(groundVAO);
	texture[2].bind(1);
	glDrawElements(GL_TRIANGLES, groundobj.indices.size(), GL_UNSIGNED_INT, 0);
	texture[2].unbind();*/
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// Sets the mouse-button callback for the current window.	
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			mouseLeftButtonPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			mouseLeftButtonPressed = false;
			lastY = 0.0;
		}
	}
}

void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	if (mouseLeftButtonPressed) {
		if (lastY == 0.0) {
			lastY = y;
		}
		else {
			double deltaY = lastY - y;
			cameraPos.y += deltaY * 0.005; // Adjust the 0.005 value to suit your needs
			lastY = y;
		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// Sets the scoll callback for the current window.
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Sets the Keyboard callback for the current window.
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_LEFT: //Rotation
			rotationAngle -= rotationIncrement;
			break;
		case GLFW_KEY_RIGHT:
			rotationAngle += rotationIncrement;
			break;
		case GLFW_KEY_UP: // Dog movement
			dogPositionY += DogMovementIncrement;
			break;
		case GLFW_KEY_DOWN:
			dogPositionY -= DogMovementIncrement;
			break;
		case GLFW_KEY_W: //Brightness
			//pointLightBrightness += 0.1f;
			lightBrightness += lightBrightnessIncrement;
			if (lightBrightness > 10)
				lightBrightness = 10;
			break;
		case GLFW_KEY_S:
			//pointLightBrightness -= 0.1f;
			lightBrightness -= lightBrightnessIncrement;
			if (lightBrightness < 0)
				lightBrightness = 0;
			break;
		case GLFW_KEY_L:
			translationX = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			translationY = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			break;

		case GLFW_KEY_I:
			pointLightBrightness += pointLightBrightnessIncrement;
			if (pointLightBrightness > 10)
				pointLightBrightness = 10;
			break;
		case GLFW_KEY_K:
			pointLightBrightness -= pointLightBrightnessIncrement;
			if (pointLightBrightness < 0)
				pointLightBrightness = 0;
			break;

		default:
			break;
		}
	}
	

	if (key == '1') {
		dog_texture = 1;
	}
	if (key == '2') {
		dog_texture = 2;
	}
	if (key == '3') {
		ground_texture = 1;
	}
	if (key == '4') {
		ground_texture = 2;
	}
}


int main(int argc, char* argv[])
{
	GLFWwindow* window;

	/* Initialize the glfw */
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assignment 2", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/*register callback functions*/
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);                                                                  //    
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	initializedGL();

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






