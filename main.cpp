#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

string createVertexShader()
{
	string str = 
	"#version 330 core\n"
	"\n"
	"layout(location = 0) in vec3 pos;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0f);\n"
	"}\n\0";

	return str;
}

string createFragmentShader(float r, float g, float b)
{

	string str = 
	"#version 330 core\n"
	"\n"
	"out vec4 color;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	color = vec4("+ to_string(r)+", "+ to_string(g)+", "+ to_string(b)+", 1.0f);\n"
	"}\n\0";

	return str;
}

static unsigned int CompileShader(unsigned int type, const string &source)
{
	unsigned int id = glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char *message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);

		cout << "Err: Failed to compile  " 
			 << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
		  	 << " shader!" << endl << message << endl;

		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const string &vertexShader, const string &fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	// check for linking errors
	int success;
	char infoLog[512];

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::VALIDATION_FAILED\n" << infoLog << std::endl;
    }

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main(void)
{
	const unsigned int windowHeight = 720;
	const unsigned int windowWidth 	= 1280;

	// Init GLFW
	if (!glfwInit()){
		exit(EXIT_FAILURE);
		fprintf(stderr, "Err: GLFW!");
	}


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Let's create window
	GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "So clear", NULL, NULL );

	if(!window){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	// Init GLEW
	if (glewInit() != GLEW_OK){
		exit(EXIT_FAILURE);
		fprintf(stderr, "Err: GLEW!");
	}
	// glfwSwapInterval(1);


	float positions[2][9] = {
		{
		-0.5,  0.5, -1.0,	// Left UP
		 0.5,  0.5,  1.0,	// Right Up
		 0.5, -0.5, -1.0	// Right Down
		},
		{
		-0.7,  0.0,	-0.5,	// left middle
		 0.7,  0.7,	-0.5,	// right up
		   0, -0.7,	-0.5	// middle down
		}
	};


	unsigned int VAOs[2];
	unsigned int VBOs[2];
	// unsigned int EBOs[2];	
	glGenVertexArrays(2, VAOs);
	glGenBuffers(2, VBOs);

	//---------First Triangle Setup----------
	//---------------------------------------
    glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]), positions[0], GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//---------Second Triangle Setup----------
	//----------------------------------------
    glBindVertexArray(VAOs[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions[1]), positions[1], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
    glBindVertexArray(0); // Unbind VAO


	string vertexShader = createVertexShader();
	string fragmentShaderRed = createFragmentShader(1.0, 0.0, 0.0);
	string fragmentShaderBlue = createFragmentShader(0.0, 0.0, 1.0);


	unsigned int shaderRed = CreateShader(vertexShader, fragmentShaderRed);
	unsigned int shaderBlue = CreateShader(vertexShader, fragmentShaderBlue);

	// Window loop
	while(!glfwWindowShouldClose(window))
	{
		// Render
		glClearColor(0.09f, 0.1f, 0.13f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// glDrawArrays(GL_TRIANGLES, 0, 3); <----------Without EBO

		glUseProgram(shaderRed);
		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glUseProgram(shaderBlue);
		glBindVertexArray(VAOs[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

	    glBindVertexArray(0); // Unbind VAO

		//Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();

	}

    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
	glDeleteProgram(shaderRed);
	glDeleteProgram(shaderBlue);

 	glfwTerminate();

 	exit(EXIT_SUCCESS);
}