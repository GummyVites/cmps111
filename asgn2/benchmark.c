#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>

int main() {
   time_t t, c, end;
   double time_taken;
   time_t rawtime;
   t = time(&rawtime);
   for (int i=0;i<10;i++) {
      int status;
         pid_t pid = fork();
         if (pid < 0) {
            perror("fork failed");
            continue;
         } 
         if (pid != 0) {   //parent code
            waitpid(-1, &status, 0);
         } else {             //child code
            if(i < 5){
                nice(5);
            }
            else if(i >= 5 && i < 10){
                nice(10);
            }
            else{
                nice(18);
            }
            usleep(15000);
            // printf("[%d] [%d] i=%d\n", getppid(), getpid(), i);
            c = time(&rawtime);
            // time_taken = ((double)c)/CLOCKS_PER_SEC;
            printf("Time@child: %ld\n", c); 
            // execvp(args[0], args);
         }
   }
   end = time(&rawtime);
   // time_taken = ((double)end-t)/CLOCKS_PER_SEC;
   printf("Time2: %ld\n", end); 
   // printf("[%d] [%d] hi\n", getppid(), getpid());
}