#include "FontManager.h"
#include "Error.h"
#include "Texture.h"

FontManager::FontManager()
{
	if (FT_Init_FreeType(&mFreetypeLib))
		ErrorBox("Error: cannot initialize freetype library");
}

FontManager::~FontManager()
{
	for (const std::pair<std::string,FT_Face> &fontFace : mFontFaces)
		FT_Done_Face(fontFace.second);

	FT_Done_FreeType(mFreetypeLib);
}

void FontManager::LoadFontFace(const std::string &fontName)
{
	FT_Face face;

	std::string path = "fonts/" + fontName;

	if (FT_New_Face(mFreetypeLib, path.c_str(), 0, &face)) 
		ErrorBox("Could not open font");

	mFontFaces.insert(std::pair<std::string, FT_Face>(fontName, face));
}

void FontManager::UnloadFontFace(const std::string &fontName)
{
	auto it = mFontFaces.find(fontName);

	if (it == mFontFaces.end())
		ErrorBox("Could not unload font");

	FT_Done_Face(it->second);
	mFontFaces.erase(it);
}

#include <d3d11.h>

void FontManager::LoadFont(std::string const &fontName, unsigned size)
{
	FT_Face face = mFontFaces[fontName];

	FT_Set_Pixel_Sizes(face, 0, size);

	FT_GlyphSlot glyph = face->glyph;

	std::vector<Character> characters;
	
	int fontAtlasWidth = 0, fontAtlasHeight = 0;

	unsigned lineSpacing = 0;

	for (int i = 32; i < 128; i++)
	{
		FT_Load_Char(face, i, FT_LOAD_RENDER);

		fontAtlasWidth += glyph->bitmap.width;

		if (glyph->bitmap.rows > fontAtlasHeight)
			fontAtlasHeight = glyph->bitmap.rows;

		if (face->glyph->metrics.vertAdvance > lineSpacing)
			lineSpacing = face->glyph->metrics.vertAdvance;
	}

	Texture fontAtlas(Texture(fontAtlasWidth, fontAtlasHeight, DXGI_FORMAT_R8_UNORM));
	
	int offset = 0;

	for (int i = 32; i < 128; i++)
	{
		FT_Load_Char(face, i, FT_LOAD_RENDER);

		int bearingX = glyph->bitmap_left;
		int bearingY = glyph->bitmap_top;
		int width = glyph->bitmap.width;
		int height = glyph->bitmap.rows;
		int advanceX = glyph->advance.x >> 6;  // fixed point format 1/64 px
		int advanceY = glyph->advance.y >> 6;
		float textureOffsetX = (float)offset / (float)fontAtlasWidth;
		float textureOffsetY = 0.0f;
		float textureWidth = (float)width / (float)fontAtlasWidth;
		float textureHeight = (float)height / (float)fontAtlasHeight;

		characters.push_back(Character{ bearingX, bearingY, width, height, advanceX, advanceY, textureOffsetX, textureOffsetY, textureWidth, textureHeight });

		fontAtlas.Update(glyph->bitmap.buffer, offset, 0, glyph->bitmap.width, glyph->bitmap.rows);

		offset += glyph->bitmap.width;
	}

	if (mFonts.find(fontName) == mFonts.end())
		mFonts.insert(std::pair<std::string,std::map<unsigned,Font>>(fontName, std::map<unsigned,Font>()));

	mFonts[fontName].insert(std::pair<unsigned,Font>(size, Font(characters, fontAtlas, lineSpacing >> 6)));
}

Font &FontManager::GetFont(const std::string &fontName, unsigned size)
{
	if (mFontFaces.find(fontName) == mFontFaces.end())
		LoadFontFace(fontName);
	
	if (mFonts.find(fontName) == mFonts.end())
		LoadFont(fontName, size);

	if (mFonts[fontName].find(size) == mFonts[fontName].end())
		LoadFont(fontName, size);

	return mFonts[fontName].at(size);	
}