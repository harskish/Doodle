#pragma once

#include <algorithm>
#include <vector>
#include "SDL.h"
#include "Utils.h"

/*
    A phenotype (an individual) in an evolutionary algorithm
*/

class Phenotype
{
public:
    Phenotype(SDL_Surface const* reference);
    ~Phenotype();

    Phenotype& operator=(Phenotype const&);
    Phenotype(Phenotype const&);

    float fitness();
    void crossover(Phenotype &other);
    void mutate();

    void draw();
    SDL_Surface *getImage() { return data; }

private:
    void randomInit();
    void drawCircle(int *genes);

    // N circles: x | y | R | r | g | b | a
    int numCircles;
    std::vector<int> genotype; // the chromosomes of the individual
    SDL_Surface* data; // image produced by this individual's genotype
    SDL_Surface const* target;
};