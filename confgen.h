#include <vector>
#include <iostream>
#include <string>

class ConfGen
{
    public:
        ConfGen();
        //~ConfGen();

        /*Init class*/
        void setInputDimention(size_t indim) {in_cnt = indim;}
        void setOutputDimention(size_t outdim) {out_cnt = outdim;}

        void setFirstHiddenLayerRange(size_t lb, size_t tb);
        void setOtherHiddenLayerRange(size_t lb, size_t tb);
        void setHiddenLayersRange(size_t lb, size_t tb);

        std::vector<size_t> get();
        bool next();
        bool prepare();
    private:
        // There are not changeable {
        // Lower bound of items of first hidden layer
        size_t firstHiddenLayerLB;
        // Higher bound of items of first hidden layer
        size_t firstHiddenLayerTB;
        // }

        // There are not changeable {
        // Lower bound of hidden layers
        size_t hiddenLayersLB;
        // Higher bound of hidden layers
        size_t hiddenLayersTB;
        // }

        // There are not changeable {
        // Lower bound of items for hidden layers
        size_t totalHiddenLayerLB;
        // Higher bound of items for hidden layers
        size_t totalHiddenLayerTB;
        // }
        
        // {In,Out}put dimentions
        size_t in_cnt;
        size_t out_cnt;

        // Changeable properties
        // For storing current first hidden layer
        size_t currentFirstHL;
        // For storing current hidden layers count
        size_t currentHL;

        // Current config
        std::vector<size_t> currentConfig;
        // Mid layers configurations
        std::vector<size_t> midLayer;

        bool prepared;

        void reset();
        friend std::ostream &operator<< (std::ostream &out, ConfGen &cfg);
};

/*Обрабатывать в цикле, сравнивая предыдущую и текущую конфигурацию*/
