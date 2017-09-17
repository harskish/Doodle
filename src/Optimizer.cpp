#include "Optimizer.h"


Optimizer::Optimizer(SDL_Surface const *reference)
{
    target = reference;
    currentBest = surfaceWithEndian(target->w, target->h);
    currentBestFitness = -FLT_MAX;
}


Optimizer::~Optimizer()
{
    SDL_FreeSurface(currentBest);
}
