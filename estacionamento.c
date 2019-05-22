/*
    Estudo Dirigido 10- Estacionamento Prova 1
    Thiago Veras Machado
    16/0146682
*/



#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h> 

#define NUMTHRDS 2
#define VECLEN 5
#define N 3
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

pthread_mutex_t estacionamento = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t professor = PTHREAD_COND_INITIALIZER;
pthread_cond_t funcionario = PTHREAD_COND_INITIALIZER;
pthread_cond_t aluno = PTHREAD_COND_INITIALIZER;
int vagas = 30;
int professor_quer = 0;
int funcionario_quer = 0;

void * professores(void * _id){

    int id = (int)(long) _id;

    while(true){
        sleep(rand()%6 +2);
        pthread_mutex_lock(&estacionamento);
        professor_quer ++;
        while(vagas == 0){
            pthread_cond_wait(&professor, &estacionamento);
        }
        professor_quer --;
        vagas--;
        pthread_mutex_unlock(&estacionamento);
        printf(RED"Professor %d estacionou e ficou com %d vagas\n"RESET, id, vagas);
        sleep(15);
        pthread_mutex_lock(&estacionamento);
        vagas++;
        printf(BLUE"Professor %d saiu do estacionamento e ficou com %d vagas\n"RESET, id, vagas);
        pthread_cond_broadcast(&professor); 
        pthread_cond_broadcast(&funcionario); 
        pthread_cond_broadcast(&aluno); 
        pthread_mutex_unlock(&estacionamento);

    }
}

void * funcionarios(void * _id){

    int id = (int)(long) _id;

    while(true){
        sleep(rand()%6 +2);
        pthread_mutex_lock(&estacionamento);
        funcionario_quer++;
        while(vagas == 0){
            pthread_cond_wait(&funcionario, &estacionamento);
            while(professor_quer){
                pthread_cond_wait(&funcionario, &estacionamento);
            }
        }
        funcionario_quer--;
        vagas--;
        pthread_mutex_unlock(&estacionamento);
        printf(YELLOW"Funcionario %d estacionou e ficou com %d vagas\n"RESET, id, vagas);
        sleep(12);
        pthread_mutex_lock(&estacionamento);
        vagas++;
        printf(MAGENTA"Funcionario %d saiu do estacionamento e ficou com %d vagas\n"RESET, id, vagas);
        pthread_cond_broadcast(&professor); 
        pthread_cond_broadcast(&funcionario); 
        pthread_cond_broadcast(&aluno); 
        pthread_mutex_unlock(&estacionamento);

    }
}

void * alunos(void * _id){

    int id = (int)(long) _id;

    while(true){
        sleep(rand()%6 +2);
        pthread_mutex_lock(&estacionamento);
        
        while(vagas == 0){
            pthread_cond_wait(&aluno, &estacionamento);
            while(professor_quer || funcionario_quer){
                pthread_cond_wait(&aluno, &estacionamento);
            }
        }
        vagas--;
        
        pthread_mutex_unlock(&estacionamento);
        printf(GREEN"Aluno %d estacionou e ficou com %d vagas\n"RESET, id, vagas);
        sleep(10);
        pthread_mutex_lock(&estacionamento);
        vagas++;
        printf(CYAN"Aluno %d saiu do estacionamento e ficou com %d vagas\n"RESET, id, vagas);
        pthread_cond_broadcast(&professor); 
        pthread_cond_broadcast(&funcionario); 
        pthread_cond_broadcast(&aluno); 
        pthread_mutex_unlock(&estacionamento);

    }
}



int main(){ 
    pthread_t threads[60];

    for(int i = 0; i < 20; i++)
        pthread_create(&threads[i], NULL, professores, (void * )(long)i);
    for(int i = 21; i < 40; i++)
        pthread_create(&threads[i], NULL, funcionarios, (void * )(long)i);
    for(int i = 41; i < 60; i++)
        pthread_create(&threads[i], NULL, alunos, (void * )(long)i);

    for(int i = 0; i < 60; i++)
        pthread_join(threads[i], NULL); 

    return 0;
}