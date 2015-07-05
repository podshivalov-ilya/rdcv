#define DEBUG
#include "nnmodel.h"
#include "lrmodel.h"
#include <iostream>
#include <sstream>
#include <string>
// gnuplot
//#include "confgen.h"

int main(int argc, const char *argv[])
{
    /*
    ConfGen cfg;
    cfg.setInputDimention(50);
    cfg.setOutputDimention(1);
    cfg.setFirstHiddenLayerRange(0, 61);
    cfg.setOtherHiddenLayerRange(1, 2);
    cfg.setHiddenLayersRange(0, 2);
    if(cfg.prepare()){
        while(cfg.next())
            std::cout << cfg << std::endl;
    }

    
    */
    std::string fileDesc = "", fileRes = "";
    int fhlT = -1, fhlB = -1;
    int ohlT = -1, ohlB = -1;
    int hlrT = -1, hlrB = -1;

    for(int i = 0; i < argc; i++){
        std::string input = argv[i];
        std::istringstream ss(input);

        std::string prm;
        std::string val;
        for(int j = 0; j < 2; j++){
            if(j == 0)
                std::getline(ss, prm, '=');
            else if(j == 1)
                std::getline(ss, val, '=');
        }
        prm = prm.substr(1);
        if(prm == "fileDesc") fileDesc = val;
        else if(prm == "fileRes") fileRes = val;
        else if(prm == "fhlT") fhlT = atoi(val.c_str());
        else if(prm == "fhlB") fhlB = atoi(val.c_str());
        else if(prm == "ohlT") ohlT = atoi(val.c_str());
        else if(prm == "ohlB") ohlB = atoi(val.c_str());
        else if(prm == "hlrT") hlrT = atoi(val.c_str());
        else if(prm == "hlrB") hlrB = atoi(val.c_str());

    }
    if(fileDesc.empty() || fileRes.empty()
            || fhlT <= 0 || fhlB <= 0
            || ohlT <= 0 || ohlB <= 0
            || hlrT <= 0 || hlrB <= 0){
        std::cout << "Please, init all params:\n"
            << "\t-fileDesc: file with descriptors (x)\n"
            << "\t-fileRes: file with results (y)\n"
            << "\t-fhlT: top bottom of the first hidden layer\n"
            << "\t-fhlB: low bottom of the first hidden layer\n"
            << "\t-ohlT: top bottom of the other hidden layer\n"
            << "\t-ohlB: low bottom of the other hidden layer\n"
            << "\t-hlrT: top bottom of count of the other hidden layers\n"
            << "\t-hlrB: low bottom of count of the other hidden layers\n";
        return -1;
    }


    std::cout << "Linear regression\n";
    LRModel *lrForCompare = new LRModel();
    std::cout << lrForCompare->loadData(fileDesc, fileRes) << std::endl;
    lrForCompare->evaluate();
    delete lrForCompare;

    std::cout << "Neural network\n";
    NNModel *rdcvNN = new NNModel();
    std::cout << rdcvNN->loadData(fileDesc, fileRes)
        << std::endl << rdcvNN->setFirstHiddenLayerRange(fhlB, fhlT)
        << std::endl << rdcvNN->setOtherHiddenLayerRange(ohlB, ohlT)
        << std::endl << rdcvNN->setHiddenLayersRange(hlrB, hlrT)
        << std::endl;
    rdcvNN->evaluate();
    delete rdcvNN;
    return 0;
}
