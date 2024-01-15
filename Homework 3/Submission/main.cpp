/*
Student Information
Student ID: 1155163257
Student Name: Chan Chun Ming
*/

#include "Dependencies/glew/glew.h"
#include "Dependencies/GLFW/glfw3.h"
#include "Dependencies/glm/glm/glm.hpp"
#include "Dependencies/glm/glm/gtc/matrix_transform.hpp"

#include "Dependencies/glm/glm/gtc/type_ptr.hpp"

#include "Shader.h"
#include "Texture.h"
#include "Misc.h"

#include <fstream>
#include <vector>

// screen setting
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

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

//Old code
Model DogObj;
GLuint DogVao, DogVbo, DogEbo;
Texture DogTexture[2];
Model GroundObj;
GLuint GroundVao, GroundVbo, GroundEbo;
Texture GroundTexture[2];

Shader shader;
int DogTextureNo = 1;
int GroundTextureNo = 1;
int CarTextureNo = 1;
glm::vec3 DogMoveDirection;//for moving
float DogRotateDirection; //for rotate

// New Code
Model SpaceCraftObj;
GLuint SpaceCraftVAO, SpaceCraftVBO, SpaceCraftEBO;
Texture SpaceCraftTexture;

Model PlanetObj;
GLuint PlanetVAO, PlanetVBO, PlanetEBO;
Texture texEarth, texNorm;

Model CraftObj;
GLuint CraftVAO, CraftVBO, CraftEBO;
Texture CraftTexture;

GLuint SkyboxVAO, SkyboxVBO;
Texture SkyboxTexture;
unsigned int E_skybox;
Shader skyboxShader;

Model RockObj;
GLuint RockVAO, RockVBO, RockEBO;
Texture RockTexture;
Texture RockTexture2;

Shader nmShader;
void loadSpaceCraft()
{
	SpaceCraftTexture.setupTexture("resources/texture/spacecraftTexture.bmp");
	SpaceCraftObj = loadOBJ("resources/object/spacecraft.obj");
	// VAO
	glGenVertexArrays(1, &SpaceCraftVAO);
	glBindVertexArray(SpaceCraftVAO);
	// VBO
	glGenBuffers(1, &SpaceCraftVBO);
	glBindBuffer(GL_ARRAY_BUFFER, SpaceCraftVBO);
	glBufferData(GL_ARRAY_BUFFER, SpaceCraftObj.vertices.size() * sizeof(Vertex), &SpaceCraftObj.vertices[0], GL_STATIC_DRAW);
	// EBO
	glGenBuffers(1, &SpaceCraftEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SpaceCraftEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, SpaceCraftObj.indices.size() * sizeof(unsigned int), &SpaceCraftObj.indices[0], GL_STATIC_DRAW);
	// Vertex Positions, uv, normal
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
}

void loadPlanet()
{
	texEarth.setupTexture("resources/texture/earthTexture.bmp");
	texNorm.setupTexture("resources/texture/earthNormal.bmp");
	PlanetObj = loadOBJ("resources/object/planet.obj");

	std::vector<glm::vec3> tangents(PlanetObj.vertices.size(), glm::vec3(0.0f, 0.0f, 0.0f));
	std::vector<glm::vec3> bitangents(PlanetObj.vertices.size(), glm::vec3(0.0f, 0.0f, 0.0f));

	// Calculate tangents and bitangents
	for (int i = 0; i < PlanetObj.indices.size(); i += 3) {
		unsigned int i0 = PlanetObj.indices[i + 0];
		unsigned int i1 = PlanetObj.indices[i + 1];
		unsigned int i2 = PlanetObj.indices[i + 2];

		glm::vec3& v0 = PlanetObj.vertices[i0].position;
		glm::vec3& v1 = PlanetObj.vertices[i1].position;
		glm::vec3& v2 = PlanetObj.vertices[i2].position;

		glm::vec2& uv0 = PlanetObj.vertices[i0].uv;
		glm::vec2& uv1 = PlanetObj.vertices[i1].uv;
		glm::vec2& uv2 = PlanetObj.vertices[i2].uv;

		glm::vec3 dp1 = v1 - v0;
		glm::vec3 dp2 = v2 - v0;

		glm::vec2 duv1 = uv1 - uv0;
		glm::vec2 duv2 = uv2 - uv0;

		float r = 1.0f / (duv1.x * duv2.y - duv1.y * duv2.x);
		glm::vec3 tangent = (dp1 * duv2.y - dp2 * duv1.y) * r;
		glm::vec3 bitangent = (dp2 * duv1.x - dp1 * duv2.x) * r;

		tangents[i0] += tangent;
		bitangents[i0] += bitangent;

		tangents[i1] += tangent;
		bitangents[i1] += bitangent;

		tangents[i2] += tangent;
		bitangents[i2] += bitangent;
	}

	// Normalize tangents and bitangents and add them to the vertex data
	for (int i = 0; i < PlanetObj.vertices.size(); i++) {
		tangents[i] = glm::normalize(tangents[i]);
		bitangents[i] = glm::normalize(bitangents[i]);
	}

	// VAO
	glGenVertexArrays(1, &PlanetVAO);
	glBindVertexArray(PlanetVAO);
	// VBO
	glGenBuffers(1, &PlanetVBO);
	glBindBuffer(GL_ARRAY_BUFFER, PlanetVBO);
	glBufferData(GL_ARRAY_BUFFER, PlanetObj.vertices.size() * sizeof(Vertex), &PlanetObj.vertices[0], GL_STATIC_DRAW);
	// EBO
	glGenBuffers(1, &PlanetEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PlanetEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, PlanetObj.indices.size() * sizeof(unsigned int), &PlanetObj.indices[0], GL_STATIC_DRAW);
	// Vertex Positions, uv, normal
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	// Bitangents
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	// Tangents VBO
	GLuint tangentVBO;
	glGenBuffers(1, &tangentVBO);
	glBindBuffer(GL_ARRAY_BUFFER, tangentVBO);
	glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	// Bitangents VBO
	GLuint bitangentVBO;
	glGenBuffers(1, &bitangentVBO);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentVBO);
	glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	//TODO: set up planet textures
	nmShader.setupShader("nm.vs", "nm.fs");
}
/*
void loadPlanet()
{
	texEarth.setupTexture("resources/texture/earthTexture.bmp");
	texNorm.setupTexture("resources/texture/earthNormal.bmp");
	PlanetObj = loadOBJ("resources/object/planet.obj");
	// VAO
	glGenVertexArrays(1, &PlanetVAO);
	glBindVertexArray(PlanetVAO);
	// VBO
	glGenBuffers(1, &PlanetVBO);
	glBindBuffer(GL_ARRAY_BUFFER, PlanetVBO);
	glBufferData(GL_ARRAY_BUFFER, PlanetObj.vertices.size() * sizeof(Vertex), &PlanetObj.vertices[0], GL_STATIC_DRAW);
	// EBO
	glGenBuffers(1, &PlanetEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PlanetEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, PlanetObj.indices.size() * sizeof(unsigned int), &PlanetObj.indices[0], GL_STATIC_DRAW);
	// Vertex Positions, uv, normal
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	//TODO: set up planet textures
	nmShader.setupShader("nm.vs", "nm.fs");
}
*/

void loadCraft()
{
	CraftTexture.setupTexture("resources/texture/craft1Texture.bmp");
	CraftObj = loadOBJ("resources/object/craft.obj");
	// VAO
	glGenVertexArrays(1, &CraftVAO);
	glBindVertexArray(CraftVAO);
	// VBO
	glGenBuffers(1, &CraftVBO);
	glBindBuffer(GL_ARRAY_BUFFER, CraftVBO);
	glBufferData(GL_ARRAY_BUFFER, CraftObj.vertices.size() * sizeof(Vertex), &CraftObj.vertices[0], GL_STATIC_DRAW);
	// EBO
	glGenBuffers(1, &CraftEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CraftEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, CraftObj.indices.size() * sizeof(unsigned int), &CraftObj.indices[0], GL_STATIC_DRAW);
	// Vertex Positions, uv, normal
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
}

GLuint amount = 200;
glm::mat4* modelMatrices = new glm::mat4[amount];
void load_Rock()
{
	RockTexture.setupTexture("resources/texture/rockTexture.bmp");
	RockTexture2.setupTexture("resources/texture/craftTexture.bmp");
	RockObj = loadOBJ("resources/object/rock.obj");
	// VAO
	glGenVertexArrays(1, &RockVAO);
	glBindVertexArray(RockVAO);
	// VBO
	glGenBuffers(1, &RockVBO);
	glBindBuffer(GL_ARRAY_BUFFER, RockVBO);
	glBufferData(GL_ARRAY_BUFFER, RockObj.vertices.size() * sizeof(Vertex), &RockObj.vertices[0], GL_STATIC_DRAW);
	// EBO
	glGenBuffers(1, &RockEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RockEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, RockObj.indices.size() * sizeof(unsigned int), &RockObj.indices[0], GL_STATIC_DRAW);
	// Vertex Positions, uv, normal
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	// Generate a large list of semi-random model transformation matrices
	srand(glfwGetTime()); // initialize random seed    
	float radius = 10.0;
	float offset = 1.5f;

	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f + 3; // keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices[i] = model;
	}
}
/*
void load_Rock()
{
	RockTexture.setupTexture("resources/texture/rockTexture.bmp");
	RockObj = loadOBJ("resources/object/rock.obj");
	// VAO
	glGenVertexArrays(1, &RockVAO);
	glBindVertexArray(RockVAO);
	// VBO
	glGenBuffers(1, &RockVBO);
	glBindBuffer(GL_ARRAY_BUFFER, RockVBO);
	glBufferData(GL_ARRAY_BUFFER, RockObj.vertices.size() * sizeof(Vertex), &RockObj.vertices[0], GL_STATIC_DRAW);
	// EBO
	glGenBuffers(1, &RockEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RockEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, RockObj.indices.size() * sizeof(unsigned int), &RockObj.indices[0], GL_STATIC_DRAW);
	// Vertex Positions, uv, normal
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
}

*/
void loadMeshes()
{
	// TODO: load objects and norm vertices to unit bbox
	// TODO: bind the data to VAO & VBO
	// TODO: set up textures
	loadSpaceCraft();
	loadPlanet();
	loadCraft();
	load_Rock();
	// TODO: prepare for normal mapping
	// TODO: create asteroid ring
	// TODO: initialize craft motion
}

void createSkybox()
{
	//Cubemap
	GLfloat skyboxVertices[] =
	{
		// positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// TODO: load skybox and bind to VAO & VBO
	// VAO 
	glGenVertexArrays(1, &SkyboxVAO);
	glBindVertexArray(SkyboxVAO);
	// VBO
	glGenBuffers(1, &SkyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, SkyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	// Vertex Positions, uv, normal
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	E_skybox = 36;

	// TODO: set up skybox textures
	std::vector<std::string> texPaths;
	texPaths.push_back(std::string("resources/skybox/right.bmp"));
	texPaths.push_back(std::string("resources/skybox/left.bmp"));
	texPaths.push_back(std::string("resources/skybox/top.bmp"));
	texPaths.push_back(std::string("resources/skybox/bottom.bmp"));
	texPaths.push_back(std::string("resources/skybox/front.bmp"));
	texPaths.push_back(std::string("resources/skybox/back.bmp"));

	SkyboxTexture.setupTextureCubemap(texPaths);
	skyboxShader.setupShader("skybox.vs", "skybox.fs");
}

void DogBind()
{
	DogTexture[0].setupTexture("resources/dog/dog_01.jpg");//setup
	DogTexture[1].setupTexture("resources/dog/dog_02.jpg");
	DogObj = loadOBJ("resources/dog/dog.obj");//load
	//vao
	glGenVertexArrays(1, &DogVao);
	glBindVertexArray(DogVao);
	//vbo
	glGenBuffers(1, &DogVbo);
	glBindBuffer(GL_ARRAY_BUFFER, DogVbo);
	glBufferData(GL_ARRAY_BUFFER, DogObj.vertices.size() * sizeof(Vertex), &DogObj.vertices[0], GL_STATIC_DRAW);
	//ebo
	glGenBuffers(1, &DogEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, DogEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, DogObj.indices.size() * sizeof(unsigned int), &DogObj.indices[0], GL_STATIC_DRAW);
	//set position,uv,normal
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
}

void GroundBind()
{
	GroundTexture[0].setupTexture("resources/ground/ground_01.jpg");//setup
	GroundTexture[1].setupTexture("resources/ground/ground_02.jpg");
	GroundObj = loadOBJ("resources/ground/ground.obj");//load
	//vao
	glGenVertexArrays(1, &GroundVao);
	glBindVertexArray(GroundVao);
	//vbo
	glGenBuffers(1, &GroundVbo);
	glBindBuffer(GL_ARRAY_BUFFER, GroundVbo);
	glBufferData(GL_ARRAY_BUFFER, GroundObj.vertices.size() * sizeof(Vertex), &GroundObj.vertices[0], GL_STATIC_DRAW);
	//ebo
	glGenBuffers(1, &GroundEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GroundEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, GroundObj.indices.size() * sizeof(unsigned int), &GroundObj.indices[0], GL_STATIC_DRAW);
	//set position,uv,normal
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
}

void sendDataToOpenGL()
{
	//TODO
	//Load objects and bind to VAO and VBO
	DogBind();
	GroundBind();

	loadMeshes();
	createSkybox();
	//Load textures
}

void initializedGL(void) //run only onceo
{
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW not OK." << std::endl;
	}

	get_OpenGL_info();
	sendDataToOpenGL();

	//TODO: set up the camera parameters	
	//TODO: set up the vertex shader and fragment shader

	shader.setupShader("vert.glsl", "frag.glsl");


	//nmShader.setupShader("nm.vs", "nm.fs");
	//nmShader.use();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

// Old Paint function
void paintDog()
{
	glm::mat4 modelTransformMatrix;
	glm::mat4 modelTransformMatrixRrotateX;
	glm::mat4 modelTransformMatrixRrotateY;
	glm::mat4 modelTransformMatrixRrotateZ;
	glm::mat4 modelTransformMatrixScale;
	glm::mat4 modelTransformMatrixTranslate;


	GLint modelMatrixUniformLocation;
	GLuint TextureID = glGetUniformLocation(shader.Return_ID(), "ourTexture");
	glActiveTexture(GL_TEXTURE0);//2 given dog
	if (DogTextureNo == 1) {
		DogTexture[0].bind(0);
	}
	else {
		DogTexture[1].bind(0);
	}
	glUniform1i(TextureID, 0);

	glBindVertexArray(DogVao);
	modelTransformMatrixTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	//set the dog is on the ground face viewer
	modelTransformMatrixRrotateX = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0));
	modelTransformMatrixRrotateY = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
	modelTransformMatrixRrotateZ = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 0, 1));
	modelTransformMatrixScale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
	modelTransformMatrix = modelTransformMatrixTranslate * modelTransformMatrixRrotateX * modelTransformMatrixRrotateY * modelTransformMatrixRrotateZ * modelTransformMatrixScale;

	modelTransformMatrix = glm::translate(modelTransformMatrix, DogMoveDirection);//key move up down
	modelTransformMatrix = glm::rotate(modelTransformMatrix, glm::radians(DogRotateDirection), glm::vec3(0.0f, 0.0f, 1.0f));//for key rotate correct

	modelMatrixUniformLocation = glGetUniformLocation(shader.Return_ID(), "modelTransformMatrix");
	glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, DogObj.indices.size(), GL_UNSIGNED_INT, 0);
}
void paintGround()
{
	GLint modelMatrixUniformLocation;
	glm::mat4 modelTransformMatrix;
	GLuint TextureID = glGetUniformLocation(shader.Return_ID(), "ourTexture");
	glActiveTexture(GL_TEXTURE0);
	if (GroundTextureNo == 1) {//2 given ground
		GroundTexture[0].bind(0);
	}
	else {
		GroundTexture[1].bind(0);
	}
	glUniform1i(TextureID, 0);
	glBindVertexArray(GroundVao);
	modelTransformMatrix = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	modelTransformMatrix = glm::scale(modelTransformMatrix, glm::vec3(6.0f, 1.0f, 6.0f));//set the scale

	modelMatrixUniformLocation = glGetUniformLocation(shader.Return_ID(), "modelTransformMatrix");
	glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, GroundObj.indices.size(), GL_UNSIGNED_INT, 0);

}

glm::vec3 mousePosition = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 SpaceCraftDirection;
float SpaceCraftRotateDirection;
glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, -18.0f);
glm::vec3 pointLightColor = glm::vec3(0.5f, 0.0f, 0.0f); // Initial brightness

bool collision = false;
glm::vec3 currentSpacecraftPosition;
void paint_SpaceCraft()
{
	glm::mat4 modelTransformMatrix;
	glm::mat4 modelRotateMatrix_X;
	glm::mat4 modelRotateMatrix_Y;
	glm::mat4 modelRotateMatrix_Z;
	glm::mat4 modelScaleMatrix;
	glm::mat4 modelMatrix;

	GLint modelMatrixUniformLocation;
	GLuint TextureID = glGetUniformLocation(shader.Return_ID(), "ourTexture");
	glActiveTexture(GL_TEXTURE0);
	SpaceCraftTexture.bind(0);
	glUniform1i(TextureID, 0);

	glBindVertexArray(SpaceCraftVAO);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -13.0f));

	modelRotateMatrix_X = glm::rotate(glm::mat4(1.0f), glm::radians(mousePosition.y), glm::vec3(1.0f, 0.0f, 0.0f));
	modelRotateMatrix_Y = glm::rotate(glm::mat4(1.0f), glm::radians(mousePosition.x), glm::vec3(0.0f, 1.0f, 0.0f));
	modelRotateMatrix_Z = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	modelScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));
	modelTransformMatrix = modelMatrix * modelRotateMatrix_X * modelRotateMatrix_Y * modelRotateMatrix_Z * modelScaleMatrix;

	modelTransformMatrix = glm::translate(modelTransformMatrix, SpaceCraftDirection);
	modelTransformMatrix = glm::rotate(modelTransformMatrix, glm::radians(SpaceCraftRotateDirection), glm::vec3(0.0f, 0.0f, 1.0f));
	modelTransformMatrix = glm::translate(modelTransformMatrix, mousePosition);

	glm::vec4 position = modelTransformMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	currentSpacecraftPosition = glm::vec3(position);

	modelMatrixUniformLocation = glGetUniformLocation(shader.Return_ID(), "modelTransformMatrix");
	glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, SpaceCraftObj.indices.size(), GL_UNSIGNED_INT, 0);
}

// New Paint funciton
glm::vec3 CraftDirection;
float CraftRotateDirection;
void paintCraft()
{
	glm::mat4 modelTransformMatrix;
	glm::mat4 modelRotateMatrix_X;
	glm::mat4 modelRotateMatrix_Y;
	glm::mat4 modelRotateMatrix_Z;
	glm::mat4 modelScaleMatrix;
	glm::mat4 modelMatrix;

	GLint modelMatrixUniformLocation;
	GLuint TextureID = glGetUniformLocation(shader.Return_ID(), "ourTexture");
	glActiveTexture(GL_TEXTURE0);
	CraftTexture.bind(0);
	glUniform1i(TextureID, 0);

	glBindVertexArray(CraftVAO);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, -5.0f));

	modelRotateMatrix_X = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelRotateMatrix_Y = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	modelRotateMatrix_Z = glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	modelScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
	modelTransformMatrix = modelMatrix * modelRotateMatrix_X * modelRotateMatrix_Y * modelRotateMatrix_Z * modelScaleMatrix;

	modelTransformMatrix = glm::translate(modelTransformMatrix, CraftDirection);
	modelTransformMatrix = glm::rotate(modelTransformMatrix, glm::radians(CraftRotateDirection), glm::vec3(0.0f, 0.0f, 1.0f));

	modelMatrixUniformLocation = glGetUniformLocation(shader.Return_ID(), "modelTransformMatrix");
	glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, CraftObj.indices.size(), GL_UNSIGNED_INT, 0);
}



float cameraMoveX;
float cameraMoveY;
float zoom = 45.0f;
float diffuseBrightness = 0.8f;
float diffuseBrightnessGreen = 0.1f;
float diffuseBrightnessBlue = 0.3f;
double xpos, ypos;
double mouseDeltaX, mouseDeltaY;
float radianY;
float move = 5.0f;
int num = 1;
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

bool firstMouse = true;
bool isLeftButtonPressed = false;
double lastX = 400, lastY = 300;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			isLeftButtonPressed = true;

			// Get the current mouse position and use it as the starting position for the mouse movement
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			lastX = x;
			lastY = y;
		}
		else if (action == GLFW_RELEASE)
		{
			isLeftButtonPressed = false;
		}
	}
}
void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	if (!isLeftButtonPressed)  // Only proceed if the left button is pressed
	{
		return;
	}

	// Calculate mouse delta
	double xoffset = x - lastX;
	double yoffset = y - lastY;
	lastX = x;
	lastY = y;

	// Adjust sensitivity
	float sensitivity = 0.5f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	// Update mouse position
	mousePosition.x += xoffset;
	mousePosition.y -= yoffset;  // Y is inverted because screen coordinates go from top to bottom
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	float speed = 2.0f; // Define a speed for the spacecraft movement
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_W:
			// Move forward (closer to the planet)
			SpaceCraftDirection.z += speed;
			break;
		case GLFW_KEY_S:
			// Move backward (away from the planet)
			SpaceCraftDirection.z -= speed;
			break;
		case GLFW_KEY_A:
			// Move left
			SpaceCraftDirection.x -= speed;
			break;
		case GLFW_KEY_D:
			// Move right
			SpaceCraftDirection.x += speed;
			break;
			// Add more cases if needed
		case GLFW_KEY_UP:
			pointLightColor += glm::vec3(0.05f); // Increase brightness
			break;
		case GLFW_KEY_DOWN:
			pointLightColor -= glm::vec3(0.05f); // Decrease brightness
			pointLightColor = glm::max(pointLightColor, glm::vec3(0.0f)); // Ensure brightness doesn't go below 0
			break;
		}
	}
}

int lightBrightness = 1.0f;
int pointLightBrightness = 1.0f;
/*
void light()
{

	// Set light properties (Directional Light)
	glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, 0.0f, 0.0f));
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
	glm::vec3 diffuseColor = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);

	shader.setVec3("lightDir", lightDir);
	shader.setVec3("lightColor", lightColor);
	shader.setVec3("ambientColor", ambientColor);
	shader.setVec3("diffuseColor", diffuseColor);
	shader.setVec3("specularColor", specularColor);

	GLint lightDirUniformLocation =
		glGetUniformLocation(shader.Return_ID(), "lightDir");
	GLint lightColorUniformLocation =
		glGetUniformLocation(shader.Return_ID(), "lightColor");
	GLuint lightBrightnessUniformLocation =
		glGetUniformLocation(shader.Return_ID(), "lightBrightness");

	glUniform3f(lightDirUniformLocation, lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(lightColorUniformLocation, lightColor.x, lightColor.y, lightColor.z);
	glUniform1f(lightBrightnessUniformLocation, lightBrightness);

	// Point light

	glm::vec3 pointLightColor = glm::vec3(0.0f, 1.0f, 1.0f);
	glm::vec3 pointLightPos = glm::vec3(1.0f, 0.0f, 0.0f);


	GLint pointLightPosUniformLocation =
		glGetUniformLocation(shader.Return_ID(), "pointLightPos");
	GLint pointLightColorUniformLocation =
		glGetUniformLocation(shader.Return_ID(), "pointLightColor");
	GLint pointLightBrightnessUniformLocation =
		glGetUniformLocation(shader.Return_ID(), "pointLightBrightness");

	glUniform3f(pointLightColorUniformLocation, pointLightPos.x, pointLightPos.y, pointLightPos.z);
	glUniform3f(pointLightColorUniformLocation, pointLightColor.x, pointLightColor.y, pointLightColor.z);
	glUniform1f(pointLightBrightnessUniformLocation, pointLightBrightness);
}
*/

void ambientLight()
{
	GLint ambientLightingUniformLocation = glGetUniformLocation(shader.Return_ID(), "ambientLight");
	float ambientLight = 1.0f;
	glUniform1f(ambientLightingUniformLocation, ambientLight);
}
void diffuseLight()
{
	glm::vec3 normalLightPosition;

	GLint directionalLightBrightness = glGetUniformLocation(shader.Return_ID(), "directionalLightBrightness");
	glUniform1f(directionalLightBrightness, diffuseBrightness);
	GLint lightPositionUniformLocation1 = glGetUniformLocation(shader.Return_ID(), "lightPositionWorldNormal");
	normalLightPosition = glm::vec3(0.0f, 10.0f, 0.0f);
	glUniform3fv(lightPositionUniformLocation1, 1, &normalLightPosition[0]);
	glm::vec3 color1(1.0f, 1.0f, 1.0f);//white
	GLint normalLightColor = glGetUniformLocation(shader.Return_ID(), "diffuseLightColorNormal");
	glUniform3fv(normalLightColor, 1, &color1[0]);
}

glm::vec3 PlanetDirection;
float PlanetRotateDirection;

glm::vec3 cameraFocus(0.0f);

glm::vec3 sunPos = glm::vec3(0.0f, 10.0f, 0.0f);
glm::mat4 viewMatrix = glm::mat4(1.0f);
glm::mat4 projectionMatrix;
/*
void paint_Planet()
{
	//initialize
	glm::mat4 modelMatrix = glm::mat4(1.0f); // Start with an identity matrix
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 10.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(radianY), glm::vec3(0, 1, 0));
	viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, cameraMoveY, -cameraMoveX));
	viewMatrix = glm::lookAt(cameraPos, cameraFocus, glm::vec3(0.0f, 0.0f, 1.0f));
	projectionMatrix = glm::perspective(glm::radians(45.0f + zoom), 1.5f, 1.0f, 100.0f);
	glm::vec3 sunPos = glm::vec3(0.0f, 10.0f, 0.0f);

	nmShader.setMat4("modelMatrix", modelMatrix);
	nmShader.setMat4("viewMatrix", viewMatrix);
	nmShader.setMat4("projectionMatrix", projectionMatrix);
	nmShader.setVec3("lightPos", sunPos);
	nmShader.setVec3("viewPos", cameraPos);
	nmShader.setInt("texColor", 0);
	nmShader.setInt("texNorm", 1);

	glActiveTexture(GL_TEXTURE0);
	texEarth.bind(0);
	nmShader.setInt("texColor", 0);

	glActiveTexture(GL_TEXTURE1);
	texNorm.bind(1);
	nmShader.setInt("texNorm", 1);

	glDrawElements(GL_TRIANGLES, PlanetObj.indices.size(), GL_UNSIGNED_INT, 0);
}
*/

void paint_Planet()
{
	glm::mat4 modelTransformMatrix;
	glm::mat4 modelRotateMatrix_X;
	glm::mat4 modelRotateMatrix_Y;
	glm::mat4 modelRotateMatrix_Z;
	glm::mat4 modelScaleMatrix;
	glm::mat4 modelMatrix;

	
	GLint modelMatrixUniformLocation;
	/*
	GLuint TextureID = glGetUniformLocation(shader.Return_ID(), "ourTexture");
	glActiveTexture(GL_TEXTURE0);
	texEarth.bind(0);
	glUniform1i(TextureID, 0);
	*/

	glActiveTexture(GL_TEXTURE0);
	texEarth.bind(0);
	GLint diffuseMapLocation = glGetUniformLocation(nmShader.Return_ID(), "diffuseMap");
	glUniform1i(diffuseMapLocation, 0);

	glActiveTexture(GL_TEXTURE1);
	texNorm.bind(1);
	GLint normalMapLocation = glGetUniformLocation(nmShader.Return_ID(), "normalMap");
	glUniform1i(normalMapLocation, 1);


	glBindVertexArray(PlanetVAO);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	modelRotateMatrix_X = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelRotateMatrix_Y = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	modelRotateMatrix_Z = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	modelScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
	modelTransformMatrix = modelMatrix * modelRotateMatrix_X * modelRotateMatrix_Y * modelRotateMatrix_Z * modelScaleMatrix;

	modelTransformMatrix = glm::translate(modelTransformMatrix, PlanetDirection);
	modelTransformMatrix = glm::rotate(modelTransformMatrix, glm::radians(PlanetRotateDirection), glm::vec3(0.0f, 1.0f, 0.0f));

	// Self rotation of the planet
	modelTransformMatrix = glm::rotate(modelTransformMatrix, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));


	modelMatrixUniformLocation = glGetUniformLocation(shader.Return_ID(), "modelTransformMatrix");
	glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, PlanetObj.indices.size(), GL_UNSIGNED_INT, 0);

	
	// Set the properties of the point light
	glm::vec3 lightPosition = glm::vec3(0.0f, -5.0f, -5.0f); // Adjust this to the desired position
	float lightConstant = 1.0f;
	float lightLinear = 0.09f; // Increase from 0.09f to 0.2f
	float lightQuadratic = 0.032f; // Increase from 0.032f to 0.07f

	// Send the point light properties to the shader
	glUniform3fv(glGetUniformLocation(shader.Return_ID(), "cameraPositionWorld"), 1, glm::value_ptr(cameraPos));
	glUniform3fv(glGetUniformLocation(shader.Return_ID(), "pointLight.position"), 1, glm::value_ptr(lightPosition));
	glUniform3fv(glGetUniformLocation(shader.Return_ID(), "pointLight.color"), 1, glm::value_ptr(pointLightColor));
	glUniform1f(glGetUniformLocation(shader.Return_ID(), "pointLight.constant"), lightConstant);
	glUniform1f(glGetUniformLocation(shader.Return_ID(), "pointLight.linear"), lightLinear);
	glUniform1f(glGetUniformLocation(shader.Return_ID(), "pointLight.quadratic"), lightQuadratic);
}

/*
void Create_Asteroid_Ring()
{
	float rotationSpeed = 0.001f; // Adjust this to change rotation speed.
	float rotationAngle = rotationSpeed * glfwGetTime(); // Calculate the rotation angle.

	for (GLuint i = 0; i < amount; i++)
	{
		// Extract the original translation from the model matrix
		glm::mat4 model = modelMatrices[i];
		glm::vec3 originalPos = glm::vec3(model[3]);

		// Reset the model matrix and apply the transformations in this order: translate -> rotate -> scale
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, originalPos);

		modelMatrices[i] = model; // Store the new model matrix
	}
}
*/
/*
void rotate_asteroid_ring()
{
	// Generate a large list of semi-random model transformation matrices
	srand(glfwGetTime()); // initialize random seed	
	float radius = 10.0;
	float offset = 1.5f;

	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices[i] = model;
	}
}
void Create_Asteroid_Ring()
{
	float rotationSpeed = 0.1f; // Adjust this to change rotation speed.
	float rotationAngle = rotationSpeed * glfwGetTime(); // Calculate the rotation angle.

	GLint modelMatrixUniformLocation;
	GLuint TextureID = glGetUniformLocation(shader.Return_ID(), "ourTexture");
	glActiveTexture(GL_TEXTURE0);
	RockTexture.bind(0);
	glUniform1i(TextureID, 0);

	glBindVertexArray(RockVAO);

	rotate_asteroid_ring();

	glm::mat4 rockOrbitIni = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 rockOrbit_M = glm::rotate(rockOrbitIni, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	for (GLuint i = 0; i < amount; i++)
	{
		glm::mat4 modelTransformMatrix = modelMatrices[i];
		int modelLoc = glGetUniformLocation(shader.Return_ID(), "modelTransformMatrix");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelTransformMatrix));
		glBindVertexArray(RockVAO);
		glDrawElementsInstanced(GL_TRIANGLES, RockObj.indices.size(), GL_UNSIGNED_INT, 0, amount);
	}
}
*/

void update_asteroid_ring() {
	float rotationSpeed = 1.0f; // Adjust this to change rotation speed.
	float rotationAngle = rotationSpeed * glfwGetTime(); // Calculate the rotation angle.

	for (unsigned int i = 0; i < amount; i++)
	{
		// Apply the rotation for this frame
		modelMatrices[i] = glm::rotate(modelMatrices[i], glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	}
}
void Create_Asteroid_Ring() {
	GLint modelMatrixUniformLocation;
	GLuint TextureID = glGetUniformLocation(shader.Return_ID(), "ourTexture");

	if (collision == false) {
		glActiveTexture(GL_TEXTURE0);
		RockTexture.bind(0);
		glUniform1i(TextureID, 0);
	}
	else
	{
		glActiveTexture(GL_TEXTURE0);
		RockTexture2.bind(0);
		glUniform1i(TextureID, 0);
	}

	glBindVertexArray(RockVAO);

	// Define the rotation speed and calculate the rotation angle.
	float rotationSpeed = 10.0f; // Adjust this to change rotation speed.
	float rotationAngle = rotationSpeed * glfwGetTime(); // Calculate the rotation angle.

	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

	for (GLuint i = 0; i < amount; i++)
	{
		// Apply the rotation for this frame to the model transformation matrix.
		glm::mat4 modelTransformMatrix = rotationMatrix * modelMatrices[i]; // Apply rotation around the origin

		int modelLoc = glGetUniformLocation(shader.Return_ID(), "modelTransformMatrix");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelTransformMatrix));
		glBindVertexArray(RockVAO);
		glDrawElementsInstanced(GL_TRIANGLES, RockObj.indices.size(), GL_UNSIGNED_INT, 0, amount);
	}
}

void paintSkybox()
{
	// Texture
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	skyboxShader.use();

	// Update camera focus based on spacecraft direction. Create a rotation matrix from the spacecraft direction
	glm::mat4 rotation =	glm::rotate(glm::mat4(1.0f), glm::radians(SpaceCraftDirection.x), glm::vec3(0.0f, 1.0f, 0.0f)) *
							glm::rotate(glm::mat4(1.0f), glm::radians(SpaceCraftDirection.y), glm::vec3(1.0f, 0.0f, 0.0f)) *
							glm::rotate(glm::mat4(1.0f), glm::radians(SpaceCraftDirection.z), glm::vec3(0.0f, 0.0f, 1.0f));

	viewMatrix = glm::lookAt(cameraPos, cameraFocus, glm::vec3(0.0f, 0.0f, 1.0f));
	viewMatrix = rotation * viewMatrix; // Rotate the view matrix

	// Convert viewMatrix to 3x3 matrix and then back to 4x4, effectively removing translation
	glm::mat4 skyboxViewMatrix = glm::mat4(glm::mat3(viewMatrix));

	projectionMatrix = glm::perspective(glm::radians(45.0f + zoom), 1.5f, 1.0f, 100.0f);
	skyboxShader.setMat4("view", skyboxViewMatrix);
	skyboxShader.setMat4("projection", projectionMatrix);
	glBindVertexArray(SkyboxVAO);

	glActiveTexture(GL_TEXTURE0);
	SkyboxTexture.bind(0);
	glDrawArrays(GL_TRIANGLES, 0, E_skybox);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
}


float calculateDistance(glm::vec3 position1, glm::vec3 position2) {
	return glm::length(position1 - position2);
}
void collision_check() {
	// Assuming you have position of spacecraft and asteroid ring.
	glm::vec3 spacecraftPosition = currentSpacecraftPosition;
	float spacecraftRadius = 12.0f; // Change this based on the actual size of your spacecraft model.

	for (GLuint i = 0; i < amount; i++) {
		glm::vec3 asteroidPosition = modelMatrices[i][3]; // Extract position from model matrix.
		float asteroidRadius = 10.0f; // Change this based on the actual size of your asteroid model.

		float distance = calculateDistance(spacecraftPosition, asteroidPosition);

		if (distance < spacecraftRadius + asteroidRadius) {
			collision = true;
		}
		else
		{
			collision = false;
		}
	}
}

void paintGL(void)  //always run
{
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //specify the background color, this is just an example
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	GLint projectionMatrixUniformLocation;
	GLint viewMatrixUniformLocation;

	viewMatrix = glm::lookAt(cameraPos, cameraFocus, glm::vec3(0.0f, 0.0f, 1.0f));
	viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, cameraMoveY, -cameraMoveX));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(radianY), glm::vec3(0, 1, 0));

	viewMatrixUniformLocation = glGetUniformLocation(shader.Return_ID(), "viewMatrix");
	glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, &viewMatrix[0][0]);

	projectionMatrix = glm::perspective(glm::radians(45.0f + zoom), 1.5f, 1.0f, 100.0f);
	projectionMatrixUniformLocation = glGetUniformLocation(shader.Return_ID(), "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

	glLineWidth(2.0f);
	ambientLight();
	//diffuseLight();

	collision_check();

	paintSkybox();

	//paintDog();
	//paintGround();

	shader.use();
	paint_SpaceCraft();
	paintCraft();
	Create_Asteroid_Ring();

	nmShader.use();
	paint_Planet();
	//TODO:
	//Set lighting information, such as position and color of lighting source
	//Set transformation matrix
	//Bind different textures

	glFlush();
}
int main(int argc, char* argv[])
{
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

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assignment 3", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/*register callback functions*/
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);                                                                    
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







