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

#define THOUSAND 10000

int main(int argc, char ** argv) {

   int first, sec, third;
   if(argc < 3){
      printf("Usage: input 3 integers between -20 and 19\n");
      exit(1);
   }
   else{
      first    = atoi(argv[1]);
      sec      = atoi(argv[2]);
      third    = atoi(argv[3]);
   }
   pid_t parent, child1, child2;
   id_t p1_id, p2_id, p3_id;

   //sleep for one sec
   struct timespec tw = { .tv_sec = 0, .tv_nsec = 10000000 }; 
   int stat;
   int process = 1;
   clock_t p1_start, p1_end;
   clock_t p2_start, p2_end;
   clock_t p3_start, p3_end;

   double p1_time_taken, p2_time_taken, p3_time_taken;
   long stupidmath1=0, stupidmath2=0, stupidmath3=0;

   int pid  = fork();
   switch(pid){
      case -1:
         printf("fork error");
         break;
      case  0:
         nice(sec);
         p2_start    = clock();
         for(long i=0; i<THOUSAND; i++){
            stupidmath1++;
            usleep(1500);
            //nanosleep(&tw, 0);
            // pid = wait(&stat);
         }
         p2_end      = clock();
         p2_time_taken = ((double) (p2_end - p2_start))/CLOCKS_PER_SEC;
         printf("Process %d with %d niceness ran in %f time\n", getpid(), getpriority(PRIO_PROCESS, getpid()), p2_time_taken);
         break;
      default:
         pid = fork();
         switch(pid){
            case -1:
               printf("fork error");
               break;
            case  0:
               nice(third);
               p3_start    = clock();
               for(long i=0; i<THOUSAND; i++){
                  stupidmath2++;
                  usleep(1500);
                  //nanosleep(&tw, 0);
               }
               p3_end      = clock();
               p3_time_taken = ((double) (p3_end - p3_start))/CLOCKS_PER_SEC;
               printf("Process %d with %d niceness ran in %f time\n", getpid(), getpriority(PRIO_PROCESS, getpid()), p3_time_taken);
               break;
            default:
               pid = wait(&stat);
               // nice(first);
               // p1_start    = clock();
               // int stat;
               // for(long i=0; i<THOUSAND; i++){
               //    stupidmath3++;
               //    usleep(15000);
               //    //nanosleep(&tw, 0);
               //    // pid = wait(&stat);
               // }
               // p1_end      = clock();
               // p1_time_taken = ((double) (p1_end - p1_start))/CLOCKS_PER_SEC;
               // printf("Process %d with %d niceness ran in %f time\n", getpid(), getpriority(PRIO_PROCESS, getpid()), p1_time_taken);
               break;
         }
   }
   exit(0);
}