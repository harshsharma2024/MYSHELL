#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define sz 1000


int main(int argc, char *argv[]){
    
    int switch_perm = 0;

    if(argc == 1){


    int pipefd1[2], pipefd2[2];
    pipe(pipefd1);
    pipe(pipefd2);
    printf("argc %d \n",argc );
    printf("+++ CSE in supervisor mode: Started\n");
    printf("+++ CSE in supervisor mode: pfd = [%d %d]\n", pipefd1[0], pipefd1[1]);
    printf("+++ CSE in supervisor mode: Forking first child in command-input mode\n");
    printf("+++ CSE in supervisor mode: Forking second child in execute mode\n");

    if (fork() == 0) {
    // Open xterm in the child process
    close(pipefd1[0]);
    close(pipefd2[1]);
    dup2(pipefd1[1],STDOUT_FILENO);
    dup2(pipefd2[0],STDIN_FILENO);
    
    char pipefd1_read[16], pipefd1_write[16],pipefd2_read[16], pipefd2_write[16];
    sprintf(pipefd1_read, "%d", pipefd1[0]);
    sprintf(pipefd1_write, "%d", pipefd1[1]);
    sprintf(pipefd2_read, "%d", pipefd2[0]);
    sprintf(pipefd2_write, "%d", pipefd2[1]);
    

    int t = execl("/usr/bin/xterm", "xterm", "-T", "First Child", "-e", "./CSE","C",pipefd1_read,pipefd1_write ,pipefd2_read,pipefd2_write,(char *)NULL);

    // printf("t  1   = %d\n", t);

    if (t == -1) {
        perror("execl");
        exit(EXIT_FAILURE);
    }
    }

    else{
        if(fork() == 0){
            
            // printf("Creating new process 2\n");

            close(pipefd1[1]);
            close(pipefd2[0]);
            dup2(pipefd1[0],STDIN_FILENO);
            dup2(pipefd2[1],STDOUT_FILENO);

            char pipefd1_read[16], pipefd1_write[16],pipefd2_read[16], pipefd2_write[16];
            sprintf(pipefd1_read, "%d", pipefd1[0]);
            sprintf(pipefd1_write, "%d", pipefd1[1]);
            sprintf(pipefd2_read, "%d", pipefd2[0]);
            sprintf(pipefd2_write, "%d", pipefd2[1]);
    

            int t = execl("/usr/bin/xterm", "xterm", "-T", "Second Child", "-e", "./CSE","E",pipefd1_read,pipefd1_write ,pipefd2_read,pipefd2_write,(char *)NULL);
            if(t == -1){
            printf("Error in exec 2\n");
        }
        }
        else{
            wait(NULL);
            printf("+++ CSE in supervisor mode: First child terminated\n+++ CSE in supervisor mode: Second child terminated\n");
        }
    }
    }
    else if(argc == 6)
    {   int t,q,w,e,r;
        while(1)
        {     
            t = (strcmp(argv[1], "C") == 0);
            q = (strcmp(argv[1], "E") == 0);
            w = (strcmp(argv[1], "C") == 0);
            e = (strcmp(argv[1], "E") == 0);
            
            if ((t&&(switch_perm == 0)))
            {
                while(1)
                {
                    fprintf(stderr, "Enter command> ");
                    char buff[sz];
                    for (int i = 0; i < sz; i++) buff[i] = '\0';
                    
                    fgets(buff, sz, stdin);

                    size_t len = strlen(buff);
                    if (len > 0 && buff[len - 1] == '\n') {
                        buff[len - 1] = '\0';
                    }


                    fflush(stdin);
                    int siz = strlen(buff);
                    write(atoi(argv[3]), buff, siz);
                    if (strncmp(buff, "exit", 4) == 0)
                    {
                        exit(0);
                    }
                    if (strncmp(buff, "swaprole", 8) == 0)
                    {
                        switch_perm++;
                        switch_perm %= 2;
                        break;
                    }
                }
            }

            
            else if ((q&&(switch_perm == 0)))
            {
                
                while(1)
                {
                    fprintf(stdout, "Waiting for command> ");
                    fflush(stdout);
                    char buff[sz];

                    for (int i = 0; i < sz; i++) buff[i] = '\0';

                    read(atoi(argv[2]), buff, sz);
                    
                    printf("%s\n", buff);
                    if (strncmp(buff, "exit", 4) == 0)
                    {
                        exit(0);
                    }
                    if (strncmp(buff, "swaprole", 8) == 0)
                    {
                        switch_perm++;
                        switch_perm %= 2;
                        break;
                    }
                    
                    
                    int cpid = fork();
                    if (cpid == -1)
                    {
                        perror("error fork");
                        exit(1);
                    }
                    if (cpid == 0)
                    {
                        
                        int stst = execl("/bin/sh", "sh", "-c", buff, NULL);
                        
                        if(stst == -1){
                            printf("Error in exec 1\n");
                            exit(1);
                        }
                    }
                    else
                    {
                        
                        wait(NULL);
                    }

                }
                
            }

            
            else if ((w&&(switch_perm == 1)))
            {
                int c;
                while (1)
                {
                    fprintf(stdout, "Waiting for command> ");
                    fflush(stdout);
                    char buff[sz];
                    for (int i = 0; i < sz; i++) buff[i] = '\0';
                    
                    read(atoi(argv[4]), buff, sz);
                    printf("%s\n", buff);

                    if (strncmp(buff, "exit", 4) == 0)
                    {
                        exit(0);
                    }
                    if (strncmp(buff, "swaprole", 8) == 0)
                    {
                        switch_perm++;
                        switch_perm %= 2;
                        break;
                    }
                    
                    int cpid = fork();
                    if (cpid == -1)
                    {
                        perror("error fork");
                        exit(1);
                    }
                    if (cpid == 0)
                    {
                        
                        int stst = execl("/bin/sh", "sh", "-c", buff, NULL);
                        
                        if(stst == -1){
                            printf("Error in exec 1\n");
                            exit(1);
                        }
                    }
                    else
                    {
                        
                        wait(NULL);
                    }
                }
            }

            
            else if ((e&&(switch_perm == 1)))
            {
                while(1)
                {
                    fprintf(stderr, "Enter command> ");
                    char buff[sz];
                    for (int i = 0; i < sz; i++) buff[i] = '\0';
                    
                    fgets(buff, sz, stdin);

                    size_t len = strlen(buff);
                    if (len > 0 && buff[len - 1] == '\n') {
                        buff[len - 1] = '\0';
                    }

                    fflush(stdin);
                    int siz = strlen(buff);
                    write(atoi(argv[5]), buff, siz);
                    if (strncmp(buff, "exit", 4) == 0)
                    {
                        exit(0);
                    }
                    if (strncmp(buff, "swaprole", 8) == 0)
                    {
                        switch_perm++;
                        switch_perm %= 2;
                        break;
                    }
                }
            }
        }
    }

    else{
        printf("Invalid number of arguments\n");
    }

    return 0;
}