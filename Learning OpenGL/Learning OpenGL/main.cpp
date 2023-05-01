#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "stb_image.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
	// Initialise GLFW window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL Context version - 3.3 or higher
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Using core profile, doesn't give us backwards compatibility features (we don't need them)

	GLFWwindow* window = glfwCreateWindow(800, 600, "Learn OpenGL", NULL, NULL); // Make a window object. We use the create window functions, giving it a width, height, and title
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // Make the context of the window the main context on the current thread
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Callback function in case user resizes window

	// Initialise GLAD function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialise GLAD" << std::endl;
		return -1;
	}

	Shader shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag");

	float triangle[] = { // Normalised Device Coordinates (NDC), where bottom left is (-1, -1) and top right is (1, 1). Centre is (0, 0)
		// positions            // colours           // texCoords
		-0.5f,  0.5f,  0.0f,    1.0f, 1.0f, 0.0f,    0.0f, 1.0f,	// top left 0
		 0.5f,  0.5f,  0.0f,    1.0f, 0.7f, 0.0f,    1.0f, 1.0f,	// top right 1
		 0.5f, -0.5f,  0.0f,    1.0f, 0.2f, 0.0f,    1.0f, 0.0f,	// bottom right 2
		-0.5f, -0.5f,  0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f 	// bottom left 3
	};
	
	unsigned int indices[] = {
		0, 2, 3,
		0, 1, 2
	};

	unsigned int VBO, VAO, EBO; // Vertex buffer object, can store a large number of vertices on the GPU's memory; vertex array object, stores vertex attributes, vbos and ebos; element buffer object, stores indices to tell opengl which order to draw vertices in
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	
	glBindVertexArray(VAO); // Binds VAO

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // State setting function, binds VBO to the GL_ARRAY_BUFFER buffer, so anything that is done on GL_ARRAY_BUFFER is done on VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW); // Copies previously defined vertex data to the currently bound GL_ARRAY_BUFFER buffer's memory
  // Static draw is used because the data for what we're drawing doesn't change, and is used many times

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // copy index data to EBO

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // how opengl should interpret vertex data in VBO (position of vertex attribute in vertex shader, size of vertex attribute, data type (in this case we're using vec3 which means 3 floats, so for size we use 3 and data type we use GL_FLOAT), should normalise, stride i.e. bytes between vertices, offset of where position data begins)
	glEnableVertexAttribArray(0); // position of vertex attribute
	
	// Colour attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Texture coordinates attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// create and bind texture object
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set texture wrapping/filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // S axis is horizontal
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT); // T axis is vertical
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate texture
	int width, height, nrChannels; // width, height, and number of colour channels in image
	unsigned char* data = stbi_load("textures/metalbox_full.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window); // If escape key is pressed, next iteration (frame) the condition will be false (!true) so the render loop will end and the application terminates

		// bg
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // State setting function
		glClear(GL_COLOR_BUFFER_BIT); // State using function 

		// bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture); // already bound but good practice

		// shader
		shader.use();

		// update uniforms
		//float timeValue = glfwGetTime();
		//float greenValue = (sin(timeValue) / 2.0f) + 0.5f; // changing green value
		//int vertexColourLocation = glGetUniformLocation(shaderProgram, "inColour");
		//glUseProgram(shaderProgram); // if not already using shader program
		//glUniform4f(vertexColourLocation, 1.0f, greenValue, 0.0f, 1.0f); // set red to 1 so colour changes from red to yellow to red and so on
		
		//int xOffsetLocation = glGetUniformLocation(shader.ID, "in_xOffset");
		//glUniform1f(xOffsetLocation, 0.5f);

		// drawing
		glBindVertexArray(VAO); // already bound but good practice
		//glDrawArrays(GL_TRIANGLES, 0, 3); // Mode, starting index, number of vertices
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// swap buffers, check and call events
		glfwSwapBuffers(window); // Swap the colour buffer that is used to render during this iteration (frame) and output it to the screen, becuase images aren't drawn in an instant (left to right, top to bottom) so the front buffer is shown while the image is being rendered in the back buffer, then the buffers are swapped
		glfwPollEvents(); // Checks if events are triggered, for example keyboard input or mouse movement, updates the window state and calls corresponding funcions		
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate(); // Clean/delete all GLFW allocated resources
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // If the escape key is pressed
		glfwSetWindowShouldClose(window, true); // Set WindowShouldClose to true
}