#include "Optimizer.h"


Optimizer::Optimizer(SDL_Surface const *reference)
{
    target = reference;
    currentBest = surfaceWithEndian(target->w, target->h);
    bestSeenFitness = 0.0;
}


Optimizer::~Optimizer()
{
    SDL_FreeSurface(currentBest);
}
