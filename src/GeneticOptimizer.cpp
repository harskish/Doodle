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
    generation++;
}
