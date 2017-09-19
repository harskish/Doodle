#pragma once

#include <algorithm>
#include <vector>
#include "SDL.h"
#include "Utils.h"

/*
    A phenotype (an individual) in an evolutionary algorithm
*/
typedef uint8_t Gene;
inline Gene randGene() { return (Gene)(rand() % 256); }

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
    void drawCircle(Gene *genes);

    int geneToX(Gene g);
    int geneToY(Gene g);
    int geneToRadius(Gene g);

    // N circles: x | y | R | r | g | b | a
    int numCircles;
    const int genesPerCircle = 7;
    std::vector<Gene> genotype; // the genes of the individual
    SDL_Surface* data; // image produced by this individual's genotype
    SDL_Surface const* target;
};