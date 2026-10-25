#include "dugl/modelling/terrain_noise.h"

#include <FastNoiseLite.h>


std::vector<std::vector<float>> generateTerrainNoise(dugl::uint rows, dugl::uint columns, float width, float length, const TerrainNoiseSettings& settings)
{
    FastNoiseLite noise(settings.seed);
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFrequency(settings.frequency);
    noise.SetFractalOctaves(settings.octaves);
    noise.SetFractalLacunarity(settings.lacunarity);
    noise.SetFractalGain(settings.gain);

    std::vector<std::vector<float>> heights(rows, std::vector<float>(columns));

    for (dugl::uint row = 0; row < rows; row++)
    {
        for (dugl::uint column = 0; column < columns; column++)
        {
            float x = width * ((float)column / (columns - 1) - 0.5f);
            float z = length * ((float)row / (rows - 1) - 0.5f);
            heights[row][column] = settings.amplitude * noise.GetNoise(x, z);
        }
    }

    return heights;
}
