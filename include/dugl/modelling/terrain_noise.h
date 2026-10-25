#pragma once

#include <vector>

#include "dugl/common.h"


struct TerrainNoiseSettings
{
    int seed = 0;
    float frequency = 0.1f;
    int octaves = 5;
    float lacunarity = 2.0f;
    float gain = 0.5f;
    float amplitude = 1.0f;
};

std::vector<std::vector<float>> generateTerrainNoise(dugl::uint rows, dugl::uint columns, float width, float length, const TerrainNoiseSettings& settings);
