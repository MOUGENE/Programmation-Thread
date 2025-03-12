#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


// Structure qui regroupe les arguments pour la fonction du thread
typedef struct {
    int id_thread; // Id du thread
    int total_thread; // Nombre total de thread
} thread_args_t;

// Fonction qui sera executée par les threads
void* hellow_world(void* arg) {
    thread_args_t* args = (thread_args_t*)arg; // Récupère l'argument passé dans le paramètre de création de thread
    printf("Hellow world ! Du thread %d, sur %d\n",args->id_thread,args->total_thread);
    //printf("%ld\n",pthread_self()); Afin de vérifier l'identifiant des threads
    free(args);
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
    // Créer un tableau de thread k étant passé en paramètre d'appel
    pthread_t threads[k];
    
    // Boucle pour créer les threads
    for(int i = 0; i < k; i++) {
        // On alloue la mémoire pour les arguments des threads
        thread_args_t* args = malloc(sizeof(thread_args_t));
        if(args == NULL) {
            fprintf(stderr,"Erreur d'allocation mémoire.\n");
            exit(EXIT_FAILURE);
        }
        args->id_thread = i + 1;
        args->total_thread = k;
        if(pthread_create(&threads[i],NULL,hellow_world,args) != 0) {
            fprintf(stderr,"Erreur lors de la création du thread.\n");
            free(args);
            exit(EXIT_FAILURE);
        }
    }

    // Boucle qui attend la fin de tout les threads
    for(int i = 0; i < k; i++) {
        if(pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr,"Erreur lors de l'attente du thread\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("Tous les threads sont terminés.\n");
    return (EXIT_SUCCESS);
}