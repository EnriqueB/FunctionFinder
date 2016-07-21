#include <iostream>
#include <string.h>
#include "individual.h"
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <stack>
#include <set>
#include <fstream>
#include <algorithm>

#define TOURNAMENT_SIZE 3
#define POPULATION_SIZE 10000
#define INPUT_SIZE 42.0

using namespace std;

double testArray [42][2];
vector <string> functionSet = { "+", "-", "*", "/" };
vector <Individual> individuals;

double crossoverRate = 0.9;
double mutationChance = 0.1;
double bestFitness = (double)INT_MAX;
int bestIndex = -1;
int generation = 0;
double calculate(double op1, double op2, string operation) {
	switch (operation[0]) {
	case '+':
		return op1 + op2;
	case '-':
		return op1 - op2;
	case '*':
		return op1 * op2;
	case '/':
		if (op2 < 0.0000001 && op2 > -0.0000001) {
			return 0;
		}
		else {
			return op1 / op2;
		}
	}
}

double evaluate(Individual ind, double valueX) {
	double ans;
	set <string> functions(functionSet.begin(), functionSet.end());
	stack <double> calculator;
	string::size_type sz;
	stringstream ss(ind.getSolution());
	string item;
	vector <string> tokens;
	while (getline(ss, item, ' ')) {
		tokens.push_back(item);
	}
	int i = tokens.size() - 1;
	while (i >= 0) {
		if (functions.find(tokens[i]) == functions.end()) {
			//not an operator, push to stack
			if (tokens[i] == "x")
				calculator.push(valueX);
			else
				calculator.push(stod(tokens[i], &sz));
		}
		else {
			double op1 = calculator.top();
			calculator.pop();
			double op2 = calculator.top();
			calculator.pop();
			calculator.push(calculate(op1, op2, tokens[i]));
		}
		i--;
	}
	return calculator.top();
}

void evaluateFitness() {
	for (int i = 0; i < POPULATION_SIZE; i++) {
		double error = 0;
		for (int j = 0; j < 42; j++) {
			error+= pow(testArray[j][1] - evaluate(individuals[i], testArray[j][0]), 2);
		}
		individuals[i].setFitness(error / INPUT_SIZE);
		if ((error / INPUT_SIZE) < bestFitness) {
			bestFitness = error / INPUT_SIZE;
			bestIndex = i;
			cout << "Generation: " << generation << "\t Best Fitness: " << bestFitness << "\nSolution: " << individuals[i].getSolution() << endl << endl;
		}
	}
}

double evaluateSingle(int index) {
	double error = 0;
	for (int j = 0; j < 21; j++) {
		error += pow(testArray[j][1] - evaluate(individuals[index], testArray[j][0]), 2);
	}
	if ((error / INPUT_SIZE) < bestFitness) {
		bestFitness = error/INPUT_SIZE;
		cout << "Generation: " << generation << "\t Best Fitness: " << bestFitness << "\nSolution: " << individuals[index].getSolution() << endl << endl;
		bestIndex = index;
	}
	return (error / 21.0);
}


int tournament() {
	double fitness = (double)INT_MAX;
	int index = 0;
	for (int i = 0; i < TOURNAMENT_SIZE; i++) {
		int ind = rand() % POPULATION_SIZE;
		string s = individuals[ind].getSolution();
		int nodes = count(s.begin(), s.end(), ' ');
		if ((individuals[ind].getFitness() + nodes*0.5)< fitness) {
			fitness = individuals[ind].getFitness();
			index = ind;
		}
	}
	return index;
}

int negativeTournament() {
	double fitness = 0;
	int index = 0;
	for (int i = 0; i < TOURNAMENT_SIZE; i++) {
		int ind = rand() % POPULATION_SIZE;
		string s = individuals[ind].getSolution();
		int nodes = count(s.begin(), s.end(), ' ');
		if (individuals[ind].getFitness() + nodes*0.5 > fitness) {
			fitness = individuals[ind].getFitness();
			index = ind;
		}
	}
	return index;
}

void generateOffspring() {
	double random = ((double)rand() / (RAND_MAX));
	Individual ind;
	if (random < crossoverRate) {
		//crossover, pick two parents
		int parent1 = tournament();
		int parent2 = tournament();
		ind.crossOver(individuals[parent1].getSolution(), individuals[parent2].getSolution());
		//find a suitable spot
		int index = negativeTournament();
		individuals[index] = ind;
		individuals[index].setFitness(evaluateSingle(index));
	}
	else {
		//mutation
		int index = tournament();
		ind.setSolution(individuals[index].getSolution());
		ind.mutate();
		individuals[index]=ind;
		individuals[index].setFitness(evaluateSingle(index));
	}
}

int main() {
	srand(time(NULL));
	vector <string> functionSet = { "+", "-", "*", "/" };
	vector <string> terminalSet = { "x" };

	ifstream f("values.txt");
	for (int i = 0; i < 42; i++) {
		f >> testArray[i][0] >> testArray[i][1];
	}
	for (int i = 0; i < POPULATION_SIZE; i++) {
		char type = 'f';

		if (rand() % 2 < 0.5) {
			type = 'g';
		}
		Individual ind(functionSet, terminalSet, -5, 5, 3+(i/2000), type, .1, 0.8);
		individuals.push_back(ind);
	}
	evaluateFitness();
	//1000 generations
	cout << "Starting...\n";
	for (; generation < 100000; generation++) {
		generateOffspring();
		
		//cout << "Generation: " << i << "\t Best Fitness: " << bestFitness << "\nSolution: " << individuals[bestIndex].getSolution() << endl << endl;
	}
	string bestSolution = individuals[bestIndex].getSolution();
	size_t nodesBest = count(bestSolution.begin(), bestSolution.end(), ' ');
	cout << "Generation: " << generation << "\t Best Fitness: " << bestFitness << "\nSolution: " << bestSolution << endl <<"NODES: "<<nodesBest+1<< endl;
	return 0;
}


