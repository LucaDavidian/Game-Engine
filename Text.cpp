#include "Text.h"
#include "GraphicsSystem.h"
#include "FontManager.h"

Text::Text(unsigned bufferSize)
	: mBufferSize(bufferSize), mFont(nullptr), mColor(XMFLOAT3(1.0f, 1.0f, 1.0f)), mScale(1.0f)
{
	mFont = &FontManager::GetInstance().GetFont("Roboto-regular.ttf", 25);

	SetNormalizedPositionCenter(XMFLOAT2());

	AllocateBuffer(mBufferSize);
}

void Text::SetNormalizedPositionTL(const XMFLOAT2 &position) 
{ 
	mNormalizedPosition = XMFLOAT2(position.x - mNormalizedTextBoxOffset.x, position.y - mNormalizedTextBoxOffset.y);
	
	// convert to screen coordinates
	float x = (mNormalizedPosition.x + 1.0f) * GraphicsSystem::GetInstance().GetDisplayWidth() / 2.0f;
	float y = (1.0f - mNormalizedPosition.y) * GraphicsSystem::GetInstance().GetDisplayHeight() / 2.0f;

	mScreenPosition = XMFLOAT2(x, y);
}

void Text::SetScreenPositionTL(const XMFLOAT2 &position)
{
	mScreenPosition = XMFLOAT2(position.x - mScreenTextBoxOffset.x, position.y - mScreenTextBoxOffset.y);

	// convert to normalized device coordinates
	float x = 2.0f * mScreenPosition.x / GraphicsSystem::GetInstance().GetDisplayWidth() - 1.0f;
	float y = 1.0f - 2.0f * mScreenPosition.y / GraphicsSystem::GetInstance().GetDisplayHeight();

	mNormalizedPosition = XMFLOAT2(x, y);
}

void Text::SetFont(const std::string &fontName, unsigned fontSize)
{
	mFont = &FontManager::GetInstance().GetFont(fontName, fontSize);

	UpdateMesh();
}

void Text::AllocateBuffer(unsigned int bufferSize)  // allocate a vertex buffer for bufferSize characters
{
	mMesh.Clear();

	std::vector<XMFLOAT2> positions(bufferSize * 6);
	std::vector<XMFLOAT2> textureCoordinates(bufferSize * 6);

	mMesh.LoadAttribute("POSITION", &positions[0], positions.size(), true);
	mMesh.LoadAttribute("TEX_COORD", &textureCoordinates[0], textureCoordinates.size(), true);
}

void Text::SetText(const std::string &text)
{
	AllocateBuffer(text.size());
		
	mTextString = text;

	UpdateMesh();
}

void Text::AppendText(const std::string &text)
{
	for (char c : text)
		AppendChar(c);
}

void Text::AppendChar(char character)
{
	// update string
	mTextString += character;

	float screenWidthSizePerPixel = 2.0f / GraphicsSystem::GetInstance().GetDisplayWidth();
	float screenHeightSizePerPixel = 2.0f / GraphicsSystem::GetInstance().GetDisplayHeight();

	if (character == '\n')
	{
		mCursorPositionX = 0.0f;
		mCursorPositionY += mFont->GetLineSpacing();
		
		mControlCharacters++;

		return;
	}

	if (mTextString.size() > mBufferSize)
	{
		AllocateBuffer(mBufferSize = mTextString.size() * 2);

		mMesh.UpdateDynamicAttribute("POSITION", &mPositions[0], mPositions.size());
		mMesh.UpdateDynamicAttribute("TEX_COORD", &mTextureCoordinates[0], mTextureCoordinates.size());

		mMesh.SetVertexCount(mTextString.size() * 6);
	}

	if (mCursorPositionX * screenWidthSizePerPixel >= mNormalizedTextBoxOffset.x + mMaxNormalizedTextBoxDimensions.x)
	{
		mCursorPositionX = 0.0f;
		mCursorPositionY += mFont->GetLineSpacing();
	}

	Character c = mFont->GetCharacter(character);

	float quadOffsetX = c.bearingX * screenWidthSizePerPixel;
	float quadOffsetY = c.bearingY * screenHeightSizePerPixel;
	float quadWidth = c.width * screenWidthSizePerPixel;
	float quadHeight = c.height * screenHeightSizePerPixel;
	float quadAdvanceX = mCursorPositionX * screenWidthSizePerPixel;
	float quadAdvanceY = mCursorPositionY * screenHeightSizePerPixel;

	float charTextureX = c.textureOffsetX;
	float charTextureY = c.textureOffsetY;
	float charTextureWidth = c.textureWidth;
	float charTextureHeight = c.textureHeight;

	// first triangle
	mPositions.push_back(XMFLOAT2(quadAdvanceX + quadOffsetX, -quadAdvanceY + quadOffsetY));
	mPositions.push_back(XMFLOAT2(quadAdvanceX + quadOffsetX + quadWidth, -quadAdvanceY + quadOffsetY));
	mPositions.push_back(XMFLOAT2(quadAdvanceX + quadOffsetX, -quadAdvanceY + quadOffsetY - quadHeight));

	mTextureCoordinates.push_back(XMFLOAT2(charTextureX, charTextureY));
	mTextureCoordinates.push_back(XMFLOAT2(charTextureX + charTextureWidth, charTextureY));
	mTextureCoordinates.push_back(XMFLOAT2(charTextureX, charTextureY + charTextureHeight));

	// second triangle
	mPositions.push_back(XMFLOAT2(quadAdvanceX + quadOffsetX, -quadAdvanceY + quadOffsetY - quadHeight));
	mPositions.push_back(XMFLOAT2(quadAdvanceX + quadOffsetX + quadWidth, -quadAdvanceY + quadOffsetY));
	mPositions.push_back(XMFLOAT2(quadAdvanceX + quadOffsetX + quadWidth, -quadAdvanceY + quadOffsetY - quadHeight));

	mTextureCoordinates.push_back(XMFLOAT2(charTextureX, charTextureY + charTextureHeight));
	mTextureCoordinates.push_back(XMFLOAT2(charTextureX + charTextureWidth, charTextureY));
	mTextureCoordinates.push_back(XMFLOAT2(charTextureX + charTextureWidth, charTextureY + charTextureHeight));

	mCursorPositionX += c.advanceX;

	// update vertex buffer
	unsigned int offset = (mTextString.size() - 1 - mControlCharacters) * 6;
	unsigned int numElements = 6;

	mMesh.UpdateDynamicAttribute("POSITION", &mPositions[offset], numElements, offset);
	mMesh.UpdateDynamicAttribute("TEX_COORD", &mTextureCoordinates[offset], numElements, offset);

	mMesh.SetVertexCount((mTextString.size() - mControlCharacters + 1) * 6);

	// calculate text box
	float minX = mPositions[0].x, maxX = 0.0f;
	float minY = 0.0f, maxY = 0.0f;

	for (XMFLOAT2 const &position : mPositions)
	{
		if (position.x > maxX)
			maxX = position.x;

		if (position.y > maxY)
			maxY = position.y;

		if (position.y < minY)
			minY = position.y;
	}

	float normalizedTextBoxWidth = maxX - minX;
	float normalizedTextBoxHeight = maxY - minY;

	// update text box
	mNormalizedTextBoxDimensions = XMFLOAT2(normalizedTextBoxWidth, normalizedTextBoxHeight);
	mScreenTextBoxDimensions = XMFLOAT2(normalizedTextBoxWidth / screenWidthSizePerPixel, normalizedTextBoxWidth / screenHeightSizePerPixel);

	// update coordinates of top left corner of text box
	mNormalizedTextBoxOffset = XMFLOAT2(minX, maxY);
	mScreenTextBoxOffset = XMFLOAT2(minX / screenWidthSizePerPixel, maxY / screenHeightSizePerPixel);
}

void Text::UpdateMesh()
{
	// clear local buffers
	mPositions.clear();
	mTextureCoordinates.clear();

	// reset cursor
	mCursorPositionX = 0;
	mCursorPositionY = 0;

	// reset control characters
	mControlCharacters = 0;

	float screenWidthSizePerPixel = 2.0f / GraphicsSystem::GetInstance().GetDisplayWidth();
	float screenHeightSizePerPixel = 2.0f / GraphicsSystem::GetInstance().GetDisplayHeight();

	// draw a quad for each character
	for (char character : mTextString)
	{
		if (mCursorPositionX == mNormalizedTextBoxOffset.x + mMaxNormalizedTextBoxDimensions.x || character == '\n')
		{
			mCursorPositionX = 0.0f;
			mCursorPositionY += mFont->GetLineSpacing();

			if (character == '\n')
			{
				mControlCharacters++;
				continue;
			}
		}

		Character c = mFont->GetCharacter(character);

		float quadOffsetX = c.bearingX * screenWidthSizePerPixel;
		float quadOffsetY = c.bearingY * screenHeightSizePerPixel;
		float quadWidth = c.width * screenWidthSizePerPixel;
		float quadHeight = c.height * screenHeightSizePerPixel;
		float quadAdvanceX = mCursorPositionX * screenWidthSizePerPixel;
		float quadAdvanceY = mCursorPositionY * screenHeightSizePerPixel;

		float charTextureX = c.textureOffsetX;
		float charTextureY = c.textureOffsetY;
		float charTextureWidth = c.textureWidth;
		float charTextureHeight = c.textureHeight;

		// first triangle
		mPositions.push_back(XMFLOAT2(quadAdvanceX + quadOffsetX, -quadAdvanceY + quadOffsetY));
		mPositions.push_back(XMFLOAT2(quadAdvanceX + quadOffsetX + quadWidth, -quadAdvanceY + quadOffsetY));
		mPositions.push_back(XMFLOAT2(quadAdvanceX + quadOffsetX, -quadAdvanceY + quadOffsetY - quadHeight));

		mTextureCoordinates.push_back(XMFLOAT2(charTextureX, charTextureY));
		mTextureCoordinates.push_back(XMFLOAT2(charTextureX + charTextureWidth, charTextureY));
		mTextureCoordinates.push_back(XMFLOAT2(charTextureX, charTextureY + charTextureHeight));

		// second triangle
		mPositions.push_back(XMFLOAT2(quadAdvanceX + quadOffsetX, -quadAdvanceY + quadOffsetY - quadHeight));
		mPositions.push_back(XMFLOAT2(quadAdvanceX + quadOffsetX + quadWidth, -quadAdvanceY + quadOffsetY));
		mPositions.push_back(XMFLOAT2(quadAdvanceX + quadOffsetX + quadWidth, -quadAdvanceY + quadOffsetY - quadHeight));

		mTextureCoordinates.push_back(XMFLOAT2(charTextureX, charTextureY + charTextureHeight));
		mTextureCoordinates.push_back(XMFLOAT2(charTextureX + charTextureWidth, charTextureY));
		mTextureCoordinates.push_back(XMFLOAT2(charTextureX + charTextureWidth, charTextureY + charTextureHeight));

		mCursorPositionX += c.advanceX;
	}

	// update vertex buffer
	mMesh.UpdateDynamicAttribute("POSITION", &mPositions[0], mPositions.size());
	mMesh.UpdateDynamicAttribute("TEX_COORD", &mTextureCoordinates[0], mTextureCoordinates.size());

	mMesh.SetVertexCount((mTextString.size() - mControlCharacters) * 6);

	// calculate text box
	float minX = mPositions[0].x, maxX = 0.0f;
	float minY = 0.0f, maxY = 0.0f;

	for (XMFLOAT2 const &position : mPositions)
	{
		if (position.x > maxX)
			maxX = position.x;

		if (position.y > maxY)
			maxY = position.y;

		if (position.y < minY)
			minY = position.y;
	}

	float normalizedTextBoxWidth = maxX - minX;
	float normalizedTextBoxHeight = maxY - minY;

	mNormalizedTextBoxDimensions = XMFLOAT2(normalizedTextBoxWidth, normalizedTextBoxHeight);
	mScreenTextBoxDimensions = XMFLOAT2(normalizedTextBoxWidth / screenWidthSizePerPixel, normalizedTextBoxWidth / screenHeightSizePerPixel);

	// calculate coordinates of top left corner of text box
	mNormalizedTextBoxOffset = XMFLOAT2(minX, maxY);
	mScreenTextBoxOffset = XMFLOAT2(minX / screenWidthSizePerPixel, maxY / screenHeightSizePerPixel);
}
