#include "Solution.h"

void Solution::mutate()
{
	auto perturbationMutation = [&](std::vector<Gene> &genotype) -> bool
	{
		bool mutated = false;

		const int mutationRate = probs.perturbation;
		for (int g = 0; g < numCircles * genesPerCircle; g++)
		{
			if (rand() % 100 < mutationRate)
			{
				genotype[g] += (-10 + rand() % 21);
				mutated = true;
			}
		}

		return mutated;
	};

	auto shuffleMutation = [&](std::vector<Gene> &genotype) -> bool
	{
		if (numCircles == 1) return false;

		const int mutationRate = probs.shuffle;
		if (rand() % 100 >= mutationRate) return false;

		int idx1, idx2;
		idx1 = rand() % numCircles;
		while ((idx2 = rand() % numCircles) == idx1);

		for (int g = 0; g < genesPerCircle; g++)
		{
			std::swap(genotype[idx1 * genesPerCircle + g], genotype[idx2 * genesPerCircle + g]);
		}

		return true;
	};

	auto addCircleMutation = [&](std::vector<Gene> &genotype) -> bool
	{
		const int mutationRate = probs.addCircle;
		if (rand() % 100 < mutationRate)
		{
			addCircle();
			return true;
		}

		return false;
	};

	auto removeCircleMutation = [&](std::vector<Gene> &genotype) -> bool
	{
		if (numCircles == 1) return false;

		const int mutationRate = probs.removeCircle;
		if (rand() % 100 < mutationRate)
		{
			auto it = genotype.begin() + (rand() % numCircles) * genesPerCircle;
			genotype.erase(it, it + genesPerCircle);
			numCircles--;
			return true;
		}

		return false;
	};

	// Perform mutations
	dirty |= perturbationMutation(genotype);
	dirty |= shuffleMutation(genotype);
	dirty |= addCircleMutation(genotype);
	dirty |= removeCircleMutation(genotype);
}