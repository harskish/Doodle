#pragma once

#include <algorithm>
#include <fstream>
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
    void mutate(const int generation);
    void addCircle();

    SDL_Surface *getImage() { return data; }
    int getNumCircles() { return numCircles; }

    void writeProbs(std::ofstream &out);

protected:
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

private:
    // Genetic operator probabilities
    struct
    {
        int crossover = 1;     // once
        int random = 1;        // per gene
        int shuffle = 5;       // once
        int addCircle =  10;   // once
        int removeCircle = 10; // once
        int perturbation = 30; // per gene
    } probs;

    const int crossoverSegments = 3;
};