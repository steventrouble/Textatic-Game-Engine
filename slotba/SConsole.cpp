#include "SConsole.h"
#include "SConsoleFont.h"

#include <iostream>
#include <cstring>

#include "GL\glfw.h"
#include <GL\GL.h>

#define NUM_BUFFERS 2

using namespace std;

const int swapSteps = 8;

int SConsole::createConsole(string title) {
	//glutInitWindowPosition(-1,-1);
	glfwInit();
	glfwSetWindowTitle(title.c_str());
	int width = m_charWidth*m_charsWide;
	int height = m_charHeight*m_charsTall;
	glfwOpenWindow(width, height, 8, 8, 8, 0, 8, 0, GLFW_WINDOW);
		
	glEnable(GL_TEXTURE_2D);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glOrtho(0,width,height,0,-100,100);
	return 0;
}

void SConsole::terminateConsole() {
	glfwCloseWindow();
	glfwTerminate();
	return;
}

int SConsole::loadFont(string path) {
	m_font = new SConsoleFont(path,1024,16,m_charWidth,m_charHeight);
	if (m_font->isLoaded()) return 0; // If loaded successfully, return 0
	doError("SConsole: loadFont: Font did not load correctly.");
	delete m_font;
	return -1; // Else do error and return -1
}

void SConsole::putChar(unsigned short toPut, unsigned int x, unsigned int y) {
	m_terminalChars[y*m_charsWide+x] = toPut;
	SConsoleRect rect = {x,y,x,y};
	m_renderQueue->push_back(rect);
	return;
}

void SConsole::putChars(charPos arr[], int len) {
	for (int i = 0; i < len; i++) {
		charPos& cur = arr[i];
		m_terminalChars[cur.y*m_charsWide+cur.x] = cur.ch;
		SConsoleRect rect = {cur.x,cur.y,cur.x,cur.y};
		m_renderQueue->push_back(rect);
	}
	return;
}

void SConsole::putChars(short array[], int len, int x, int y) {
	int endx = (x+len)%m_charsWide;
	int endy = y+len/m_charsWide;
	int end = endx+endy*m_charsWide;
	int it = 0;
	for (int i = x+y*m_charsWide; i < end; i++) {
		m_terminalChars[i] = array[it++];
	}
	SConsoleRect rect = {x, y, endx, endy};
	m_renderQueue->push_back(rect);
	return;
}

void SConsole::conCpy(short* from) {
	short *size = m_charsWide*m_charsTall+from;
	short *it, *it2;
	for (it = from, it2 = m_terminalChars; it < size; it++, it2++)
		if (*it != -1) *it2 = *it;
	SConsoleRect rect = {0, 0, m_charsWide, m_charsTall};
	m_renderQueue->push_back(rect);
	return;
}

void SConsole::moveChar(int x1, int y1, int x2, int y2) 
{
	short toMove = m_terminalChars[x1+y1*m_charsWide];
	short toSwap = m_terminalChars[x2+y2*m_charsWide];
	SConsoleRect rect = {x1,y1,x2,y2};
	float ii = (float) x1;
	float jj = (float) y1;
	float istep = (float) (x2-x1)/((float) swapSteps);
	float jstep = (float) (y2-y1)/((float) swapSteps);
	glfwSwapInterval(1);
	for (int iter = 0; iter < swapSteps; iter++) {
		m_renderQueue->push_back(rect);
		refreshNoSwap();
		m_font->putChar(toSwap, (int) x1, y1);
		m_font->putChar(toMove, ii+=istep, jj+=jstep);
		glfwSwapBuffers();
	}
	glfwSwapInterval(0);
	putChar(toSwap,x1,y1);
	putChar(toMove,x2,y2);
	refresh();
	return;
}

#define mkArr(type,name,size) type *name = new type[size];
#define foreach for (int i=0; i < len; i++)
#define x1i arr[i].x1
#define x2i arr[i].x2
#define y1i arr[i].y1
#define y2i arr[i].y2

void SConsole::moveChars(SConsoleRect arr[], int len) 
{
	mkArr(short,toMove,len);
	short *toSwap = new short[len];
	foreach toMove[i] = m_terminalChars[x1i+y1i*m_charsWide];
	foreach toSwap[i] = m_terminalChars[x2i+y2i*m_charsWide];
	mkArr(float,ii,len);
	mkArr(float,jj,len);
	mkArr(float,istep,len);
	mkArr(float,jstep,len);
	foreach ii[i] = x1i;
	foreach jj[i] = y1i;
	foreach istep[i] = (x2i-x1i)/((float) swapSteps);
	foreach jstep[i] = (y2i-y1i)/((float) swapSteps);
	SConsoleRect all = {0,0,m_charsWide,m_charsTall};
	glfwSwapInterval(1);
	for (int iter = 0; iter < swapSteps; iter++) {
		m_renderQueue->push_back(all);
		refreshNoSwap();
		foreach m_font->putChar(toSwap[i], (int) x1i, y1i);
		foreach m_font->putChar(toMove[i], ii[i]+=istep[i], jj[i]+=jstep[i]);
		glfwSwapBuffers();
	}
	glfwSwapInterval(0);
	foreach putChar(toSwap[i],x1i,y1i);
	foreach putChar(toMove[i],x2i,y2i);
	refresh();

	delete[] toMove;
	delete[] toSwap;
	delete[] ii;
	delete[] jj;
	delete[] istep;
	delete[] jstep;
	return;
}

void SConsole::fillRect(unsigned short toPut, unsigned int x1, unsigned int y1,
					unsigned int x2, unsigned int y2) {
	SConsoleRect rect = {x1,y1,x2,y2};
	fillRect(toPut,rect);
	return;
}

void SConsole::fillRect(unsigned short toPut, const SConsoleRect& rect) {
	int y2Tw = rect.y2 * m_charsWide;
	for (int jj = rect.y1*m_charsWide; jj <= y2Tw; jj+=m_charsWide) {
		int x2Tj = rect.x2 + jj;
		for (int ii = rect.x1+jj; ii <= x2Tj; ii++)
			m_terminalChars[ii] = toPut;
	}
	m_renderQueue->push_back(rect);
}

void SConsole::refresh() {
	for (int buffer = 0; buffer < NUM_BUFFERS; buffer++) {
		int iter = 0;
		while (m_renderQueue->size() != iter) {
			SConsoleRect* rect = &(m_renderQueue->at(iter));
			for (int j = rect->y1; j <= rect->y2; j++)
				for (int i = rect->x1; i <= rect->x2; i++)
					m_font->putChar(m_terminalChars[j*m_charsWide+i],i,j);
			if (buffer == NUM_BUFFERS-1)
				m_renderQueue->pop_front();
			else iter++;
		}
		glfwSwapBuffers();
	}
	return;
}

void SConsole::refreshNoSwap() {
	while (m_renderQueue->size()) {
		SConsoleRect* rect = &(m_renderQueue->front());
		for (int j = rect->y1; j <= rect->y2; j++)
			for (int i = rect->x1; i <= rect->x2; i++)
				m_font->putChar(m_terminalChars[j*m_charsWide+i],i,j);
		m_renderQueue->pop_front();
	}
	return;
}


/////////////////////////////////////////////////////
void printAboutScreen(SConsole& console) {
#include "about.h"
	short *arr = new short[40*80];
	for (int i = 0; i < 40*80; i++)
		arr[i] = muffins[i];
	SConsoleRect *swappers = new SConsoleRect[20*80];
	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 80; j++) {
		swappers[i*80+j].x1 = j;
		swappers[i*80+j].y1 = i*2;
		swappers[i*80+j].x2 = j;
		swappers[i*80+j].y2 = i*2+1;
		}
	}
	console.conCpy(arr);
	console.moveChars(swappers, 20*80);
	console.moveChars(swappers, 20*80);
	console.refresh();
	delete[] arr;	
	delete[] swappers;
}

struct player {
	int x, y, ch;
};

player dudette = {1,1,'@'};

struct keys {
	int up, down, left, right, esc, f1, f2;
};

keys oldk;
keys newk = {false, false, false, false, false, false, false};

inline int m(int i, int j) {
	// i in (3,77)c35
	// j in (3,22)c7.5
	double x0 = ((i-3)/21.14)-2.5;
	double y0 = ((j-1)/19.0)-1;

	double x = 0;
	double y = 0;

	int iteration = 0;
	const int max_iteration = 1000;

	while ( x*x + y*y < 4  &&  iteration < max_iteration ) //Remember: 4 == (2*2)
	{
		double xtemp = x*x - y*y + x0;
		y = 2*x*y + y0;

		x = xtemp;

		iteration++;
	}

	return (iteration/100)?(iteration/100)+'0'-1:-1;
}

// Test functions
bool controlPlayer(SConsole& console) {
	oldk = newk;
	glfwPollEvents();
	newk.esc = glfwGetKey(GLFW_KEY_ESC);
	if (!oldk.esc && newk.esc)
		return true;
	newk.up = glfwGetKey(GLFW_KEY_UP);
	if (newk.up) {
		console.moveChar(dudette.x,dudette.y,dudette.x,dudette.y-1);
		dudette.y--;
		return false;
	}
	newk.down = glfwGetKey(GLFW_KEY_DOWN);
	if (newk.down) {
		console.moveChar(dudette.x,dudette.y,dudette.x,dudette.y+1);
		dudette.y++;
		return false;
	}
	newk.left = glfwGetKey(GLFW_KEY_LEFT);
	if (newk.left) {
		console.moveChar(dudette.x,dudette.y,dudette.x-1,dudette.y);
		dudette.x--;
		return false;
	}
	newk.right = glfwGetKey(GLFW_KEY_RIGHT);
	if (newk.right) {
		console.moveChar(dudette.x,dudette.y,dudette.x+1,dudette.y);
		dudette.x++;
		return false;
	}
	newk.f1 = glfwGetKey(GLFW_KEY_F1);
	if (!oldk.f1 && newk.f1) {
		printAboutScreen(console);
		dudette.x = 1;
		dudette.y = 1;
		dudette.ch = 0;
		console.putChar(dudette.ch,dudette.x,dudette.y);
		return false;
	}
	newk.f2 = glfwGetKey(GLFW_KEY_F2);
	if (!oldk.f2 && newk.f2) {
		console.fillRect('.',1,1,78,38);
		dudette.x = 1;
		dudette.y = 1;
		dudette.ch = '@';
		short arr[80*40];
		for (int j = 0; j < 40; j++)
			for (int i = 0; i < 80; i++)
				arr[i+j*80] = (i>3&&i<77&&j>1&&j<39)?m(i,j):-1;
		console.conCpy(arr);
		console.putChar(dudette.ch,dudette.x,dudette.y);
		return false;
	}
	return false;
}

double thresh = 4.0;

#define makeStaticArray(x) charPos x[] = {{
#define and },{
#define endArray }};

int main() {
	SConsole console(80,40,8,12);
	console.createConsole("Hello!");
	console.loadFont("font.tga");

	// Initialize the map.
	
	console.fillRect('.',1,1,78,38);
	console.fillRect('-',1,0,78,0);
	console.fillRect('|',0,1,0,38);
	console.fillRect('-',1,39,78,39);
	console.fillRect('|',79,1,79,38);
	makeStaticArray(tim)
		'+',0,0		and
		'+',79,0	and
		'+',79,39	and
		'+',0,39
	endArray
	console.putChars(tim, 4);
	console.putChar(dudette.ch,dudette.x,dudette.y);

	short arr[80*40];
	for (int j = 0; j < 40; j++)
		for (int i = 0; i < 80; i++)
			arr[i+j*80] = (i>3&&i<77&&j>1&&j<39)?m(i,j):-1;
	console.conCpy(arr);
		/*
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			console.putChar(i*16+j,j,i);*/
	// Main game loop
	while (1) {
		if (controlPlayer(console)) break;
		console.refresh();
	}

	console.terminateConsole();
	return 0;
}