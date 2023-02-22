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
    signal(SIGQUIT, quitHandler);
    //repeat prompt while exit code is 0
    while (1) {
    //while(*exit_c == 0){
    //while(TRUE){
        //char * command[1000];
        //char ** command = malloc(sizeof(char*));

        print_prompt();
        get_user_input(command);
        //get_user_input(exit_c, command);
        //printf("print%s",*command);
        //char * exit_s = "exit";
        if(strcmp(*command,"exit\n") == 0){
            break;
        }
        
        if(**command == 'c' && *((*command)+1) == 'd'){
            cd(command);
            continue;
        }
//
        size_t len;
        size_t len_concat;
        
        char *delim = " ,\n";
        char *token;
        strcpy(str, *command);
        token = strtok(str, delim);
        
        int arg_num = 0;
        while(token){
            //printf("TOKEN %s\n",  token);
            arg_num ++;
            token = strtok(NULL, delim);
        }
        
        char* arglist [arg_num+1];
//        printf("arg_num: %d\n", arg_num);
        
        char *delim2 = " ,\n";
        char *token2;
        strcpy(str2, *command);
//            char* arglist [arg_num+1];
        token2 = strtok(str2, delim2);
        for(int i=0; i<arg_num; i++){
            
            arglist[i] = token2;
            token2 = strtok(NULL, delim2);
            //printf("token %s\n",  token2);
            
        }
        arglist[arg_num] = NULL;
        
        
        char * loc =  strchr(*command, '/');
        if(loc == NULL) {
            //3. Locating programs: only the base name
            // concat /usr/bin
            // ret = execv(concat2,arglist);
//            printf("nolocation: %s",loc);
            
            len = strlen(*command);
            strcat(concat, "/usr/bin/");
            len_concat = strlen(concat);
            
//            char *delim = " ,\n";
//            char *token;
            strcpy(str, *command);
            
            token = strtok(str, delim);//token: the first arg in command
            strcat(concat, token);// "/bin/" + the first arg in command
            
            strncpy(concat2,concat,len_concat + len-1); //eliminate '/0'
            
//     
      
            
            
        }
        else{
            //1. Locating programs: absolute path
            if(**command == '/'){
//                printf("before arglist 1st ele: %s",arglist[0]);
                char * ptr = strrchr(arglist[0] , '/' );
                
//                printf("location: %s",loc);
                concat2 = str;
                arglist[0] = ptr + 1;
//
            }
            else{//2. Locating programs: relative path
                
                char cwd[256];
                
            
                if (getcwd(cwd, sizeof(cwd)) == NULL)
                    perror("getcwd() error");
                else{
//                    printf("cwd:%s",cwd);
//                    printf("location: %s",loc);
                    concat2 = strcat(cwd,"/");
//                    printf("concat2: %s",concat2);
//                    printf("str: %s",str);
                    strcat(concat2, str);
//                    printf("AFTER concat2: %s",concat2);
                    char * ptr = strrchr(arglist[0] , '/' );
                    arglist[0] = ptr + 1;
                }
            }

        }
  
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
                fprintf(stderr,"Error: invalid program\n");//Error: invalid program
                
                        //perror("execve error");
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
