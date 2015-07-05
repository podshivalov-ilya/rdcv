#include "controller.h"

RDCVController::RDCVController()
{
    cfg = new ConfGen();
}

RDCVController::~RDCVController()
{
    delete ConfGen();
}

char * RDCVController::getData()
{
    if(cfg->next())
        return cfg->textConf().c_str();
    return NULL;
}

void RDCVController::setData(char * d)
{
    data.clear();
    data = d;
    std::cout << data << std::endl;
}
