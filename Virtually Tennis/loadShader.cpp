#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>

#include <GL/glew.h>
#include "loadShader.h"

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderString;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in); // vertex_file_path is the shader file to be read
	if(VertexShaderStream.is_open()){ // if the file exists with correct path and opens successfully
		std::string Line = "";
		while(getline(VertexShaderStream, Line)) // extracted from the file stream 'vertexShaderStream'
			VertexShaderString += "\n" + Line; // and put it into the shader string 'VertexShaderString'
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderString;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderString += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderString.c_str(); // convert 'string' to C char*, required by the function 'glShaderSource'
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL); // associate the shader string to teh shaderID
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result); 	// query the value of 'Compile Status', return 'GL_TRUE' is the compilation is successful
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength); // query the number of character in the log information
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]); // print the log information
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderString.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram(); // create a 'ProgramID' for storing a combined shader programme
	glAttachShader(ProgramID, VertexShaderID); // attach a shader to the programme
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID); // link all shaders together

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result); // query the link status
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID); // free memory used by a shader
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}


