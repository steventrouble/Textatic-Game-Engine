#include <string>
#include <deque>

#define CHARS_WIDE 80
#define CHARS_TALL 25
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 12

class SConsoleFont;

struct SConsoleRect {
	int x1, y1;
	int x2, y2;
};

struct charPos {
	short ch;
	int x;
	int y;
};

class SConsole {
public:
	/** SConsole::SConsole()
	 *	Constructs a new SConsole.
	 *	For now, it does nothing.
	 */
	SConsole(	int charsWide = CHARS_WIDE, int charsTall = CHARS_TALL, 
				int charWidth = CHAR_WIDTH, int charHeight = CHAR_HEIGHT) {
		m_charsWide = charsWide;
		m_charsTall = charsTall;
		m_charWidth = charWidth;
		m_charHeight = charHeight;
		m_terminalChars = new short[m_charsTall*m_charsWide];
		m_renderQueue = new std::deque<SConsoleRect>();
		return;
	}

	~SConsole() {
		delete[] m_terminalChars;
		return;
	}

	/** SConsole::createConsole()
	 *	Creates a new window, which should in time represent a console
	 */
	int createConsole(std::string title);

	/** SConsole::terminateConsole()
	 *	Should terminate and close the console window
	 */
	void terminateConsole();

	/** SConsole::readError()
	 *	Returns the most recent error to have happened if there was one.
	 */
	std::string readError() { return m_error; }
	
	/** SConsole::loadFont()
	 *	Loads the font into the console
	 */
	int loadFont(std::string path);

	/**	SConsole::putChar()
	 *	Put a character on the screen.
	 */
	void putChar(unsigned short toPut, unsigned int x, unsigned int y);
	void putChars(charPos array[], int len);
	void putChars(short array[], int len, int x, int y);

	/**	SConsole::conCpy()
	 *	Copy an array into the m_terminalChars array
	 */
	void conCpy(short* from);

	/** 	SConsole::moveChar()
	 *	Move a character from one place to another.
	 *	Plays an animation for the character moving.
	 *	Automatically calls refresh()
	 */
	void moveChar(int x1, int y1, int x2, int y2);
	void moveChars(SConsoleRect arr[], int len);

	/**	SConsole::fillRect()
	 *	Fill an entire rectangle on the screen with a single character
	 *	The preferred function is the prior, as it requires less passing
	 		by value.
	 */
	void fillRect(unsigned short toPut, const SConsoleRect& rect);
	void fillRect(unsigned short toPut, unsigned int x1, unsigned int y1,
				unsigned int x2, unsigned int y2);

	/**	SConsole::refresh()
	 *	(Re)draws all rectangles in the queue.
	 */
	void refresh();
	void refreshNoSwap();

private: // Private functions and variables
	/** SConsole::doError
	 *	Report an error that can be accessed by the readError() function
	 */
	void doError(std::string error) {
		m_error = error;
		return;
	}

	// Private variables
	std::string m_error;
	short* m_terminalChars;
	std::deque<SConsoleRect>* m_renderQueue;
	int m_charsWide;
	int m_charsTall;
	int m_charWidth;
	int m_charHeight;
	SConsoleFont* m_font;
};
