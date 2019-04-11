#include "HWShaderManager.h"
#include "../OSUtils.h"

HWShaderManager& HWShaderManager::GetHWShaderManager() {
    static HWShaderManager instance;
    return instance;
}

HWShaderManager::HWShaderManager()
{
}

HWShaderManager::~HWShaderManager()
{
	Reset();
}

void HWShaderManager::Init()
{
#if defined (USE_ProgrammablePipeLine)
        LoadDefaultShaders();
#else
#error "Not implemented"
#endif
}

void HWShaderManager::Reset()
{
#if defined (USE_ProgrammablePipeLine)
	for(std::vector<gxHWShader*>::iterator it = hwShaderList.begin(); it != hwShaderList.end(); ++it)
	{
		gxHWShader* shader = *it;
		GX_DELETE(shader);
	}
	hwShaderList.clear();

//    std::vector<stHWShaderSnippet*>* submaplist=&hwShaderSnippetList;
//    for(std::vector<stHWShaderSnippet*>::iterator it = submaplist->begin(); it != submaplist->end(); ++it)
//    {
//        stHWShaderSnippet* snippet = *it;
//        GX_DELETE(snippet);
//    }
//    hwShaderSnippetList.clear();

//    for(std::vector<gxSurfaceShader*>::iterator it = surfaceShaderList.begin(); it != surfaceShaderList.end(); ++it)
//    {
//        gxSurfaceShader* surfaceshader = *it;
//        GX_DELETE(surfaceshader);
//    }
//    surfaceShaderList.clear();
#else
#error "Not implemented"
#endif
}

//gxSurfaceShader* HWShaderManager::LoadSurfaceShader(const std::string& filename)
//{
//    for(std::vector<gxSurfaceShader*>::iterator it = surfaceShaderList.begin(); it != surfaceShaderList.end(); ++it)
//    {
//        gxSurfaceShader* surfaceshader = *it;
//        if(strcmp(gxUtil::getFileNameFromPath(filename.c_str()), surfaceshader->getSurfaceShaderFileName())==0)
//            return surfaceshader;
//    }
//
//    gxSurfaceShader* newShader = new gxSurfaceShader();
//    if(!newShader->loadSurfaceShader(filename.c_str()))
//    {
//        GX_DELETE(newShader);
//        return NULL;
//    }
//
//    surfaceShaderList.push_back(newShader);
//    return newShader;
//}

void HWShaderManager::LoadDefaultShaders()
{
#if defined (USE_ProgrammablePipeLine)

	std::string resource_dir_root_path;

#if defined(GEAR_WINDOWS)
	resource_dir_root_path="res/shadersWin32/";
#elif defined(GEAR_ANDROID)
	resource_dir_root_path=OSUtils::cpp_getPath("/res/shadersWin32/");
#elif defined(GEAR_APPLE)
    resource_dir_root_path=OSUtils::cpp_getPath("/res/shadersWin32/");
#else
	DEBUG_PRINT("ERROR : Shaders not implemented for this platform ");
	return;
#endif

//    //load code snippets
//    stHWShaderSnippet* snippet=LoadCodeSnippet(resource_dir_root_path+"snippets/matrices_uniform_vars.snippet");
//    if(snippet)
//        hwShaderSnippetList.push_back(snippet);
//    snippet=LoadCodeSnippet(resource_dir_root_path+"snippets/vertex_attrib_vars.snippet");
//    if(snippet)
//        hwShaderSnippetList.push_back(snippet);
//    snippet=LoadCodeSnippet(resource_dir_root_path+"snippets/vertex_main.snippet");
//    if(snippet)
//        hwShaderSnippetList.push_back(snippet);
//    snippet=LoadCodeSnippet(resource_dir_root_path+"snippets/fragment_main.snippet");
//    if(snippet)
//        hwShaderSnippetList.push_back(snippet);
//    snippet=LoadCodeSnippet(resource_dir_root_path+"snippets/light_unifrom_vars.snippet");
//    if(snippet)
//        hwShaderSnippetList.push_back(snippet);
//    snippet=LoadCodeSnippet(resource_dir_root_path+"snippets/material_unifrom_vars.snippet");
//    if(snippet)
//        hwShaderSnippetList.push_back(snippet);
//    snippet = LoadCodeSnippet(resource_dir_root_path + "snippets/vertex_with_shadow_main.snippet");
//    if (snippet)
//        hwShaderSnippetList.push_back(snippet);
//    snippet = LoadCodeSnippet(resource_dir_root_path + "snippets/fragment_with_shadow_main.snippet");
//    if (snippet)
//        hwShaderSnippetList.push_back(snippet);

	//HW shaders
	gxHWShader* pShader=new gxHWShader();
    if(pShader->loadShader(resource_dir_root_path+"hwshader/only_diffuse.glsl"))	//0
		hwShaderList.push_back(pShader);
	else
		GX_DELETE(pShader);
	pShader=new gxHWShader();
    if(pShader->loadShader(resource_dir_root_path+"hwshader/only_diffuse_with_color_pointer.glsl"))	//1
		hwShaderList.push_back(pShader);
	else
		GX_DELETE(pShader);
	pShader=new gxHWShader();
    if(pShader->loadShader(resource_dir_root_path+"hwshader/guishader.glsl"))	//2
		hwShaderList.push_back(pShader);
	else
		GX_DELETE(pShader);
	pShader=new gxHWShader();
    if(pShader->loadShader(resource_dir_root_path+"hwshader/blurshader.glsl"))	//3
		hwShaderList.push_back(pShader);
	else
		GX_DELETE(pShader);
	pShader=new gxHWShader();
    if(pShader->loadShader(resource_dir_root_path+"hwshader/shadowmap.glsl"))	//4
		hwShaderList.push_back(pShader);
	else
		GX_DELETE(pShader);
	pShader=new gxHWShader();
    if(pShader->loadShader(resource_dir_root_path+"hwshader/fog_linear_shader.glsl"))	//5
		hwShaderList.push_back(pShader);
	else
		GX_DELETE(pShader);
	pShader=new gxHWShader();
    if(pShader->loadShader(resource_dir_root_path+"hwshader/fog_exp_shader.glsl"))	//6
		hwShaderList.push_back(pShader);
	else
		GX_DELETE(pShader);
	pShader=new gxHWShader();
    if(pShader->loadShader(resource_dir_root_path+"hwshader/fog_exp2_shader.glsl"))	//7
		hwShaderList.push_back(pShader);
	else
		GX_DELETE(pShader);

    //uv_with_color.glsl
    pShader=new gxHWShader();
    if(pShader->loadShader(resource_dir_root_path+"hwshader/uv_with_color.glsl"))    //8
        hwShaderList.push_back(pShader);
    else
        GX_DELETE(pShader);
    
	//surface shaders
//    LoadSurfaceShader(resource_dir_root_path+"surfaceShader/Diffuse.shader");
//    LoadSurfaceShader(resource_dir_root_path+"surfaceShader/Diffuse_vlit.shader");
//    LoadSurfaceShader(resource_dir_root_path+"surfaceShader/NormalMap.shader");
//    LoadSurfaceShader(resource_dir_root_path+"surfaceShader/NormalMapSpecular.shader");
//    LoadSurfaceShader(resource_dir_root_path+"surfaceShader/MultiTex.shader");
//    LoadSurfaceShader(resource_dir_root_path+"surfaceShader/NormalMapSeperateSpecular.shader");
//    LoadSurfaceShader(resource_dir_root_path+"surfaceShader/NormalMapSeperateEmissive.shader");
//    LoadSurfaceShader(resource_dir_root_path+"surfaceShader/NormalMapSeperateSpecularEmissive.shader");
#else
#error "Not implemented."
#endif
}

gxHWShader* HWShaderManager::LoadShaderFromFile(const std::string& relativePath)
{
	std::string resource_dir_root_path;

#if defined(GEAR_WINDOWS)
	resource_dir_root_path = "res/shadersWin32/";
#elif defined(GEAR_ANDROID)
	resource_dir_root_path = OSUtils::cpp_getPath("/res/shadersWin32/");
#elif defined(GEAR_APPLE)
	resource_dir_root_path = OSUtils::cpp_getPath("/res/shadersWin32/");
#else
    #error ERROR : Shaders not implemented for this platform ";
#endif

	//HW shaders
	gxHWShader* pShader = new gxHWShader();
	if (pShader->loadShader(resource_dir_root_path + relativePath))
		hwShaderList.push_back(pShader);
	else
		GX_DELETE(pShader);

	return pShader;
}

gxHWShader* HWShaderManager::LoadShaderFromBuffer(const std::string& name, const std::string& buffer)
{
	for(std::vector<gxHWShader*>::iterator it = hwShaderList.begin(); it != hwShaderList.end(); ++it)
	{
		gxHWShader* hwshaderNode = *it;
		if(hwshaderNode->getShaderName().compare(name)==0)
			return hwshaderNode;
	}

	if(buffer.length()==0) return NULL;

	gxHWShader* shader = new gxHWShader();
	if(!shader->loadShaderFromBuffer(name, buffer))
	{
		GX_DELETE(shader);
	}
	hwShaderList.push_back(shader);
	return shader;
}

//HWShaderManager::stHWShaderSnippet* HWShaderManager::LoadCodeSnippet(const std::string& filename)
//{
//    //read snippet code
//    long fileSz=0;
//    FILE* fp=fopen(filename.c_str(), "r");
//    if(fp==NULL) return NULL;
//
//    fseek(fp, 0, SEEK_END);
//    fileSz=ftell(fp);
//    fclose(fp);
//    
//    if(!fileSz) return NULL;
//    
//    //vertex shader source
//    fp=fopen(filename.c_str(), "r");
//    if(fp==NULL) return NULL;
//
//    stHWShaderSnippet* newSnippetCode = new stHWShaderSnippet();
//
//    newSnippetCode->size=fileSz;
//    newSnippetCode->snippet=new char[newSnippetCode->size+1];
//    memset((void*)newSnippetCode->snippet, 0, newSnippetCode->size);
//    fread((void*)newSnippetCode->snippet, 1, newSnippetCode->size, fp);
//    fclose(fp);
//    newSnippetCode->snippet[newSnippetCode->size]='\0';
//    //
//
//#if GEAR_DEBUG
//    //DEBUG_PRINT("SNIPPET:\n%s", newSnippetCode->snippet);    //Note:- Heap will corrupt if the buffer size is greater than FILENAME_MAX
//    DEBUG_PRINT("%s loaded", filename.c_str());
//#endif
//
//    return newSnippetCode;
//}

#if defined (USE_ProgrammablePipeLine)
gxHWShader* HWShaderManager::GetHWShader(int index)
{
	if(index>=(int)hwShaderList.size()) return NULL;
	return hwShaderList[index];
}
#endif

void HWShaderManager::update(float dt)
{
//#if defined (USE_ProgrammablePipeLine)
//	for(std::vector<gxHWShader*>::iterator it = hwShaderList.begin(); it != hwShaderList.end(); ++it)
//	{
//		gxHWShader* hwshaderNode = *it;
//		hwshaderNode->updateShaderVars(dt);
//	}
//#endif
}
