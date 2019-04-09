#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMTHRDS 5
#define VECLEN 4
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

int database[4] = {10, 20, 30, 40};
pthread_t threads[NUMTHRDS];
pthread_mutex_t mutex_write, mutex_read;
pthread_cond_t ok_to_write;
pthread_attr_t attr;
int readers;

char* color[6] = {
    "\x1b[31m", "\x1b[32m", "\x1b[33m",
    "\x1b[34m", "\x1b[35m", "\x1b[36m"
};


void print(){
    int i;
    for(i = 0; i < VECLEN; i++)
        printf(" database[%d] = %d", i, database[i]);
    printf("\n");
}

int ler(int idx, int thread){
    pthread_mutex_lock(&mutex_write);
    pthread_mutex_lock(&mutex_read);
    
	readers++;

    pthread_mutex_unlock(&mutex_read);
    pthread_mutex_unlock(&mutex_write);

    int num = database[idx];

    pthread_mutex_lock(&mutex_read);

    readers--;
    if(!readers)
        pthread_cond_signal(&ok_to_write);
    
    pthread_mutex_unlock(&mutex_read);

    return num;
}

void escrever(int idx, int num, int thread){
    pthread_mutex_lock(&mutex_write);
    pthread_mutex_lock(&mutex_read);

    while(readers){
        pthread_cond_wait(&ok_to_write, &mutex_read);
    }

    pthread_mutex_unlock(&mutex_read);

    database[idx] = num;

    pthread_mutex_unlock(&mutex_write);
}


void *solve_incorrect(void *t){

    long idx = (long)t;
    int my_id = (int)idx;
    
    printf("Starting thread %d\n", my_id);
    
    int x = database[my_id];
    int y = database[(my_id + 1)%VECLEN];

    database[my_id] = x+y;

    sleep(1);

    printf("Finishing thread %d\n", my_id);

    pthread_exit(NULL);
}

void *reader(void *t){

    int my_id = (int)(long)t;

    printf("%s Reader %d Started\n" RESET, color[my_id], my_id + 3);

    for(int qnt = 0; qnt < 10; qnt++){
    
        int x = rand()%4;
        int y = rand()%4;

        printf("%s Lendo database[%d] = %d\n" RESET, color[my_id], x, ler(x,my_id));
        sleep(rand()%2 + 1.5);
        printf("%s Lendo database[%d] = %d\n" RESET, color[my_id], y, ler(y,my_id));
        sleep(rand()%4 + 1);

    }

    printf("%s Reader %d Finished\n" RESET, color[my_id], my_id + 3);

    pthread_exit(NULL);

}

void *writer(void *t){

    int my_id = (int)(long)t;

    printf("%s Writer %d Started\n" RESET, color[my_id], my_id);

    for(int qnt = 0; qnt < 10; qnt++){

        int x = rand()%4;
        int value_x = rand()%200;
        
        printf("%s Escrevendo database[%d] o valor %d\n" RESET, color[my_id], x, value_x);
        escrever(x,value_x,my_id);
        sleep(rand()%5 + 0.5);

    }

    printf("%s Writer %d Finished\n" RESET, color[my_id], my_id);

    pthread_exit(NULL);
}

int main(){ 

    print();

    pthread_mutex_init(&mutex_read, NULL);
    pthread_mutex_init(&mutex_write, NULL);
    pthread_cond_init (&ok_to_write, NULL);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(long i = 0; i < NUMTHRDS; i++){
        if(i < 3)
            pthread_create(&threads[(int)i], &attr, writer, (void *)i);
        else
            pthread_create(&threads[(int)i], &attr, reader, (void *)i);
    }

    for (int i = 0; i < NUMTHRDS; i++) 
        pthread_join(threads[i], NULL);
    
    int sum = 0;

    for(int i = 0; i < VECLEN; i++)
        sum += database[i];

    printf("Final database = ");
    print();
    printf ("Final sum = %d \n", sum);

    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&mutex_read);
    pthread_mutex_destroy(&mutex_write);
    pthread_cond_destroy(&ok_to_write);
    pthread_exit (NULL);

    return 0;
}
