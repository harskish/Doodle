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

    virtual bool step();

private:
    int generation;
    int populationSize;

    // Population
    std::vector<Phenotype> currentPopulation;
    std::vector<Phenotype> nextPopulation;
};

