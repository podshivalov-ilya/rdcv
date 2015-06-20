#include "confgen.h"

ConfGen::ConfGen()
{
    // There are (4) parametres that should be initialized
    firstHiddenLayerLB = 0;
    firstHiddenLayerTB = 0;

    in_cnt = 0;
    out_cnt = 1;

    // There are (4) parametres that initialized manually
    hiddenLayersLB = 0;
    hiddenLayersTB = 0;

    totalHiddenLayerLB = 0;     // if one of previous two parametres not null there are should be not null too
    totalHiddenLayerTB = 0;     // if one of previous two parametres not null there are should be not null too
    currentFirstHL = 0;
    currentHL = 0;
    prepared = false;
}

void ConfGen::setFirstHiddenLayerRange(size_t lb, size_t tb)
{
    firstHiddenLayerLB = lb < tb? lb: tb;
    firstHiddenLayerTB = lb < tb? tb: lb;
}

void ConfGen::setOtherHiddenLayerRange(size_t lb, size_t tb)
{
    totalHiddenLayerLB = lb < tb? lb: tb;
    totalHiddenLayerTB = lb < tb? tb: lb;
}

void ConfGen::setHiddenLayersRange(size_t lb, size_t tb)
{
    hiddenLayersLB = lb < tb? lb: tb;
    hiddenLayersTB = lb < tb? tb: lb;
    currentHL = hiddenLayersLB;
}

std::vector<size_t> ConfGen::get()
{
    if(currentConfig.size() < 3)
        return std::vector<size_t>();
    else
        return currentConfig;
}

void ConfGen::reset()
{
    currentFirstHL = firstHiddenLayerLB;

    midLayer.clear();
    for(size_t i = 0; i < currentHL; i++)
        midLayer.push_back(totalHiddenLayerLB);
    currentHL++;
}

bool ConfGen::next()
{
    if(prepared){
        if(currentFirstHL > firstHiddenLayerTB){
            if(currentHL > hiddenLayersTB)
                return false;
            reset();
        }
        else if(midLayer.size() > 0){
            for(size_t i = midLayer.size() - 1; i >= 0; i--){
                if(midLayer[i] >= totalHiddenLayerTB){
                    midLayer[i] = totalHiddenLayerLB;
                    if(i <= 0){
                        currentFirstHL++;
                        if(currentFirstHL > firstHiddenLayerTB){
                            if(currentHL > hiddenLayersTB)
                                return false;
                            reset();
                        }
                        break;
                    }
                    continue;
                }
                else{
                    midLayer[i]++;
                    break;
                }
            }
        }
        else{
            currentFirstHL++;
            if(currentFirstHL > firstHiddenLayerTB){
                currentHL++;
                if(currentHL > hiddenLayersTB)
                    return false;
                reset();
            }
        }

        // Build dsc
        /*Input layer*/
        currentConfig.clear();
        currentConfig.push_back(in_cnt);

        currentConfig.push_back(currentFirstHL);

        if(midLayer.size() > 0)
            currentConfig.insert(currentConfig.end(), midLayer.begin(), midLayer.end());

        /*Output layer*/
        currentConfig.push_back(out_cnt);
        return true;
    }
    else
        return false;
}

bool ConfGen::prepare()
{
    currentFirstHL = firstHiddenLayerLB;
    currentHL = hiddenLayersLB;
    if(hiddenLayersLB != 0){
        reset();
        midLayer[midLayer.size() - 1] = totalHiddenLayerLB - 1;
    }
    if(firstHiddenLayerLB == 0 || (totalHiddenLayerLB == 0 && hiddenLayersTB != 0))
        return prepared;
    prepared = true;
    return prepared;
}

std::ostream &operator<< (std::ostream &out, ConfGen &cfg)
{
    std::vector<size_t> config = cfg.get();
    for(size_t i = 0; i < config.size() - 1; i++){
        out << config[i] << "-";
    }
    out << config[config.size() - 1];
    return out;
}
