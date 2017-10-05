#include "AnnealingOptimizer.h"

AnnealingOptimizer::AnnealingOptimizer(SDL_Surface const * reference) : Optimizer(reference)
{
    steps = 0;
    stepsWithoutImprovement = 0;
    T = T0;
    current = new Solution(reference);
}

AnnealingOptimizer::~AnnealingOptimizer()
{
    delete current;
}

void AnnealingOptimizer::printStats()
{
    auto fitnessPercentage = [&](double f) { return f / (255UL * 255UL * 3UL * target->w * target->h); };

    double curr = 100 * pow(fitnessPercentage(current->fitness()), 30);
    double best = 100 * pow(fitnessPercentage(bestSeenFitness), 30);

    printf("[AnnealingOptimizer] Iteration: %d, T: %.2f, fitness: %.2f%% (best: %.2f%%)\n", steps, T, curr, best);
}

bool AnnealingOptimizer::step()
{
    // Cooling functions
    auto alpha1 = [](float T) -> float
    {
        const float k = 1.0f - 1e-6f;
        return k*T;
    };

    auto alpha2 = [](float T) -> float
    {
        const float k = 0.005f;
        return T / (1 + k*T);
    };

    // Acceptance probability, ascent -> always accept, descent -> accept probabilistically
    auto P = [](float T, float deltaF) -> float
    {
        return (deltaF > 0) ? 1.0f : exp(deltaF / T);
    };

    // Pick a random neighbor
    Solution n = *current;
    n.mutate();

    // Check acceptance
    bool newBestFound = false;
    float deltaF = float(n.fitness() - current->fitness());
    float r = (float)rand() / RAND_MAX;
    if (P(T, deltaF) >= r)
    {
        *current = n;

        if (current->fitness() > bestSeenFitness)
        {
            stepsWithoutImprovement = 0;
            SDL_Surface* best = current->getImage();
            this->bestSeenFitness = current->fitness();

            if (SDL_BlitSurface(best, NULL, this->currentBest, NULL))
                throw std::runtime_error("Failed to update current best solution");

            newBestFound = true;
        }
        else
        {
            stepsWithoutImprovement++;
        }
    }

    // Cooling
    T = alpha1(T);

    steps++;
    if (steps % 500 == 0)
        printStats();

    return newBestFound;
}
