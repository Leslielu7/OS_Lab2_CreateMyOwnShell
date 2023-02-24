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
#include<fcntl.h>
#include <signal.h>

#include<errno.h>
extern int errno;

static volatile int keepRunning = 0;

void intHandler(//int dummy
                ) {
    keepRunning = 1;
    //exit(0);
}
void quitHandler(//int dummy
                ) {
                    fclose(stdin);
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

//built-in command:cd
void cd(char ** command){
    
    char *str = malloc(1000);
    char *delim = " ,\n";
    char *token;
    int arg_num = 0;
    
    strcpy(str, *command);
    
    token = strtok(str, delim);
    while(token){
        arg_num ++;
        token = strtok(NULL, delim);
    }
    
    //If cd is called with 0 or 2+ arguments, Error: invalid command
    if(arg_num != 2)
        fprintf(stderr,"Error: invalid command\n");

    else{
        char *command2 = malloc(100);
        
        size_t len_path = strlen(*command);
        strncpy(command2,*command,len_path -1);
        //If the directory does not exist, Error: invalid directory
        if(chdir((command2)+3) == -1)
            fprintf(stderr,"Error: invalid directory\n");
 
        free(command2);
    }
    free(str);
    
}

//built-in command:exit
//void exit(){
//    break;
//}


//body
int main(
        //int argc, const char * argv[]
) {
    // insert code here...
    
    char ** command = malloc(sizeof(char*));
    *command = malloc(sizeof(char*));
    char * concat = malloc(1000);
    char *str = malloc(1000);
    char * concat2 = malloc(1000);
    char *str2 = malloc(1000);
    
    int ret = 0;
    
    //sighandler_t
    signal(SIGINT, intHandler);
    signal(SIGQUIT, quitHandler);
   
    while (1) {
        print_prompt();
        get_user_input(command);
        
        //EOF
        if (feof(stdin)) break;
        
        if(strcmp(*command,"\n") == 0){
            continue;
        }
        
        if(strcmp(*command,"exit\n") == 0){
            break;
        }
        
        if(**command == 'c' && *((*command)+1) == 'd'){
            cd(command);
            continue;
        }
        
        
        //count arg_num of user input
        char *delim = " ,\n,>,<";
        char *token;
        strcpy(str, *command);
        token = strtok(str, delim);
        
        int arg_num = 0;
        while(token){
            arg_num ++;
            token = strtok(NULL, delim);
        }
        
        //printf("arg_num:%d\n",arg_num);
        
      
        
        //    I/O redirection
        // input redirection
//        char * loc_in =  strchr(*command, '<');
        // output redirection
        char * loc_in =  strstr(*command, " < ");
        char * loc_out =  strstr(*command, " > ");
        char * loc_app =  strstr(*command, " >> ");
        //printf("*command:%s\n", *command);
        //printf("loc_out:%s\n", loc_out);
        int saved_stdout;
        int saved_stdin;
        int fd;
        

        //printf("out loc_app:%s\n", loc_app);
        //printf("out loc_out:%s\n", loc_out);
        
        
        if(loc_in != NULL){
            loc_in = strtok(loc_in+3,"\n");
            //printf("1. loc_in:%s\n", loc_in);
            fd = open(loc_in, O_RDONLY, 0777);
            //printf("2. loc_in:%s\n", loc_in);
            if (fd == -1)
            {
                // print which type of error have in a code
                fprintf(stderr,"Error: invalid file\n");
                
//                 print program detail "Success or failure"
//                perror("Program");
            }
            else {
                //fflush(stdout);
                saved_stdin = dup(STDIN_FILENO);
                dup2(fd,0);
                close(fd);
                arg_num --;
            }
        }
        else if(loc_app != NULL){
            loc_app = strtok(loc_app+4,"\n");
        //printf("1. loc_app:%s\n", loc_app);
            fd = open(loc_app,O_WRONLY | O_CREAT | O_APPEND, 0777);
            //printf("2. loc_app:%s\n", loc_app);
            //https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/
//            if (fd == -1)
//                {
//                    // print which type of error have in a code
//                    printf("Error Number % d\n", errno);
//
//                    // print program detail "Success or failure"
//                    perror("Program");
//                }
//            else {
                //fflush(stdout);
                saved_stdout = dup(STDOUT_FILENO);
                dup2(fd,1);
                close(fd);
//                }
         
            arg_num --;
           // continue;
        
            // concat2
            //strcat();
//            loc_app = strtok(loc_app+2,"\n");
//            printf("1. loc_app:%s\n", loc_app);
//            if((fd = open(loc_app,O_WRONLY | O_CREAT |O_APPEND, 0777)) == -1)
//                {
//                    // print which type of error have in a code
//                    printf("Error Number % d\n", errno);
//
//                    // print program detail "Success or failure"
//                    perror("Program");
//                }
//            else{
//                printf("2. loc_app:%s\n", loc_app);
//                //https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/
//                dup2(fd,1);
//                close(fd);
//                arg_num --;
//            }
           // continue;
                
        }
        else if(loc_out != NULL){
                // concat2
                //strcat();
                loc_out = strtok(loc_out+3,"\n");
            //printf("1. loc_out:%s\n", loc_out);
                fd = open(loc_out,O_WRONLY | O_CREAT | O_TRUNC, 0777);
                //printf("2. loc_out:%s\n", loc_out);
                //https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/
//                if (fd == -1)
//                    {
//                        // print which type of error have in a code
//                        printf("Error Number % d\n", errno);
//
//                        // print program detail "Success or failure"
//                        perror("Program");
//                    }
//                else {
                    saved_stdout = dup(STDOUT_FILENO);
                    dup2(fd,1);
                    close(fd);
//            }
             
                arg_num --;
               // continue;
            }
//
        //initialize arglist for execv()
        char* arglist [arg_num+1];
        //char *delim2 = " ,\n";
        char *token2;
        strcpy(str2, *command);
        token2 = strtok(str2, delim);
        for(int i=0; i<arg_num; i++){
            arglist[i] = token2;
            token2 = strtok(NULL, delim);
        }
        arglist[arg_num] = NULL;

//        printf("arglist0:%s",arglist[0]);
//        printf("arglist1:%s",arglist[1]);
//        printf("arglist2:%s",arglist[2]);
        
        //   Locating programs
        char * loc =  strchr(*command, '/');
        if(loc == NULL) {//3. Locating programs: only the base name
            size_t len;
            size_t len_concat;
            
            len = strlen(*command);
            strcat(concat, "/usr/bin/");
            len_concat = strlen(concat);

            strcpy(str, *command);
            token = strtok(str, delim);//token: the first arg in command
            strcat(concat, token);// "/usr/bin/" + the first arg in command
            
            strncpy(concat2,concat,len_concat + len-1); //eliminate '/0'
            
        }
        else{//1. Locating programs: absolute path
            if(**command == '/'){
                char * ptr = strrchr(arglist[0] , '/' );
                concat2 = str;
                arglist[0] = ptr + 1;
            }
            else{//2. Locating programs: relative path
                char cwd[256];
                if (getcwd(cwd, sizeof(cwd)) == NULL)
                    perror("getcwd() error");
                else{
                    concat2 = strcat(cwd,"/");
                    strcat(concat2, str);
                    char * ptr = strrchr(arglist[0] , '/' );
                    arglist[0] = ptr + 1;
                }
            }

        }
        
        //printf("concat2:%s\n",concat2);

        //milestone3: run a simple program, such as ls
        int pid;
        
        pid = fork();
        if (pid < 0) {
//          // fork failed (this shouldn't happen)
            perror("fork() error");
        } else if (pid == 0) {
          // child (new process)
            //char* sample2[] = {"cat",  NULL};
            //ret = execv(concat2,sample2);
            ret = execv(concat2,arglist);// //char* sample2[] = {"ls", "-l", NULL};
            if (ret == -1) {
                fprintf(stderr,"Error: invalid program\n");
                //continue;//Error: invalid program
                //perror("execve error");
                    }
            
            //close(saved_stdout);
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
            dup2(saved_stdin, 0);
            dup2(saved_stdout, 1);
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
