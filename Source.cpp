#include <iostream>
#include <string.h>
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
#include <cmath>
#include <utility>

#include "individual.h"


#define TOURNAMENT_SIZE 5
#define POPULATION_SIZE 1000
#define INPUT_SIZE 11.0
#define SAMPLE_SIZE 11
#define GENERATIONS 100000
#define PARSIMONY_PRESSURE 0.3
#define AMMOUNT_VARIABLES 3

using namespace std;

double testArray [SAMPLE_SIZE][1+AMMOUNT_VARIABLES];
vector <pair <string, int> > functionSet;
vector <string> terminalSet;
vector <Individual> individuals;
int variables [57];
vector <vector <double> > variableValues;


double crossoverRate = 0.9;
double mutationChance = 0.1;
double bestFitness = (double)INT32_MAX;
int bestIndex = -1;
int generation = 0;

vector <double> calculate(const vector <vector <double> > ops, string operation) {
    //this method executes the operations found
    //in the function string
	vector <double> ans;
	switch (operation[0]) {
		case '+':	//arity 2
			for(int i=0; i<ops[0].size(); i++){
				ans.push_back(ops[0][i]+ops[1][i]);
			}
			return ans;
		case '-':	//arity 2
			for(int i=0; i<ops[0].size(); i++){
				ans.push_back(ops[0][i]-ops[1][i]);
			}
			return ans;
		case '*':	//arity 2
			for(int i=0; i<ops[0].size(); i++){
				ans.push_back(ops[0][i]*ops[1][i]);
			}
			return ans;
		case '/':	//arity 2
			//protected division, can't divide by 0
			for(int i=0; i<ops[0].size(); i++){
				if (ops[1][i] < 0.0000001 && ops[1][i] > -0.0000001) {
					ans.push_back(0.0);
				}
				else {
					ans.push_back(ops[0][i]/ops[1][i]);
				}
			}
			return ans;
		case 'l':	//arity 2
			for(int i=0; i<ops[0].size(); i++){
				if(ops[0][i]<=1.001 && ops[0][i] >= 0.999){
					ans.push_back(0.0);
				}
				else if(ops[0][i]<=0.01 || ops[1][i]<=0.01){
					ans.push_back(0.0);
				}
				else{
					ans.push_back(log(ops[1][i])/log(ops[0][i]));
				}
			}
			return ans;
		case 's':	//arity 1
			for(int i=0; i<ops[0].size(); i++){
				ans.push_back(sin(ops[0][i]));
			}
			return ans;	
		case 'c':	//arity 2
			for(int i=0; i<ops[0].size(); i++){
				ans.push_back(cos(ops[0][i]));
			}
			return ans;
		case 'r': //arity 0			//DOESNT WORK YET
			for(int i=0; i<SAMPLE_SIZE; i++){
				double constant = (rand() % (int)(10 * 1000 + abs(-10 * 1000))) / 1000.0 - abs(-10);
				ans.push_back(constant);
			}	
			return ans;
	}
}

int getArity(string func){
	for(int i=0; i<functionSet.size(); i++){
		if(functionSet[i].first == func){
			return functionSet[i].second;
		}
	}
	return -1;
}

vector <double> evaluate(int index) {
	vector <string> functions;
	for(int i=0; i<functionSet.size(); i++){
		functions.push_back(functionSet[i].first);
	}
	stack <vector <double> > calculator;
	
	string::size_type sz;
	stringstream ss(individuals[index].getSolution());
	string item;
	vector <string> tokens;
	//cout<<"String: "<<individuals[index].getSolution()<<endl;
    //tokenize the string
    //perhaps a faster approach would be to move through the
    //string like in the mutation and crossover methods
	while (getline(ss, item, ' ')) {
		tokens.push_back(item);
	}
	int i = tokens.size() - 1;
    //the string is evaluated from the end towards the front
	while (i >= 0) {
		//CHECK IF THIS STILL WORKS
		if (find(functions.begin(), functions.end(), tokens[i]) == functions.end()) {
			//not an operator, push to stack

			if (tokens[i][0]>64 && variables[tokens[i][0]-'A']>=0){   //If an x is found, then substitute it for the wanted value
				calculator.push(variableValues[variables[tokens[i][0]-'A']]);    //This should be changed to allow for more than only x as a value
			}
			else{
				//Push a vector of the value;
				//cout<<"Token: "<<tokens[i]<<endl;
				double val = stod(tokens[i], &sz);
				vector <double> v (SAMPLE_SIZE, val);
				calculator.push(v); //push the value of the token
			}
		}
		else {
			//Find arity of the operator
			vector <vector <double> > ops;
			int arity = getArity(tokens[i]);		//consider changing this to an array for faster search
			for(int j = 0; j < arity; j++){
				ops.push_back(calculator.top());
				calculator.pop();
			}
			calculator.push(calculate(ops, tokens[i]));
		}
		i--;
	}
	return calculator.top();
}

void evaluateFitness() {
    //Evaluate the string against the multiple test cases
	for (int i = 0; i < POPULATION_SIZE; i++) {
		vector <double> results;
		results = evaluate(i);
		double error=0;
		for (int j = 0; j < SAMPLE_SIZE; j++) {
            //Mean square error
			error+= pow(testArray[j][0] - results[j], 2);
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
	vector <double> results = evaluate(index);
	for (int j = 0; j < SAMPLE_SIZE; j++) {
		error += pow(testArray[j][0] - results[j], 2);
	}
	if ((error / INPUT_SIZE) < bestFitness) {
		bestFitness = error/INPUT_SIZE;
		bestIndex = index;
		cout << "Generation: " << generation << "\t Best Fitness: " << bestFitness <<"\nSolution: " << individuals[index].getSolution() << endl << endl;
	}
	return error/INPUT_SIZE;
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
		if (((individuals[ind].getFitness() + nodes*PARSIMONY_PRESSURE) < fitness)==type) {
			if(!type){
				if(ind == bestIndex){
					i--;
					continue;
				}
			}
			fitness = individuals[ind].getFitness() + nodes*PARSIMONY_PRESSURE;
			index = ind;
		}
	}
	return index;
}

void generateOffspring() {
	double random = ((double)rand() / (RAND_MAX));
	Individual ind(functionSet, terminalSet);
	int index;
    //offspring is generated either by crossover or mutation
	if (random < crossoverRate) {
		//crossover, pick two parents
		int parent1 = tournament(true);
		int parent2 = tournament(true);
		ind.crossOver(individuals[parent1].getSolution(), individuals[parent2].getSolution());
		//find a suitable candidate to replace
		index = tournament(false);
		individuals[index].setSolution(ind.getSolution());
		individuals[index].setFitness(evaluateSingle(index));
	}
	else {
		//mutation
		index = tournament(true);
		ind.setSolution(individuals[index].getSolution());
		ind.mutate();
        index = tournament(false);
		individuals[index].setSolution(ind.getSolution());
		individuals[index].setFitness(evaluateSingle(index));
	}
}

int main() {
	srand(time(NULL));
	for(int i=0; i<57; i++){
		variables[i] = -1;
	}
	
	//read function set from file
	ifstream fs("functions.txt");
	string func; 
	int arity;
	while(fs>>func){
		fs>>arity;
		functionSet.push_back(make_pair(func, arity));	
	}
	ifstream f("values.txt");

	string ter;
	f>>ter;
	for(int i=0; i<AMMOUNT_VARIABLES; i++){
		f>>ter;
		terminalSet.push_back(ter);
		variables[ter[0]-'A'] = i;
	}
	for (int i = 0; i < SAMPLE_SIZE; i++) {
		for(int j=0; j<AMMOUNT_VARIABLES + 1; j++){
			f >> testArray[i][j];
		}
	}
	for(int i=1; i<AMMOUNT_VARIABLES+1; i++){
		vector <double> vals;
		for(int j=0; j<SAMPLE_SIZE; j++){
			vals.push_back(testArray[j][i]);
		}
		variableValues.push_back(vals);
	}
	for (int i = 0; i < POPULATION_SIZE; i++) {
		char type = 'f';

		if (rand() % 2 < 0.5) {
			type = 'g';
		}
		Individual ind(functionSet, terminalSet, -10, 10, 2+0*(i/(POPULATION_SIZE/2)), type, mutationChance, crossoverRate);
		individuals.push_back(ind);
	}
	evaluateFitness();
	cout << "Starting...\n";
	for (; generation < GENERATIONS; generation++) {
		generateOffspring();
        if(bestFitness < 0.00001){
			cout<<"Found a good candidate\n";
            break;
        }
	}
	string bestSolution = individuals[bestIndex].getSolution();
	size_t nodesBest = count(bestSolution.begin(), bestSolution.end(), ' ');
	cout << "Generation: " << generation << "\t Best Fitness: " << bestFitness << "\nSolution: " << bestSolution << endl <<"NODES: "<<nodesBest+1<< endl;
	return 0;
}