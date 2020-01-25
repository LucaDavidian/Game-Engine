#pragma once

#include "TextShader.h"
#include <vector>

class Text;

class TextRenderer
{
public:
	static TextRenderer &GetInstance() { static TextRenderer instance; return instance; }
	void AddText(Text *text);
	void Render();
private:
	TextRenderer() = default;
	TextShader mShader;
	std::vector<Text*> mTexts;
};

