#ifndef TEXT_H
#define TEXT_H

#include <DirectXMath.h>

#include "Font.h"
#include "Mesh.h"

using namespace DirectX;

class Text
{
public:
	Text(unsigned bufferSize = 100);

	const std::string GetText() const { return mTextString; }
	void SetText(const std::string &text);
	void AppendText(const std::string &text); 
	void AppendChar(char c);

	void SetFont(const std::string &fontName, unsigned fontSize);
	const Font *GetFont() { return mFont; }

	void SetNormalizedPositionTL(const XMFLOAT2 &position);
	void SetNormalizedPositionTR(const XMFLOAT2 &position) { SetNormalizedPositionTL(XMFLOAT2(position.x - mNormalizedTextBoxDimensions.x, position.y)); }
	void SetNormalizedPositionBL(const XMFLOAT2 &position) { SetNormalizedPositionTL(XMFLOAT2(position.x, position.y + mNormalizedTextBoxDimensions.y)); }
	void SetNormalizedPositionBR(const XMFLOAT2 &position) { SetNormalizedPositionTL(XMFLOAT2(position.x - mNormalizedTextBoxDimensions.x, position.y + mNormalizedTextBoxDimensions.y)); }
	void SetNormalizedPositionCenter(const XMFLOAT2 &position) { SetNormalizedPositionTL(XMFLOAT2(position.x - mNormalizedTextBoxDimensions.x / 2.0f, position.y + mNormalizedTextBoxDimensions.y / 2.0f)); }

	XMFLOAT2 const &GetNormalizedPosition() const { return mNormalizedPosition; }

	void SetScreenPositionTL(const XMFLOAT2 &position);
	void SetScreenPositionTR(const XMFLOAT2 &position) { SetScreenPositionTL(XMFLOAT2(position.x - mScreenTextBoxDimensions.x, position.y)); }
	void SetScreenPositionBL(const XMFLOAT2 &position) { SetScreenPositionTL(XMFLOAT2(position.x, position.y + mScreenTextBoxDimensions.y)); }
	void SetScreenPositionBR(const XMFLOAT2 &position) { SetScreenPositionTL(XMFLOAT2(position.x - mScreenTextBoxDimensions.x, position.y + mScreenTextBoxDimensions.y)); }
	void SetScreenPositionCenter(const XMFLOAT2 &position) { SetScreenPositionTL(XMFLOAT2(position.x - mScreenTextBoxDimensions.x / 2.0f, position.y + mScreenTextBoxDimensions.y / 2.0f)); }

	XMFLOAT2 GetScreenPosition() const { return mScreenPosition; }
	
	void SetScale(float scale) { mScale = scale; }
	float GetScale() const { return mScale; }

	void SetNormalizedTextBoxDimensions(XMFLOAT2 const &dimensions) { mMaxNormalizedTextBoxDimensions = dimensions; }

	void SetColor(XMFLOAT3 const &color) { mColor = color; }
	XMFLOAT3 const &GetColor() const { return mColor; }

	Mesh GetMesh() { return mMesh; }	
private:
	void AllocateBuffer(unsigned int bufferSize);
	void UpdateMesh();
	Mesh mMesh;

	unsigned int mBufferSize;  // size in num of chars
	
	std::string mTextString;
	int mControlCharacters = 0;

	float mCursorPositionX = 0.0f;
	float mCursorPositionY = 0.0f;

	std::vector<XMFLOAT2> mPositions;
	std::vector<XMFLOAT2> mTextureCoordinates;

	XMFLOAT2 mNormalizedPosition;  // position is text origin
	XMFLOAT2 mScreenPosition;

	XMFLOAT2 mNormalizedTextBoxOffset;
	XMFLOAT2 mScreenTextBoxOffset;

	XMFLOAT2 mNormalizedTextBoxDimensions;
	XMFLOAT2 mScreenTextBoxDimensions;

	XMFLOAT2 mMaxNormalizedTextBoxDimensions = XMFLOAT2(0.5f, 0.5f);

	float mScale;

	const Font *mFont;

	XMFLOAT3 mColor;
};

#endif  // TEXT_H