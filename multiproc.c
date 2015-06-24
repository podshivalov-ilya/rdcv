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

static void signals_handler(int signo)
{
    if(signo == SIGUSR1){
        printf("exit success!\n");
        exit(EXIT_SUCCESS);
    }
    else if(signo == SIGPIPE){
        perror("writing to the pipe in main process failed!\n");
    }
}

int main(int argc, char **argv)
{
    const int childrenCount = 8;

    int fd1[childrenCount][2];
    int fd2[childrenCount][2];

    signal(SIGUSR1, signals_handler);

    pid_t pid;
    for(int i = 1; i <= childrenCount; i++){

        pipe(fd1[i-1]);
        pipe(fd2[i-1]);

        if((pid = fork()) < 0){
            perror("Error while calling fork()");
            return 1;
        }
        else if(pid == 0){
            printf("Child %d PID %d\n", i, getpid());

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
                    exit(EXIT_FAILURE);
                }
                if(fds[0].revents & POLLIN){
                    char c[BUFSIZ];
                    int len = 0;

                    int val = 0;
                    if ((len = read(fds[0].fd, c, BUFSIZ)) != 0){
                        val = getpid();

                        char buf[33];
                        sprintf(buf, "%s %d №%d", c, val, i++);
                        write(1, buf, 33);
                        continue;
                    }
                }
            }

            while(1)
                pause();

            // there is point where child process finished execution
            //return 0;
        }
        else{
            close(fd1[i-1][1]);
            close(fd2[i-1][0]);

            fd1[i-1][1] = fd2[i-1][1];

            // fd1[i-1][0] read fd
            // fd1[i-1][1] write fd

            printf("Parent iteration %d write fd %d\n", i, fd1[i-1][1]);
        }
    }
    signal(SIGPIPE, signals_handler);

    // PARENT
    // Initializing pipe watcher
    int epfd = epoll_create(childrenCount);
    if(epfd < 0)
        perror("epoll_create!");

    for(size_t i = 0; i < childrenCount; i++){
        struct epoll_event event;
        int ret;

        event.data.ptr = fd1[i];
        event.events = EPOLLIN;

        ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd1[i][0], &event);
        if(ret)
            perror("epoll_ctl");
    }

    struct epoll_event *eBack;
    eBack = malloc(sizeof(struct epoll_event));
    if(!eBack){
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for(size_t i = 0; i < childrenCount; i++){
        char buf[33];
        sprintf(buf, "%d", getpid());   // Send parent id to children
        write(fd1[i][1], buf, 33);
    }

    int i = 0;
    // There is waiter for 1 event because after getting results of processes this send new data for processing
    while(epoll_wait(epfd, eBack, 1, -1) > 0){
        //
        if(eBack->events == EPOLLIN){
            char c[BUFSIZ];
            int len = 0;
            int *fd = (int *)eBack->data.ptr;
            if((len = read(fd[0], c, BUFSIZ)) != 0)
                printf("%d: %s\n", i++, c);

            char buf[33];
            sprintf(buf, "%d", getpid());   // Send parent id to children
            write(fd[1], buf, 33);
        }
    }
    free(eBack);

    printf("waiting finished\n");

    while(1)
        pause();

    return 0;
}
