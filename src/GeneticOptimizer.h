#pragma once

#include "Optimizer.h"
#include "SDL.h"
#include "Phenotype.h"
#include <iostream>
#include <vector>
#include <string>

class GeneticOptimizer : public Optimizer
{
public:
    GeneticOptimizer(SDL_Surface const *reference);
    ~GeneticOptimizer();

    bool step();

private:
    Phenotype &selectParent(std::vector<std::pair<int, double>> &cdf);
    std::vector<std::pair<int, double>> getFitnesses(std::vector<Phenotype> &population);
    std::vector<std::pair<int, double>> buildRouletteCdf(std::vector<std::pair<int, double>> &fitnesses);
    std::vector<std::pair<int, double>> buildRankCdf(std::vector<std::pair<int, double>> &fitnesses);

    bool stepProper();
    bool stepForceAscent();

    void writeProgress();
	void saveImage();

    int generation;
    int populationSize;
    int stepsWithoutImprovement;

    double currentBestFitness; // of current population

    // Population
    std::vector<Phenotype> currentPopulation;
};

