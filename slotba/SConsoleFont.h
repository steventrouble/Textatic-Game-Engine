#include <string>

class SConsoleFont {
public:
	/** SConsoleFont::SConsoleFont()
	 *	Create the font proper
	 */
	SConsoleFont(std::string path, unsigned int width, unsigned int height,
		unsigned int charWidth, unsigned charHeight);

	/**	SConsoleFont::putChar
	 *	Put a character in a specific place.
	 */
	void putChar(unsigned short ch, int x, int y);
	void putChar(unsigned short ch, float x, float y);

	/** SConsoleFont::*
	 *	These functions simply return data to the user.
	 */
	unsigned int width() { return m_width; }
	unsigned int height() { return m_height; }
	bool isLoaded() { return true; } // Currently not functional;
	unsigned int textureID() { return m_textureID; }
	~SConsoleFont();

private:
	unsigned int m_textureID;
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_charWidth;
	unsigned int m_charHeight;
	int* m_fontSingle;
};
