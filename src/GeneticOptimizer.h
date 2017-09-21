#pragma once

#include "Optimizer.h"
#include "SDL.h"
#include "Phenotype.h"
#include <iostream>
#include <vector>

class GeneticOptimizer : public Optimizer
{
public:
    GeneticOptimizer(SDL_Surface const *reference);
    ~GeneticOptimizer() = default;

    bool step();

private:
    Phenotype &selectParent(std::vector<std::pair<int, float>> &cdf);

    int generation;
    int populationSize;
    int stepsWithoutImprovement;

    // Population
    std::vector<Phenotype> currentPopulation;
};

