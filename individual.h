#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <stack>
#include <set>
#include <algorithm>


using namespace std;

class Individual {
private:
	string solution;
	double fitness;
	vector <string> functions;
	vector <string> terminals;
	double minValue;
	double maxValue;
	double mutationChance;
	double crossoverChance;

public:
	Individual();
	Individual(vector <string> functionSet, vector <string> terminalSet, int minimum, int maximum, int depth, char type, double mC, double cC);
	//sets
	void setSolution(string s);
	void setFitness(double f);

	//gets
	string getSolution();
	double getFitness();

	string initialize(int depth, char type);

	int endIndexOfNode(int start, string str);

	void mutate();

	void crossOver(string a, string b);

	void print();
};

Individual::Individual() {
	solution = "";
	fitness = 0;
	minValue = 0;
	maxValue = 0;
	mutationChance = 0;
	crossoverChance = 1;
}

Individual::Individual(vector <string> functionSet, vector <string> terminalSet, int minimum, int maximum, int depth, char type, double mC, double cC) {
	functions = functionSet;
	terminals = terminalSet;
	minValue = minimum / 1.0;
	maxValue = maximum / 1.0;
	solution = initialize(depth, type);
	fitness = 0;
	mutationChance = mC;
	crossoverChance = cC;
}

void Individual::setSolution(string s) {
solution = s;
}

void Individual::setFitness(double f) {
	fitness = f;
}

string Individual::getSolution() {
	return solution;
}

double Individual::getFitness() {
	return fitness;
}

//rampant half and half
string Individual::initialize(int depth, char type) {
	double r = (double)rand() / ((double)RAND_MAX);
	if (depth == 0 || (type == 'g' && r < ((double)(terminals.size()) / ((double)(terminals.size() + functions.size()))))) {
		//stop growth for this branch
		int random = rand() % (terminals.size() + 1);
		if (random == terminals.size()) {
			//generate and return a constant
			double constant = (rand() % (int)(maxValue * 1000 - minValue * 1000)) / 1000.0;
			ostringstream strs;
			strs << constant;
			return strs.str();
		}
		else {
			return terminals[random];
		}
	}
	else {
		int random = rand() % functions.size();
		string function = functions[random];
		//all functions have arity 2
		string argument1 = initialize(depth - 1, type);
		string argument2 = initialize(depth - 1, type);
		return function + " " + argument1 + " " + argument2;
	}
}

void Individual::print() {
	//print a tree
	int x = 0;
}

int Individual::endIndexOfNode(int start, string str) {
	//check if node is a number
	int i = start;
	if ((str[i] >= 48 && str[i] <= 57) | str[i] == 'x') {
		//find end of number
		while (i<str.length() && str[i] != ' ')
			i++;
		return i;
	}

	//if str is a function:
	stack <char> s;
	s.push(str[i]);
	i += 2;
	while (!s.empty()) {
		//advance to the next node
		if ((str[i] >= 48 && str[i] <= 57) | str[i] == 'x') {
			//node is a terminal
			while (i < str.length() && str[i] != ' ')
				i++;

			bool top = true;
			//check if top of stack is a terminal
			while ((s.top() >= 48 && s.top() <= 57) | s.top() == 'x') {
				top = false;
				s.pop();
				s.pop();
				if (s.empty()) {
					return i;
				}
				else {
					if ((s.top() >= 48 && s.top() <= 57) | s.top() == 'x') {
						continue;
					}
					else {
						s.push('0');
						break;
					}
				}
			}
			if (top) {
				s.push('0');
			}
		}
		else {
			s.push(str[i]);
			i++;
		}
		i++;
	}
	return i;
}

void Individual::mutate() {
	//go node per node checking for mutation
	int endIndex = 0;
	for (int i = 0; i < solution.length(); i++) {
		double random = (double)rand() / ((double)RAND_MAX);
		if (random < mutationChance) {
			//this node is mutated
			char type = 'g';
			if (rand() % 2 < 0.5) {
				type = 'g';
			}
			endIndex = endIndexOfNode(i, solution);
			string mutation = initialize((rand() % 4) + 1, type);
			solution.replace(i, endIndex - i, mutation);
			i = i + mutation.length();
			while (i<solution.length() && solution[i] != ' ')
				i++;
			continue;
		}
		else {
			//advance to the next node
			if (solution[i] >= 48 && solution[i] <= 57) {
				//node is a number
				while (i<solution.length() && solution[i] != ' ')
					i++;
			}
			else {						//THIS SHOULD CHANGE IF FUNCTIONS ARE REPRESENTED BY MORE THAN ONE LETTER
				//not a number, advance once
				i++;
			}
		}
	}

}

void Individual::crossOver(string a, string b) {
	int nodesA = count(a.begin(), a.end(), ' ');
	int nodesB = count(b.begin(), b.end(), ' ');
	if (nodesA == 0 && nodesB == 0) {
		solution = a;
		return;
	}
	if (nodesA == 0) {
		solution = b;
		return;
	}
	if (nodesB == 0) {
		solution = a;
		return;
	}

	//look for a crossOver spot in parent a
	int crossOverIndex_A = -1;
	int crossOverIndex_B = -1;

	while (crossOverIndex_A == -1) {
		for (int i = 0; i < a.length(); i++) {
			if (rand() % 10 <= 1) {
				//pick this node
				crossOverIndex_A = i;
				break;
			}
			//advance to the next node
			if(a[i]>=48 && a[i] <= 57){
				//node is a number
				while (i<a.length() && a[i] != ' ')
					i++;
			}
			else {						//THIS SHOULD CHANGE IF FUNCTIONS ARE REPRESENTED BY MORE THAN ONE LETTER
				//not a number, advance once
				i++;
			}
		}
	}
	while (crossOverIndex_B == -1) {
		for (int i = 0; i < b.length(); i++) {
			if (rand() % 10 <= 1) {
				//pick this node
				crossOverIndex_B = i;
				break;
			}
			//advance to the next node
			if (b[i] >= 48 && b[i] <= 57) {
				//node is a number
				while (i<b.length() && b[i] != ' ')
					i++;
			}
			else {						//THIS SHOULD CHANGE IF FUNCTIONS ARE REPRESENTED BY MORE THAN ONE LETTER
										//not a number, advance once
				i++;
			}
		}
	}
	solution = a;
	int endA = endIndexOfNode(crossOverIndex_A, a);
	int endB = endIndexOfNode(crossOverIndex_B, b);
	string s = b.substr(crossOverIndex_B, endB-crossOverIndex_B);
	solution.replace(crossOverIndex_A, endA-crossOverIndex_A, b.substr(crossOverIndex_B, endB - crossOverIndex_B));
}

#endif