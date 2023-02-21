//
//  main.c
//  lab2nyush
//
//  Created by Leslie Lu on 2/17/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //getcwd()
#include <string.h> //strrchr()
#include <sys/wait.h>
#include <signal.h>

static volatile int keepRunning = 0;

void intHandler(//int dummy
                ) {
    keepRunning = 1;
    //exit(0);
}


//Milestone#1: print prompt
void print_prompt(){
    char cwd[256];
    char * ptr = NULL;
    
    // getcwd() error handling reference: https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-getcwd-get-path-name-working-directory
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        perror("getcwd() error"); //The perror() function prints an error message to stderr
    else{
        ptr = strrchr( cwd, '/' );
        if(ptr == cwd)
         //printf("[nyush %s]$ ", "/");
//        {  printf("[cwd]%s$ ",cwd);
            printf("[nyush %s]$ ", ptr);
            
//        }
        else
            printf("[nyush %s]$ ", ptr+1);
        fflush(stdout);
    }
}

//Milestone#2: repeat prompt, get user input
void get_user_input( char ** command){
    //void get_user_input(int * exit_c, char ** command){
    //char * lineptr = NULL;
    size_t n = 0;

    getline(command, &n, stdin);
    
    
    //if(**command == 'e') { *exit_c = -1;}// if user inputs 'e', set exit code to -1 to leave prompt
//    else {
//        //printf("size:%zu, string:%s\n", n, *command);
//        //fflush(stdout);
//        //free(*command);
//    }
}

//built-in commands:cd
void cd(char ** command){
    

    
    char *command2 = malloc(100);
    
    size_t len_path = strlen(*command);
    strncpy(command2,*command,len_path -1);
    
    //printf("strlen: %zu",len_path);
//    printf("cdpath: %s",(*command)+3);
//    printf("cdpath: %s",(*command)+4);
//    printf("cdpath: %s",(*command)+5);
//    printf("cdpath: %s",(*command)+6);
//    printf("cdpath: %s",(*command)+7);
//    printf("cdpath: %s",(*command)+8);
//
//    printf("token1: %s",(token)+2);
//    printf("token2: %s",(token)+3);
//    printf("token3: %s",(token)+4);
//    printf("token4: %s",(token)+5);
//    printf("token5: %s",(token)+6);
    //printf("token: %s",(*token)+7);
    
    if(chdir((command2)+3) == -1)
        perror("chdir() error");
    
    free(command2);
    
    
}


//body
int main(
        //int argc, const char * argv[]
) {
    // insert code here...
    
    //set exit code to default 0
//    int * exit_c = malloc(sizeof(int));
//    *exit_c = 0;
    
    char ** command = malloc(sizeof(char*));
    char * concat = malloc(1000);
    char *str = malloc(1000);
    char * concat2 = malloc(1000);
    char *str2 = malloc(1000);
    
    int ret = 0;
    
    //sighandler_t
    signal(SIGINT, intHandler);
    
    //repeat prompt while exit code is 0
    while (1) {
    //while(*exit_c == 0){
    //while(TRUE){
        //char * command[1000];
        //char ** command = malloc(sizeof(char*));

        print_prompt();
        get_user_input(command);
        //get_user_input(exit_c, command);
        
        if(**command == 'c' && *((*command)+1) == 'd'){
            cd( command);
            continue;
        }
        
       // printf("BEFORE:  string:%s\n",  *command);
        size_t len = strlen(*command);
        //char * concat = malloc(5+(sizeof(char))*len);
        //char * concat = malloc(1000);
        strcat(concat, "/bin/");
        
//        printf("CONCAT:  %s\n",  concat);
//        printf("LEN_CONCAT:  %ld\n",  strlen(concat));
        size_t len_concat = strlen(concat);

        //char *str = malloc(sizeof(char)*len);
        //char *str = malloc(1000);
        char *delim = " ,\n";
        char *token;
        strcpy(str, *command);
        
        token = strtok(str, delim);//token: the first arg in command
        strcat(concat, token);// "/bin/" + the first arg in command
        
        //char * concat2 = malloc(5+(sizeof(char))*len);
//        char * concat2 = malloc(1000);
        strncpy(concat2,concat,len_concat + len-1); //eliminate '/0'
        
        //printf("AFTER:  string:%s\n",  *command);
        
        //turn "char ** command" into "char *const parmList[]" for execv
        int arg_num = 0;
        while(token){
            //printf("TOKEN %s\n",  token);
            arg_num ++;
            token = strtok(NULL, delim);
        }
        
        //char *str2 = malloc(sizeof(char)*len);
//        char *str2 = malloc(1000);
        char *delim2 = " ,\n";
        char *token2;
        strcpy(str2, *command);
        char* arglist [arg_num+1];
        token2 = strtok(str2, delim2);
        for(int i=0; i<arg_num; i++){
            
            arglist[i] = token2;
            token2 = strtok(NULL, delim2);
            //printf("token %s\n",  token2);
            
        }
        arglist[arg_num] = NULL;

//        printf("concat2:  %s\n",  concat2);
////
//        printf("1MAIN:  string:%s\n",  *arglist);
//        printf("2MAIN:  string:%s\n",  *(arglist+1));
//        printf("3MAIN:  string:%s\n",  *(arglist+2));
        //printf("4MAIN:  string:%s\n",  *(arglist+3));

        //char* sample2[] = {"ls", "-l", NULL};
        
        //milestone3: run a simple program, such as ls
        int pid;
        
        pid = fork();
        if (pid < 0) {
//          // fork failed (this shouldn't happen)
            perror("fork() error");
        } else if (pid == 0) {
          // child (new process)
            
            //execv
            ret = execv(concat2,arglist);
            if (ret == -1) {
                        perror("execve error");
                    }
            
            //execl
          
//            int ret = execl("/bin/ps", *command, NULL);
//            //if(concat == "/bin/ls") printf("same!");
//            if (ret == -1) {
//                        perror("execl error");
//                    }
            //execl("/bin/ps", *command, NULL);  // or another variant
            //pause();
          // exec failed
            //exit( EXIT_FAILURE );
            exit(-1);
        }
        else{ //parent
                wait(NULL);
         // waitpid(-1, ...);
        }
        

        
        * command = malloc(sizeof(char*));
       concat = malloc(1000);
      str = malloc(1000);
         concat2 = malloc(1000);
        str2 = malloc(1000);
      //
        
//        * command = "";
//         concat = "";
//        str = "";
//         concat2 = "";
//        str2 = "";
        
//        if(keepRunning){
//            free(*command);
//            free(command);
//            free(concat);
//            free(concat2);
//            free(str);
//            free(str2);
//            //exit(0);
//            break;
//        }
        
    }
    

   // return EXIT_SUCCESS;

//                free(command);
//                free(concat);
//                free(concat2);
//                free(str);
//                free(str2);
//////
//    exit(0);
    //free(exit_c);
    return 0;
}
