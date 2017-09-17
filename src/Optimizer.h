#pragma once
#include <float.h>
#include "SDL.h"

/*
    Base class for an optimization algorithm
*/

class Optimizer
{
public:
    Optimizer(SDL_Surface const *reference);
    ~Optimizer();

    // Run optimizer for one iteration
    virtual bool step() = 0;
    
    // Poll (and eventually show) current progress
    SDL_Surface* getCurrentBest() { return currentBest; }

protected:
    SDL_Surface* currentBest;
    float currentBestFitness;
    SDL_Surface const* target; // target of omtimization
};

