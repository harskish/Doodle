#pragma once
#include <fstream>
#include "SDL.h"
#include "Utils.h"

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

    // Write to stdout and progress file (csv)
    virtual void writeProgress() = 0;
    virtual void writeParameters() = 0;

protected:
    SDL_Surface* currentBest;
    double bestSeenFitness;
    SDL_Surface const* target; // target of optimization
    std::ofstream progressStream;
};

