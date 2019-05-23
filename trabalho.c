#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
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

 
pthread_mutex_t linf = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_veterano_entrar = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_veterano_sair = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_calouro_sair = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_calouro_entrar = PTHREAD_COND_INITIALIZER;
pthread_cond_t evacuar = PTHREAD_COND_INITIALIZER;
pthread_cond_t fim_evacuar = PTHREAD_COND_INITIALIZER;


int veterano_entrar = 0, enchente = 0, espera = 9;
int vagas = 20;
int estado_veterano[60];
int estado_calouro[60];

void entrar_veterano(int id){

    // Pega o lock do linf
    pthread_mutex_lock(&linf);

    // Setando prioridade
    veterano_entrar ++;

    // Se não tiver vaga ou estiver tendo enchente, aluno não pode entrar no linf
    while(vagas == 0 || enchente == 1){
        // Aluno dorme até poder entrar
        pthread_cond_wait(&cond_veterano_entrar, &linf);
    }

    // Aluno não quer mais entrar, pois já entrou
    veterano_entrar --;
    // Macar que o aluno está dentro do linf
    estado_veterano[id] = DENTRO;
    // COnsumiu a vaga
    vagas--;

    // Swap no tempo para aluno demorar mais para entrar    
    if(vagas == 0){
        espera = 15;
    }

    // Libera o lock do linf
    pthread_mutex_unlock(&linf);

    // Imprime que o aluno entrou
    printf(RED "Veterano %d entrou no linf e ficou com %d vagas\n"RESET, id, vagas);

    // Tempo que o aluno fica no linf
    sleep(rand()%4 + 5);

    // Pega o lock para retirar o aluno do linf
    pthread_mutex_lock(&linf);

    // Pode ser que durante seu estudo, aconteceu uma enchente, então essa função irá dormir para a thread de evacuar liberar o aluno
    while(enchente == 1){
        pthread_cond_wait(&fim_evacuar, &linf);
    } 

    // Se houve uma enchente, então ele já foi evacuado, logo não tem como tirar o aluno que já saiu
    if(estado_veterano[id] == FORA ){ 
        pthread_mutex_unlock(&linf);
        return;
    }

    // Devolve a vaga
    vagas++;

    // Se o linf ficou vazio, reduzir o tempo de espera para os alunos entrarem mais rápido
    if(vagas == 20){
        espera = 9;
    }

    // Coloca seu estado para fora
    estado_veterano[id] = FORA;
    // Imprime que o aluno saiu
    printf(GREEN "Veterano %d saiu do linf e ficou com %d vagas\n"RESET, id, vagas);
    
    // Acorda os outros alunos que estavam esperando para entrar
    pthread_cond_broadcast(&cond_veterano_entrar); 
    pthread_cond_broadcast(&cond_calouro_entrar); 

    // Libera o lock do linf
    pthread_mutex_unlock(&linf);
}

void entrar_calouro(int id){

    // Pega o lock do linf
    pthread_mutex_lock(&linf);

    // Aluno não pode entrar se não tiver vagas, ou algum veterano quer entrar ou está tendo enchente
    while(vagas == 0 || veterano_entrar > 0 || enchente == 1){
        // Dorme até poder entrar
        pthread_cond_wait(&cond_calouro_entrar, &linf);
    }

    // Marca que o calouro entrou
    estado_calouro[id] = DENTRO;
    // Consome a vaga
    vagas--;

    // Mesma ideia para fazer com que os alunos demorem a entrar no linf pois está sem vaga
    if(vagas == 0){
        espera = 15;
    }
    
    // Libera o lock do linf
    pthread_mutex_unlock(&linf);

    // Imprime que o calouro entrou no linf
    printf(YELLOW "\t\t\t\t\t\t\tCalouro %d entrou no linf e ficou com %d vagas\n"RESET, id, vagas);
    // Tempo que o calouro fica no linf
    sleep(rand()%4 + 3);

    // Pega o lock do linf para o aluno sair
    pthread_mutex_lock(&linf);

    // Pode ser que durante seu estudo, aconteceu uma enchente, então essa função irá dormir para a thread de evacuar liberar o aluno
    while(enchente == 1){
        pthread_cond_wait(&fim_evacuar, &linf);
    } 

    // Se houve uma enchente, então ele já foi evacuado, logo não tem como tirar o aluno que já saiu
    if(estado_calouro[id] == 0){ 
        pthread_mutex_unlock(&linf);
        return;
    }

    // Libera a vaga
    vagas++;
    // Seta o estado para fora, pois ele saiu
    estado_calouro[id] = FORA;

    // Faz que o tempo de espera dos alunos seja menor para ocupar as vagas mais rápido
    if(vagas == 20){
        espera = 9;
    }

    // Imprime que o aluno foi embora
    printf(BLUE "\t\t\t\t\t\t\tCalouro %d saiu do linf e ficou com %d vagas\n"RESET, id, vagas);

    // Acorda os outros alunos que queriam entrar
    pthread_cond_broadcast(&cond_veterano_entrar); 
    pthread_cond_broadcast(&cond_calouro_entrar); 

    // Libera o lock do linf
    pthread_mutex_unlock(&linf);
}

void * esvaziar_calouro(void * _id){

    // Pega o id da thread 
    int id = (int)(long)_id;

    // Fica em um laço pois ele evacua o aluno toda vez que tiver uma enchente
    while(true){
        sleep(rand()%3 + 2);
        pthread_mutex_lock(&linf);

        // Thread dorme esperando o sinal para evacuar
        pthread_cond_wait(&evacuar, &linf);

        // Se o calouro não estava no linf, então ele nao precisa evacuar
        if(estado_calouro[id] == FORA){
            pthread_mutex_unlock(&linf);
            continue;    
        }

        // Livera a vaga
        vagas++;
        // Marca que o calouro saiu
        estado_calouro[id] = 0;
        // Imprime que o aluno saiu correndo do linf
        printf(MAGENTA "\t\t\t\t\t\t\tCalouro %d saiu CORRENDO %d vagas\n"RESET, id, vagas);
        // Tempo que o aluno demorou pra sair
        sleep(1);
        // Se liberou todo mundo, avisar que acabou a evacuação
        if(vagas == 20){
            pthread_cond_signal(&fim_evacuar);
        }

        pthread_mutex_unlock(&linf);
    }
}

void * esvaziar_veterano(void * _id){
    int id = (int)(long)_id;
    // Fica em um laço pois ele evacua o aluno toda vez que tiver uma enchente
    while(true){
        sleep(rand()%3 + 2);
        pthread_mutex_lock(&linf);

        // Thread dorme esperando o sinal para evacuar
        pthread_cond_wait(&evacuar, &linf);

        // Se o veterano não estava no linf, então ele nao precisa evacuar
        if(estado_veterano[id] == FORA){
            pthread_mutex_unlock(&linf);
            continue;    
        }
        // Libera a vaga
        vagas++;
        // Marca que o aluno saiu
        estado_veterano[id] = FORA;
        // Imprime que o aluno saiu correndo
        printf(CYAN "Veterano %d saiu CORRENDO %d vagas\n"RESET, id, vagas);
        // Tempo que o aluno demorou pra correr
        sleep(1);
        // Se liberou todo mundo, avisar que acabou a evacuação
        if(vagas == 20){
            pthread_cond_signal(&fim_evacuar);
        }

        pthread_mutex_unlock(&linf);
       
    }
}


void * veteranos(void * _id){
    int id = (int)(long)_id;
    while (true){
        sleep(rand()%espera + 1);
        entrar_veterano(id);
    }
}

void * calouros(void * _id){
    int id = (int)(long)_id;
    while (true){
        sleep(rand()%espera + 1);
        entrar_calouro(id);
    }
}

void * alagar(){
    while(true){
        // Período que demora para acontecer uma enchente
        sleep(rand()%10 + 10);
        // Locka o linf
        pthread_mutex_lock(&linf);

        // Marca que está tendo uma enchente
        enchente = 1;

        // Imprime na tela
        printf(RED"\t\t\t\t _______ .__   __.   ______  __    __   _______ .__   __. .___________._______  __   __   __                         __\n");
        printf("\t\t\t\t|   ____||  \\ |  |  /      ||  |  |  | |   ____||  \\ |  | |           |   ____||  | |  | |  |                       /' )\n"); 
        printf("\t\t\t\t|  |__   |   \\|  | |  ,----'|  |__|  | |  |__   |   \\|  | `---|  |----|  |__   |  | |  | |  |                      /'   (                          ,\n"); 
        printf("\t\t\t\t|   __|  |  . `  | |  |     |   __   | |   __|  |  . `  |     |  |    |   __|  |  | |  | |  |                   __/'     )                        .' `;\n"); 
        printf("\t\t\t\t|  |____ |  |\\   | |  `----.|  |  |  | |  |____ |  |\\   |     |  |    |  |____ |__| |__| |__|           _.-~~~~'          ``---..__             .'   ;\n"); 
        printf("\t\t\t\t|_______||__| \\__|  \\______||__|  |__| |_______||__| \\__|     |__|    |_______|(__) (__) (__)      _.--'   b)                      ``--...____.'   .'\n"); 
        printf("\t\t\t\t                                                                                                  (     _.      )).      `-._                     <\n");
        printf("\t\t\t\t                                                                                                   `vvvvvvv-)-.....___.-     `-.         __...--'-.'.\n");
        printf("\t\t\t\t                                                                                                     `^^^^^'-------.....`-.___.'----... .'         `.;\n");
        printf("\t\t\t\t                                                                                                                                       `-`           `   \n"RESET);                                                                                          
        
        // Acorda as threads para evacuar os alunos
        pthread_cond_broadcast(&evacuar);

        // Volta a dormir até terminar de evacuar
        pthread_cond_wait(&fim_evacuar, &linf);
        
        // Marca que acabou a enchente
        enchente = 0;

        // Imprime que todo mundo foi evacuado
        printf(GREEN"\t\t\t\t __       __  .__   __.  _______     ___________    ____  ___       ______  __    __       ___       _______   ______\n"); 
        printf("\t\t\t\t|  |     |  | |  \\ |  | |   ____|   |   ____\\   \\  /   / /   \\     /      ||  |  |  |     /   \\     |       \\ /  __  \\  \n");
        printf("\t\t\t\t|  |     |  | |   \\|  | |  |__      |  |__   \\   \\/   / /  ^  \\   |  ,----'|  |  |  |    /  ^  \\    |  .--.  |  |  |  | \n");
        printf("\t\t\t\t|  |     |  | |  . `  | |   __|     |   __|   \\      / /  /_\\  \\  |  |     |  |  |  |   /  /_\\  \\   |  |  |  |  |  |  | \n");
        printf("\t\t\t\t|  `----.|  | |  |\\   | |  |        |  |____   \\    / /  _____  \\ |  `----.|  `--'  |  /  _____  \\  |  '--'  |  `--'  | \n");
        printf("\t\t\t\t|_______||__| |__| \\__| |__|        |_______|   \\__/ /__/     \\__\\ \\______| \\______/  /__/     \\__\\ |_______/ \\______/  \n"RESET);

        // Tempo que os alunos ficam com medo para voltar no linf
        sleep(rand()%3 + 3);

        // Acorda as threads para voltarem a entrar no linf
        pthread_cond_broadcast(&cond_veterano_entrar);
        pthread_cond_broadcast(&cond_calouro_entrar);

        pthread_mutex_unlock(&linf);
    }

}


int main(){ 

    pthread_t threads[63];

    memset(estado_veterano, 0, sizeof(estado_veterano));
    memset(estado_calouro, 0, sizeof(estado_calouro));

    for(int i = 0; i < 15; i++)
        pthread_create(&threads[i], NULL, veteranos, (void * )(long)i);
    for(int i = 15; i < 30; i++)
        pthread_create(&threads[i], NULL, esvaziar_veterano, (void * )(long)i-15);
    for(int i = 30; i < 45; i++)
        pthread_create(&threads[i], NULL, calouros, (void * )(long)i);
    for(int i = 45; i < 60; i++)
        pthread_create(&threads[i], NULL, esvaziar_calouro, (void * )(long)i-15);
    
    pthread_create(&threads[60], NULL, alagar, NULL);
    pthread_create(&threads[61], NULL, esvaziar_calouro, NULL);
    pthread_create(&threads[62], NULL, esvaziar_veterano, NULL);

    for(int i = 0; i < 63; i++)
        pthread_join(threads[i], NULL); 

    return 0;
}