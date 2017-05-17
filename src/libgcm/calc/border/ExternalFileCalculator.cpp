#include "libgcm/calc/border/ExternalFileCalculator.hpp"

#include <boost/lexical_cast.hpp>
#include "libgcm/node/CalcNode.hpp"

using boost::lexical_cast;

using namespace gcm;
using std::vector;

ExternalFileCalculator::ExternalFileCalculator()
{
    U_gsl = gsl_matrix_alloc (9, 9);
    om_gsl = gsl_vector_alloc (9);
    x_gsl = gsl_vector_alloc (9);
    p_gsl = gsl_permutation_alloc (9);
    data.clear();
    time.clear();
};

ExternalFileCalculator::~ExternalFileCalculator()
{
    gsl_matrix_free(U_gsl);
    gsl_vector_free(om_gsl);
    gsl_vector_free(x_gsl);
    gsl_permutation_free(p_gsl);
    data.clear();
    time.clear();
};

void ExternalFileCalculator::setParameters(const xml::Node& params)
{
    char fname[1000];
    strcpy(fname, params["fname"].c_str());
    int stepmin = lexical_cast<int>(params.getAttributeByName("stepmin", "0"));
    int stepmax = lexical_cast<int>(params.getAttributeByName("stepmax", "10000000"));
    FILE* f = fopen(fname, "r");
    int test = 1, i=0;
    float tmp, start;
    test = fscanf(f, "%f", &tmp);
    for (int i = 0; test > 0; i++)
    {
        if (i < stepmin || i > stepmax) continue;
	if (i == stepmin) start = tmp; 
	time.push_back(tmp - start);
        for (int j=0; j<9; j++)
        {
            test = fscanf(f, "%f", &tmp);
            data.push_back(tmp);
        }
        test = fscanf(f, "%f", &tmp);
        printf("%d %d\n", i, test);
    }
    fclose(f);
};

void ExternalFileCalculator::doCalc(CalcNode& cur_node, CalcNode& new_node, RheologyMatrixPtr matrix,
                            vector<CalcNode>& previousNodes, bool inner[],
                            float outer_normal[], float scale)
{
    float ts = Engine::getInstance().getCurrentTime();
    int i=0;
    for(; i<time.size(); i++)
	if (ts < time[i]) break;
    
    for(int j = 0; j < 9; j++)
        new_node.values[j] = data[i*9 + j];

};
