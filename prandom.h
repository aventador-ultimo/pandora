#pragma once

#include "passert.h"
#include <random>
#include <map>

namespace PRandom
{

///////////////////////////////////////////////////////////////////////////////

std::default_random_engine GENERATOR;
std::uniform_int_distribution<int> DICE(1, 6);

int IRandom(int min, int max)
{
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(GENERATOR);
}

int GetOneDice()
{
    const int result = DICE(GENERATOR);
    PASSERT(result >= 1);
    PASSERT(result <= 6);
    return result;
}

int GetSumTwoDices()
{
    const int result = DICE(GENERATOR) + DICE(GENERATOR);
    PASSERT(result >= 2);
    PASSERT(result <= 12);
    return result;
}

///////////////////////////////////////////////////////////////////////////////

int GenerateHumanIntelligence()
{
    const int first = GetOneDice();
    int rating = 0;

    switch(first)
    {
    case 1:
        rating = 6;
        break;

    case 2:
    case 3:
        rating = 7;
        break;

    case 4:
    case 5:
        rating = 8;
        break;

    case 6:
        rating = 9;
        break;

    default:
        PASSERT(0);
        break;
    };

    PASSERT(rating >= 6);
    PASSERT(rating <= 9);
    return rating;
}

///////////////////////////////////////////////////////////////////////////////

int GenerateAlienRating()
{
    const int first = GetSumTwoDices();
    int rating = 0;
    switch(first)
    {
    case 2:
    case 3:
        rating = 1;
        break;

    case 4:
        rating = 2;
        break;

    case 5:
        rating = 3;
        break;

    case 6:
        rating = 4;
        break;

    case 7:
        rating = 5;
        break;

    case 8:
        rating = 6;
        break;

    case 9:
        rating = 7;
        break;

    case 10:
        rating = 8;
        break;

    case 11:
        rating = 9;
        break;

    case 12:
    {
        const int second = GetOneDice();
        switch(second)
        {
        case 1:
        case 2:
            rating = 9;
            break;

        case 3:
        case 4:
            rating = 10;
            break;

        case 5:
            rating = 11;
            break;

        case 6:
            rating = 12;
            break;

        default:
            PASSERT(0);
            break;
        };
    }
    break;

    default:
        PASSERT(0);
        break;
    };
    PASSERT(rating != 0);

    return rating;
}

///////////////////////////////////////////////////////////////////////////////

int GenerateExplorationParagraph(bool travelling, bool city_second_time)
{
    static const int ExplorationParagraph[6][6] =
    {
        {53, 56, 59, 62, 65, 68},
        {71, 74, 78, 120, 100, 110},
        {130, 140, 143, 146, 150, 154},
        {157, 160, 165, 168, 171, 174},
        {177, 182, 184, 186, 188, 190},
        {192, 194, 196, 198, 200, 202}
    };
    int first = GetOneDice();
    int second = GetOneDice();
    if(travelling == true)
    {
        first--;
        second--;
    }
    if(city_second_time == true)
    {
        first++;
        second++;
    }
    if(first < 1)
    {
        first = 1;
    }
    if(second < 1)
    {
        second = 1;
    }
    if(first > 6)
    {
        first = 6;
    }
    if(second > 6)
    {
        second = 6;
    }
    PASSERT(ExplorationParagraph[0][0] == 53);
    PASSERT(ExplorationParagraph[5][5] == 202);
    return ExplorationParagraph[first - 1][second - 1];
}

///////////////////////////////////////////////////////////////////////////////

int GenerateStarEventCase(int travel_length)
{
    PASSERT(travel_length > 0);
    if(PRandom::GetSumTwoDices() <= travel_length)
    {
        return PRandom::IRandom(0, 10);
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

static int GenerateStarEventCaseVoid_travel_length = 0;

int GenerateStarEventCaseVoid()
{
    return GenerateStarEventCase(GenerateStarEventCaseVoid_travel_length);
}

///////////////////////////////////////////////////////////////////////////////

const std::vector<float> NormalizeHistogram(const std::vector<int> &raw_count)
{
    const int total_count = std::accumulate(raw_count.begin(), raw_count.end(), 0);
    PASSERT(total_count > 0);
    const float total_count_float = static_cast<float>(total_count);
    std::vector<float> histogram(raw_count.size(), 0.0f);
    for(size_t i = 0; i < raw_count.size(); i++)
    {
        histogram[i] = static_cast<float>(raw_count[i]) / total_count_float;
    }
    return histogram;
}

///////////////////////////////////////////////////////////////////////////////

void PrintNormalizedHistogram(const std::vector<float> &histogram)
{
    for(int i = 0; i < static_cast<int>(histogram.size()); i++)
    {
        if(histogram[i] > 0)
        {
            printf("%2d %4.1f%% \n", i, histogram[i] * 100.0f);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

const std::vector<float> GenerateHistogram(int (*generate)(), int max_tries)
{
    PASSERT(generate != NULL);
    std::vector<int> raw_count;
    PASSERT(static_cast<size_t>(max_tries) >= raw_count.size());
    for(int i = 0; i < max_tries; i++)
    {
        const int generate_value = generate();
        PASSERT(generate_value >= 0);
        if(generate_value >= static_cast<int>(raw_count.size()))
        {
            raw_count.resize(generate_value + 1);
        }
        raw_count[generate_value]++;
    }
    return NormalizeHistogram(raw_count);
}

///////////////////////////////////////////////////////////////////////////////

const std::vector<float> OneDiceHistogram(int max_tries)
{
    return GenerateHistogram(GetOneDice, max_tries);
}

///////////////////////////////////////////////////////////////////////////////

const std::vector<float> SumTwoDicesHistogram(int max_tries)
{
    return GenerateHistogram(GetSumTwoDices, max_tries);
}

///////////////////////////////////////////////////////////////////////////////

const std::vector<float> AlienRatingHistogram(int max_tries)
{
    return GenerateHistogram(GenerateAlienRating, max_tries);
}

///////////////////////////////////////////////////////////////////////////////

const std::vector<float> HumanIntelligenceHistogram(int max_tries)
{
    return GenerateHistogram(GenerateHumanIntelligence, max_tries);
}

///////////////////////////////////////////////////////////////////////////////

const std::vector<float> StarEventHistogram(int max_tries, int travel_length)
{
    GenerateStarEventCaseVoid_travel_length = travel_length;
    return GenerateHistogram(GenerateStarEventCaseVoid, max_tries);
}

///////////////////////////////////////////////////////////////////////////////

}
