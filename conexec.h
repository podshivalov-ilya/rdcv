class Controller
{
    public:
        virtual char * getData() = 0;
        virtual void setData(char *) = 0;
};

class Executor
{
    public:
        virtual char * getData() = 0;
        virtual void setData(char *) = 0;
        // Here I need in method for sending pointer to common data
};
