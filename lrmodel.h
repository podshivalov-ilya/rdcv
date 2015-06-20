#ifndef LRMODEL
#define LRMODEL

#include <shark/Data/Csv.h>
#include <shark/ObjectiveFunctions/Loss/SquaredLoss.h>
#include <shark/Algorithms/Trainers/LinearRegression.h>

#include <iostream>
#include <string>

using std::cerr;
using std::endl;

class LRModel
{
    private:
        int nValidationSet;
        shark::RegressionDataset dataset;

        shark::LinearRegression trainer;
        shark::LinearModel<> model;
        shark::SquaredLoss<> loss;
    public:
        LRModel();

        bool loadData(std::string dataFileName, std::string labelsFileName);
        int evaluate();
};

#endif
