#pragma once

#include "Optimizer.h"
#include "SDL.h"
#include "Solution.h"
#include <iostream>
#include <vector>
#include <string>

/* Simulated Annealing Optimizer*/
class AnnealingOptimizer : public Optimizer
{
public:
    AnnealingOptimizer(SDL_Surface const *reference);
    ~AnnealingOptimizer();

	void saveImage();

    void writeParameters();
    void writeProgress();
    bool step();

private:
    
    int steps;
    int stepsWithoutImprovement; // for restarting
    float T; // temperature
    const float T0 = 10.0f;
    const float Tend = 0.1f;
    const int coolingFunction = 1; // 1 or 2
    const float k1 = 1.0f - 1e-5f;
    const float k2 = 0.005f;
    Solution *current;

};

