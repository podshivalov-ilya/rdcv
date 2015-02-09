#ifndef NNMODEL
#define NNMODEL

#include <iostream>
#include <vector>
#include <string>

#include <cmath>

#include <shark/Data/Dataset.h>
#include <shark/Data/Csv.h>
#include <shark/ObjectiveFunctions/Loss/SquaredLoss.h>
#include <shark/ObjectiveFunctions/ErrorFunction.h>
#include <shark/Models/FFNet.h>
#include <shark/Core/Exception.h>

#include <shark/Algorithms/GradientDescent/Rprop.h>

// For pre-training
#include <shark/Unsupervised/RBM/BinaryRBM.h>
#include <shark/Algorithms/GradientDescent/SteepestDescent.h>
#include <shark/ObjectiveFunctions/Regularizer.h>

using shark::LabeledData;
using shark::RealVector;
using shark::RealMatrix;

using std::vector;
using std::cerr;
using std::endl;

typedef enum evalstate {
    EVALNORM = 0,
    CHECKPARAMS,
} EVALSTATE;

typedef shark::FFNet<shark::LogisticNeuron, shark::LinearNeuron> Network;
typedef shark::LabeledData<shark::RealVector, shark::RealVector> Dataset;

class NNModel // RDCV
{
    private:
        int nAllData; // 4 by default
        int nCalibrationSet; // 4 by default

        // Lower bound of items of first hidden layer
        unsigned int firstHiddenLayerLB;
        // Higher bound of items of first hidden layer
        unsigned int firstHiddenLayerTB;

        // Lower bound of hidden layers
        unsigned int hiddenLayersLB;
        // Higher bound of hidden layers
        unsigned int hiddenLayersTB;
        // Lower bound of items for hidden layers
        unsigned int totalHiddenLayerLB;
        // Higher bound of items for hidden layers
        unsigned int totalHiddenLayerTB;
        // Input dimention
        size_t in_cnt;
        // Set boltzman
        bool preBM;

        // NN Model
        Network network;
        // NN Dataset
        Dataset dataset;
        // NN Loss
        shark::SquaredLoss<> loss;

        /* ======================================*/
        bool checkParams();
        // Function which generate NN configurations from ranges
        std::vector<std::vector<size_t> > genMidLayers(unsigned beg, unsigned end, unsigned count);
        int push_back_s(std::vector<size_t> *changeable, const std::vector<size_t> &items);

//#ifdef DEBUG
        // For output network arcitecture
        int echoArch(vector<size_t> arr, std::string pre = "");
//#endif

    public:
        NNModel();

        bool setFirstHiddenLayerRange(unsigned int lb, unsigned int tb);
        bool setOtherHiddenLayerRange(unsigned int lb, unsigned int tb);
        bool setHiddenLayersRange(unsigned int lb, unsigned int tb);
        void setPreBM(bool);

        //bool loadData(std::string dataFileName);
        bool loadData(std::string dataFileName, std::string labelsFileName);
        shark::BinaryRBM trainRBM(
                shark::UnlabeledData<RealVector> const& data,//the data to train with
                std::size_t numHidden,//number of features in the AutoencoderModel
                std::size_t iterations, //number of iterations to optimize
                double regularisation,//strength of the regularisation
                double learningRate // learning rate of steepest descent
                );
        Network unsupervisedPreTraining(
                shark::UnlabeledData<RealVector> const& data,
                vector<size_t> layers,
                double regularisation, std::size_t iterations, double learningRate
                );

        int evaluate();
};

#endif
