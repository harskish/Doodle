#include "Optimizer.h"


Optimizer::Optimizer(SDL_Surface const *reference)
{
    iterations = 0;
    target = reference;
    currentBest = SDL_CreateRGBSurface(SDL_SWSURFACE, target->w, target->h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
}


Optimizer::~Optimizer()
{
    SDL_FreeSurface(currentBest);
}
