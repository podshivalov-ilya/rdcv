extern "C"{
#include <unistd.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/epoll.h>
} 

#include <vector>
#include <string>
#include "conexec.h"

enum status
{
    ERRFORK -1,
    ERRPOLL -2,
    ERRECRT -3,
    ERRECTL -4,
    ERRMALL -5,
    SUCCESS 0
};

class Parallelizer
{
    public:
        Parallelizer();

        void setController(Controller *c){ ctl = c; }
        void addExecutor(Executor *e){ execs.push_back(e); }
        extern "C" int start();

    private:
        Controller *ctl;
        std::vector<Executor *> execs;
        // void *data;
        extern "C" static void signals_handler(int signo)
};
