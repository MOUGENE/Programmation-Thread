#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define SPIN 4000000

int compteur = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* thread_compteur(void* arg) {
    int iteration = *((int*)arg);
    for(int i = 0; i < iteration; i++) {
        if(pthread_mutex_lock(&mutex) != 0) {
            fprintf(stderr,"Erreur mutex_lock compteur");
            exit(EXIT_FAILURE);
        }
        compteur++;
        if(pthread_mutex_unlock(&mutex) != 0) {
            fprintf(stderr,"Erreur mutex_unlock compteur");
            exit(EXIT_FAILURE);
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    if(argc != 2) {
        fprintf(stderr, "Il manque un argument.\n");
        exit(EXIT_FAILURE);
    }

    int k = atoi(argv[1]);
    if(k <= 0) {
        fprintf(stderr, "La création de thread doit être positive.\n");
        exit(EXIT_FAILURE);
    }
    int diff_temps;
    time_t temps_1, temps_2;

    pthread_t threads[k];
    int iteration = SPIN / k;

    temps_1 = time(NULL);

    for(int i = 0; i < k; i++) {
        if(pthread_create(&threads[i], NULL,thread_compteur,&iteration) != 0) {
            fprintf(stderr,"Erreur lors de la création du thread.\n");
            exit(EXIT_FAILURE);
        }
    }

    temps_2 = time(NULL);
    diff_temps = (int) temps_2 - (int) temps_1;

    for(int i = 0; i < k; i++) {
        if(pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr,"Erreur lors de l'attente du thread.\n");
            exit(EXIT_FAILURE);
        }
    }

    /*for(size_t i = 0; i < SPIN; ++i) {
        if(pthread_mutex_lock(&mutex) != 0) {
            fprintf(stderr,"Erreur mutex_lock compteur.\n");
            exit(EXIT_FAILURE);
        }
        compteur++;
        if(pthread_mutex_unlock(&mutex) != 0) {
            fprintf(stderr,"Erreur mutex_unlock compteur.\n");
            exit(EXIT_FAILURE);
        }
    }*/

    
    printf("Valeur finale du compteur : %d\n",compteur);
    printf("Temps : %d secondes\n",diff_temps);

    exit(EXIT_SUCCESS);
}