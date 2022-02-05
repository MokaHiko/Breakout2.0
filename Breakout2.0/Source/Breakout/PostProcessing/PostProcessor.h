#pragma once
#include "Shader.h"
#include "Texture.h"

class PostProcessor
{
public:
	// state
	Shader PostProcessingShader;
	Texture2D Texture;
	unsigned int Width, Height;

	bool Confuse, Chaos, Shake;

	PostProcessor(Shader shader, unsigned int width, unsigned int height);
	~PostProcessor();

	// prepares the postprocessor's framebuffer operations before rending the game;
	void BeginRender();

	// should be called after the rendering the game, stores all rendered data into a texture;
	void EndRender();

	// renders the PostPrcessor texture quad(as a screen-encompassing large sprite)
	void Render(float time);
private:
	// render state
	unsigned int MSFBO, FBO; // Multisampled FBO. FBo is regular framebuffer, used for blitting the MSColor buffer to the texture;
	unsigned int RBO; // rbo is used for multisampled color buffer
	unsigned int VAO;

	// initialze quad for renndering postprocessing texture
	void initRenderData();
};

