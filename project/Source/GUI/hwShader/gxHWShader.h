#pragma once

//#include "../renderer/gxRenderer.h"
#include "../gxDebug.h"
#include <vector>
//#include "../core/object3d.h"
#include <iostream>	//included to support 'std::string' in android
#include <string>
#if GEAR_APPLE
    #include <OpenGL/gl.h>
#elif GEAR_ANDROID
    #include <GLES2/gl2.h>
#else
    #include <gl.h>
#endif
#include "../../Physics/core/types.h"

//class object3d;
struct stUniformLocation
{
	stUniformLocation()
	{
		uniform_ref=-1;
		memset(uniform_ref_name, 0, sizeof(uniform_ref_name));
	}

	int uniform_ref;
	char uniform_ref_name[64];	//uniform vars in shaders must not exceed 64 charachters
};

struct stAttribLocation
{
	stAttribLocation()
	{
		attrib_ref=-1;
		memset(attrib_ref_name, 0, sizeof(attrib_ref_name));
	}

	int attrib_ref;
	char attrib_ref_name[64];	//uniform vars in shaders must not exceed 64 charachters
};

class gxHWShader
{
public:
	gxHWShader();
	virtual ~gxHWShader();

	bool loadShader(const std::string& shaderFile);
	bool loadShaderFromBuffer(const std::string& name, const std::string& shaderBuffer);
	bool compileShader(GLuint* shader, GLenum type, const std::string& source);
	bool linkProgram();
	
	void attachShader();
	void detachShader();
	void destroyShader();
	void enableProgram();
	void disableProgram();
	bool validateProgram();
	
	GLuint getProgram()			{	return shaderProgram;		}
	GLuint getVertexShader()	{	return vertexShaderID;	}
	GLuint getFragmentShader()	{	return fragmentShaderID;	}
	
	void showShaderLog(const std::string& title);

	int getUniformLoc(const char* uvar);
	virtual int getAttribLoc(const char* uvar);

	void sendUniform1f(const char* name, float x);
	void sendUniform2f(const char* name, float x, float y);
	void sendUniform3f(const char* name, float x, float y, float z);
	void sendUniform4f(const char* name, float x, float y, float z, float w);
	void sendUniform1i(const char* name, int x);
	void sendUniform2i(const char* name, int x, int y);
	void sendUniform3i(const char* name, int x, int y, int z);
	void sendUniform4i(const char* name, int x, int y, int z, int w);
	void sendUniformTMfv(const char* name, const float* matrix, bool transpose, int size);
	void sendUniform2fv(const char* name, const float* input);
	void sendUniform3fv(const char* name, const float* input);
	void sendUniform4fv(const char* name, const float* input);
	void sendUniform4fv(const char* name, const float* input, int nVectors);

	void sendAttrib2fv(const char* name, const float* input);
	void sendAttrib3fv(const char* name, const float* input);
	void sendAttrib4fv(const char* name, const float* input);
	void sendAttrib1f(const char* name, float x);
    
	const std::string& getShaderName()	{	return shaderName; }

	//predefined vars
	void sendUniform_GEAR_MODELVIEW(const float* input);
	void sendUniform_GEAR_MVP(const float* input);
	void sendUniform_GEAR_MODEL_MATRIX(const float* input);
	void sendUniform_GEAR_MODEL_INVERSE(const float* input);
	void sendUniform_material_diffuse(const float* input);
	void sendUniform_material_ambient(const float* input);
	void sendUniform_material_specular(const float* input);
	void sendUniform_material_shininess(float input);
	void sendUniform_GEAR_Time(const float* time);
	void sendUniform_GEAR_ScreenParams(const float* screenParams);

	int getAttrib_vIN_Position();
	int getAttrib_vIN_Normal();
	int getAttrib_Tangent();

private:
	void clearUniformRefVarList();
	void clearAttribRefVarList();

    GLuint shaderProgram;
	GLuint vertexShaderID;
	GLuint fragmentShaderID;

	std::vector<stUniformLocation*> uniformRefVariableList;
	std::vector<stAttribLocation*> attribRefVariableList;
	std::string shaderName;

	//predefined vars
	int unifrom_GEAR_MVP;
	int unifrom_GEAR_MODELVIEW;
	int unifrom_GEAR_MODEL_MATRIX;
	int unifrom_GEAR_MODEL_INVERSE;
	int unifrom_material_diffuse;
	int unifrom_material_ambient;
	int unifrom_material_specular;
	int unifrom_material_shininess;
	int unifrom_GEAR_Time;
	int unifrom_GEAR_ScreenParams;

	int attrib_vIN_Position;
	int attrib_vIN_Normal;
	int attrib_Tangent;
};
