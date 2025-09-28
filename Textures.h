#pragma once
#include "Render.h"

class Texture
{
private:
	unsigned int RenderID;
	unsigned char* data;
	std::string filepath;
	int Height, Weight, nrChannels;
public:
	Texture(const std::string& path);
	~Texture();

	void Bind(unsigned int slot);
	void Unbind();

	inline int GetWeight() { return Weight; }
	inline int GetHeight() { return Height; }

};