#pragma once
#include <Shader.h>
#include <Texture.h>
class ResourceManager
{
public:
	static std::map<std::string, Shader> Shaders;
	static std::map<std::string, Texture2D> Textures;

	static Shader LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);

	//retrives a stored shader
	static Shader GetShader(std::string name);

	//loads (and generates) a texture from a file
	static Texture2D LoadTexture(const char* file, bool alpha, std::string name);

	//retreves a stored texture
	static Texture2D GetTexture(std::string name);

	//proeprly de-allocates all loaded resources
	static void Clear();

private:
	//private constructor because we do not actually want resource manager objects. Its members and functions should be publicly available(static).
	ResourceManager();

	//loads and generates a shader from a file
	static Shader loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);

	//loads a single texture from a file
	static Texture2D loadTextureFromFile(const char* file, bool alpha);
};


