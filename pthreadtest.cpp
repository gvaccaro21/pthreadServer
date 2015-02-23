


// thread01.c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 2
#define INITIAL_SEED 1

typedef struct  {
        int x,y;
} operands;

void *worker_thread(void *arg) {
        operands *my_opers = (operands *)arg;
        int sum = my_opers->x + my_opers->y;
        printf("%i ", my_opers->x);
        printf("%i\n", my_opers->y);
//        printf("This is worker_thread #%ld\n", (long)sum);
        pthread_exit(NULL);
}

int main(int argc, const char* argv[]) {
        int do_fixed = 0;

        /* check command line arguments for fixed seed flag */
        for (int i=0; i<argc; i++) {
                if (strstr(argv[i], "fixed")) {
                        do_fixed = 1;
                        break;
                }
        }
        pthread_t my_thread[N];
        operands my_operands[N];

        if (do_fixed) {
                srand(INITIAL_SEED);
        } else {
                srand(time(NULL));
        }


        long id;
        for(id = 2; id <= N; id++) {
                my_operands[id].x = rand() % 100;
                my_operands[id].y = rand() % 100;
                int ret = pthread_create(&my_thread[id], NULL, &worker_thread, (void*)&(my_operands[id]));
                if(ret != 0) {
                        printf("Error: pthread_create() failed\n");
                        exit(EXIT_FAILURE);
                }
        }

        pthread_exit(NULL);
}

