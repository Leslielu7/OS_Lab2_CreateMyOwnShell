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

//void intHandler(//int dummy
//                ) {
//    continue;
//    //exit(0);
//}
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
        fprintf(stderr,"Error: invalid program\n");
        //perror("getcwd() error"); //The perror() function prints an error message to stderr
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
    char *str3 = malloc(1000);
    
    //char *str_p = malloc(1000);
    
    int ret = 0;
    
    //sighandler_t
    signal(SIGINT, SIG_IGN);
//    signal(SIGINT, intHandler);
//    signal(SIGQUIT, quitHandler);
   
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
        
        //count pipe_num (|) of user input
        
        //        char *delim_p = "|";
        //        char *token_p;
        //        strcpy(str_p, *command);
        //        token_p = strtok(str_p, delim_p);
        
        int arg_num_p = 0;
        //        while(token_p){
        //            arg_num_p ++;
        //            token_p = strtok(NULL, delim_p);
        //        }
        
        char * loc_p =  strstr(*command, " | ");
        char * loc_p_next = NULL;
        if(loc_p) { loc_p_next = loc_p+3;}//command for the next for-loop
        
//        printf("loc_p:%s\n",loc_p);
//
//        printf("loc_p_next:%s\n",loc_p_next);
        
        while(loc_p){
           // printf("loc_p:%s\n",loc_p);
            arg_num_p ++;
            loc_p =  strstr(loc_p+3, " | ");
        }
        //arg_num_p ++;//nums of process
        
        //printf("loc_p:%s\n",loc_p);
        //printf("arg_num_p:%d\n",arg_num_p);
        
//        int pipefd[arg_num_p+2][2];// num of pipes
//        int pids[arg_num_p+1];// num of process
//
        int pipefd[arg_num_p][2];// num of pipes, if arg_num_p=1: 1 pipe, 2 process
        int pids[arg_num_p+1];// num of process
        
      
            for (int i=0; i<arg_num_p; i++){
                if(pipe(pipefd[i])==-1){
                    printf("Error creating pipe\n: %d", errno);
                }
            }
        
//        pipefd[arg_num_p+2][0] = '\0';
//        pipefd[arg_num_p+2][1] = '\0';
////
//        for (int i=0; i<=arg_num_p+1; i++){
//            if(i!=0) close(pipefd[i][1]);
//            if(i!=arg_num_p) close(pipefd[arg_num_p][0]);
//        }
        
       // char * buff = *command;
//        if(write(pipefd[0][1], buff, sizeof(buff)) == -1){
//            printf("Error at writing1\n: %d", errno);
//            return 4;
//        }
//
//        if(read(pipefd[arg_num_p][0], buff, sizeof(*buff)) == -1){
//            printf("Error at reading2\n");
//            return 3;
//        }
        //printf("in buff: %s", buff);
        
        for (int i=0; i<arg_num_p+1; i++){
           // printf("i: %d\n",i);
            
//            pids[i] = fork();
            
            char *delim_p = "|,\n";
           // printf("*delim_p: %s\n",delim_p);
            char *token_p;
          //  printf("*token_p: %s\n",token_p);
            strcpy(str, *command);
//            printf("*command: %s\n",*command);
//            printf("str: %s\n",str);
//            printf("len_str: %ld\n",strlen(str));
            token_p = strtok(str, delim_p);//token_p is the sub command
            if (token_p == NULL) token_p = *command;//no pipe found
            
//            printf("token_p fresh1:%s\n", token_p);
//            printf("Len_token_p fresh1:%ld\n", strlen(token_p));
            // in each pipe sub-interval command
            //count arg_num of user input
            char *delim = " ,\n,>,<";
            char *token;
            //            strcpy(str, *command);
            strcpy(str3, token_p);
//            printf("str:%s\n", str3);
//            printf("token_p fresh1.5:%s\n", token_p);
//            printf("Len_token_p fresh1.5:%ld\n", strlen(token_p));
            token = strtok(str3, delim);
//            printf("tokenhere:%s\n", token);
//            printf("str:%s\n", str3);
//            printf("token_p fresh2:%s\n", token_p);
//            printf("Len_token_p fresh2:%ld\n", strlen(token_p));
            
            
            int arg_num = 0;
            // if(*token == '|')arg_num = 1;
            
            while(token //&& *token != '|'
                  ){
                //printf("token:%s\n", token);////
                arg_num ++;
                token = strtok(NULL, delim);
            }
//            printf("token_p fresh3:%s\n", token_p);
//            printf("Len_token_p fresh3:%ld\n", strlen(token_p));
//            printf("here1\n");
//
            char * loc_in =  strstr(token_p, " < ");
            char * loc_out =  strstr(token_p, " > ");
            char * loc_app =  strstr(token_p, " >> ");
            //printf("token_p fresh4:%s\n", token_p);
            //            char * loc_in =  strstr(*command, " < ");
            //            char * loc_out =  strstr(*command, " > ");
            //            char * loc_app =  strstr(*command, " >> ");
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
                    continue;
                    //                 print program detail "Success or failure"
                    //                perror("Program");
                }
                else {
                    //fflush(stdout);
                    saved_stdin = dup(STDIN_FILENO);
                    dup2(fd,STDIN_FILENO);
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
                dup2(fd,STDOUT_FILENO);
                close(fd);
                //                }
                
                arg_num --;
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
                dup2(fd,STDOUT_FILENO);
                close(fd);
                //            }
                
                arg_num --;
                // continue;
            }
            //
            
            //            int arg_num = 0;
            //            if(*token == '|')arg_num = 1;
            //
            //            while(token && *token != '|'){
//            printf("here3\n");
//            printf("arg_num: %d\n", arg_num);
//            printf("here4\n");
            
            //initialize arglist for execv()
            char* arglist [arg_num+1];
            //char *delim2 = " ,\n";
            char *token2;
            //strcpy(str2, *command);
            strcpy(str2, token_p);
//
//            printf("str2: %s\n", str2);
//            printf("token_p: %s\n", token_p);
//
            token2 = strtok(str2, delim);
            //if(*token2 == '|') arglist[0] = token2;
            for(int i=0; i<arg_num; i++){
            //while( i<arg_num && token2 && *token2 != '|'){
                arglist[i] = token2;
                token2 = strtok(NULL, delim);
                //arg_num++;
            }
            arglist[arg_num] = NULL;
//            printf("i: %d\n", i);
//            printf("arg_num: %d\n", arg_num);
//            printf("here2\n");
//            printf("token_p: %s\n", token_p);
//                    printf("arglist0:%s",arglist[0]);
//                    printf("arglist1:%s",arglist[1]);
//
//                        printf("arglist2:%s",arglist[2]);
//            printf("arglist3:%s",arglist[3]);
          
            //   Locating programs
            //char * loc =  strchr(*command, '/');
            
            char * loc =  strchr(token_p, '/');
            
            if(loc == NULL) {//3. Locating programs: only the base name
                size_t len;
                size_t len_concat;
                
                //len = strlen(*command);
                len = strlen(token_p);
                //printf("len: %ld\n",len);
               // if(i==1)exit(0);//breakpoint
                strcat(concat, "/usr/bin/");
                len_concat = strlen(concat);
                //printf("len_concat: %ld\n",len_concat);
                //strcpy(str, *command);
                strcpy(str, token_p);
                token = strtok(str, delim);//token: the first arg in command
                strcat(concat, token);// "/usr/bin/" + the first arg in command
                
                //printf("len_concat: %ld\n",len_concat);
                
                if(i == arg_num_p) strncpy(concat2,concat,len_concat + len);
                else strncpy(concat2,concat,len_concat + len-1); //eliminate '/0'
                
            }
            else{//1. Locating programs: absolute path
                //if(**command == '/'){
                if(*token_p == '/'){
                    char * ptr = strrchr(arglist[0] , '/' );
                    concat2 = str3;
                    arglist[0] = ptr + 1;
                }
                else{//2. Locating programs: relative path
                    char cwd[256];
                    if (getcwd(cwd, sizeof(cwd)) == NULL)
                        fprintf(stderr,"Error: invalid program\n");
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
            
//            int pid1;
//
//            pid1 = fork();
            pids[i] = fork();
            //printf(":%d\n",pids[i]);
            if (pids[i] < 0) {
                //          // fork failed (this shouldn't happen)
                perror("fork() error");
            } else if (pids[i] == 0) {
                // child (new process)
                
//                if(read(pipefd[i][0],buff,sizeof(*buff)) == -1){
//                    fprintf(stderr,"Error: at reading3\n");
//                    return 3;
                if(i==0 && i!=arg_num_p){//0~
                    //printf(":%c\n",pipefd[i][1]);
//                    for(int j=0; j<arg_num_p; j++){
//                        close(pipefd[j][0]);
//                        if(i!=j) close(pipefd[j][1]);
//                    }
                    
                    for(int j=0; j<arg_num_p; j++){
                        if(i!=j){
                            close(pipefd[j][1]);//==-1;
//                            if(close(pipefd[j][1])==-1)
//                               // printf("Here1.0.1: Error creating pipe\n: %d", errno);
//
                        }
                        close(pipefd[j][0]);//==-1;
//                            if(close(pipefd[j][0])==-1)
//                               // printf("Here1.0.2: Error creating pipe\n: %d", errno);
//
                    }
                    
                    
                    dup2(pipefd[i][1],STDOUT_FILENO);//
                    //close(pipefd[i][0]);//
                    close(pipefd[i][1]);
                   // dup2(pipefd[i][0],STDIN_FILENO);
                    //dup2(pipefd[i][0],STDOUT_FILENO);
                    
                    //if(close(pipefd[i][1])==0)//;
                   // printf("Here1.1: Error creating pipe\n: %d", errno);
//                    if(close(pipefd[i][0])==-1)//;
//                    printf("Here1.2: Error creating pipe\n: %d", errno);
                    
//                    for(int j=0; j<arg_num_p; j++){
//                        close(pipe[j][0]);
//                        close(pipe[j][1]);
//                    }
                }
                else if(i==0){// i==0 && i==arg_num_p
                    //dup2(pipefd[i][1],STDOUT_FILENO);
                    close(pipefd[i][0]);
                    close(pipefd[i][1]);
                }
                else if(i!=0 && i==arg_num_p) {// i!=0 && i==arg_num_p
                    //printf("Here i:%d ", i);
                    for(int j=0; j<arg_num_p; j++){
                        if(i-1!=j)close(pipefd[j][0]);//{
//                            if(close(pipefd[j][0])==-1)
//                               // printf("Here3.0.1: Error creating pipe\n: %d", errno);
//
//                        }
                       
//                            if(
                        close(pipefd[j][1]);//==-1)
                               // printf("Here3.0.2: Error creating pipe\n: %d", errno);
                        
                    }
                    
                    dup2(pipefd[i-1][0],STDIN_FILENO);
//                    close(pipefd[i][1]);
//                    close(pipefd[i][0]);
//                    close(pipefd[i-1][0]);
                   // dup2(pipefd[i-1][1],STDOUT_FILENO);
//                    if(close(pipefd[i][1])==-1)//;
//                    printf("Here3.1: Error creating pipe\n: %d", errno);
//                    if(close(pipefd[i][0])==-1)//;
//                    printf("Here3.2: Error creating pipe\n: %d", errno);
                    //if(
                       close(pipefd[i-1][0]);//==-1)//;
                   // printf("Here3.3: Error creating pipe\n: %d", errno);
//                    if(close(pipefd[i-1][1])==0)//;
//                    printf("Here3.4: Error creating pipe\n: %d", errno);
                }
                else {// 0<i<arg_num_p
                    for(int j=0; j<arg_num_p; j++){
                        if(i-1!=j)close(pipefd[j][0]);//{
//                            if(close(pipefd[j][0])==-1)
//                                //printf("Here4.0.1: Error creating pipe\n: %d", errno);
//
//                        }
                        if(i!=j)close(pipefd[j][1]);//{
//                            if(close(pipefd[j][1])==-1)
//                                //printf("Here4.0.2: Error creating pipe\n: %d", errno);
//                        }
                    }
                    
                    dup2(pipefd[i-1][0],STDIN_FILENO);
                    dup2(pipefd[i][1],STDOUT_FILENO);
//                    close(pipefd[i][1]);//4
//                    close(pipefd[i-1][0]);
//                    close(pipefd[i][0]);
                    
//                    dup2(pipefd[i-1][1],STDIN_FILENO);
//                    dup2(pipefd[i][0],STDOUT_FILENO);
//                    close(pipefd[i][1]);//4
//                    close(pipefd[i-1][0]);
                    
                    //if(
                    close(pipefd[i-1][0]);//==-1)//;
                    //printf("Here4.1: Error creating pipe\n: %d", errno);
                   // if(
                       close(pipefd[i][1]);//==-1)//;
                    //printf("Here4.2: Error creating pipe\n: %d", errno);
                    
                }
//                if(write(pipefd[i+1][1],buff,sizeof(*buff)) == -1){
//                    fprintf(stderr,"Error: at writing4\n");
//                    return 4;
//                }//write to std output
                
//                for (int i=0; i<arg_num_p+1; i++){
//                    close(pipefd[i][1]);
//                    close(pipefd[i][0]);
//                }
                
                //printf("heyyyy here:\n");
                //printf("concat2:%s\n", concat2);
                ret = execv(concat2,arglist);// //char* sample2[] = {"ls", "-l", NULL};
              
                if (ret == -1) {
                    fprintf(stderr,"Error: invalid program\n");
                    //continue;//Error: invalid program
                    //perror("execve error");
                }
                
                //close(saved_stdout);
                // exec failed
                //exit( EXIT_FAILURE );
                exit(-1);
            }
            
            else{//parent process
                
//                for (int i=0; i<arg_num_p+1; i++){
//                    close(pipefd[i][1]);
//                    close(pipefd[i][0]);
//                }
//                if(write(pipefd[i+1][1],buff,sizeof(*buff)) == -1){
//                    fprintf(stderr,"Error: at writing4\n");
//                    return 4;
//                }//write to std output
                //exit(0);
                //printf("here~~\n");
                
//                close(pipefd[i][0]);
//                close(pipefd[i+1][1]);
                //waitpid(pid,NULL,0);
//                if (saved_stdin)
                    if(loc_in!=NULL) dup2(saved_stdin, STDIN_FILENO);//redirected input
//                if (saved_stdout)
                    if(loc_out!=NULL||loc_app!=NULL) dup2(saved_stdout, STDOUT_FILENO);//redirected output
                //            close(saved_stdout);
                //waitpid(-1, ...);
//                int w1 = wait(NULL);
//                printf("waited for %d\n",w1);

//
                
//                for (int i=0; i<=arg_num_p; i++){
//                    if(i!=0) close(pipefd[i][1]);
//                    if(i!=arg_num_p) close(pipefd[arg_num_p][0]);
//                }
//
//                char * buff = *command;
//                if(write(pipefd[0][1], buff, sizeof(*buff)) == -1){
//                    printf("Error at writing\n");
//                    return 4;
//                }
//
//                if(read(pipefd[arg_num_p][0], buff, sizeof(*buff)) == -1){
//                    printf("Error at reading\n");
//                    return 3;
//                }
//
//                for (int i=0; i<=arg_num_p; i++){
                    //wait(NULL);
//                }
            }
//            printf("1. *command: %s",*command);
//            printf("1.strlen *command: %ld",strlen(*command));
            if(loc_p_next!=NULL) *command = loc_p_next;
            
//            printf("2. *command: %s",*command);
//            printf("2.strlen *command: %ld",strlen(*command));
            //else *command = loc_p_next;
//            for (int i=0; i<=arg_num_p; i++){
//                if(i!=0) close(pipefd[i][1]);
//                if(i!=arg_num_p) close(pipefd[arg_num_p][0]);
//            }
            //exit(0);
           // * command = malloc(sizeof(char*));
           concat = malloc(1000);
          str = malloc(1000);
             concat2 = malloc(1000);
            str2 = malloc(1000);
            str3 = malloc(1000);
            //arglist = []
        }
         //shell
        
//        close pipes
//        for (int i=0; i<arg_num_p+1; i++){
//            if(i!=0) close(pipefd[i][1]);// don't close pipefd[0][1]
//            if(i!=arg_num_p) close(pipefd[arg_num_p][0]);// don't close pipefd[arg_num_p][0]
//        }
        
       // close pipes
        for (int i=0; i<arg_num_p; i++){
//            close(pipefd[i][1])
           // if(
               close(pipefd[i][1]);//==-1)//;
            //printf("Shell: Error creating pipe: %d, %d", errno,i);//
           // close(pipefd[i][0]);//
           // if(
            close(pipefd[i][0]);//==-1)//;
            //printf("Shell: Error creating pipe: %d, %d", errno,i);//
        }

        //char * buff = *command;
//        if(write(pipefd[0][1], buff, sizeof(*buff)) == -1){
//            printf("Error at writing1\n");
//            return 4;
//        }

//        if(read(pipefd[arg_num_p][0], buff, sizeof(*buff)) == -1){
//            printf("Error at reading2\n");
//            return 3;
//        }
        
        //wait for processes
        for (int i=0; i<arg_num_p+1; i++){
            //int w1 =
            wait(NULL);
            //printf("waited for %d\n",w1);
            //int w2 =
            //wait(NULL);
            //printf("waited for %d\n",w2);
            //wait(NULL);
        }
        
//            * command = malloc(sizeof(char*));
//           concat = malloc(1000);
//          str = malloc(1000);
//             concat2 = malloc(1000);
//            str2 = malloc(1000);
   
    }
    //while(1): prompt
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
}//main
