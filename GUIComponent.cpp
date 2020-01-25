#include "GUIComponent.h"
#include "GraphicsSystem.h"

void GUIComponent::SetScreenPositionTL(const XMFLOAT2 &screenPosition)
{
	float dx = screenPosition.x - mScreenPositionTL.x;
	float dy = screenPosition.y - mScreenPositionTL.y;

	float deltaRelX;
	float deltaRelY;

	if (mParent)
	{
		deltaRelX = dx / mParent->GetScreenDimensions().x;
		deltaRelY = dy / mParent->GetScreenDimensions().y;
	}
	else
	{
		deltaRelX = dx / GraphicsSystem::GetInstance().GetDisplayWidth();
		deltaRelY = dy / GraphicsSystem::GetInstance().GetDisplayHeight();
	}

	SetRelativePositionTL(XMFLOAT2(mRelativePositionTL.x + deltaRelX, mRelativePositionTL.y + deltaRelY));
}

void GUIComponent::SetScreenDimensions(const XMFLOAT2 &screenDimensions)
{
	float dx = screenDimensions.x - mScreenDimensions.x;
	float dy = screenDimensions.y - mScreenDimensions.y;

	float deltaRelWidth;
	float deltaRelHeight;

	if (mParent)
	{
		deltaRelWidth = dx / mParent->GetScreenDimensions().x;
		deltaRelHeight = dy / mParent->GetScreenDimensions().y;
	}
	else
	{
		deltaRelWidth = dx / GraphicsSystem::GetInstance().GetDisplayWidth();
		deltaRelHeight = dy / GraphicsSystem::GetInstance().GetDisplayHeight();
	}

	SetRelativeDimensions(XMFLOAT2(mRelativeDimensions.x + deltaRelWidth, mRelativeDimensions.y + deltaRelHeight));
}

bool GUIComponent::IsInside(float x, float y)
{
	return (x >= GetScreenPositionTL().x && x <= GetScreenPositionTR().x &&
			y >= GetScreenPositionTL().y && y <= GetScreenPositionBL().y);
}
 
void GUIComponent::Update_()
{
	XMFLOAT2 parentNormalizedPositionTL;
	XMFLOAT2 parentNormalizedDimensions;

	if (!mParent)
	{
		parentNormalizedPositionTL = XMFLOAT2(-1.0f, 1.0f);
		parentNormalizedDimensions = XMFLOAT2(2.0f, 2.0f);
	}
	else
	{
		parentNormalizedPositionTL = mParent->mNormalizedPositionTL;
		parentNormalizedDimensions = mParent->mNormalizedDimensions;
	}

	float normalizedPositionX = parentNormalizedPositionTL.x + parentNormalizedDimensions.x * mRelativePositionTL.x;
	float normalizedPositionY = parentNormalizedPositionTL.y - parentNormalizedDimensions.y * mRelativePositionTL.y;

	float normalizedWidth = parentNormalizedDimensions.x * mRelativeDimensions.x;
	float normalizedHeight = parentNormalizedDimensions.y * mRelativeDimensions.y;

	mNormalizedPositionTL = XMFLOAT2(normalizedPositionX, normalizedPositionY);
	mNormalizedDimensions = XMFLOAT2(normalizedWidth, normalizedHeight);

	XMFLOAT2 parentScreenPositionTL;
	XMFLOAT2 parentScreenDimensions;

	if (!mParent)
	{
		parentScreenPositionTL = XMFLOAT2(0.0f, 0.0f);
		parentScreenDimensions = XMFLOAT2(GraphicsSystem::GetInstance().GetDisplayWidth(), GraphicsSystem::GetInstance().GetDisplayHeight());
	}
	else
	{
		parentScreenPositionTL = mParent->mScreenPositionTL;
		parentScreenDimensions = mParent->mScreenDimensions;
	}	

	float screenPositionX = parentScreenPositionTL.x + parentScreenDimensions.x * mRelativePositionTL.x;
	float screenPositionY = parentScreenPositionTL.y + parentScreenDimensions.y * mRelativePositionTL.y;

	float screenWidth = parentScreenDimensions.x * mRelativeDimensions.x;
	float screenHeight = parentScreenDimensions.y * mRelativeDimensions.y;

	mScreenPositionTL = XMFLOAT2(screenPositionX, screenPositionY);
	mScreenDimensions = XMFLOAT2(screenWidth, screenHeight);
}

