#pragma once

//#include "../renderer/gxRenderer.h"
#include <vector>
#include "gxHWShader.h"
#include "../../Physics/core/types.h"
//#include "../core/gxSurfaceShader.h"

#define HW_BUILTIN_DEFAULT_SHADER_ONLY_DIFFUSE					0
#define HW_BUILTIN_DEFAULT_SHADER_ONLY_DIFFUSE_WITH_COLOR_PTR	1
#define HW_BUILTIN_DEFAULT_SHADER_ONLY_GUI_SHADER				2
#define HW_BUILTIN_DEFAULT_SHADER_ONLY_BLUR_SHADER				3
#define HW_BUILTIN_DEFAULT_SHADER_ONLY_SHADOWMAP_SHADER			4
#define HW_BUILTIN_DEFAULT_SHADER_ONLY_FOG_LINEAR_SHADER		5
#define HW_BUILTIN_DEFAULT_SHADER_ONLY_FOG_EXP_SHADER			6
#define HW_BUILTIN_DEFAULT_SHADER_ONLY_FOG_EXP2_SHADER			7

class HWShaderManager
{
public:

	struct stHWShaderSnippet
	{
		stHWShaderSnippet()
		{
			snippet=NULL;
			size=0;
		}
		~stHWShaderSnippet()
		{
			GX_DELETE_ARY(snippet);
		}
		char* snippet;
		long size;
	};

	HWShaderManager();
	~HWShaderManager();
	
	void Init();
	void Reset();
    void update(float dt);
	
#if defined (USE_ProgrammablePipeLine)
	gxHWShader* GetHWShader(int index);
#endif
    
//    stHWShaderSnippet* getShaderSnippet(int index)          {    return hwShaderSnippetList[index];    }
    gxHWShader* LoadShaderFromBuffer(const std::string& name, const std::string& buffer);
	gxHWShader* LoadShaderFromFile(const std::string& relativePath);

//    gxSurfaceShader* LoadSurfaceShader(const std::string& filename);
//    std::vector<gxSurfaceShader*>* getSurfaceShaderList()    {    return &surfaceShaderList;    }

private:
	void LoadDefaultShaders();
	stHWShaderSnippet* LoadCodeSnippet(const std::string& filename);
	
#if defined (USE_ProgrammablePipeLine)
//    std::vector<gxSurfaceShader*> surfaceShaderList;
	std::vector<gxHWShader*> hwShaderList;
//    std::vector<stHWShaderSnippet*> hwShaderSnippetList;
#endif
    
public:
    static HWShaderManager& GetHWShaderManager();
};
