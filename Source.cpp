/*
 * Idea: Instead of strings, generate list of opperations
 */

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
#include <cmath>
#include <limits>
#include <cstdint>

#define TOURNAMENT_SIZE 3
#define POPULATION_SIZE 1000
#define INPUT_SIZE 21.0
#define SAMPLE_SIZE 21
#define GENERATIONS 10000

using namespace std;

double testArray [SAMPLE_SIZE][2];
vector <string> functionSet = { "+", "-", "*", "/" };
vector <Individual> individuals;

double crossoverRate = 0.9;
double mutationChance = 0.1;
double bestFitness = (double)INT32_MAX;
int bestIndex = -1;
int generation = 0;

double calculate(double op1, double op2, string operation) {
    //this method executes the operations found
    //in the function string
	switch (operation[0]) {
	case '+':
		return op1 + op2;
	case '-':
		return op1 - op2;
	case '*':
		return op1 * op2;
	case '/':
        //protected division, can't divide by 0
		if (op2 < 0.0000001 && op2 > -0.0000001) {
			return 0;
		}
		else {
			return op1 / op2;
		}
	}
}

/*
 * This method should be modified to only parse once
 * and execute the tree with different values of X
 */
double evaluate(Individual ind, double valueX) {
	set <string> functions(functionSet.begin(), functionSet.end());
	stack <double> calculator;
	string::size_type sz;
	stringstream ss(ind.getSolution());
	string item;
	vector <string> tokens;

    //tokenize the string
    //perhaps a faster approach would be to move through the
    //string like in the mutation and crossover methods
	while (getline(ss, item, ' ')) {
		tokens.push_back(item);
	}
	int i = tokens.size() - 1;
    //the string is evaluated from the end towards the front
	while (i >= 0) {
		if (functions.find(tokens[i]) == functions.end()) {
			//not an operator, push to stack
			if (tokens[i] == "x")   //If an x is found, then substitute it for the wanted value
				calculator.push(valueX);    //This should be changed to allow for more than only x as a value
			else
				calculator.push(stod(tokens[i], &sz)); //push the value of the token
		}
		else {
            /*
            * If an operator is found, two values are extracted from the
            * stack and the operation is applied.
            * This should be changed to accommodate for different
            * arities in the operation set
            */
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
    //Evaluate the string against the multiple test cases
	for (int i = 0; i < POPULATION_SIZE; i++) {
		double error = 0;
		for (int j = 0; j < SAMPLE_SIZE; j++) {
            //Mean square error
			error+= pow(testArray[j][1] - evaluate(individuals[i], testArray[j][0]), 2);
		}
		individuals[i].setFitness(error / INPUT_SIZE);
		if ((error / INPUT_SIZE) < bestFitness) { //This is used to keep track of the generations
			bestFitness = error / INPUT_SIZE;
			bestIndex = i;
			cout << "Generation: " << generation << "\t Best Fitness: " << bestFitness << "\nSolution: " << individuals[i].getSolution() << endl << endl;
		}
    }
}

double evaluateSingle(int index) {
    //only run the tests for a single individual
	double error = 0;
	for (int j = 0; j < SAMPLE_SIZE; j++) {
		error += pow(testArray[j][1] - evaluate(individuals[index], testArray[j][0]), 2);
	}
	if ((error / INPUT_SIZE) < bestFitness) {
		bestFitness = error/INPUT_SIZE;
		cout << "Generation: " << generation << "\t Best Fitness: " << bestFitness << "\nSolution: " << individuals[index].getSolution() << endl << endl;
		bestIndex = index;
	}
	return (error / INPUT_SIZE);
}


int tournament(bool type){
    //Tournament selection
    //type = true for normal, false for negative tournament
	double fitness = (double)INT32_MAX;
    if(!type)fitness = 0;
	int index = 0;
	for (int i = 0; i < TOURNAMENT_SIZE; i++) {
		int ind = rand() % POPULATION_SIZE;
		string s = individuals[ind].getSolution();
		int nodes = count(s.begin(), s.end(), ' ');
        //check tournament type
		if (((individuals[ind].getFitness() + nodes*0.2)< fitness)==type) {
			fitness = individuals[ind].getFitness();
			index = ind;
		}
	}
	return index;
}
/*
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
*/
void generateOffspring() {
	double random = ((double)rand() / (RAND_MAX));
	Individual ind;
    //offspring is generated either by crossover or mutation
	if (random < crossoverRate) {
		//crossover, pick two parents
		int parent1 = tournament(true);
		int parent2 = tournament(true);
		ind.crossOver(individuals[parent1].getSolution(), individuals[parent2].getSolution());
		//find a suitable candidate to replace
		int index = tournament(false);
		individuals[index] = ind;
		individuals[index].setFitness(evaluateSingle(index));
	}
	else {
		//mutation
		int index = tournament(true);
		ind.setSolution(individuals[index].getSolution());
		ind.mutate();
        index = tournament(false);
		individuals[index]=ind;
		individuals[index].setFitness(evaluateSingle(index));
	}
}

int main() {
	srand(time(NULL));
	vector <string> functionSet = { "+", "-", "*", "/" };
	vector <string> terminalSet = { "x" };

	ifstream f("values.txt");
	for (int i = 0; i < SAMPLE_SIZE; i++) {
		f >> testArray[i][0] >> testArray[i][1];
	}
	for (int i = 0; i < POPULATION_SIZE; i++) {
		char type = 'f';

		if (rand() % 2 < 0.5) {
			type = 'g';
		}
		Individual ind(functionSet, terminalSet, -5, 5, 3+(i/(POPULATION_SIZE/5)), type, mutationChance, crossoverRate);
		individuals.push_back(ind);
	}
	evaluateFitness();
	cout << "Starting...\n";
	for (; generation < GENERATIONS; generation++) {
		generateOffspring();
        if(bestFitness < 0.000001){
            break;
        }
	}
	string bestSolution = individuals[bestIndex].getSolution();
	size_t nodesBest = count(bestSolution.begin(), bestSolution.end(), ' ');
	cout << "Generation: " << generation << "\t Best Fitness: " << bestFitness << "\nSolution: " << bestSolution << endl <<"NODES: "<<nodesBest+1<< endl;
	return 0;
}


