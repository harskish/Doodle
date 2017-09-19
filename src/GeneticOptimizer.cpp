#include "GeneticOptimizer.h"


GeneticOptimizer::GeneticOptimizer(SDL_Surface const *reference) : Optimizer(reference)
{
    generation = 0;
    populationSize = 4;

    for (int i = 0; i < populationSize; i++) {
        currentPopulation.push_back(Phenotype(reference));
    }
}


bool GeneticOptimizer::step()
{
    if (generation % 5000 == 0)
        std::cout << "[GeneticOptimizer] Generation " << generation << std::endl;

    std::vector<float> fitnesses(currentPopulation.size());
    nextPopulation = currentPopulation; // clone current generation
    
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < nextPopulation.size(); i++)
    {
        Phenotype &p = nextPopulation[i];
        p.mutate();
        fitnesses[i] = p.fitness();
    }

    generation++;

    int iBest = std::distance(fitnesses.begin(), std::max_element(fitnesses.begin(), fitnesses.end()));
    float fBest = fitnesses[iBest];
    if (fBest > currentBestFitness)
    {
        SDL_Surface* best = nextPopulation[iBest].getImage();
        this->currentBestFitness = fBest;
        
        // TEST: fill new population with clones of best individual
        //currentPopulation = nextPopulation; // advance to next population
        std::for_each(currentPopulation.begin(), currentPopulation.end(), [&](Phenotype &p) { p = nextPopulation[iBest]; });
        
        if (SDL_BlitSurface(best, NULL, this->currentBest, NULL))
            throw std::runtime_error("Faied to update current best solution");

        std::cout << "[GeneticOptimizer] New best fitness: " << currentBestFitness << std::endl;
        return true;
    }
    
    return false;
}
