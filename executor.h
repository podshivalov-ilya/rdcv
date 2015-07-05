#include "conexec.h"

class RDCVExecutor : public Executor
{
    public:
        RDCVExecutor();
        ~RDCVExecutor();

        void setData(char *);
        char * getData();

        void exec();
};
