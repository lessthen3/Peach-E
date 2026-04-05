/*******************************************************************
 *                        Peach-E v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <cmath>

#include <glm/glm.hpp>

///PeachCore
#include "Utils/Math.h"

namespace PeachCore {

    struct NoiseGenerator
    {
        explicit 
            NoiseGenerator(uint32_t seed = 42)
        {
            reseed(seed);
        }

        void reseed(uint32_t seed) 
        {
            p.resize(256);
            std::iota(p.begin(), p.end(), 0);
            std::default_random_engine engine(seed);
            std::shuffle(p.begin(), p.end(), engine);
            p.insert(p.end(), p.begin(), p.end()); // Duplicate for overflow handling
        }

        [[nodiscard]] float 
            noise(float x, float y, float z = 0.0f) 
            const
        {
            // Find unit cube that contains point
            int X = static_cast<int>(std::floor(x)) & 255;
            int Y = static_cast<int>(std::floor(y)) & 255;
            int Z = static_cast<int>(std::floor(z)) & 255;

            // Find relative x, y, z of point in cube
            x -= std::floor(x);
            y -= std::floor(y);
            z -= std::floor(z);

            // Compute fade curves for each of x, y, z
            float u = fade(x);
            float v = fade(y);
            float w = fade(z);

            // Hash coordinates of the 8 cube corners
            int A = p[X] + Y; int AA = p[A] + Z; int AB = p[A + 1] + Z;
            int B = p[X + 1] + Y; int BA = p[B] + Z; int BB = p[B + 1] + Z;

            // Blend results from 8 corners of cube, wtf 
            return Math::Lerp
            (
                w, 
                Math::Lerp(v, Math::Lerp(u, grad(p[AA], x, y, z),
                grad(p[BA], x - 1, y, z)),
                Math::Lerp(u, grad(p[AB], x, y - 1, z),
                    grad(p[BB], x - 1, y - 1, z))),
                Math::Lerp(v, Math::Lerp(u, grad(p[AA + 1], x, y, z - 1),
                    grad(p[BA + 1], x - 1, y, z - 1)),
                    Math::Lerp(u, grad(p[AB + 1], x, y - 1, z - 1),
                        grad(p[BB + 1], x - 1, y - 1, z - 1)))
            );
        }

    private:
        std::vector<int> p;

        static float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }

        static float grad(int hash, float x, float y, float z) 
        {
            int h = hash & 15;
            float u = h < 8 ? x : y;
            float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
            return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
        }

        /**
         * Fractal Brownian Motion (fBm)
         * @param octaves: How many layers? (1-8 is usually enough, more = RIP CPU)
         * @param persistence: How much does each layer's volume drop? (default 0.5f)
         * @param lacunarity: How much does each layer's frequency jump? (default 2.0f)
         */
        [[nodiscard]] float fractal(float x, float y, int octaves, float persistence = 0.5f, float lacunarity = 2.0f) const {
            float total = 0.0f;
            float frequency = 1.0f;
            float amplitude = 1.0f;
            float maxValue = 0.0f;  // Used for normalizing the result to [0, 1] or [-1, 1]

            for (int i = 0; i < octaves; i++) {
                total += noise(x * frequency, y * frequency) * amplitude;

                maxValue += amplitude;

                amplitude *= persistence;
                frequency *= lacunarity;
            }

            return total / maxValue;
        }

        /**
        * Domain Warping: Distorts the space before sampling
        * @param q: The "first layer" of distortion
        * @param r: The "second layer" of distortion
        * @return A beautifully swirled noise value
        */
        [[nodiscard]] float warped(float x, float y, float strength = 4.0f) const {
            // 1. Create a "distorted" coordinate (q)
            glm::vec2 q = {
                fractal(x + 0.0f, y + 0.0f, 3),
                fractal(x + 5.2f, y + 1.3f, 3) // Offsets to prevent symmetry
            };

            // 2. Create a "second-order" distorted coordinate (r)
            glm::vec2 r = {
                fractal(x + strength * q.x + 1.7f, y + strength * q.y + 9.2f, 3),
                fractal(x + strength * q.x + 8.3f, y + strength * q.y + 2.8f, 3)
            };

            // 3. Sample the final noise using the warped coordinates
            return fractal(x + strength * r.x, y + strength * r.y, 3);
        }
    };
}
