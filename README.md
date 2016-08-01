# FunctionFinder
A genetic programming approach to finding a function that aproximates a series of X and Y pairs.

Parameters:

<b>TOURNAMENT_SIZE:<b> Determines the ammount of individuals used in tournaments.
<b>POPULATION_SIZE:<b> Determines the ammount of individuals to create at the start
<b>INPUT_SIZE:<b> Used for obtaining fitness. Must be the same as <b>SAMPLE_SIZE<b> but in decimal format
<b>SAMPLE_SIZE:<b> Ammount of data available
<b>GENERATIONS:<b> Maximum ammount of generations
<b>PARSIMONY_PRESSURE:<b> Constant that reduces fitness of large individuals while evaluating.
