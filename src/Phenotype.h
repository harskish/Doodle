#pragma once

#include <algorithm>
#include <vector>
#include <set>
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
    Phenotype(void) = delete;
    ~Phenotype();

    Phenotype& operator=(Phenotype const&);
    Phenotype(Phenotype const&);

    double fitness();
    void crossover(Phenotype &other);
    void mutate();
    void addCircle();

    SDL_Surface *getImage() { return data; }
    int getNumCircles() { return numCircles; }

private:
    void randomInit();
    void draw();
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
    double fitnessValue; // cached fitness value
    bool dirty; // is fitness value up to date?

    // Genetic operator probabilities
    struct
    {
        int crossover = 10;
        int random = 5;
        int shuffle = 5;
        int addCircle = 45;
        int removeCircle = 45;
        int perturbation = 30;
    } probs;
};