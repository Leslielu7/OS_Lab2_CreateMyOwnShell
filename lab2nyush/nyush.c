//
//  main.c
//  lab2nyush
//
//  Created by Leslie Lu on 2/17/23.
//
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include<fcntl.h>
#include <signal.h>


#include<errno.h>
extern int errno;

pid_t c_pid ;
char jList [100][1000] ;
int pList [100];
int c = 0;
int num_cmd = 0;


//CTRL+C, SIGINT
void intHandler(int sig) {
  
    if(getpid()!=c_pid)kill(c_pid, sig);
        else return;
  
}

//Ctrl-Z, SIGTSTP:
void stpHandler(int sig) {//stop child

    if(getpid()!=c_pid)kill(c_pid, sig);
        else return;

}


//Ctrl-D, SIGQUIT:
void quitHandler() {
                    fclose(stdin);
                    exit(0);
}

//Milestone#1: print prompt
void print_prompt(){
    char cwd[256];
    char * ptr = NULL;
    
    // getcwd() error handling reference: https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-getcwd-get-path-name-working-directory
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        fprintf(stderr,"Error: invalid program\n");
       
    else{
        ptr = strrchr( cwd, '/' );
        if(ptr == cwd)
       
            printf("[nyush %s]$ ", ptr);
            
        else
            printf("[nyush %s]$ ", ptr+1);
        fflush(stdout);
    }
}

//Milestone#2: repeat prompt, get user input
void get_user_input( char ** command){
   
    size_t n = 0;

    getline(command, &n, stdin);

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
    {
        fprintf(stderr,"Error: invalid command\n");
        
    }

    else{
        char *command2 = malloc(100);
        
        size_t len_path = strlen(*command);
        strncpy(command2,*command,len_path -1);
        //If the directory does not exist, Error: invalid directory
        if(chdir((command2)+3) == -1)
        {
            fprintf(stderr,"Error: invalid directory\n"); }
 
        free(command2);
    }
    free(str);
    return;
}


void redir_in (int * i, int * arg_num_p, int * arg_num,
               int* saved_stdin, int * fd_in, char ** loc_in,
               int *is_loc_app, int * is_loc_out,
               int * c){
    if(*loc_in != NULL){
   
        
        if(*i!=0 && *arg_num_p>0){// if multiple, only first program can include "<"
            fprintf(stderr,"Error: invalid command\n");


            *c = 1;
            return;
        }

        char * loc_in_cpy = malloc(1000);
        strcpy(loc_in_cpy,*loc_in);
        *loc_in = strtok(loc_in_cpy+3,"\n");
      
        char * token_in_blank = strstr(*loc_in, " ");
 
        if(token_in_blank != NULL && !(*is_loc_out) && !(*is_loc_app)
           && *(token_in_blank+1)) {//blank between filename
            fprintf(stderr,"Error: invalid command\n");
            *c = 1;
            return;
        }
        
        
        char * delim_in = " ";
        char * token_in = malloc(1000);
        char * token = malloc(1000);
        
        strcpy(token_in,*loc_in);
   
        token = strtok(token_in, delim_in);
     
        *fd_in = open(token, O_RDONLY, 0777);
       
        if (*fd_in == -1)
        {
            // print which type of error have in a code
            fprintf(stderr,"Error: invalid file\n");
            *c = 1;
            return;
  
        }
        else {

            *saved_stdin = dup(STDIN_FILENO);
            dup2(*fd_in,STDIN_FILENO);
            close(*fd_in);
            (*arg_num) --;
            
            
        }

    }
}

void redir_app (int * i, int * arg_num_p, int * arg_num, //int (* pipefd)[*arg_num_p][2],
                int* saved_stdout, int * fd_app, char ** loc_app, int *is_loc_in, //int *is_loc_app,
                int * is_loc_out, int * c//,  char ** file_temp, int * fd_temp
                ){
    if(*loc_app != NULL){

        if(* is_loc_out == 1){
            *c = 1;
            return ;
        }
        
        if(*i!=*arg_num_p && *arg_num_p>0){// if multiple, only last program can include ">>"
            fprintf(stderr,"Error: invalid command\n");
            *c = 1;
            return ;
        }
        
        *loc_app = strtok(*loc_app+4,"\n");
   
        char * token_app_blank = strstr(*loc_app, " ");
   
        if(token_app_blank != NULL && !(*is_loc_out) && !(*is_loc_in)
           && *(token_app_blank+1)) {//blank between filename
            fprintf(stderr,"Error: invalid command\n");
            *c = 1;
            return ;
        }


        char * delim_app = " ";
        char * token_app = malloc(1000);

       
        strcpy(token_app,*loc_app);
        token_app = strtok(token_app, delim_app);
     
        
        *fd_app = open(token_app,O_WRONLY | O_CREAT | O_APPEND, 0777);
     
                *saved_stdout = dup(STDOUT_FILENO);
                dup2(*fd_app,STDOUT_FILENO);
                close(*fd_app);

            (*arg_num) --;
        return;

    }
   
}


void redir_out (int * i, int * arg_num_p, int * arg_num, //int (* pipefd)[*arg_num_p][2],
                int* saved_stdout, int * fd_out, char ** loc_out, int *is_loc_in, //int *is_loc_app,
                int * is_loc_app, int * c){
    if(*loc_out != NULL){
   

        if(* is_loc_app ==1){
            *c = 1;
        
            return;
        }
        
        
        if(*i!=*arg_num_p && *arg_num_p>0){// if multiple, only last program can include ">"
            fprintf(stderr,"Error: invalid command\n");
            *c = 1;
          
            return;
        }

    
        *loc_out = strtok(*loc_out+3,"\n");
    
        char * token_out_blank = strstr(*loc_out, " ");
      
        if(token_out_blank != NULL && !(*is_loc_app) && !(*is_loc_in)
           && *(token_out_blank+1)) {//blank between filename
            fprintf(stderr,"Error: invalid command\n");
            *c = 1;
            return;
        }


        char * delim_out = " ";
        char * token_out = malloc(1000);
        
        strcpy(token_out, *loc_out);
     
        token_out = strtok(token_out, delim_out);
        
        *fd_out = open(token_out,O_WRONLY | O_CREAT | O_TRUNC, 0777);

            *saved_stdout = dup(STDOUT_FILENO);
            dup2(*fd_out,STDOUT_FILENO);
            close(*fd_out);
            (*arg_num) --;

    }
}




void jobs(//char (*jList)[100][1000], int (*pList)[100],
          int* num_cmd//, pid_t * pid
        ){//A job may be suspended by Ctrl-Z, the SIGTSTP signal, or the SIGSTOP signal.
       
    for(int i=1; i<= *num_cmd; i++){
        
        printf("[%d] %s",i, (jList)[i-1]);

    }

}

void fg(int i){
  
    //printf("num_cmd:%d\n",num_cmd);
    if(num_cmd==0||i>num_cmd){
            fprintf(stderr," Error: invalid job\n");
            c=1;
            return;
        }
    pid_t c_pid_now = (pList)[i-1];

    char * c_cmd = jList[i-1];
    kill(c_pid_now,SIGCONT);
  
    if(i!=num_cmd){
        for (int j=i+1;j<=num_cmd;j++){//num_cmd =3, j=1,2,3
           
                char * copy = jList[j-1];
                strcpy(jList[j-2],copy);
                pList[j-2] = pList[j-1];
   
        }
    }else if (i==num_cmd ){//i==j or i>=j
        strcpy(jList[i-1],"\0");
        pList[i-1] = 0;
    }
    
    num_cmd--;
 
    
    int status;
    
    waitpid(c_pid_now, &status, WUNTRACED);
    if(WIFSTOPPED(status)){//true, child suspended
 
        num_cmd++;
        strcpy(jList[num_cmd-1] ,c_cmd);
      
        pList[num_cmd-1] = c_pid_now;
    }

    c = 1;
   
}

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
    
  
    
    int ret = 0;
    
  
    signal(SIGINT, intHandler);
    signal(SIGTSTP, stpHandler);
  
    int fg_index;
    
    while (1) {
        print_prompt();
        get_user_input(command);
    
        //EOF
        if (feof(stdin)) break;
        
        if(strcmp(*command,"\n") == 0){
            continue;
        }
        
        if(strcmp(*command,"exit\n") == 0){
            if(strlen(*command)==5){
                if(num_cmd>0){fprintf(stderr,"Error: there are suspended jobs\n");}
                else exit(0);
            }
            else {fprintf(stderr,"Error: invalid command\n");}
                    continue;
        }
        
        char * exit1 = strstr(*command,"exit ");
        if(exit1){
            fprintf(stderr,"Error: invalid command\n");
            continue;
        }
        
  
        
        if(**command == '|'){
            fprintf(stderr,"Error: invalid command\n");
            continue;
        }
        
      
        if(**command == 'c' && *((*command)+1) == 'd'){
            //cd_flag =
            cd(command);
            continue;
        }
        

        if(**command == 'f' && *((*command)+1) == 'g'//&& *((*command)+3) == 's'
           ){

            char * fg_input = ((*command)+3);
            
            if( *((*command)+2) == ' ' && !(strstr(fg_input," "))){
                //build input char* with null terminator
                *(fg_input+strlen(fg_input)-1) = '\0';
              
                fg_index = atoi(fg_input);

                fg(fg_index);
                //continue;
            }
            else {
                fprintf(stderr,"Error: invalid command\n");
                continue;
            }
        }
        if(c==1) {c=0; continue;}
        
        if(**command == 'j' && *((*command)+1) == 'o'&& *((*command)+2) == 'b'&& *((*command)+3) == 's'){
            if(strlen(*command)==5){
              
                jobs(&num_cmd);
              
            }

            else fprintf(stderr,"Error: invalid command\n");
            continue;
        }
        //count pipe_num (|) of user input
        
        int arg_num_p = 0;
       
        char * loc_p =  strstr(*command, " | ");
       
        char * loc_p_next = loc_p;
        
        char * loc_p_invalid =  strstr(*command, " |");
      
        if(loc_p_invalid && !(loc_p)){
            fprintf(stderr,"Error: invalid command\n");
            continue;
        }
        
        while(loc_p){
           
            arg_num_p ++;
            loc_p =  strstr(loc_p+3, " | ");
        }

        int pipefd[arg_num_p][2];// num of pipes, if arg_num_p=1: 1 pipe, 2 process
        int pids[arg_num_p+1];// num of process
        
      
            for (int i=0; i<arg_num_p; i++){
                if(pipe(pipefd[i])==-1){
                    printf("Error creating pipe\n: %d", errno);
                }
            }

        for (int i=0; i<arg_num_p+1; i++){
          
            char *delim_p = "|,\n";
         
            char *token_p;
          
            strcpy(str, *command);
            
            token_p = strtok(str, delim_p);//token_p is the sub command
            if (token_p == NULL) token_p = *command;//no pipe found
          
            
            if(i==0&&arg_num_p>0){loc_p_next = loc_p_next+3;}
            else if(i<arg_num_p){
                loc_p_next = strstr(loc_p_next, " | ");
                loc_p_next += 3;//command for the next for-loop
            }
         
            //invalid command: end with "|", "<", etc.
            
            if((*(token_p + (int)strlen(token_p)-1)) == '<' ||
               (*(token_p + (int)strlen(token_p)-1)) == '>' ||
               ((*(token_p + (int)strlen(token_p)-1)) == '>' && (*token_p + strlen(token_p)-2) == '>')
                
               )
            {
                fprintf(stderr,"Error: invalid command\n");
                continue;
            }
            
            // in each pipe sub-interval command
            //count arg_num of user input
            char *delim = " ,\n,>,<";
            char *token;
           
            strcpy(str3, token_p);

            token = strtok(str3, delim);
            
            int arg_num = 0;
         
            while(token){
             
                arg_num ++;
                token = strtok(NULL, delim);
            }

            char * loc_in =  strstr(token_p, " < ");
            char * loc_out =  strstr(token_p, " > ");
            char * loc_app =  strstr(token_p, " >> ");
            char * loc_in_invalid =  strstr(token_p, " << ");
           
            if(loc_in_invalid) {
                fprintf(stderr,"Error: invalid command\n");
                continue;
            }
            
        
            
            int saved_stdout;
            int saved_stdin;
            int fd_in;
            int fd_app;
            int fd_out;
            
            int is_loc_in = 0;
            int is_loc_app = 0;
            int is_loc_out = 0;
            if(loc_in)is_loc_in = 1;
            if(loc_app)is_loc_app = 1;
            if(loc_out)is_loc_out = 1;
      
            // two "<"
            if(loc_in){
                if(strstr(loc_in+1, " < ") != NULL) {
                    fprintf(stderr,"Error: invalid command\n");
                    // *c = 1;
                    continue;
                }}
            
            // two ">>"
            if(loc_app){
                if(strstr(loc_app+2, " >> ") != NULL) {
                    fprintf(stderr,"Error: invalid command\n");
                    // *c = 1;
                    continue;
                }}
            
            // two ">"
            if(loc_out){
                if(strstr(loc_out+1, " > ") != NULL) {
                    fprintf(stderr,"Error: invalid command\n");
                    // *c = 1;
                    continue;
                }}
            

            for(int r=(int)strlen(*command) ; r>0; r--){

                if(is_loc_in ==1){
                    if(r == (int)strlen(loc_in)  ){
                       
                        redir_in (&i, &arg_num_p, &arg_num,
                                  &saved_stdin, &fd_in, &loc_in, &is_loc_app, &is_loc_out, &c);
                        is_loc_in ++;
                    }
                }
              
                if(c==1)break;
                if(is_loc_app ==1){
                    if(r == (int)strlen(loc_app)  ){
                       
                        redir_app (&i, &arg_num_p, &arg_num,
                                   &saved_stdout, &fd_app, &loc_app, &is_loc_in, &is_loc_out, &c
                                   
                                                   );
                        is_loc_app ++;
                    }
                }
                if(c==1)break;
                if(is_loc_out ==1){
                    if(r == (int)strlen(loc_out)  ){
                        redir_out (&i, &arg_num_p, &arg_num,
                                   &saved_stdout, &fd_out, &loc_out, &is_loc_in, &is_loc_app, &c);
                        is_loc_out ++;
                    }
                }
                if(c==1)break;
                
            }
           
            if(c==1)break;

     
            //initialize arglist for execv()
            char* arglist [arg_num+1];
        
            char *token2;
          
            strcpy(str2, token_p);

            token2 = strtok(str2, delim);
          
            for(int i=0; i<arg_num; i++){
        
                arglist[i] = token2;
                token2 = strtok(NULL, delim);
               
            }
            arglist[arg_num] = NULL;

            
            char * loc =  strchr(token_p, '/');
            
            if(loc == NULL) {//3. Locating programs: only the base name
                size_t len;
                size_t len_concat;
                
              
                len = strlen(token_p);
              
                strcat(concat, "/usr/bin/");
                len_concat = strlen(concat);
              
                strcpy(str, token_p);
                token = strtok(str, delim);//token: the first arg in command
                strcat(concat, token);// "/usr/bin/" + the first arg in command
                
                
                if(i == arg_num_p) strncpy(concat2,concat,len_concat + len);
                else strncpy(concat2,concat,len_concat + len-1); //eliminate '/0'
                
            }
            else{//1. Locating programs: absolute path
              
                if(*token_p == '/'){
                
                    concat2 = str3;
                   
                    arglist[0] = str3;
                }
                else{//2. Locating programs: relative path
                    char cwd[256];
                    if (getcwd(cwd, sizeof(cwd)) == NULL)
                        fprintf(stderr,"Error: invalid program\n");
                    else{
                        concat2 = strcat(cwd,"/");
                        strcat(concat2, str3);
                        char * ptr = strrchr(arglist[0] , '/' );
                        arglist[0] = ptr + 1;
                    }
                }
                
            }
            
    
            //milestone3: run a simple program, such as ls
 
            pids[i] = fork();
        
            if (pids[i] < 0) {
               
                perror("fork() error");
            } else if (pids[i] == 0) {
      
                if(i==0 && i!=arg_num_p){//0~
                
                    
                    for(int j=0; j<arg_num_p; j++){
                        if(i!=j){
                            close(pipefd[j][1]);//==-1;
                    }
                        close(pipefd[j][0]);//==-1;

                    }
            
                    
                    dup2(pipefd[i][1],STDOUT_FILENO);//
                   
                    close(pipefd[i][1]);
 
                }
                else if(i==0){// i==0 && i==arg_num_p
                   
//                    close(pipefd[i][0]);
//                    close(pipefd[i][1]);
                }
                else if(i!=0 && i==arg_num_p) {// i!=0 && i==arg_num_p
                
                    for(int j=0; j<arg_num_p; j++){
                        if(i-1!=j)close(pipefd[j][0]);
                        close(pipefd[j][1]);//==-1)
                             
                        
                    }

                    
                    dup2(pipefd[i-1][0],STDIN_FILENO);

                       close(pipefd[i-1][0]);//==-1)//;
                  
                }
                else {// 0<i<arg_num_p
                    for(int j=0; j<arg_num_p; j++){
                        if(i-1!=j)close(pipefd[j][0]);
                        
                        if(i!=j)close(pipefd[j][1]);
                        
                    }
                    
                    dup2(pipefd[i-1][0],STDIN_FILENO);
                    dup2(pipefd[i][1],STDOUT_FILENO);
                   
                    close(pipefd[i-1][0]);
                    
                       close(pipefd[i][1]);
                    
                    
                }


                ret = execv(concat2,arglist);// //char* sample2[] = {"ls", "-l", NULL};
              
                if (ret == -1) {
                    fprintf(stderr,"Error: invalid program\n");
                  
                    
                }
                
                exit(-1);
            }
            
            else{
                
                c_pid = pids[i];
            
                
                if(loc_in!=NULL) {
                    
                    dup2(saved_stdin, STDIN_FILENO);}//redirected input

                if(loc_out!=NULL||loc_app!=NULL) {
                    
                    dup2(saved_stdout, STDOUT_FILENO);}//redirected output
               
            }
            

            if(i<arg_num_p) *command = loc_p_next;
            
         
           concat = malloc(1000);
          str = malloc(1000);
             concat2 = malloc(1000);
            str2 = malloc(1000);
            str3 = malloc(1000);
       
        }
         //shell
        
       // close pipes
        for (int i=0; i<arg_num_p; i++){
      
            close(pipefd[i][1]);//==-1)//;
            close(pipefd[i][0]);//==-1)//;
            
        }
        
        //wait for child processes

        for (int i=0; i<arg_num_p+1; i++){

            int status;
          
            waitpid(c_pid, &status, WUNTRACED);
            if(WIFSTOPPED(status)){//true, child suspended

                num_cmd++;
                strcpy(jList[num_cmd-1] ,*command);
               
                pList[num_cmd-1] = c_pid;
            }

            
        }
     

    }
 

        free(*command);
        free(command);
        free(concat);
        free(concat2);
        free(str);
        free(str2);
        free(str3);

    return 0;
}
