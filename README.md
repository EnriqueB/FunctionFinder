# FunctionFinder
A genetic programming approach to finding a function that aproximates a series of data points.

## Compilation
`g++ -std=c++11 Source.cpp`

### Parameters:

**TOURNAMENT_SIZE:** Determines the ammount of individuals used in tournaments.

**POPULATION_SIZE:** Determines the ammount of individuals to create at the start

**INPUT_SIZE:** Used for obtaining fitness. Must be the same as **SAMPLE_SIZE** but in decimal format

**SAMPLE_SIZE:** Ammount of data available

**GENERATIONS:** Maximum ammount of generations

**PARSIMONY_PRESSURE:** Constant that reduces fitness of large individuals while evaluating.

**AMMOUNT_VARIABLES:** Determines the ammount of variables to read. Does **NOT** take into account the variable to be aproximated. I.e. in y = 2x+3z, the 'y' should not be counted for this value.

### functions.txt
This file contains the list of functions that the system will use. Each function can only be represented by one character\
#### Format
*functionChar* arity

For example:
> \+ 2

> \* 2

> p 1

> h 3

Some of the most common functions have been implemented, but the user can add new functions by adding the procedures in the source.cpp file.

### values.txt
This file contains the list of variables and values that the system will use. The values most be in decimal format. (1.0, 100.2, 30.0, etc.).

#### Format
*variableChar* *variableChar* *variableChar* ...

value value value

value value value

For example:

> y x z

>1.0 0.0 1.0

>2.0 1.0 1.0

>3.0 0.0 5.0

>4.0 9.0 2.0

>-1.0 10.0 20.0
