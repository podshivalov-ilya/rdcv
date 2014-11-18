#include <iostream>
#include <vector>
#include <string>

#include <cmath>

#include <shark/Data/Dataset.h>
#include <shark/Data/Csv.h>
#include <shark/ObjectiveFunctions/Loss/SquaredLoss.h>
#include <shark/Models/FFNet.h>

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

LabeledData<RealVector, RealVector> loadData(size_t *inCount)
{
    shark::Data<RealVector> data;
    shark::Data<RealVector> labels;

    std::string dataFile = "./data/descriptors.csv";
    std::string labelsFile = "./data/mtp.csv";

    try{
        shark::importCSV(data, dataFile, ' ', '#');
        shark::importCSV(labels, labelsFile, ' ', '#');
    } catch (...) {
        std::cerr << "Unable to open file " <<  dataFile << " and/or " << labelsFile << ". Check paths!" << std::endl;
        exit(EXIT_FAILURE);
    }
    LabeledData<RealVector, RealVector> dataset(data, labels);
    return dataset;
}

int echoArray(std::vector<std::vector<double> *> arr, std::string pre = "")
{
    std::vector<std::vector<double> *>::iterator ul;
    for(ul = arr.begin(); ul != arr.end(); ul++)
    {
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

int echoArch(vector<vector<size_t> > arr, std::string pre = "")
{
    std::cout << pre;
    for(int ul = 0; ul < arr.size(); ul++) {
        if(ul == arr.size() - 1)
            std::cout << arr[ul].size();
        else
            std::cout << arr[ul].size() << "->";
    }
    std::cout << std::endl;
}

int main(int argc, const char *argv[])
{
    /* Tested!
       vector<vector<size_t> > l = genMidLayers(-1, 3, 5);
       for(vector<vector<size_t> >::iterator i = l.begin(); i != l.end(); i++){
       for(vector<size_t>::iterator j = i->begin(); j != i->end(); j++){
       std::cout << *j << " ";
       }
       std::cout << std::endl;
       }
       std::cout << l.size() << std::endl;
       return 0;
       */
    int n = 4;

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
    unsigned int generalHiddenLayerLB = 1;
    // Higher bound of items for hidden layers
    unsigned int generalHiddenLayerTB = 6;

    shark::FFNet<shark::LogisticNeuron, shark::LinearNeuron> network;
    LabeledData<RealVector, RealVector> dataset = loadData();
    size_t in_cnt dataset.inputContainer.numberOfElements();
    shark::SquaredLoss<> loss;


    for(int fhl = firstHiddenLayerLB; fhl < firstHiddenLayerTB; fhl++){
        for(int hlCount = hiddenLayersLB; hlCount < hiddenLayersTB; hlCount++){
            if(hlCount > 0){
                vector<vector<size_t> > hiddenMidLayers = genMidLayers(generalHiddenLayerLB, generalHiddenLayersTB, hlCount);
                vector<vector<size_t> >::iterator l;
                for(l = hiddenMidLayers.begin(); l != hiddenMidLayers.end(); l++){
                    std::vector<size_t> layers;
                    // Setting up architecture of NN
                    layers.push_back(in_cnt);
                    layers.push_back(fhl);
                    push_back_s(&layer, l);
                    layers.push_back(1);

                    network.setStructure(layers, shark::FFNetStructures::Normal, true);
                    // All prepared
                    echoArch(layers);
                }
            }
        }
    }

    return 0;
    //*/
}
