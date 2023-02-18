//
//  main.c
//  lab2nyush
//
//  Created by Leslie Lu on 2/17/23.
//

#include <stdio.h>
#include <unistd.h> //getcwd()
#include <string.h> //strrchr()

//body

int main(
         //int argc, const char * argv[]
) {
    // insert code here...
    
    //Milestone#1
    char cwd[256];
    char * ptr = NULL;

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
    
    //Milestone#2
    
    return 0;
}
