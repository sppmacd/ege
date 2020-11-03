/*
EGE - Extendable Game Engine
Copyright (c) Sppmacd 2020
*/

#include "Random.h"

#include <ege/main/Config.h>
#include <ege/util/system.h>

namespace EGE
{
    // Implementations
    namespace Internal
    {
        uint64_t lcg_gen(uint64_t seed, uint64_t a, uint64_t c, uint64_t m)
        {
            uint64_t val = (a * seed + c) % m;
            return val;
        }
    }

    // Random
    uint64_t Random::nextInt(uint64_t range)
    {
        ASSERT(m_randomizer);
        ASSERT(range != 0);
        return m_randomizer->nextInt() % range;
    }

    float Random::nextFloat(float range, uint64_t precision)
    {
        return (nextInt(precision) / (float)precision) * range;
    }

    double Random::nextDouble(double range, uint64_t precision)
    {
        return (nextInt(precision) / (float)precision) * range;
    }

    int64_t Random::nextIntRanged(int64_t begin, int64_t end)
    {
        if(begin == end)
            return begin; // Begin and end are equal; nothing to randomize :)

        if(begin > end)
            std::swap(begin, end); // Begin is bigger than end, swap them.
        return nextInt(end - begin + 1) + begin;
    }

    float Random::nextFloatRanged(float begin, float end)
    {
        if(begin == end)
            return begin; // Begin and end are equal; nothing to randomize :)

        if(begin > end)
            std::swap(begin, end); // Begin is bigger than end, swap them.
        return nextFloat(end - begin) + begin;
    }

    double Random::nextDoubleRanged(double begin, double end)
    {
        if(begin == end)
            return begin; // Begin and end are equal; nothing to randomize :)

        if(begin > end)
            std::swap(begin, end); // Begin is bigger than end, swap them.
        return nextDouble(end - begin) + begin;
    }

    Random& Random::goodRandom()
    {
        // TODO: replace it by better generator
        static Random random(System::exactTime().ns);
        return random;
    }

    Random& Random::fastRandom()
    {
        static Random random(System::exactTime().ns);
        return random;
    }
}