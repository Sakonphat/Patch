// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Include others OpenGL helper 
#include <shader.hpp>
#include <controls.hpp>

#include "genmesh.h"


int main(void)
{
	// Initialise GLFW
	if (!glfwInit()){
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Polygon mesh construction", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.2 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Dark background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("perpixelPhong.vert", "perpixelPhong.frag");
	GLuint program_wireframe = LoadShaders("simpleColor.vert", "simpleColor.frag");
	GLuint program_flatshade = LoadShaders("perpixelPhong.vert", "perpixelPhong.frag");
	programID = program_wireframe;

	//array of vec3 to store Geometry 
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	//Create VBO
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);

	//Create+set VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	// 2nd attribute buffer : normals
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//Create new geometery each frame
	GenerateBezierPatch(vertices, normals,50.0f);

	//Transfer data to VBO
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_DYNAMIC_DRAW);

	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set the shader program
		glUseProgram(programID);

		// switch shading mode Wireframe or Flatshade
		if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS){
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			programID = program_wireframe;
		}
		if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS){
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			programID = program_flatshade;
		}
		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// send uniform variable to shader
		glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(programID, "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(programID, "ViewMatrix"), 1, GL_FALSE, &ViewMatrix[0][0]);

		glm::vec3 ObjColor = glm::vec3(1.0, 1.0, 0.0);
		glm::vec3 LightColor = glm::vec3(1.0, 1.0, 1.0);
		glm::vec3 lightPos = glm::vec3(20, 20, 20);
		float shiny = 100;
		glUniform1f(glGetUniformLocation(programID, "Shininess"), shiny);
		glUniform3f(glGetUniformLocation(programID, "Kd"), ObjColor.x, ObjColor.y, ObjColor.z);
		glUniform3f(glGetUniformLocation(programID, "Ld"), LightColor.x, LightColor.y, LightColor.z);
		glUniform3f(glGetUniformLocation(programID, "LightPosition"), lightPos.x, lightPos.y, lightPos.z);

		// Draw
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);


	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

