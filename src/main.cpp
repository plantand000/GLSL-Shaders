#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "Program.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define NUM_LIGHTS 2
#define NUM_MATERIALS 3

GLFWwindow *window;

std::vector<Program> programs;
int selectedProgram = 0; // init to Gouraud Shader
std::vector<float> posBuff;
std::vector<float> norBuff;
std::vector<float> texBuff;

glm::vec3 eye(0.0f, 0.0f, 4.0f); // in world space

struct materialStruct {
	glm::vec3 ka, kd, ks;
	float s;
} materials[NUM_MATERIALS];
int selectedMaterial = 0; // index of selected material

struct lightStruct {
	glm::vec3 position;
	glm::vec3 color;
} lights[NUM_LIGHTS];
int selectedLight = 0;

void Display()
{		
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f);
	glm::mat4 viewMatrix = glm::lookAt(eye, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, -1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// calculate model inverse and transpose (may be backwards)
	glm::mat4 normalTransform = glm::transpose(glm::inverse(modelMatrix));
	programs[selectedProgram].Bind();
	programs[selectedProgram].SendUniformData(modelMatrix, "model");
	programs[selectedProgram].SendUniformData(viewMatrix, "view");
	programs[selectedProgram].SendUniformData(projectionMatrix, "projection");
	programs[selectedProgram].SendUniformData(normalTransform, "normalTransform");
	programs[selectedProgram].SendUniformData(eye, "e");
	// send material data
	programs[selectedProgram].SendUniformData(materials[selectedMaterial].ka, "ka");
	programs[selectedProgram].SendUniformData(materials[selectedMaterial].kd, "kd");
	programs[selectedProgram].SendUniformData(materials[selectedMaterial].ks, "ks");
	programs[selectedProgram].SendUniformData(materials[selectedMaterial].s, "s");
	// send light data
	programs[selectedProgram].SendUniformData(lights[0].position, "lights[0].position");
	programs[selectedProgram].SendUniformData(lights[0].color, "lights[0].color");
	programs[selectedProgram].SendUniformData(lights[1].position, "lights[1].position");
	programs[selectedProgram].SendUniformData(lights[1].color, "lights[1].color");
	glDrawArrays(GL_TRIANGLES, 0, posBuff.size() / 3);
	programs[selectedProgram].Unbind();
}

// Keyboard character callback function
void CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
	switch (key) 
	{
	case 'q':
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	case 'm':
		if (selectedMaterial == NUM_MATERIALS-1) {
			selectedMaterial = 0;
		} 
		else {
			selectedMaterial++;
		}
		std::cout << "Material " << selectedMaterial << " selected." << std::endl;
		break;
	case 'M':
		if (selectedMaterial == 0) {
			selectedMaterial = NUM_MATERIALS - 1;
		} 
		else {
			selectedMaterial--;
		}
		std::cout << "Material " << selectedMaterial << " selected." << std::endl;
		break;
	case '0':
		selectedProgram = 0;
		std::cout << "Program 0 selected." << std::endl;
		break;
	case '1':
		selectedProgram = 1;
		std::cout << "Program 1 selected." << std::endl;
		break;
	case '2':
		selectedProgram = 2;
		std::cout << "Program 2 selected." << std::endl;
		break;
	case 'l':
		if (selectedLight == NUM_LIGHTS - 1) {
			selectedLight = 0;
		} else {
			selectedLight++;
		}
		std::cout << "Light " << selectedLight << " selected." << std::endl;
		break;
	case 'L':
		if (selectedLight == 0) {
			selectedLight = NUM_LIGHTS - 1;
		} else {
			selectedLight--;
		}
		std::cout << "Light " << selectedLight << " selected." << std::endl;
		break;
	case 'x':
		lights[selectedLight].position.x += 0.2;
		break;
	case 'X':
		lights[selectedLight].position.x -= 0.2;
		break;
	case 'y':
		lights[selectedLight].position.y += 0.2;
		break;
	case 'Y':
		lights[selectedLight].position.y -= 0.2;
		break;
	case 'z':
		lights[selectedLight].position.z += 0.2;
		break;
	case 'Z':
		lights[selectedLight].position.z -= 0.2;
		break;
	default:
		break;
	}
}


void FrameBufferSizeCallback(GLFWwindow* lWindow, int width, int height)
{
	glViewport(0, 0, width, height);
}

void LoadModel(char* name)
{
	// Taken from Shinjiro Sueda with slight modification
	std::string meshName(name);
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string errStr;
	bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &errStr, meshName.c_str());
	if (!rc) {
		std::cerr << errStr << std::endl;
	}
	else {
		// Some OBJ files have different indices for vertex positions, normals,
		// and texture coordinates. For example, a cube corner vertex may have
		// three different normals. Here, we are going to duplicate all such
		// vertices.
		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces (polygons)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = shapes[s].mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
					if (!attrib.normals.empty()) {
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 0]);
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 1]);
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 2]);
					}
					if (!attrib.texcoords.empty()) {
						texBuff.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
						texBuff.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
					}
				}
				index_offset += fv;
				// per-face material (IGNORE)
				shapes[s].mesh.material_ids[f];
			}
		}
	}
}

void Init()
{
	glfwInit();
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment4 - Andrew Plant", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetCharCallback(window, CharacterCallback);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	LoadModel("../obj/bunny.obj");

	// materials information
	// material 1
	materials[0].ka = glm::vec3(0.2,0.2,0.2);
	materials[0].kd = glm::vec3(0.8,0.7,0.7);
	materials[0].ks = glm::vec3(1.0,1.0,1.0);
	materials[0].s = 10.0;
	// material 2
	materials[1].ka = glm::vec3(0.0,0.2,0.2);
	materials[1].kd = glm::vec3(0.5,0.7,0.2);
	materials[1].ks = glm::vec3(0.1,1.0,0.1);
	materials[1].s = 100.0;
	// material 3
	materials[2].ka = glm::vec3(0.2,0.2,0.2);
	materials[2].kd = glm::vec3(0.1,0.3,0.9);
	materials[2].ks = glm::vec3(0.1,0.1,0.1);
	materials[2].s = 1.0;

	//lighting information
	lights[0].position = glm::vec3(0.0,0.0,3.0);
	lights[0].color = glm::vec3(0.5,0.5,0.5);
	lights[1].position = glm::vec3(0.0,3.0,0.0);
	lights[1].color = glm::vec3(0.2,0.2,0.2);

	// Gouraud shaders
	Program program;
	program.SetShadersFileName("../shaders/shader1.vert", "../shaders/shader1.frag");
	program.Init();
	program.SendAttributeData(posBuff, "vPositionModel");
	program.SendAttributeData(norBuff, "vNormalModel");
	programs.push_back(program);
	
	// Phong Shaders cannot init multiple shaders bug
	Program program2;
	program2.SetShadersFileName("../shaders/shader2.vert", "../shaders/shader2.frag");
	program2.Init();
	program2.SendAttributeData(posBuff, "vPositionModel");
	program2.SendAttributeData(norBuff, "vNormalModel");
	programs.push_back(program2);

	// Silouette shaders
	Program program3;
	program3.SetShadersFileName("../shaders/shader3.vert", "../shaders/shader3.frag");
	program3.Init();
	program3.SendAttributeData(posBuff, "vPositionModel");
	program3.SendAttributeData(norBuff, "vNormalModel");
	programs.push_back(program3);
}


int main()
{	
	Init();
	while ( glfwWindowShouldClose(window) == 0) 
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Display();
		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}