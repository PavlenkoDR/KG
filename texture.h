#ifndef __INCLUDE_MY_TEXTURE_

#define __INCLUDE_MY_TEXTURE_

#include <SOIL/SOIL.h>
#include <string>
#include <GL/glew.h>
#include <iostream>


class texture{
private:
	//int width, height;
	//unsigned char *image;
    std::string m_fileName;
    //GLenum m_textureTarget;
    GLuint m_textureObj;
public:
	int width, height;
	unsigned char *image;
    GLenum m_textureTarget;
	texture();
	texture(GLenum TextureTarget, const std::string &src);
	~texture();
	inline const unsigned char *_image() const{return image;};
	inline int _width()								{return width;};
	inline int _height()								{return height;};
    bool Load();
    void Bind(GLenum TextureUnit);
};

#endif
