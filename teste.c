#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>
#include <semaphore.h>

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"
#define DENTRO 1
#define FORA 0

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;;
pthread_t thread;

void * teste(){
    struct timespec   ts;
    struct timeval    tp;
    ts.tv_sec  = tp.tv_sec;
    ts.tv_nsec = tp.tv_usec * 1000;
    ts.tv_sec += 50 * 1000;
    pthread_mutex_lock(&mutex);
    printf("Começou\n");
    pthread_cond_timedwait(&cond, &mutex, &ts);
    printf("Cabou\n");
    pthread_mutex_unlock(&mutex);
}

int main(){
    printf("A\n");
    pthread_create(&thread, NULL, teste, NULL);
    pthread_join(thread, NULL);
    printf("B\n");
    return 0;
}