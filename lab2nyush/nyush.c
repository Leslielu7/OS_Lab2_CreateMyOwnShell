//
//  main.c
//  lab2nyush
//
//  Created by Leslie Lu on 2/17/23.
//
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //getcwd()
#include <string.h> //strrchr()
#include <sys/wait.h>
#include<fcntl.h>
#include <signal.h>

#include<errno.h>
//#include<getopt.h>
extern int errno;
extern int opterr;


//static volatile int keepRunning = 0;

pid_t c_pid ;
char jList [100][1000] ;
int pList [100];
int c = 0;
int num_cmd = 0;

//char ** command ;
//*command = malloc(sizeof(char*));
//char ** command = malloc(sizeof(char*));
//*command = malloc(sizeof(char*));
// Ctrl-C or Ctrl-Z
// don’t expect to terminate or suspend the shell. Therefore, your shell should ignore the following signals: SIGINT, SIGQUIT, and SIGTSTP


//CTRL+C, SIGINT
void intHandler(int sig) {
    //printf("intHandler: getpid:%d\n",getpid());
    //printf("intHandler: c_pid:%d\n",c_pid);
    if(getpid()!=c_pid)kill(c_pid, sig);
        else return;
    //wait(NULL);
                    
                    
                    
//    kill(pid, SIGSTOP);//kill child
//    kill(getpid(), SIGSTOP);//kill child
    //kill(getpid(), SIGTSTP);// kill shell
    //exit(0);
}

//Ctrl-Z, SIGTSTP:
void stpHandler(int sig) {//stop child

//    printf("c_pid:%d\n",c_pid);
//    kill(c_pid, sig);
    if(getpid()!=c_pid)kill(c_pid, sig);
        else return;
//    pid_t pid_child = waitpid(-1, &status, WUNTRACED);
//    printf("pid_child:%d\n",pid_child);
    //kill(pid_child , SIGTSTP);//stop child

}



//Ctrl-D, SIGQUIT:
void quitHandler(//int dummy
                ) {
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
//    printf("strlen:%ld\n",strlen(*command));
//    *(*command+strlen(*command)-1) = NULL;
//    printf("str__:%s\n",*command+strlen(*command)-1);
//    printf("str__:%s\n",*command+strlen(*command)-2);
    //if(**command == 'e') { *exit_c = -1;}// if user inputs 'e', set exit code to -1 to leave prompt
//    else {
//        //printf("size:%zu, string:%s\n", n, *command);
//        //fflush(stdout);
//        //free(*command);
//    }
}

//built-in command:cd
int cd(char ** command){
    
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
    return -1;
}


void redir_in (int * i, int * arg_num_p, int * arg_num,// int (* pipefd)[*arg_num_p][2],
               int* saved_stdin, int * fd_in, char ** loc_in, //int *is_loc_in,
               int *is_loc_app, int * is_loc_out, //int * b,
               int * c){
    if(*loc_in != NULL){
        //printf("done.\n");
        //                if(cd_flag==-1) {
        //                    fprintf(stderr,"Error: invalid command\n");
        //                    continue;
        //
        //                }
        
        
        
        if(*i!=0 && *arg_num_p>0){// if multiple, only first program can include "<"
            fprintf(stderr,"Error: invalid command\n");
//            for(int j=0; j<*arg_num_p; j++){
//                close((*pipefd)[j][1]);//==-1;
//                close((*pipefd)[j][0]);//==-1;
//            }
//
            *c = 1;
            return;
        }

        //printf("here~~*loc_in:%s\n", *loc_in);
        char * loc_in_cpy = malloc(1000);
        strcpy(loc_in_cpy,*loc_in);
        *loc_in = strtok(loc_in_cpy+3,"\n");
        //printf("heyhere~~*loc_in:%s\n", *loc_in);
        
        //if(*is_loc_in>1) return;
        
        //                printf("1. loc_in:%s\n", loc_in);
        //                printf("1. strlen_loc_in:%ld\n", strlen(loc_in));
        char * token_in_blank = strstr(*loc_in, " ");
     //   printf("token_in_blank:%s\n", token_in_blank);
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
     //   printf("*loc_in:%s\n", *loc_in);
        token = strtok(token_in, delim_in);
      //  printf("token_in:%s\n", token_in);
        *fd_in = open(token, O_RDONLY, 0777);
        //printf("2. loc_in:%s\n", loc_in);
        if (*fd_in == -1)
        {
            // print which type of error have in a code
            fprintf(stderr,"Error: invalid file\n");
            *c = 1;
            return;
            //                 print program detail "Success or failure"
            //                perror("Program");
        }
        else {
            //fflush(stdout);
            *saved_stdin = dup(STDIN_FILENO);
            dup2(*fd_in,STDIN_FILENO);
            close(*fd_in);
            (*arg_num) --;
            
            
        }
//        free(loc_in_cpy);
//        free(token_in);
//        free(token);
    }
}

//char*
void redir_app (int * i, int * arg_num_p, int * arg_num, //int (* pipefd)[*arg_num_p][2],
                int* saved_stdout, int * fd_app, char ** loc_app, int *is_loc_in, //int *is_loc_app,
                int * is_loc_out, int * c//,  char ** file_temp, int * fd_temp
                ){
    if(*loc_app != NULL){
        //                if(cd_flag==-1) {
        //                    fprintf(stderr,"Error: invalid command\n");
        //                    continue;
        //
        //                }

        if(* is_loc_out == 1){
            *c = 1;
            return ;
        }
        
        if(*i!=*arg_num_p && *arg_num_p>0){// if multiple, only last program can include ">>"
            fprintf(stderr,"Error: invalid command\n");
            *c = 1;
            return ;
        }
        
        //printf("here~~*loc_app:%s\n", *loc_app);
        *loc_app = strtok(*loc_app+4,"\n");
        //printf("heyhere~~*loc_app:%s\n", *loc_app);

        //if(*is_loc_in>1) return;

        //                printf("1. loc_in:%s\n", loc_in);
        //                printf("1. strlen_loc_in:%ld\n", strlen(loc_in));
        char * token_app_blank = strstr(*loc_app, " ");
        //printf("token_app_blank:%s\n", token_app_blank);
        if(token_app_blank != NULL && !(*is_loc_out) && !(*is_loc_in)
           && *(token_app_blank+1)) {//blank between filename
            fprintf(stderr,"Error: invalid command\n");
            *c = 1;
            return ;
        }


        char * delim_app = " ";
        char * token_app = malloc(1000);

       // printf("*loc_app:%s\n", *loc_app);
        strcpy(token_app,*loc_app);
        token_app = strtok(token_app, delim_app);
        //printf("token_app:%s\n", token_app);
        
        *fd_app = open(token_app,O_WRONLY | O_CREAT | O_APPEND, 0777);
     
        //printf("is loc in:%d\n", * is_loc_in );
//            if(*is_loc_in == 1){// create a temporary file for "cat >> output.txt < input.txt"
//               // cat a > /tmp/tempfile ; mv /tmp/tempfile a
////                char * buf = "";
////                char * file_temp = "temporary.txt";
//                * fd_temp = open(*file_temp,O_WRONLY | O_CREAT | O_TRUNC, 0777);
//                //= creat(file_temp, 0777);
////                *saved_stdout = dup(STDOUT_FILENO);
////                dup2(fd_temp,STDOUT_FILENO);
////                printf("hiii\n");
////                //read temporary, append to output.txt, dup2, remove temporary
////                while(read(fd_temp,buf,strlen(buf))>0){
////                    write(*fd_app,buf,strlen(buf));
////                }
////                dup2(*fd_app,STDOUT_FILENO);
////
//
////                //remove(file_temp);
//                //char * file_temp = "temporary.txt";
//                //int fd_temp;
//
//
//                *saved_stdout = dup(STDOUT_FILENO);
//                dup2(*fd_temp,STDOUT_FILENO);
//              //  printf("hiii\n");
//                //read temporary, append to output.txt, dup2, remove temporary
////                while(read(fd_temp,buf,strlen(buf))>0){
////                    write(*fd_app,buf,strlen(buf));
////                }
////                dup2(*fd_app,STDOUT_FILENO);
////
//                close(*fd_temp);
////                //remove(file_temp);
//                close(*fd_app);
////
//                printf("token_app:%s\n", token_app);
//            }
//            else{
                *saved_stdout = dup(STDOUT_FILENO);
                dup2(*fd_app,STDOUT_FILENO);
                close(*fd_app);
//            }
            (*arg_num) --;
        return; //token_app;
//        }
    }
    //else {return "";}
}


void redir_out (int * i, int * arg_num_p, int * arg_num, //int (* pipefd)[*arg_num_p][2],
                int* saved_stdout, int * fd_out, char ** loc_out, int *is_loc_in, //int *is_loc_app,
                int * is_loc_app, int * c){
    if(*loc_out != NULL){
        //                if(cd_flag==-1) {
        //                    fprintf(stderr,"Error: invalid command\n");
        //                    continue;
        //
        //                }

        if(* is_loc_app ==1){
            *c = 1;
          //  printf("been here");
            return;
        }
        
        
        if(*i!=*arg_num_p && *arg_num_p>0){// if multiple, only last program can include ">"
            fprintf(stderr,"Error: invalid command\n");
            *c = 1;
           // printf("C=2\n");
            return;
        }

       //printf("here~~*loc_out:%s\n", *loc_out);
        *loc_out = strtok(*loc_out+3,"\n");
      //printf("heyhere~~*loc_out:%s\n", *loc_out);

        //if(*is_loc_in>1) return;

        //                printf("1. loc_in:%s\n", loc_in);
        //                printf("1. strlen_loc_in:%ld\n", strlen(loc_in));
        char * token_out_blank = strstr(*loc_out, " ");
      //  printf("token_out_blank:%s\n", token_out_blank);
        if(token_out_blank != NULL && !(*is_loc_app) && !(*is_loc_in)
           && *(token_out_blank+1)) {//blank between filename
            fprintf(stderr,"Error: invalid command\n");
            *c = 1;
            return;
        }


        char * delim_out = " ";
        char * token_out = malloc(1000);
        
        strcpy(token_out, *loc_out);
      //  printf("*loc_out:%s\n", *loc_out);
        token_out = strtok(token_out, delim_out);
      //  printf("token_out:%s\n", token_out);
        
        *fd_out = open(token_out,O_WRONLY | O_CREAT | O_TRUNC, 0777);

            *saved_stdout = dup(STDOUT_FILENO);
            dup2(*fd_out,STDOUT_FILENO);
            close(*fd_out);
            (*arg_num) --;
//        }
    }
}


//void jobs(char *** joblist,  int* num_cmd, pid_t * pid){//A job may be suspended by Ctrl-Z, the SIGTSTP signal, or the SIGSTOP signal.
//    for(int i=1; i<= *num_cmd; i++){
//        printf("[%d] %s %d\n",i,*(*joblist+1000*(i-1)), *pid);
//    }
//
//}

void jobs(//char (*jList)[100][1000], int (*pList)[100],
          int* num_cmd//, pid_t * pid
        ){//A job may be suspended by Ctrl-Z, the SIGTSTP signal, or the SIGSTOP signal.
    for(int i=1; i<= *num_cmd; i++){
        //printf("[%d] %s %d\n",i,*(*jList+1000*(i-1)), *pid);
        printf("[%d] %s",i, (jList)[i-1]);
//        printf("[%d] %s %d\n",i, (jList)[i-1], (pList)[i-1]);
    }

}

void fg(int i){
   // printf("inside fg\n");
    
    if(num_cmd==0||i>num_cmd){
            fprintf(stderr," Error: invalid job\n");
            c=1;
            return;
        }
    
    c_pid = (pList)[i-1];
    char * c_cmd = jList[i-1];
    kill(c_pid,SIGCONT);
    //printf("inside fg_c_cmd:%s\n", c_cmd);
    //printf("inside fg_c_pid:%d\n", c_pid);
    //clear process items in jList and pList
//    strcpy(jList[i-1],'\0');
//    pList[i-1]= -1;
   // printf("here\n");
    //wait(NULL);
   // printf("AFTER inside fg_c_pid:%d\n", c_pid);
    //update jList and pList
//    for(int i=1; i<= num_cmd; i++){
//        jList[i-1]
//    }
  //  printf("num_cmd:%d\n", num_cmd);
   // printf("HEY inside fg_c_pid:%d\n", c_pid);
    for (int j=1;j<=num_cmd;j++){
    //    printf("3.inside fg_c_pid:%d\n", c_pid);
        if(i!=j && i<j){
          //  printf("4.inside fg_c_pid:%d\n", c_pid);
            char * copy = jList[j-1];
            strcpy(jList[j-2],copy);
            pList[j-2] = pList[j-1];
           // printf("5.inside fg_c_pid:%d\n", c_pid);
        }
        strcpy(jList[num_cmd-1],"\0");
        //jList[num_cmd]=
        pList[num_cmd-1] = 0;
    }
    
    //printf("1.inside fg_c_pid:%d\n", c_pid);
    num_cmd--;
    //printf("2.inside fg_c_pid:%d\n", c_pid);
    
    int status;
    
    waitpid(c_pid, &status, WUNTRACED);
    if(WIFSTOPPED(status)){//true, child suspended
//        printf("In status: c_pid:%d\n",c_pid);//terminated child's pid
//        printf("In status: *command:%s\n",c_cmd);//terminated child's command
        //*(joblist + 1000*(num_cmd-1))= *command;
//                **jList[num_cmd-1] = *command;
        num_cmd++;
        strcpy(jList[num_cmd-1] ,c_cmd);
        //strcpy(pList[num_cmd-1] , (int)c_pid);
        pList[num_cmd-1] = c_pid;
    }
//    wait(NULL);
    c = 1;
    //return;
}

//body
int main(
        //int argc, const char * argv[]
) {
    // insert code here...
    //opterr = 0;
    //printf("argc:%d\n", argc);
  //  printf("argc:%d\n", argc);
    //char * jList [100];
    //int * pList [100];
    
//    char ** joblist = malloc(sizeof(char*));
//    for(int s=0; s<100; s++){
//        * joblist = malloc(sizeof(char)*1000);
//    }
//    char jList [100][1000] ;
//    int pList [100];
    
    
    
    
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
    //signal(SIGINT, SIG_IGN);
    signal(SIGINT, intHandler);
    signal(SIGTSTP, stpHandler);
    //signal(SIGQUIT, quitHandler);
//    signal(SIGQUIT, quitHandler);
    int fg_index;
    
    while (1) {
        print_prompt();
        get_user_input(command);
      //  ((int)strlen(*joblist))
        //num_cmd++;
       // *(joblist + 1000*(num_cmd-1))= *command;
        
        
        
        //printf("num_cmd:%d\n",num_cmd);
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
        
  
        
        if(**command == '|'){
            fprintf(stderr,"Error: invalid command\n");
            continue;
        }
        
        
        //int cd_flag = 0;
        if(**command == 'c' && *((*command)+1) == 'd'){
            //cd_flag =
            cd(command);
            continue;
        }
        
//        if(**command == 'f' && *((*command)+1) == 'g' ){
//            int target_index = *((*command)+3);//the index of fg
//            //pid_t target_pid =
//               // kill(target_pid,SIGCONT);
//
//            continue;
//        }
        if(**command == 'f' && *((*command)+1) == 'g'//&& *((*command)+3) == 's'
           ){
//            printf("Inside main() fg\n");
//            printf("Inside main() %s\n",((*command)+3));
            char * fg_input = ((*command)+3);
            
            if( *((*command)+2) == ' ' && !(strstr(fg_input," "))){
                //build input char* with null terminator
                *(fg_input+strlen(fg_input)-1) = '\0';
                //int fg_index;
                //sscanf(fg_input, "%d", &fg_index);
                //int
                fg_index = atoi(fg_input);
//                printf("fg_input %ld\n",strlen(fg_input));
//                printf("fg_index %d\n",fg_index);
                fg(fg_index);
                //continue;
            }
            else {
                fprintf(stderr,"Error: invalid command\n");
                continue;
            }
        }
        if(c==1) {c=0; continue;}
//        char * is_Job = strstr(*command, "jobs\n");
//        printf("?%s:\n",is_Job );
        if(**command == 'j' && *((*command)+1) == 'o'&& *((*command)+2) == 'b'&& *((*command)+3) == 's'){
            if(strlen(*command)==5){
                //pid_t pid = getpid();
                //jobs(&joblist,  &num_cmd, &pid);
                jobs(//&jList, &pList,
                     &num_cmd//, &pid
                     );
                //printf("jobs");
            }

            else fprintf(stderr,"Error: invalid command\n");
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
       
        char * loc_p_next = loc_p;
//        if(loc_p) { loc_p_next = loc_p+3;}//command for the next for-loop
        
        char * loc_p_invalid =  strstr(*command, " |");
        //printf("loc_p: %s\n",loc_p_invalid);
        if(loc_p_invalid && !(loc_p)){
            fprintf(stderr,"Error: invalid command\n");
            continue;
        }
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
           //printf("i: %d\n",i);
            
//            pids[i] = fork();
            
            char *delim_p = "|,\n";
           // printf("*delim_p: %s\n",delim_p);
            char *token_p;
          //  printf("*token_p: %s\n",token_p);
            strcpy(str, *command);
//            printf("1.*command: %s\n",*command);
//            printf("str: %s\n",str);
//            printf("len_str: %ld\n",strlen(str));
            token_p = strtok(str, delim_p);//token_p is the sub command
            if (token_p == NULL) token_p = *command;//no pipe found
            //printf("2.*command: %s\n",*command);
            
            if(i==0&&arg_num_p>0){loc_p_next = loc_p_next+3;}
            else if(i<arg_num_p){
                loc_p_next = strstr(loc_p_next, " | ");
                loc_p_next += 3;//command for the next for-loop
            }
            //printf("loc_p_next~~~: %s\n",loc_p_next);
            //invalid command: begin with "|"
//            if(*token_p == '|' ){
//                fprintf(stderr,"Error: invalid command\n");
//                continue;
//            }
            
            //invalid command: end with "|", "<", etc.
            
//            printf("2. token_p: %s\n",token_p + (int)strlen(token_p)-1);
            if((*(token_p + (int)strlen(token_p)-1)) == '<' ||
               (*(token_p + (int)strlen(token_p)-1)) == '>' ||
               ((*(token_p + (int)strlen(token_p)-1)) == '>' && (*token_p + strlen(token_p)-2) == '>')
                
               )
            {
                fprintf(stderr,"Error: invalid command\n");
                continue;
            }
            
            
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
            
            //printf(" 1.token_p:%s\n", token_p);
            char * loc_in =  strstr(token_p, " < ");
            char * loc_out =  strstr(token_p, " > ");
            char * loc_app =  strstr(token_p, " >> ");
            char * loc_in_invalid =  strstr(token_p, " << ");
            //printf(" 2.token_p:%s\n", token_p);
            
            //printf(" loc_in:%s\n", loc_in);
//            printf(" strlen_loc_in:%ld\n", strlen(loc_in));
//            printf(" loc_out:%s\n", loc_out);
//            printf(" strlen_loc_out:%ld\n", strlen(loc_out));
            //printf(" loc_app:%s\n", loc_app);
//            printf(" strlen_loc_app:%ld\n", strlen(loc_app));
//
            if(loc_in_invalid) {
                fprintf(stderr,"Error: invalid command\n");
                continue;
            }
            
            //printf("token_p fresh4:%s\n", token_p);
            //            char * loc_in =  strstr(*command, " < ");
            //            char * loc_out =  strstr(*command, " > ");
            //            char * loc_app =  strstr(*command, " >> ");
            //printf("*command:%s\n", *command);
            
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
            
            //int b = 0;
            
            
          //  printf("(int)strlen(*command):%d\n", (int)strlen(*command));
            //printf("(int)strlen(loc_in):%d\n", (int)strlen(loc_in));
           // printf("(int)strlen(loc_out):%d\n", (int)strlen(loc_out));
            //printf("(int)strlen(loc_app):%d\n", (int)strlen(loc_app));
            
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
            
//            char * buf = "";
//            char * file_temp = "temporary.txt";
//            int fd_temp;
//            char * prt_token_app;
            
          //  printf("hereeeee");
            for(int r=(int)strlen(*command) ; r>0; r--){
//                printf("r:%d\n",r);
//                printf("is_loc_in:%d\n",is_loc_in);
//                printf("is_loc_in:%d\n",is_loc_app);
//                printf("(int)strlen(loc_in):%s\n", loc_in);
//                printf("token_p:%s\n", token_p);
               // printf("(int)strlen(loc_out):%d\n", (int)strlen(loc_out));
                //printf("(int)strlen(loc_app):%s\n", loc_app);
                if(is_loc_in ==1){
                    if(r == (int)strlen(loc_in)  ){
                        //printf("is_loc_in:%d\n",is_loc_in);
                        redir_in (&i, &arg_num_p, &arg_num, //&pipefd,
                                  &saved_stdin, &fd_in, &loc_in, &is_loc_app, &is_loc_out, &c);
                        is_loc_in ++;
                    }
                }
                //printf("end in:\n");
                if(c==1)break;
                if(is_loc_app ==1){
                    if(r == (int)strlen(loc_app)  ){
                        //prt_token_app =
                        redir_app (&i, &arg_num_p, &arg_num, //&pipefd,
                                   &saved_stdout, &fd_app, &loc_app, &is_loc_in, &is_loc_out, &c
                                    //,&file_temp, &fd_temp
                                                   );
                        is_loc_app ++;
                    }
                }
                if(c==1)break;
                if(is_loc_out ==1){
                    if(r == (int)strlen(loc_out)  ){
                        redir_out (&i, &arg_num_p, &arg_num, //&pipefd,
                                   &saved_stdout, &fd_out, &loc_out, &is_loc_in, &is_loc_app, &c);
                        is_loc_out ++;
                    }
                }
                if(c==1)break;
                
            }
            //if(c==2)break;
            if(c==1)break;

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
//                     if(arg_num>1)  printf("arglist2:%s",arglist[2]);
//            if(arg_num>2) printf("arglist3:%s",arglist[3]);
          
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
                    //char * ptr = strrchr(arglist[0] , '/' );
//                    printf("str3:%s\n",str3);
//                    printf("arglist[0]:%s\n",arglist[0]);
//                    printf("str3:%ld\n",strlen(str3));
//                    printf("len_arglist[0]:%ld\n",strlen(arglist[0]));
                    concat2 = str3;
                    //arglist[0] = ptr + 1;
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
                    //if(loc_in!=NULL) saved_stdin = dup(pipefd[i-1][0]);
//                    if(loc_out!=NULL||loc_app!=NULL)saved_stdout = dup(pipefd[i][1]);
                    
                    dup2(pipefd[i][1],STDOUT_FILENO);//
                    //close(pipefd[i][0]);//
                    close(pipefd[i][1]);
 
                }
                else if(i==0){// i==0 && i==arg_num_p
                    //dup2(pipefd[i][1],STDOUT_FILENO);
//                    close(pipefd[i][0]);
//                    close(pipefd[i][1]);
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
//                    saved_stdout = dup(STDOUT_FILENO);
//                    dup2(fd_app,STDOUT_FILENO);
                 //   if(loc_app!=NULL)printf("i'm app");
                   // if(loc_out!=NULL)printf("i'm out");
                    
                    dup2(pipefd[i-1][0],STDIN_FILENO);

                       close(pipefd[i-1][0]);//==-1)//;
                  
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
                    //if(
                    close(pipefd[i-1][0]);//==-1)//;
                    //printf("Here4.1: Error creating pipe\n: %d", errno);
                   // if(
                       close(pipefd[i][1]);//==-1)//;
                    //printf("Here4.2: Error creating pipe\n: %d", errno);
                    
                }

//                printf("heyyyy here:\n");
//                printf("concat2:%s\n", concat2);
                //pid_t pid2 = getpid();
                //printf("pid before execv%d:\n", pid2);
                //signal(SIGINT, intHandler);
                ret = execv(concat2,arglist);// //char* sample2[] = {"ls", "-l", NULL};
              
                if (ret == -1) {
                    fprintf(stderr,"Error: invalid program\n");
                    //continue;//Error: invalid program
                    //perror("execve error");
                }
                
                // exec failed
                //exit( EXIT_FAILURE );
                exit(-1);
            }
            
            else{//parent process
               // signal(SIGINT, intHandler);
                c_pid = pids[i];
            
                
                //waitpid(pid,NULL,0);
//                if(loc_in!=NULL && loc_app!=NULL && (int)strlen(loc_app) < (int)strlen(loc_in)) {
//                    fd_app = open(prt_token_app,O_WRONLY | O_CREAT | O_APPEND, 0777);
//                    fd_temp = open(file_temp,O_RDONLY | O_CREAT | O_TRUNC, 0777);
//
//                    while(read(fd_temp,buf,sizeof(char))>0){
//                        write(fd_app,buf,sizeof(char));
//                    }
//
//                    //dup2(fd_app,STDOUT_FILENO);
//                    //
//
//                    //                //remove(file_temp);
//                    close(fd_app);
//
//                    dup2(saved_stdout, STDOUT_FILENO);
//                    close(saved_stdout);
//                    close(fd_temp);
//                }
//
                if(loc_in!=NULL) {
                    //printf("i:%d, in\n:",i);
                    dup2(saved_stdin, STDIN_FILENO);
                    //close(saved_stdin);
                }//redirected input
//                if (saved_stdout)
                if(loc_out!=NULL||loc_app!=NULL) {
                    //printf("i:%d, out\n:",i);
                    dup2(saved_stdout, STDOUT_FILENO);}//redirected output
                //            close(saved_stdout);
                //waitpid(-1, ...);
                

                
            }
            
//            int status;
//            //pid_t sus_pid;
//            //sus_pid =
//            waitpid(c_pid, &status, WUNTRACED);
//            if(WIFSTOPPED(status)){//true, child suspended
//                printf("In status: c_pid:%d\n",c_pid);//terminated child's pid
//                printf("In status: *command:%s\n",*command);//terminated child's command
//                //*(joblist + 1000*(num_cmd-1))= *command;
////                **jList[num_cmd-1] = *command;
//                num_cmd++;
//                strcpy(jList[num_cmd-1] ,*command);
//                //strcpy(pList[num_cmd-1] , (int)c_pid);
//                pList[num_cmd-1] = c_pid;
//            }
            
//            printf("1. *command: %s",*command);
//            printf("1.strlen *command: %ld",strlen(*command));
//            printf("arg_num_p: %d\n",arg_num_p);
//            printf("loc_p_next: %s\n",loc_p_next);
            if(i<arg_num_p) *command = loc_p_next;
            
           // * command = malloc(sizeof(char*));
           concat = malloc(1000);
          str = malloc(1000);
             concat2 = malloc(1000);
            str2 = malloc(1000);
            str3 = malloc(1000);
            //arglist = []
        }
         //shell
        
       // close pipes
        for (int i=0; i<arg_num_p; i++){
           // if(
            close(pipefd[i][1]);//==-1)//;
            //printf("Shell: Error creating pipe: %d, %d", errno,i);//
           // if(
            close(pipefd[i][0]);//==-1)//;
            //printf("Shell: Error creating pipe: %d, %d", errno,i);//
        }
        
        //wait for child processes
//        int status;
//        int c_pid;
        for (int i=0; i<arg_num_p+1; i++){
//            int w1 =
            //wait(NULL);
//            printf("waited for %d\n",w1);
            //int w2 =
            //wait(NULL);
            //printf("waited for %d\n",w2);
            //wait(NULL);
//            int status;
//
            int status;
            //pid_t sus_pid;
            //sus_pid =
            waitpid(c_pid, &status, WUNTRACED);
            if(WIFSTOPPED(status)){//true, child suspended
//                printf("In status: c_pid:%d\n",c_pid);//terminated child's pid
//                printf("In status: *command:%s\n",*command);//terminated child's command
                //*(joblist + 1000*(num_cmd-1))= *command;
//                **jList[num_cmd-1] = *command;
                num_cmd++;
                strcpy(jList[num_cmd-1] ,*command);
                //strcpy(pList[num_cmd-1] , (int)c_pid);
                pList[num_cmd-1] = c_pid;
            }
            
//            waitpid(c_pid, &status, WUNTRACED);
//            if(WIFSTOPPED(status)){//true, child suspended
//                printf("In status: c_pid:%d\n",c_pid);//terminated child's pid
//                printf("In status: *command:%s\n",*command);//terminated child's command
//                //*(joblist + 1000*(num_cmd-1))= *command;
////                **jList[num_cmd-1] = *command;
//                num_cmd++;
//                strcpy(jList[num_cmd-1] ,*command);
//                //strcpy(pList[num_cmd-1] , (int)c_pid);
//                pList[num_cmd-1] = c_pid;
//            }
            
            
        }
        //printf("status:%d",status);
//
    }
    //while(1): prompt
   // return EXIT_SUCCESS;

        free(*command);
        free(command);
        free(concat);
        free(concat2);
        free(str);
        free(str2);
        free(str3);
//////
//    exit(0);
    //free(exit_c);
    return 0;
}//main
