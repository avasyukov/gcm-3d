Grid::Grid() { logger = NULL; rheology = NULL; method = NULL; };

Grid::~Grid() { };

void Grid::attach(Logger* new_logger) { logger = new_logger; };

void Grid::attach(NumericalMethod* new_numerical_method) { method = new_numerical_method; };

void Grid::attach(RheologyCalculator* new_rheology) { rheology = new_rheology; };
