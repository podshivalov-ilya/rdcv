//#define DEBUG
//#include "nnmodel.h"
//#include "lrmodel.h"
// gnuplot
#include "confgen.h"

int main(int argc, const char *argv[])
{
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

    
    /*
    std::cout << "Linear regression\n";
    LRModel *lrForCompare = new LRModel();
    std::cout << lrForCompare->loadData("./data/2/descriptors.csv", "./data/2/mtp.csv") << std::endl;
    lrForCompare->evaluate();
    delete lrForCompare;

    std::cout << "Neural network\n";
    NNModel *rdcvNN = new NNModel();
    std::cout << rdcvNN->loadData("./data/2/descriptors.csv", "./data/2/mtp.csv")
        << std::endl << rdcvNN->setFirstHiddenLayerRange(50, 61)
        << std::endl << rdcvNN->setOtherHiddenLayerRange(20, 21)
        << std::endl << rdcvNN->setHiddenLayersRange(1, 2)
        << std::endl;
    rdcvNN->evaluate();
    delete rdcvNN;
    return 0;
    */
}
