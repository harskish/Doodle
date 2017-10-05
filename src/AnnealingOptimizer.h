#pragma once

#include "Optimizer.h"
#include "SDL.h"
#include "Phenotype.h"
#include <iostream>
#include <vector>
#include <string>

// TODO: Make Solution superclass of Phenotype
typedef Phenotype Solution;

/* Simulated Annealing Optimizer*/
class AnnealingOptimizer : public Optimizer
{
public:
    AnnealingOptimizer(SDL_Surface const *reference);
    ~AnnealingOptimizer();

	void saveImage();

    void printStats();
    bool step();

private:
    
    int steps;
    int stepsWithoutImprovement; // for restarting
    float T; // temperature
    const float T0 = 10.0f;
    const float Tend = 0.1f;
    Solution *current;

};

