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
            printf("[nyush %s]$ \n", ptr);
        else
            printf("[nyush %s]$ \n", ptr+1);
        fflush(stdout);
    }
}

//Milestone#2: repeat prompt, get user input
void get_user_input(int * exit){
    char * lineptr = NULL;
    size_t n = 0;

    getline(&lineptr, &n, stdin);
    
    if(*lineptr == 'e') { *exit = -1;} // if user inputs 'e', set exit code to -1 to leave prompt
    else {
        printf("size:%zu, string:%s\n", n, lineptr);
        free(lineptr);
    }
}

//body
int main(
       // int argc, const char * argv[]
) {
    // insert code here...
    
    //set exit code to default 0
    int * exit = malloc(sizeof(int));
    *exit = 0;
    
    while(*exit == 0){
        print_prompt();
        get_user_input(exit);
        
//        pid = fork();
//        if (pid < 0) {
//          // fork failed (this shouldn't happen)
//        } else if (pid == 0) {
//          // child (new process)
//          execv(...);  // or another variant
//          // exec failed
//        } else {
//          // parent
//          waitpid(-1, ...);
//        }
        
    }
    
    
    return 0;
}
