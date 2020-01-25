#ifndef FONT_H
#define FONT_H

#include <vector>
#include "Character.h"
#include "Texture.h"

class Font
{
public:
	Font(const std::vector<Character> &characters, const Texture &fontAtlas, unsigned lineSpacing) : mCharacters(characters), mFontAtlas(fontAtlas), mLineSpacing(lineSpacing) {}
	Texture GetAtlas() const { return mFontAtlas; }
	Character GetCharacter(char character) const { if (character < 32 || character >= 128) return mCharacters[63]; return mCharacters[character - 32]; }
	unsigned GetLineSpacing() const { return mLineSpacing; }
private:
	std::vector<Character> mCharacters;
	unsigned mLineSpacing;
	Texture mFontAtlas;
};

#endif  // FONT_H