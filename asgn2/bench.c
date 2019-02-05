#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h> 
#include<sys/wait.h>

#define FULL 1000000000
#define QRTR 250000000
#define HALF 500000000
#define TQRT 750000000

void simulate(int parentNiceness, int firstChildNiceness, int secondChildNiceness) {
    int C1_avrg = 0;
    int C2_avrg = 0;
    int C3_avrg = 0;
    int C4_avrg = 0;

    int B1_avrg = 0;
    int B2_avrg = 0;
    int B3_avrg = 0;
    int B4_avrg = 0;

    int A1_avrg = 0;
    int A2_avrg = 0;
    int A3_avrg = 0;
    int A4_avrg = 0;

    pid_t parent_pid = getpid();
    pid_t c1;
    pid_t c2;
    char proc;

    time_t quarterTime;
    time_t halfTime;
    time_t threequarterTime;
    time_t lastTime;

    int delta;

    proc = '0';
    nice(parentNiceness); // change parent niceness

    c1 = fork();
    if(c1 == 0){
            proc = '1';
            nice(firstChildNiceness); // change child1 niceness
            c2 = fork();
            if(c2 == 0){
                proc = '2';
                nice(secondChildNiceness); // change child2 niceness
            }
    }

    for(int exec = 0; exec < 5; exec++){
        time_t startTime = time(0);
        for(int i = 0; i <= FULL; i++){ 
            //                                                      (P, c1,  c2)
            // program prints every quarter billion for each process(0, 1, or 2)
            switch(proc){
                case '2':
                    if(i == QRTR){
                        quarterTime = time(0);
                        delta = quarterTime-startTime;
                        C1_avrg += delta;
                        //printf("C%d: %d delta_time: %d\n", i/QRTR, i, delta);
                    }else if(i == HALF){
                        halfTime = time(0);
                        delta = halfTime-startTime;
                        C2_avrg += delta;
                        //printf("C%d: %d delta_time: %d\n", i/QRTR, i, delta);
                    }else if(i == TQRT){
                        threequarterTime = time(0);
                        delta = threequarterTime-startTime;
                        C3_avrg += delta;
                        //printf("C%d: %d delta_time: %d\n", i/QRTR, i, delta);
                    }else if(i == FULL){
                        lastTime = time(0);
                        delta = lastTime-startTime;
                        C4_avrg += delta;
                        //printf("C%d: %d delta_time: %d\n", i/QRTR, i, delta);
                    }
                    break;
                case '1':
                    if(i == QRTR){
                        quarterTime = time(0);
                        delta = quarterTime-startTime;
                        B1_avrg += delta;
                        //printf("B%d: %d delta_time: %d\n", i/QRTR, i, delta);
                    }else if(i == HALF){
                        halfTime = time(0);
                        delta = halfTime-startTime;
                        B2_avrg += delta;
                        //printf("B%d: %d delta_time: %d\n", i/QRTR, i, delta);
                    }else if(i == TQRT){
                        threequarterTime = time(0);
                        delta = threequarterTime-startTime;
                        B3_avrg += delta;
                        //printf("B%d: %d delta_time: %d\n", i/QRTR, i, delta);
                    }else if(i == FULL){
                        lastTime = time(0);
                        delta = threequarterTime-startTime;
                        B4_avrg += delta;
                    }
                    break;
                case '0':
                    if(i == QRTR){
                        quarterTime = time(0);
                        delta = quarterTime-startTime;
                        A1_avrg += delta;
                        //printf("A%d: %d delta_time: %d\n", i/QRTR, i, delta);
                    }else if(i == HALF){
                        halfTime = time(0);
                        delta = halfTime-startTime;
                        A2_avrg += delta;
                        //printf("A%d: %d delta_time: %d\n", i/QRTR, i, delta);
                    }else if(i == TQRT){
                        threequarterTime = time(0);
                        delta = threequarterTime-startTime;
                        A3_avrg += delta;
                        //printf("A%d: %d delta_time: %d\n", i/QRTR, i, delta);
                    }else if(i == FULL){
                        lastTime = time(0);
                        delta = lastTime-startTime;
                        A4_avrg += delta;
                        //printf("A%d: %d delta_time: %d\n", i/QRTR, i, delta);
                    }
                    break;
            }
        }
    }
	float magnitude = 5.0;
        if(c2 == 0){
            printf("Average time for C1: %.6f\n", (float)C1_avrg/magnitude);
            printf("Average time for C2: %.6f\n", (float)C2_avrg/magnitude);
            printf("Average time for C3: %.6f\n", (float)C3_avrg/magnitude);
            printf("Average time for C4: %.6f\n", (float)C4_avrg/magnitude);
		    exit(0);
	    }
        if(c1 == 0){
            wait(NULL);
            printf("Average time for B1: %.6f\n", (float)B1_avrg/magnitude);
            printf("Average time for B2: %.6f\n", (float)B2_avrg/magnitude);
            printf("Average time for B3: %.6f\n", (float)B3_avrg/magnitude);
            printf("Average time for B4: %.6f\n", (float)B4_avrg/magnitude);
            exit(0);
        }
        if(getpid() == parent_pid){
            wait(NULL);
            printf("Average time for A1: %.6f\n", (float)A1_avrg/magnitude);
            printf("Average time for A2: %.6f\n", (float)A2_avrg/magnitude);
            printf("Average time for A3: %.6f\n", (float)A3_avrg/magnitude);
            printf("Average time for A4: %.6f\n", (float)A4_avrg/magnitude);
        }
}

int main(int argc, char ** argv){
    // set1: 1, 1, 1
    // set1: 5, 5, 10
    // set2: 14, 6, 12
    printf("executing..\n");
    int parentNice = atoi(argv[0]);
    int firstNice = atoi(argv[1]);
    int secondNice = atoi(argv[2]);
    simulate(parentNice, firstNice, secondNice);
}

