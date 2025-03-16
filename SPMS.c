//
// COMP2432 Operating Systems Group Project
// SPMS - Smart Parking Management System
// Project Pica Magpie [https://en.wikipedia.org/wiki/Magpie]
//
// WANG Yuqi
// YANG Xikun
// LIU Siyuan
// JIN Yixiao
//

#include "input.h"
#include "output.h"
#include "utils.h"
#include "scheduler.h"
#include "analyzer.h"

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() { 
    while (1) {
        int pipefd[2];
        Request input;
        
        if (pipe(pipefd) == -1) {
            perror("pipe");
            exit(1);
        }
        
        int pid = fork();
        if (pid == 0) {
            close(pipefd[0]); 
            Request input = fetch_input();
            write(pipefd[1], &input, sizeof(Request));
            close(pipefd[1]);
            exit(0);
        } else {
            close(pipefd[1]);
            read(pipefd[0], &input, sizeof(Request));
            close(pipefd[0]);
            waitpid(pid, NULL, 0);
        }
    }
}