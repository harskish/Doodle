#include "GeneticOptimizer.h"


GeneticOptimizer::GeneticOptimizer(SDL_Surface const *reference) : Optimizer(reference)
{
    generation = 0;
    populationSize = 4;

    for (int i = 0; i < populationSize; i++) {
        individuals.push_back(Phenotype(reference));
    }
}


GeneticOptimizer::~GeneticOptimizer()
{
}

bool GeneticOptimizer::step()
{
    if (generation % 1000 == 0)
        std::cout << "[GeneticOptimizer] Generation " << generation << std::endl;

    std::vector<float> fitnesses(individuals.size());
    
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < individuals.size(); i++)
    {
        Phenotype &p = individuals[i];
        p.mutate();
        p.draw();
        fitnesses[i] = p.fitness();
    }

    generation++;

    int iBest = std::distance(fitnesses.begin(), std::max_element(fitnesses.begin(), fitnesses.end()));
    float fBest = fitnesses[iBest];
    if (fBest > currentBestFitness)
    {
        SDL_Surface* best = individuals[iBest].getImage();
        this->currentBestFitness = fBest;
        if (SDL_BlitSurface(best, NULL, this->currentBest, NULL))
            throw std::runtime_error("Faied to update current best solution");
        return true;
    }
    
    return false;
}
