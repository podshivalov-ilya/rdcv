#include <unistd.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/inotify.h>

static void siguser_handler(int signo)
{
    if(signo == SIGUSR1){
        printf("exit success!\n");
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char **argv)
{
    const int childrenCount = 8;

    int fd1[childrenCount][2];
    int fd2[childrenCount][2];

    signal(SIGUSR1, siguser_handler);

    pid_t pid;
    for(int i = 1; i <= childrenCount; i++){
        pid = fork();

        pipe(fd1[i-1]);
        pipe(fd2[i-1]);

        if(pid < 0){
            perror("Error while calling fork()");
            return 1;
        }
        else if(pid == 0){
            printf("Child %d PID %d\n", i, getpid());

            if(i > 1){
                // Here I close connection with previous process
                for(int j = 0; j <= i-2; j++){
                    close(fd1[j][0]);
                    close(fd1[j][1]);
                }
            }

            dup2(fd2[i-1][0], 0);    // Input fd
            close(fd2[i-1][0]);

            dup2(fd1[i-1][1], 1);    // Output fd
            close(fd1[i-1][1]);

            while(1)
                pause();
            break;
        }
        else{
            close(fd1[i-1][1]);
            close(fd2[i-1][0]);

            fd1[i-1][1] = fd2[i-1][1];
            // Forked process could have this connection. I should distruct it in next child. But how?

            // fd1[i-1][0] read fd
            // fd1[i-1][1] write fd

            printf("Parent iteration %d\n", i);
        }
    }

    while(1)
        pause();

    return 0;
}
