#include <iostream>
#include <string>
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
#define GENERATIONS 100000
#define PARSIMONY_PRESSURE 0.3

using namespace std;

vector <vector <double> > testArray;
vector <pair <char, int> > functionSet;
vector <char> terminalSet;
vector <Individual> individuals;
int variables [58];
// Stores the arity of a function if it exists. If not, it stores a -1
int functions[93];
vector <vector <double> > variableValues;

double crossoverRate = 0.9;
double mutationChance = 0.1;
double bestFitness = (double)INT32_MAX;
int bestIndex = -1;
int generation = 0;
int SAMPLE_SIZE;
double INPUT_SIZE;
// Does not count 'y', i.e. having y, x, z  = 2
int AMMOUNT_VARIABLES;


/*
 * This method receives a vector of vectors of doubles and uses it to 
 * calculate the results of the required operation. The ammount of 
 * vectors received depends on the arity of the operation required.
 */
vector <double> calculate(const vector <vector <double> > ops, string operation) {
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
			//protected logarithm
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
		case 'r': //arity 0
			for(int i=0; i<SAMPLE_SIZE; i++){
				double constant = (rand() % (int)(10 * 1000 + abs(-10 * 1000))) / 1000.0 - abs(-10);
				ans.push_back(constant);
			}	
			return ans;
	}
}

/*
 * This method parses and evaluates a solution tree for all of the
 * training examples. It tokenizes the string and evaluates 
 * it from the end to the front.
 */
vector <double> evaluate(int index) {
	stack <vector <double> > calculator;
	string::size_type sz;
	stringstream ss(individuals[index].getSolution());
	string item;
	vector <string> tokens;
	while (getline(ss, item, ' ')) {
		tokens.push_back(item);
	}
	int i = tokens.size() - 1;
	//Evaluate solution backwards
	while (i >= 0) {
		if (functions[tokens[i][0] - 33] < 0 || tokens[i].length() > 1){
			//not an operator, push to stack
			if (tokens[i][0] > 64 && variables[tokens[i][0] - 'A'] >= 0){	
				//if the token is a variable then insert the vector of values
				calculator.push(variableValues[variables[tokens[i][0] - 'A']]);
			}
			else{
				//Push a vector of the value
				double val = strtod(tokens[i].c_str(), NULL);
				vector <double> v (SAMPLE_SIZE, val);
				calculator.push(v); //push the value of the token
			}
		}
		else {
			//A vector of vectors is created to manage
			//functions with different arities.
			vector <vector <double> > ops;
			int arity = functions[tokens[i][0] - 33];
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

void evaluateFitness() {
    // Evaluates fitness of all the population.
	for (int i = 0; i < POPULATION_SIZE; i++) {
		vector <double> results;
		results = evaluate(i);
		individuals[i].setFitness(evaluateSingle(i));
    }
}

int tournament(bool type){
    // Tournament selection
    // Type = true for normal, false for negative tournament
	double fitness = (double)INT32_MAX;
    if (!type) fitness = 0;
	int index = 0;
	for (int i = 0; i < TOURNAMENT_SIZE; i++) {
		int ind = rand() % POPULATION_SIZE;
		string s = individuals[ind].getSolution();
		int nodes = count(s.begin(), s.end(), ' ');
        //check tournament type
		if (((individuals[ind].getFitness() + nodes*PARSIMONY_PRESSURE) < fitness) == type) {
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
    // Offspring is generated either by crossover or mutation
	if (random < crossoverRate) {
		//crossover, pick two parents
		int parent1 = tournament(true);
		int parent2 = tournament(true);
		ind.crossOver(individuals[parent1].getSolution(), individuals[parent2].getSolution());
		index = tournament(false);
		individuals[index].setSolution(ind.getSolution());
		individuals[index].setFitness(evaluateSingle(index));
	}
	else {
		// Mutation
		index = tournament(true);
		ind.setSolution(individuals[index].getSolution());
		ind.mutate();
        index = tournament(false);
		individuals[index].setSolution(ind.getSolution());
		individuals[index].setFitness(evaluateSingle(index));
	}
}

void readFunctionSet(){
	ifstream fs("functions.txt");
	char func; 
	int arity;
	while(fs>>func){
		fs>>arity;
		functions[func-33] = arity;
		functionSet.push_back(make_pair(func, arity));	
	}
	fs.close();
}

void readVariables(){
	// Read values and variables from file
	string varFile;
	cout << "Name of variable file: ";
	cin >> varFile;
	ifstream fs(varFile);
	char ter;
	// Read variable letters first
	string vars;
	getline(fs, vars);
	AMMOUNT_VARIABLES = count(vars.begin(), vars.end(), ' ');
	for (int i = 2, j = 0; i < vars.length(); i += 2, j++) {
		terminalSet.push_back(vars[i]);
		variables[vars[i] - 'A'] = j;
	}

	//read values
	double val;
	int i = 0;
	while (fs >> val) {
		vector <double> ts;
		ts.push_back(val);
		for (int j = 1; j<AMMOUNT_VARIABLES+1; j++) {
			fs >> val;
			ts.push_back(val);
		}
		testArray.push_back(ts);
		i++;
	}
	SAMPLE_SIZE = i;
	INPUT_SIZE = (double)SAMPLE_SIZE;

	//create a vector of values for each variables
	//this vectors are used when parsing the tree
	for(int i=1; i<AMMOUNT_VARIABLES+1; i++){
		vector <double> vals;
		for(int j=0; j<SAMPLE_SIZE; j++){
			vals.push_back(testArray[j][i]);
		}
		variableValues.push_back(vals);
	}
	fs.close();
}

int main() {

	srand(time(NULL));
	memset(variables, -1, sizeof(variables));
	memset(functions, -1, sizeof(functions));
	INPUT_SIZE = (double)SAMPLE_SIZE;

	readFunctionSet();
	readVariables();

	//generate individuals
	for (int i = 0; i < POPULATION_SIZE; i++) {
		char type = 'f';
		if (rand() % 2 < 0.5) {
			type = 'g';
		}
		Individual ind(functionSet, terminalSet, -10, 10, 2+(i/(POPULATION_SIZE/5)), type, mutationChance, crossoverRate);
		individuals.push_back(ind);
	}
	//evaluate fitness of the initial population
	evaluateFitness();

	//start of the run
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