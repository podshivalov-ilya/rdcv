#include "parallelizer.h"

Parallelizer::Parallelizer()
{
    //
}

static void Parallelizer::signals_handler(int signo)
{
    if(signo == SIGUSR1){
        printf("exit success!\n");
        exit(EXIT_SUCCESS);
    }
    else if(signo == SIGPIPE){
        perror("writing to the pipe in main process failed!\n");
    }
}

int Parallelizer::start()
{
    const int childrenCount = execs.size();

    int fd1[childrenCount][2];
    int fd2[childrenCount][2];

    signal(SIGUSR1, signals_handler);

    pid_t pid;
    for(int i = 1; i <= childrenCount; i++){

        pipe(fd1[i-1]);
        pipe(fd2[i-1]);

        if((pid = fork()) < 0){

            perror("Error while calling fork()");
            return ERRFORK;
        }
        else if(pid == 0){
            //printf("Child %d PID %d\n", i, getpid());

            if(i > 1){
                // Here I close connection with previous processes
                for(int j = 0; j <= i-2; j++){
                    close(fd1[j][0]);
                    close(fd1[j][1]);
                }
            }

            dup2(fd2[i-1][0], 0);    // Input fd
            close(fd2[i-1][0]);

            dup2(fd1[i-1][1], 1);    // Output fd
            close(fd1[i-1][1]);

            // CHILD 

            struct pollfd fds[1];
            fds[0].fd = 0;
            fds[0].events = POLLIN;

            int i = 0;
            while(1){
                int ret = poll(fds, 1, -1);
                if(ret == -1){
                    perror("poll error");
                    return ERRPOLL;
                }
                if(fds[0].revents & POLLIN){
                    char c[BUFSIZ];
                    int len = 0;

                    int val = 0;
                    if ((len = read(fds[0].fd, c, BUFSIZ)) != 0){
                        execs[i-1]->setData(c);
                        execs[i-1]->exec();

                        char buf[BUFSIZ];
                        sprintf(buf, "%s %s", c, execs[i-1]->getResults());
                        write(1, buf, BUFSIZ);
                        continue;
                    }
                }
            }
            break;
        }
        else{
            close(fd1[i-1][1]);
            close(fd2[i-1][0]);

            fd1[i-1][1] = fd2[i-1][1];

            // fd1[i-1][0] read fd
            // fd1[i-1][1] write fd
            //printf("Parent iteration %d write fd %d\n", i, fd1[i-1][1]);
        }
    }
    signal(SIGPIPE, signals_handler);

    // PARENT
    // Initializing pipe watcher
    int epfd = epoll_create(childrenCount);
    if(epfd < 0){
        perror("epoll_create!");
        return ERRECRT;
    }

    for(size_t i = 0; i < childrenCount; i++){
        struct epoll_event event;
        int ret;

        event.data.ptr = fd1[i];
        event.events = EPOLLIN;

        ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd1[i][0], &event);
        if(ret){
            perror("epoll_ctl");
            return ERRECTL;
        }
    }

    struct epoll_event *eBack;
    eBack = malloc(sizeof(struct epoll_event));
    if(!eBack){
        perror("malloc");
        return ERRMALL;
    }

    for(size_t i = 0; i < childrenCount; i++){
        char buf[BUFSIZ];
        sprintf(buf, "%s", ctl->getData());   // Send parent controller data to executor
        write(fd1[i][1], buf, BUFSIZ);
    }

    int i = 0;
    // There is waiter for 1 event because after getting results of processes this send new data for processing
    while(epoll_wait(epfd, eBack, 1, -1) > 0){
        if(eBack->events == EPOLLIN){
            char c[BUFSIZ];
            int len = 0;
            int *fd = (int *)eBack->data.ptr;
            if((len = read(fd[0], c, BUFSIZ)) != 0){
                // it should be in controller
                printf("%d: %s\n", i++, c);
                ctl->setData(c);
            }

            char buf[BUFSIZ];
            sprintf(buf, "%s", ctl->getData());   // Send parent controller data to executor
            write(fd[1], buf, BUFSIZ);
        }
    }
    free(eBack);

    while(1)
        pause();

    return SUCCESS;
}
