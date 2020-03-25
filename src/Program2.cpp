
#include "Program2.h"
#include <iostream>
#include <cassert>
#include <fstream>

#include "GLSL.h"



void Program2::setShaderNames(const std::string &v, const std::string &f)
{
	vShaderName = v;
	fShaderName = f;
}

bool Program2::init()
{
	GLint rc;

	// Create shader handles
	GLuint VS = glCreateShader(GL_VERTEX_SHADER);
	GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);

	// Read shader sources
	std::string vShaderString = readFileAsString(vShaderName);
	std::string fShaderString = readFileAsString(fShaderName);
	const char *vshader = vShaderString.c_str();
	const char *fshader = fShaderString.c_str();
	CHECKED_GL_CALL(glShaderSource(VS, 1, &vshader, NULL));
	CHECKED_GL_CALL(glShaderSource(FS, 1, &fshader, NULL));

	// Compile vertex shader
	CHECKED_GL_CALL(glCompileShader(VS));
	CHECKED_GL_CALL(glGetShaderiv(VS, GL_COMPILE_STATUS, &rc));
	if (!rc)
	{
		if (isVerbose())
		{
			GLSL::printShaderInfoLog(VS);
			std::cout << "Error compiling vertex shader " << vShaderName << std::endl;
		}
		return false;
	}

	// Compile fragment shader
	CHECKED_GL_CALL(glCompileShader(FS));
	CHECKED_GL_CALL(glGetShaderiv(FS, GL_COMPILE_STATUS, &rc));
	if (!rc)
	{
		if (isVerbose())
		{
			GLSL::printShaderInfoLog(FS);
			std::cout << "Error compiling fragment shader " << fShaderName << std::endl;
		}
		return false;
	}

	// Create the program and link
	pid = glCreateProgram();
	CHECKED_GL_CALL(glAttachShader(pid, VS));
	CHECKED_GL_CALL(glAttachShader(pid, FS));
	CHECKED_GL_CALL(glLinkProgram(pid));
	CHECKED_GL_CALL(glGetProgramiv(pid, GL_LINK_STATUS, &rc));
	if (!rc)
	{
		if (isVerbose())
		{
			GLSL::printProgramInfoLog(pid);
			std::cout << "Error linking shaders " << vShaderName << " and " << fShaderName << std::endl;
		}
		return false;
	}

	return true;
}

void Program2::bind()
{
	CHECKED_GL_CALL(glUseProgram(pid));
}

void Program2::unbind()
{
	CHECKED_GL_CALL(glUseProgram(0));
}

void Program2::addAttribute(const std::string &name)
{
	attributes[name] = GLSL::getAttribLocation(pid, name.c_str(), isVerbose());
}

void Program2::addUniform(const std::string &name)
{
	uniforms[name] = GLSL::getUniformLocation(pid, name.c_str(), isVerbose());
}

GLint Program2::getAttribute(const std::string &name) const
{
	std::map<std::string, GLint>::const_iterator attribute = attributes.find(name.c_str());
	if (attribute == attributes.end())
	{
		if (isVerbose())
		{
			std::cout << name << " is not an attribute variable" << std::endl;
		}
		return -1;
	}
	return attribute->second;
}

GLint Program2::getUniform(const std::string &name) const
{
	std::map<std::string, GLint>::const_iterator uniform = uniforms.find(name.c_str());
	if (uniform == uniforms.end())
	{
		if (isVerbose())
		{
			std::cout << name << " is not a uniform variable" << std::endl;
		}
		return -1;
	}
	return uniform->second;
}
