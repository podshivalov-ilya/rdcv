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

using shark::LabeledData;
using shark::RealVector;
using shark::RealMatrix;

using std::vector;

vector<vector<size_t> > genMidLayers(unsigned beg, unsigned end, unsigned count)
{
    vector<vector<size_t> > lst;
    if(count == 1){
        for(int i = beg; i <= end; i++){
            vector<size_t> l;
            l.push_back(i);
            lst.push_back(l);
        }
    }
    else{
        vector<vector<size_t> > lst2 = genMidLayers(beg, end, count - 1);
        for(int i = beg; i <= end; i++){
            for(int j = 0; j < lst2.size(); j++){
                vector<size_t> l = lst2[j];
                l.push_back(i);
                lst.push_back(l);
            }
        }
    }
    return lst;
}

int push_back_s(vector<size_t> *changeable, const vector<size_t> &items)
{
    vector<size_t>::const_iterator item;
    for(item = items.begin(); item != items.end(); item++){
        changeable->push_back(*item);
    }
}

LabeledData<RealVector, RealVector> loadData()
{
    shark::Data<RealVector> data;
    shark::Data<RealVector> labels;

    std::string dataFile = "descriptors.csv";
    std::string labelsFile = "mtp.csv";

    try{
        shark::importCSV(labels, labelsFile, ' ', '#');
        shark::importCSV(data, dataFile, ' ', '#');
    } catch (shark::Exception e){
        std::cerr << "Exception!\n\t" << e.file() << "\t" << e.line() << "\t" << e.what() << std::endl;
        std::cerr << "Unable to open file " <<  dataFile << " and/or " << labelsFile << ". Check paths!" << std::endl;
        exit(EXIT_FAILURE);
    }
    LabeledData<RealVector, RealVector> dataset(data, labels);
    return dataset;
}

int echoArray(std::vector<std::vector<double> *> arr, std::string pre = "")
{
    std::vector<std::vector<double> *>::iterator ul;
    for(ul = arr.begin(); ul != arr.end(); ul++) {
        std::vector<double>::iterator dl;
        std::cout << pre;
        for(dl = (*ul)->begin(); dl != (*ul)->end(); dl++)
            std::cout << *dl << "\t";
        std::cout << std::endl;
    }
}

int echoArrayP(std::vector<std::vector<double> *> arr, std::string pre = "")
{
    std::cout << pre;
    for(int ul = 0; ul < arr.size(); ul++)
        std::cout << arr[ul] << "\t";
    std::cout << std::endl;
}

int echoArch(vector<size_t> arr, std::string pre = "")
{
    std::cout << pre;
    for(int ul = 0; ul < arr.size(); ul++) {
        if(ul == arr.size() - 1)
            std::cout << arr[ul];
        else
            std::cout << arr[ul] << "->";
    }
    std::cout << std::endl;
}

int main(int argc, const char *argv[])
{
    // Number of partitions
    int nAllData = 4;
    int nCalibrationSet = 4;

    // ========== First hidden layer configuration ========== //
    // Lower bound of items of first hidden layer
    unsigned int firstHiddenLayerLB = 12;
    // Higher bound of items of first hidden layer
    unsigned int firstHiddenLayerTB = 20;

    // ========== Other hidden layers configuration ========== //
    // Lower bound of hidden layers
    unsigned int hiddenLayersLB = 1;
    // Higher bound of hidden layers
    unsigned int hiddenLayersTB = 4;
    // Lower bound of items for hidden layers
    unsigned int totalHiddenLayerLB = 1;
    // Higher bound of items for hidden layers
    unsigned int totalHiddenLayerTB = 6;

    shark::FFNet<shark::LogisticNeuron, shark::LinearNeuron> network;
    LabeledData<RealVector, RealVector> dataset = loadData();
    std::cout << "DS: " << dataset.numberOfElements() << std::endl;

    size_t in_cnt = dataset.inputs().element(0).size();

    shark::SquaredLoss<> loss;

    for(int fhl = firstHiddenLayerLB; fhl < firstHiddenLayerTB; fhl++){
        for(int hlCount = hiddenLayersLB; hlCount < hiddenLayersTB; hlCount++){
            if(hlCount > 0){
                vector<vector<size_t> > hiddenMidLayers = genMidLayers(totalHiddenLayerLB, totalHiddenLayerTB, hlCount);
                vector<vector<size_t> >::iterator l;
                for(l = hiddenMidLayers.begin(); l != hiddenMidLayers.end(); l++){
                    std::vector<size_t> layers;
                    // Setting up architecture of NN
                    layers.push_back(in_cnt);
                    layers.push_back(fhl);
                    push_back_s(&layers, *l);
                    layers.push_back(1);

                    network.setStructure(layers, shark::FFNetStructures::Normal, true);
                    // All prepared
                    echoArch(layers);

                    LabeledData<RealVector, RealVector> testSet, validationSet, trainingSet;
                    trainingSet = dataset;
                    testSet = shark::splitAtElement(trainingSet, static_cast<size_t>((1.0 - 1.0/nAllData) * trainingSet.numberOfElements()));
                    validationSet = shark::splitAtElement(trainingSet, static_cast<size_t>((1.0 - 1.0/nCalibrationSet) * trainingSet.numberOfElements()));

                    shark::initRandomUniform(network, -0.1, 0.1);
                    shark::IRpropPlus optimizer;
                    shark::ErrorFunction<RealVector, RealVector> error(trainingSet, &network, &loss);
                    optimizer.init(error);
                    for(int i = 0; i < trainingSet.numberOfElements(); i++)
                        optimizer.step(error);
                }
            }
        }
    }

    std::cin.get();
    return 0;
    //*/
}
