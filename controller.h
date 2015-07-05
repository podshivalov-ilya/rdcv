#include "conexec.h"
#include "confgen.h"

// Implement here class properties for confgen and access methods for 'em
class RDCVController : public Controller
{
    public:
        RDCVController();
        ~RDCVController(){ ~Controller(); }
        char * getData();
        void setData(char *);

        void setInputSize(size_t inpSiz) { cfg->setInputDimention(inpSiz); }
        void setFirstHL(size_t lb, size_t tb) { cfg->setFirstHiddenLayerRange(lb, tb); }
        void setOtherHL(size_t lb, size_t tb) { cfg->setOtherHiddenLayerRange(lb, tb); }
        void setRangeHL(size_t lb, size_t tb) { cfg->setHiddenLayersRange(lb, tb); }

        bool prepare() { bool cfg.prepare(); }

    private:
        ConfGen *cfg;
        std::string data;
};
