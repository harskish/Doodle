#include "GeneticOptimizer.h"


GeneticOptimizer::GeneticOptimizer(SDL_Surface const *reference) : Optimizer(reference)
{
    generation = 0;
    populationSize = 128;
    stepsWithoutImprovement = 0;
    currentBestFitness = 0.0;

    for (int i = 0; i < populationSize; i++) {
        currentPopulation.push_back(Phenotype(reference));
    }

    progressStream.open("out/gen_stats.csv", std::ofstream::out | std::ofstream::trunc);
    progressStream << "generation,current,best\n";
    writeParameters();
}

GeneticOptimizer::~GeneticOptimizer()
{
    progressStream.close();
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
    std::sort(fitnesses.begin(), fitnesses.end(), [](const std::pair<int, double> &f1, const std::pair<int, double> &f2)
    {
        return f1.second > f2.second;
    });

    return fitnesses; // NRVO
}

std::vector<std::pair<int, double>> GeneticOptimizer::buildRouletteCdf(std::vector<std::pair<int, double>> &fitnesses) {
    // Remove worst fitness from all (bring out differences)
    double fWorst = fitnesses.back().second;
    for(auto &f: fitnesses)
    {
        f.second -= (fWorst - 1); // no bias
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

    return fitnessCdf;
};

std::vector<std::pair<int, double>> GeneticOptimizer::buildRankCdf(std::vector<std::pair<int, double>> &fitnesses) {
    const double max = 1.9; //1.1
    const double min = 2.0 - max;
    const size_t n = fitnesses.size();

    auto h = [&](int r) -> double { return max-(max-min)*float(r-1)/(n-1); }; // r = rank

    // Build pdf
    for (int i = 0; i < n; i++)
    {
        fitnesses[i].second = h(i) / n;
    }

    // Build cdf
    std::vector<std::pair<int, double>> fitnessCdf(fitnesses.size());
    fitnessCdf[0] = fitnesses[0];
    for (int i = 1; i < fitnesses.size(); i++)
    {
        fitnessCdf[i].first = fitnesses[i].first;
        fitnessCdf[i].second = fitnessCdf[i - 1].second + fitnesses[i].second;
    }

    return fitnessCdf;
};

// Not correct from an algorithmic standpoint, but cool for visualizations
bool GeneticOptimizer::stepForceAscent()
{
    // Build next generation by cloning and mutation
    std::vector<Phenotype> nextPopulation = currentPopulation;
    
    // Make mutations
    std::for_each(nextPopulation.begin(), nextPopulation.end(), [&](Phenotype &p) { p.mutate(generation); });

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

    // Restrict breeding pool
    int toBreed = fitnesses.size() * selectionCutoff / 100;
    fitnesses.resize(toBreed);

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

    // Build CDF for parent sampling
    auto fitnessCdf = (useRankCdf) ? buildRankCdf(fitnesses) : buildRouletteCdf(fitnesses);

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
        p1.mutate(generation);
        p2.mutate(generation);

        // Insert
        nextPopulation.push_back(p1);
        nextPopulation.push_back(p2);
    }

    // Replace previous generation
    const bool forceAscent = false;
    if (!forceAscent || getFitnesses(nextPopulation).front().second > bestSeenFitness)
        currentPopulation = nextPopulation;

    return newBestFound;
}

void GeneticOptimizer::writeProgress()
{
    auto fitnessPercentage = [&](double f) { return f / ((Uint64)(255 * 255 * 3) * (Uint64)target->w * (Uint64)target->h); };

    double curr = fitnessPercentage(currentBestFitness);
    double currScaled = 100 * pow(curr, 30);
    double best = fitnessPercentage(bestSeenFitness);
    double bestScaled = 100 * pow(best, 30);

    printf("[GeneticOptimizer] Generation: %d, fitness: %.2f%% (best: %.2f%%)\n", generation, currScaled, bestScaled);

    // Write to csv file
    if (!progressStream.good())
        std::cout << "Could not export progress to csv" << std::endl;
    else
        progressStream << generation << "," << curr << "," << best << std::endl << std::flush;
}

void GeneticOptimizer::writeParameters()
{
    std::ofstream paramFile("out/gen_params.txt", std::ofstream::out | std::ofstream::trunc);

    if (!paramFile.good())
        std::cout << "Could not export simulation parameters" << std::endl;
    else
    {
        paramFile << "Algorithm: Genetic Optimizer" << std::endl;
        paramFile << "Mode: " << ((forceAscentMode) ? "Force ascent" : "Standard") << std::endl;
        paramFile << "CDF type: " << ((useRankCdf) ? "Rank" : "Roulette") << std::endl;
        paramFile << "Population size: " << populationSize << std::endl;
        paramFile << "Breeding cutoff: " << selectionCutoff << "%" << std::endl;
        currentPopulation.front().writeProbs(paramFile);
        paramFile.close();
    }       
}

void GeneticOptimizer::saveImage()
{
	std::string filename = "out/gen_out_" + std::to_string(generation / 1000) + "k.bmp";
	SDL_SaveBMP(this->currentBest, filename.c_str());
}

bool GeneticOptimizer::step()
{
	generation++;

	if (generation % 50 == 0)
		writeProgress();

	if (generation % 5000 == 0)
		saveImage();
	
    if (forceAscentMode)
        return stepForceAscent();
    else
        return stepProper();
}
