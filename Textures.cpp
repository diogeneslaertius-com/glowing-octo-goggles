#include "Textures.h"
#include "Render.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_resize2.h"

Texture::Texture(const std::string& path)
	:RenderID(0), filepath(path),
	Height(0), Weight(0), nrChannels(0), data(nullptr)
{
	stbi_set_flip_vertically_on_load(1);
	data = stbi_load(path.c_str(), &Weight, &Height, &nrChannels, 0);
	glGenTextures(1, &RenderID);
	glBindTexture(GL_TEXTURE_2D, RenderID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Weight, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cerr << "Textures is not defined " << path << std::endl;
	}
	stbi_image_free(data);
}

Texture::~Texture()
{
	glDeleteTextures(1, &RenderID);
}

void Texture::Bind(unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, RenderID);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);

}
