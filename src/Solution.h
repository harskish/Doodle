#pragma once
#include "Phenotype.h"

// Solution used in Simulated Annealing, has its own probabilities
class Solution : public Phenotype
{
public:
	Solution(SDL_Surface const* reference) : Phenotype(reference) {};

	void mutate(int iteration); // used to find neighbors

private:
	struct
	{
		int shuffle = 5;
		int addCircle = 15;
		int removeCircle = 15;
		int perturbation = 10; // per gene!
	} probs;
};