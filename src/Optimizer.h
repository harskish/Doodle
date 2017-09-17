#pragma once
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
    virtual void step() = 0;
    
    // Poll (and eventually show) current progress
    SDL_Surface* getCurrentBest() { return currentBest; }

protected:
    int iterations;
    SDL_Surface* currentBest;
    SDL_Surface const* target; // target of omtimization
};

