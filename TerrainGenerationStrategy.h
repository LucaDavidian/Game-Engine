#ifndef TERRAIN_STRATEGY_H
#define TERRAIN_STRATEGY_H

class Terrain;
class Model;
class StaticMeshComponent;

class TerrainGenerationStrategy
{
public:	
	virtual ~TerrainGenerationStrategy();
	virtual StaticMeshComponent *GenerateTerrain(Terrain *terrain) = 0;
protected:
	TerrainGenerationStrategy();
};

#include "Noise.h"

class ProceduralTerrainGenerationStrategy : public TerrainGenerationStrategy
{
public:
	StaticMeshComponent *GenerateTerrain(Terrain *terrain) override;
private:
	Noise mNoiseGenerator;                   // noise generator class 
	float ComputeHeight(float x, float z, float startFrequency, float startAmplitude, float persistence, int octaves);
};

#include <vector>
#include <string>

class HeightMapTerrainGenerationStrategy : public TerrainGenerationStrategy
{
public:
	HeightMapTerrainGenerationStrategy(const std::string &heightMapFilePath) { LoadHeightMap(heightMapFilePath); }
	StaticMeshComponent *GenerateTerrain(Terrain *terrain) override;
private:
	void LoadHeightMap(std::string const &heightMapFilePath);
	float SampleHeightMap(float x, float z, float scaleFactor);
	std::vector<unsigned char> mHeightMap;   // height map
	unsigned int mHeightMapWidth;
	unsigned int mHeightMapDepth;
};

#endif  // TERRAIN_STRATEGY_H