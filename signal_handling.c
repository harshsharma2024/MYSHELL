#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

struct rowval{
    int NO;
    int PID;
    int PGID;
    char *STATUS;
    char *NAME;
}row_val;


struct rowval arr[11];

int countr = 0;

void SIGNAL_TER(int sig){
    for(int i=0;i<countr;i++){
        if(arr[i].STATUS == "RUNNING"){
            arr[i].STATUS = "TERMINATED";
            kill(arr[i].PID,SIGINT);
        }
    }
    printf("\nmgr> ");
    fflush(stdout);
}

void SIGNAL_SUS(int sig){
    for(int i=0;i<countr;i++){
        if(arr[i].STATUS == "RUNNING"){
            arr[i].STATUS = "SUSPENDED";
            kill(arr[i].PID,SIGTSTP);
        }
    }
    printf("\nmgr> ");
    fflush(stdout);
}

void SIGCHLD_HAND(int sig_num)
{
    int status;
    pid_t pid;
    pid = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status))
    {
        for (int i = 0; i < countr; i++)
        {
            if (arr[i].PID == pid)
            {
                arr[i].STATUS = "FINISHED";
            }
        }
    }

    printf("\nmgr> ");
}


int main(){

    signal(SIGINT,SIGNAL_TER);
    signal(SIGTSTP,SIGNAL_SUS);
    signal(SIGCHLD,SIGCHLD_HAND);
    
    arr[0].NO = countr;
    arr[0].PID = getpid();
    arr[0].PGID = getpgid(0);
    arr[0].STATUS = "SELF";
    arr[0].NAME = "mgr";
    countr++;

    printf("mgr>");

    while(1){
        
        char input;
        scanf("%c",&input);
        if(input == 'p'){
            printf("NO\tPID\tPGID\tSTATUS\t\tNAME\n");
            for(int i=0;i<countr;i++){
                printf("%d\t%d\t%d\t%s\t%s\n",arr[i].NO,arr[i].PID,arr[i].PGID,arr[i].STATUS,arr[i].NAME);
            }
            printf("mgr> ");
        }
        else if(input == 'r'){
            if(countr == 11){
                printf("Process table is full. Quitting...\n");
                exit(0);
            }
            // start a new job
            int pid = fork();
            setpgid(pid,0);
            char arg = 'A' + rand() % 26;
            arr[countr].NO = countr;
            arr[countr].PID = pid;
            arr[countr].PGID = getpgid(pid);
            arr[countr].STATUS = "RUNNING";
            char jobn[10] = "job";
            jobn[3] = ' ';
            jobn[4] = arg;
            jobn[5] = '\0';
            sprintf(jobn, "job %c", arg);
            arr[countr].NAME = strdup(jobn); 
            countr++;
            if(pid == 0){

                execl("./job", "job",&arg, NULL);
                printf("mgr> ");

                exit(0);

            }
            else{
                printf("mgr> ");
            }

        }
        else if(input == 'c'){

            
            printf("\n Suspended Jobs : ");
            for(int i=0;i<countr;i++){
                if(arr[i].STATUS == "SUSPENDED"){
                    printf("%d, ",arr[i].NO);
                }
            }



            int which_index_to_continue;
            printf("(Pick one) : ");
            scanf("%d",&which_index_to_continue);

            if(which_index_to_continue<0 || which_index_to_continue>countr){
                printf("Job number does not exist\n");
                printf("mgr> ");
                continue;
            }   
            
            if(arr[which_index_to_continue].STATUS == "SUSPENDED"){
                arr[which_index_to_continue].STATUS = "RUNNING";
                kill(arr[which_index_to_continue].PID,SIGCONT);
            }
            else{
                printf("\n Job is not suspended \n");
            }
            printf("mgr> ");
        }
        else if(input == 'k'){
            printf("\n Suspended Jobs : ");

            for(int i=0;i<countr;i++){
                if(arr[i].STATUS == "SUSPENDED"){
                    printf("%d, ",arr[i].NO);
                }
            }

            int which_index_to_kill;
            printf("(Pick one) : ");
            scanf("%d",&which_index_to_kill);

            if(which_index_to_kill<0 || which_index_to_kill>countr){
                printf("Job number does not exist\n");
                printf("mgr> ");
                continue;
            }

            if(arr[which_index_to_kill].STATUS == "SUSPENDED"){
                arr[which_index_to_kill].STATUS = "KILLED";
                kill(arr[which_index_to_kill].PID,SIGINT);
            }
            else{
                printf("\n Job is not suspended \n");
            }
            printf("mgr> ");
        }
        else if(input == 'h'){
            printf("\t Command : \t\t Action \n");
            printf("\t p : \t\t Print the table \n");
            printf("\t r : \t\t Start a new job \n");
            printf("\t c : \t\t Continue a suspended job \n");
            printf("\t k : \t\t Kill a suspended job \n");
            printf("\t q : \t\t Quit the program \n");

            printf("mgr> ");
        }
        else if(input == 'q'){
            // Kill all the child processes
            for(int i=1;i<countr;i++){
                kill(arr[i].PID,SIGINT);
            }
            // Kill the parent process
            exit(0);
        }
    }



}