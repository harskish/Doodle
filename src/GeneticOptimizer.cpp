#include "GeneticOptimizer.h"


GeneticOptimizer::GeneticOptimizer(SDL_Surface const *reference) : Optimizer(reference)
{
    generation = 0;
    populationSize = 8;

    for (int i = 0; i < populationSize; i++) {
        individuals.push_back(Phenotype(reference));
    }
}


GeneticOptimizer::~GeneticOptimizer()
{
}

void GeneticOptimizer::step()
{
    std::cout << "[GeneticOptimizer] Performing step " << generation << std::endl;
    individuals[0].mutate();
    individuals[0].draw();
    SDL_Surface* best = individuals[0].getImage();
    this->currentBest = best;
    generation++;
}
