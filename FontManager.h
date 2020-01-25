#pragma once

#include <string>
#include <map>
#include "Font.h"

#include "ft2build.h"
#include FT_FREETYPE_H

class FontManager
{
public:
	static FontManager &GetInstance() { static FontManager instance; return instance; }
	~FontManager();

	Font &GetFont(const std::string &fontName, unsigned size);
private:
	FontManager();
	void LoadFontFace(const std::string &fontName);
	void UnloadFontFace(const std::string &fontName);
	void LoadFont(std::string const &fontName, unsigned size);

	FT_Library mFreetypeLib;

	std::map<std::string,FT_Face> mFontFaces;
	std::map<std::string,std::map<unsigned,Font>> mFonts;
};

