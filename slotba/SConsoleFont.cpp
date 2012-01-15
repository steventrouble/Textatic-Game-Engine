#include "SConsoleFont.h"
#include "lodepng.h"

#include "GL/glfw.h"
#include <GL/GL.h>

using namespace std;

SConsoleFont::SConsoleFont(string path, unsigned int width, unsigned int height,
	unsigned int charWidth, unsigned int charHeight) 
{
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	// when texture area is small, bilinear filter the closest mipmap
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	// when texture area is large, bilinear filter the original
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	// the texture wraps over at the edges (repeat)
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glfwLoadTexture2D(path.c_str(),GLFW_ORIGIN_UL_BIT);

	m_charWidth = charWidth;
	m_charHeight = charHeight;
	m_width = width;
	m_height = height;
	//delete[] imageData;
}

SConsoleFont::~SConsoleFont() {
	glDeleteTextures(1,&m_textureID);
	return;
}

void SConsoleFont::putChar(unsigned short ch, int x, int y) {
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	//float tim = (((x+y)%2)/4.0)+0.75;
	glColor3f(1.0,1.0,1.0);
	float pos = (float) m_charWidth*(ch);
	int xpos = m_charWidth*x;
	int ypos = m_charHeight*y;
	glBegin(GL_QUADS);
			glTexCoord2f(pos/m_width, 0);				
		glVertex2i(xpos,				ypos);
			glTexCoord2f(pos/m_width, m_charHeight/(float) m_height);
		glVertex2i(xpos,				ypos+m_charHeight);
			glTexCoord2f((pos+m_charWidth)/m_width, m_charHeight/(float) m_height);	
		glVertex2i(xpos+m_charWidth,	ypos+m_charHeight);
			glTexCoord2f((pos+m_charWidth)/m_width, 0);				
		glVertex2i(xpos+m_charWidth,	ypos);
	glEnd();
	return;
}

void SConsoleFont::putChar(unsigned short ch, float x, float y) {
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	//float tim = (((x+y)%2)/4.0)+0.75;
	glColor3f(1.0f,1.0f,1.0f);
	float pos = (float) m_charWidth*(ch);
	float xpos = m_charWidth*x;
	float ypos = m_charHeight*y;
	glBegin(GL_QUADS);
			glTexCoord2f(pos/m_width, 0);				
		glVertex2f(xpos,				ypos);
			glTexCoord2f(pos/m_width, m_charHeight/(float) m_height);
		glVertex2f(xpos,				ypos+m_charHeight);
			glTexCoord2f((pos+m_charWidth)/m_width, m_charHeight/(float) m_height);	
		glVertex2f(xpos+m_charWidth,	ypos+m_charHeight);
			glTexCoord2f((pos+m_charWidth)/m_width, 0);				
		glVertex2f(xpos+m_charWidth,	ypos);
	glEnd();
	return;
}