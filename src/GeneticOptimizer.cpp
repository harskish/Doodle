#include "GeneticOptimizer.h"


GeneticOptimizer::GeneticOptimizer(SDL_Surface const *reference) : Optimizer(reference)
{
    generation = 0;
    populationSize = 64;
    stepsWithoutImprovement = 0;
    currentBestFitness = 0.0;

    for (int i = 0; i < populationSize; i++) {
        currentPopulation.push_back(Phenotype(reference));
    }
}


// Sample parents proportional to their fitnesses (using fitness cdf)
Phenotype& GeneticOptimizer::selectParent(std::vector<std::pair<int, double>> &cdf)
{
    double r1 = rand() / (double)RAND_MAX;
    int idx = 0;

    for (int i = 0; i < cdf.size(); i++)
    {
        if (cdf[i].second > r1)
        {
            idx = cdf[i].first;
            break;
        }
    }

    return currentPopulation[idx];
}

std::vector<std::pair<int, double>> GeneticOptimizer::getFitnesses(std::vector<Phenotype> &population)
{
    std::vector<std::pair<int, double>> fitnesses(population.size());

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < population.size(); i++)
    {
        Phenotype &p = population[i];
        fitnesses[i] = std::make_pair(i, p.fitness());
    }

    // Sort by fitness, best fitness first
    std::sort(fitnesses.begin(), fitnesses.end(), [&](std::pair<int, double> &f1, std::pair<int, double> &f2)
    {
        return f1.second > f2.second;
    });

    return fitnesses; // NRVO
}

// Not correct from an algorithmic standpoint, but cool for visualizations
bool GeneticOptimizer::stepForceAscent()
{
    // Build next generation by cloning and mutation
    std::vector<Phenotype> nextPopulation = currentPopulation;
    
    // Make mutations
    std::for_each(nextPopulation.begin(), nextPopulation.end(), [](Phenotype &p) { p.mutate(); });
    generation++;

    // Update fitnesses
    auto fitnesses = getFitnesses(nextPopulation);
    int iBest = fitnesses.front().first;
    double fBest = fitnesses.front().second;

    // Force ascent: only update if fitness improved
    if (fBest > bestSeenFitness)
    {
        stepsWithoutImprovement = 0;
        SDL_Surface* best = currentPopulation[iBest].getImage();
        this->bestSeenFitness = fBest;
        this->currentBestFitness = fBest;

        if (SDL_BlitSurface(best, NULL, this->currentBest, NULL))
            throw std::runtime_error("Failed to update current best solution");

        // Clone current best into next generation
        std::for_each(currentPopulation.begin(), currentPopulation.end(), [&](Phenotype &p) { p = nextPopulation[iBest]; });
        return true;
    }

    stepsWithoutImprovement++;
    return false;
}

// Proper genetic algorithm
bool GeneticOptimizer::stepProper()
{
    // Return value of function
    bool newBestFound = false;

    // Evaluate fitness of population
    std::vector<std::pair<int, double>> fitnesses = getFitnesses(currentPopulation);

    // Check if preview needs to be updated
    int iBest = fitnesses.front().first;
    double fBest = fitnesses.front().second;
    currentBestFitness = fBest;
    if (fBest > bestSeenFitness)
    {
        stepsWithoutImprovement = 0;
        SDL_Surface* best = currentPopulation[iBest].getImage();
        this->bestSeenFitness = fBest;

        if (SDL_BlitSurface(best, NULL, this->currentBest, NULL))
            throw std::runtime_error("Failed to update current best solution");

        newBestFound = true;
    }
    else {
        stepsWithoutImprovement++;
    }

    // Build fitness pdf
    double fitnessSum = 0.0f;
    for (std::pair<int, double> &e : fitnesses) { fitnessSum += e.second; }
    for (std::pair<int, double> &e : fitnesses) { e.second /= fitnessSum; }

    // Build fitness cdf
    std::vector<std::pair<int, double>> fitnessCdf(fitnesses.size());
    fitnessCdf[0] = fitnesses[0];
    for (int i = 1; i < fitnesses.size(); i++)
    {
        fitnessCdf[i].first = fitnesses[i].first;
        fitnessCdf[i].second = fitnessCdf[i - 1].second + fitnesses[i].second;
    }

    // Build next generation by selection
    std::vector<Phenotype> nextPopulation;
    while (nextPopulation.size() < currentPopulation.size())
    {
        // Roulette wheel selection, make copies
        Phenotype p1 = selectParent(fitnessCdf);
        Phenotype p2 = selectParent(fitnessCdf);

        // Crossover (modifies both)
        p1.crossover(p2);

        // Mutate
        p1.mutate();
        p2.mutate();

        // Insert
        nextPopulation.push_back(p1);
        nextPopulation.push_back(p2);
    }

    // Replace previous generation
    const bool forceAscent = false;
    if (!forceAscent || getFitnesses(nextPopulation).front().second > bestSeenFitness)
        currentPopulation = nextPopulation;

    generation++;
    return newBestFound;
}

void GeneticOptimizer::printStats()
{
    auto fitnessPercentage = [&](double f){ return 100 * f / (255UL * 255UL * 3UL * target->w * target->h); };

    printf("[GeneticOptimizer] Generation: %d, fitness: %f%% (best: %f%%)\n",
           generation, fitnessPercentage(currentBestFitness), fitnessPercentage(bestSeenFitness));
}

bool GeneticOptimizer::step()
{
    if (generation % 50 == 0)
        printStats();

    constexpr bool forceAscentMode = false; // less correct, but cool for visualzation
    if (forceAscentMode)
        return stepForceAscent();
    else
        return stepProper();
}
