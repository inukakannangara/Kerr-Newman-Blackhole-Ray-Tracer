#include "Shader.h"

using namespace std;

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	string vertexShaderCode;
	string fragmentShaderCode;
	ifstream vertexShaderFile;
	ifstream fragmentShaderFile;

	vertexShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fragmentShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

	try
	{
		vertexShaderFile.open(vertexShaderPath);
		fragmentShaderFile.open(fragmentShaderPath);
		stringstream vertexShaderStream, fragmentShaderStream;

		vertexShaderStream << vertexShaderFile.rdbuf();
		fragmentShaderStream << fragmentShaderFile.rdbuf();

		vertexShaderFile.close();
		fragmentShaderFile.close();

		vertexShaderCode = vertexShaderStream.str();
		fragmentShaderCode = fragmentShaderStream.str();
	}
	catch (ifstream::failure exception)
	{
		cout << "ERROR: shader file could not be read " << endl <<  exception.code() << endl;
	}

	const char* vertexShaderSource = vertexShaderCode.c_str();
	const char* fragmentShaderSource = fragmentShaderCode.c_str();

	unsigned int vertexShader, fragmentShader;
	int success;
	char infoLog[512];

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR: vertex shader compilation failed" << endl << infoLog << endl;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR: fragment shader compilation failed" << endl << infoLog << endl;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		cout << "ERROR: shader linking failed" << endl << infoLog << endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::deleteShader()
{
	glDeleteProgram(ID);
}

void Shader::setUniformFloat(const char* uniformName, float uniformValue)
{
	GLuint uniformLocation = glGetUniformLocation(ID, uniformName);
	glUniform1f(uniformLocation, uniformValue);
}

void Shader::setUniformVec3(const char* uniformName, vec3 uniformVec3)
{
	GLuint uniformLocation = glGetUniformLocation(ID, uniformName);
	glUniform3f(uniformLocation, uniformVec3.x, uniformVec3.y, uniformVec3.z);
}

void Shader::setUniformInt(const char* uniformName, int integer)
{
	GLuint uniformLocation = glGetUniformLocation(ID, uniformName);
	glUniform1i(uniformLocation, integer);
}
