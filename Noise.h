#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Noise
{
public:
	Noise();
	~Noise();
	float PerlinNoise(float x, float y);
	float PerlinNoiseTable(float x, float y);
	float PerlinNoiseTable(float x, float y, float z);
	float RandomNoise(float x, float y);
	float RandomNoiseTable(float x, float y);
private:
	float Lerp(float a, float b, float t);
	float CosineRemap(float t);
	float FadeRemap(float t);
	XMFLOAT2 GetGradient(int x, int y);
	XMFLOAT2 GetGradientTable(int x, int y);
	XMFLOAT3 GetGradientTable(int x, int y, int z);
	float GetNoise(int x, int y);
	float GetNoiseTable(int x, int y);
	float GetSmoothedNoise(int x, int y);
	float GetSmoothedNoiseTable(int x, int y);
	static const int tableSize = 256;
	unsigned int mPermutationTable[tableSize * 2];
	float mRandomValues[tableSize];
	XMFLOAT2 mRandomGradients2D[tableSize];
	XMFLOAT3 mRandomGradients3D[tableSize];
};

