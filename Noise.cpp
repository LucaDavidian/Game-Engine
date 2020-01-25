#include "Noise.h"
#include <cmath>
#include <cstdlib>
#include <random>

/* noise functions return a float value between -1.0 and 1.0 */

Noise::Noise() 
{
	std::default_random_engine rand;
	std::mt19937 generator(rand());
	std::uniform_real_distribution<> distr;

	// generate random values
	for (int i = 0; i < tableSize; i++)
		mRandomValues[i] = distr(generator) * 2.0f - 1.0f;

	// generate random gradients
	generator.seed(rand());
	for (int i = 0; i < tableSize; i++)
	{
		// 2D gradients table
		mRandomGradients2D[i] = XMFLOAT2(distr(generator) * 2.0f - 1.0f, distr(generator) * 2.0f - 1.0f);
		XMStoreFloat2(&mRandomGradients2D[i], XMVector2Normalize(XMLoadFloat2(&mRandomGradients2D[i])));

		// 3D gradients table
		mRandomGradients3D[i] = XMFLOAT3(distr(generator) * 2.0f - 1.0f, distr(generator) * 2.0f - 1.0f, distr(generator) * 2.0f - 1.0f);
		XMStoreFloat3(&mRandomGradients3D[i], XMVector3Normalize(XMLoadFloat3(&mRandomGradients3D[i])));
	}

	// generate permutation table
	for (int i = 0; i < tableSize; i++)
		mPermutationTable[i] = i;
	std::uniform_int_distribution<> int_distr;
	generator.seed(rand());
	for (int i = 0; i < tableSize; i++)
	{
		std::swap(mPermutationTable[i], mPermutationTable[int_distr(generator) & tableSize - 1]);
		mPermutationTable[i + tableSize] = mPermutationTable[i];
	}
}

Noise::~Noise()
{
}

float Noise::PerlinNoise(float x, float y)
{
	int xi = (int)std::floor(x);
	int yi = (int)std::floor(y);

	float xf = x - xi;
	float yf = y - yi;

	XMFLOAT2 g1 = GetGradient(xi, yi);
	XMFLOAT2 g2 = GetGradient(xi + 1, yi);
	XMFLOAT2 g3 = GetGradient(xi, yi + 1);
	XMFLOAT2 g4 = GetGradient(xi + 1, yi + 1);

	XMFLOAT2 p1(xf, yf);
	XMFLOAT2 p2(xf - 1, yf);
	XMFLOAT2 p3(xf, yf - 1);
	XMFLOAT2 p4(xf - 1, yf - 1);

	XMFLOAT2 dot;
	XMStoreFloat2(&dot, XMVector2Dot(XMLoadFloat2(&p1), XMLoadFloat2(&g1)));
	float i1 = dot.x;
	XMStoreFloat2(&dot, XMVector2Dot(XMLoadFloat2(&p2), XMLoadFloat2(&g2)));
	float i2 = dot.x;
	XMStoreFloat2(&dot, XMVector2Dot(XMLoadFloat2(&p3), XMLoadFloat2(&g3)));
	float i3 = dot.x;
	XMStoreFloat2(&dot, XMVector2Dot(XMLoadFloat2(&p4), XMLoadFloat2(&g4)));
	float i4 = dot.x;

	float a = Lerp(i1, i2, CosineRemap(xf));
	float b = Lerp(i3, i4, CosineRemap(xf));

	float c = Lerp(a, b, CosineRemap(yf));

	return c;
}

XMFLOAT2 Noise::GetGradient(int x, int y)
{
	std::mt19937 generator(x * 564433 + y * 3234445);
	std::uniform_real_distribution<> distr;;
	XMFLOAT2 gradient(distr(generator) * 2.0f - 1.0f, distr(generator) * 2.0f - 1.0f);
	XMStoreFloat2(&gradient, XMVector2Normalize(XMLoadFloat2(&gradient)));

	return gradient;
}

float Noise::PerlinNoiseTable(float x, float y)
{
	int x0 = (int)std::floor(x) & tableSize - 1;
	int y0 = (int)std::floor(y) & tableSize - 1;
	int x1 = x0 + 1 & tableSize - 1;
	int y1 = y0 + 1 & tableSize - 1;

	float xf = x - (int)std::floor(x);
	float yf = y - (int)std::floor(y);

	XMFLOAT2 g1 = GetGradientTable(x0, y0);
	XMFLOAT2 g2 = GetGradientTable(x1, y0);
	XMFLOAT2 g3 = GetGradientTable(x0, y1);
	XMFLOAT2 g4 = GetGradientTable(x1, y1);

	XMFLOAT2 p1(xf, yf);
	XMFLOAT2 p2(xf - 1, yf);
	XMFLOAT2 p3(xf, yf - 1);
	XMFLOAT2 p4(xf - 1, yf - 1);

	XMFLOAT2 dot;
	XMStoreFloat2(&dot, XMVector2Dot(XMLoadFloat2(&p1), XMLoadFloat2(&g1)));
	float i1 = dot.x;
	XMStoreFloat2(&dot, XMVector2Dot(XMLoadFloat2(&p2), XMLoadFloat2(&g2)));
	float i2 = dot.x;
	XMStoreFloat2(&dot, XMVector2Dot(XMLoadFloat2(&p3), XMLoadFloat2(&g3)));
	float i3 = dot.x;
	XMStoreFloat2(&dot, XMVector2Dot(XMLoadFloat2(&p4), XMLoadFloat2(&g4)));
	float i4 = dot.x;

	float a = Lerp(i1, i2, CosineRemap(xf));
	float b = Lerp(i3, i4, CosineRemap(xf));

	float c = Lerp(a, b, CosineRemap(yf));

	return c;
}

XMFLOAT2 Noise::GetGradientTable(int x, int y)
{
	return mRandomGradients2D[mPermutationTable[mPermutationTable[x] + y]];
}

float Noise::PerlinNoiseTable(float x, float y, float z)
{
	int x0 = (int)std::floor(x) & tableSize - 1;
	int y0 = (int)std::floor(y) & tableSize - 1;
	int z0 = (int)std::floor(z) & tableSize - 1;
	int x1 = x0 + 1 & tableSize - 1;
	int y1 = y0 + 1 & tableSize - 1;
	int z1 = z0 + 1 & tableSize - 1;

	float xf = x - (int)std::floor(x);
	float yf = y - (int)std::floor(y);
	float zf = z - (int)std::floor(z);

	XMFLOAT3 g1 = GetGradientTable(x0, y0, z0);
	XMFLOAT3 g2 = GetGradientTable(x1, y0, z0);
	XMFLOAT3 g3 = GetGradientTable(x0, y1, z0);
	XMFLOAT3 g4 = GetGradientTable(x0, y0, z1);
	XMFLOAT3 g5 = GetGradientTable(x1, y1, z0);
	XMFLOAT3 g6 = GetGradientTable(x1, y0, z1);
	XMFLOAT3 g7 = GetGradientTable(x0, y1, z1);
	XMFLOAT3 g8 = GetGradientTable(x1, y1, z1);

	XMFLOAT3 pos1(xf, yf, zf);
	XMFLOAT3 pos2(xf - 1, yf, zf);
	XMFLOAT3 pos3(xf, yf - 1, zf);
	XMFLOAT3 pos4(xf, yf, zf - 1);
	XMFLOAT3 pos5(xf - 1, yf - 1, zf);
	XMFLOAT3 pos6(xf - 1, yf, zf - 1);
	XMFLOAT3 pos7(xf, yf - 1, zf - 1);
	XMFLOAT3 pos8(xf - 1, yf - 1, zf - 1);

	XMFLOAT3 dot;
	XMStoreFloat3(&dot, XMVector3Dot(XMLoadFloat3(&pos1), XMLoadFloat3(&g1)));
	float i1 = dot.x;
	XMStoreFloat3(&dot, XMVector3Dot(XMLoadFloat3(&pos2), XMLoadFloat3(&g2)));
	float i2 = dot.x;
	XMStoreFloat3(&dot, XMVector3Dot(XMLoadFloat3(&pos3), XMLoadFloat3(&g3)));
	float i3 = dot.x;
	XMStoreFloat3(&dot, XMVector3Dot(XMLoadFloat3(&pos4), XMLoadFloat3(&g4)));
	float i4 = dot.x;
	XMStoreFloat3(&dot, XMVector3Dot(XMLoadFloat3(&pos5), XMLoadFloat3(&g5)));
	float i5 = dot.x;
	XMStoreFloat3(&dot, XMVector3Dot(XMLoadFloat3(&pos6), XMLoadFloat3(&g6)));
	float i6 = dot.x;
	XMStoreFloat3(&dot, XMVector3Dot(XMLoadFloat3(&pos7), XMLoadFloat3(&g7)));
	float i7 = dot.x;
	XMStoreFloat3(&dot, XMVector3Dot(XMLoadFloat3(&pos8), XMLoadFloat3(&g8)));
	float i8 = dot.x;

	float a = Lerp(i1, i2, CosineRemap(xf));
	float b = Lerp(i3, i5, CosineRemap(xf));

	float c = Lerp(a, b, CosineRemap(yf));

	float e = Lerp(i4, i6, CosineRemap(xf));
	float f = Lerp(i7, i8, CosineRemap(xf));

	float g = Lerp(e, f, CosineRemap(yf));

	float h = Lerp(c, g, CosineRemap(zf));

	return h;
}

XMFLOAT3 Noise::GetGradientTable(int x, int y, int z)
{
	return mRandomGradients3D[mPermutationTable[mPermutationTable[mPermutationTable[x] + y] + z]];
}

float Noise::RandomNoise(float x, float y)
{
	int xi = (int)std::floor(x);
	int yi = (int)std::floor(y);

	float xf = x - xi;
	float yf = y - yi;

	float a = GetSmoothedNoise(xi, yi);
	float b = GetSmoothedNoise(xi + 1, yi);
	float c = GetSmoothedNoise(xi, yi + 1);
	float d = GetSmoothedNoise(xi + 1, yi + 1);

	float i1 = Lerp(a, b, CosineRemap(xf));
	float i2 = Lerp(c, d, CosineRemap(xf));

	float i3 = Lerp(i1, i2, CosineRemap(yf));

	return i3;
}

float Noise::GetSmoothedNoise(int x, int y)
{
	float corners = (GetNoise(x + 1, y + 1) + GetNoise(x - 1, y - 1) + GetNoise(x + 1, y - 1) + GetNoise(x - 1, y + 1)) / 16.0f;
	float sides = (GetNoise(x + 1, y) + GetNoise(x - 1, y) + GetNoise(x, y + 1) + GetNoise(x, y - 1)) / 8.0f;
	float center = GetNoise(x, y) / 4.0f;
	return corners + sides + center;
}

float Noise::GetNoise(int x, int y)
{
	//srand(x * 656000 + y * 865900 + seed);
	//return (rand() / (float)RAND_MAX) * 2.0f - 1.0f;
	std::mt19937 generator(x * 2323200 + y * 331213250);
	std::uniform_real_distribution<> distr;
	return distr(generator) * 2.0f - 1.0f;
}

float Noise::RandomNoiseTable(float x, float y)
{
	int x0 = (int)std::floor(x) & tableSize - 1;
	int y0 = (int)std::floor(y) & tableSize - 1;
	int x1 = x0 + 1 & tableSize - 1;
	int y1 = y0 + 1 & tableSize - 1;

	float xf = x - (int)std::floor(x);
	float yf = y - (int)std::floor(y);

	float a = GetSmoothedNoiseTable(x0, y0);
	float b = GetSmoothedNoiseTable(x1, y0);
	float c = GetSmoothedNoiseTable(x0, y1);
	float d = GetSmoothedNoiseTable(x1, y1);

	float i1 = Lerp(a, b, CosineRemap(xf));
	float i2 = Lerp(c, d, CosineRemap(xf));

	float i3 = Lerp(i1, i2, CosineRemap(yf));

	return i3;
}

float Noise::GetSmoothedNoiseTable(int x, int y)
{
	int xp1 = x + 1 & tableSize - 1;
	int xm1 = x - 1 & tableSize - 1;
	int yp1 = y + 1 & tableSize - 1;
	int ym1 = y - 1 & tableSize - 1;
	float corners = (GetNoiseTable(xp1, yp1) + GetNoiseTable(xm1, ym1) + GetNoiseTable(xp1, ym1) + GetNoiseTable(xm1, yp1)) / 16.0f;
	float sides = (GetNoiseTable(x, yp1) + GetNoiseTable(x, ym1) + GetNoiseTable(xm1, y) + GetNoiseTable(xp1, y)) / 8.0f;
	float center = GetNoiseTable(x, y) / 4.0f;
	return corners + sides + center;
}

float Noise::GetNoiseTable(int x, int y)
{
	return mRandomValues[mPermutationTable[x] + y];
}

float Noise::Lerp(float a, float b, float t)
{
	return (1 - t) * a + t * b;
}

#include <corecrt_math_defines.h>

float Noise::CosineRemap(float t)
{
	return (1.0f - std::cos(t * M_PI)) / 2.0f;
}

float Noise::FadeRemap(float t)
{
	return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}
