#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gsl/gsl_rng.h>
#include <pthread.h>
#include <sys/sysinfo.h>

int* tab; // Tableau qui contient les valeurs
int taille; // Taille du tableau
int nb_threads; // Nombre de threads
int* res_tab; // Tableau qui contient les résultats au fur et a mesure

void* somme(void* arg) {
    size_t id = *(size_t*)arg;
    size_t start = id * (taille / nb_threads);
    size_t end = start + (taille / nb_threads);

    for(size_t i = start; i < end; i++) {
        res_tab[id] += tab[i];
    }

    printf("Thread %ld : somme partielle = %d\n",id,res_tab[id]);
    free(arg);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr,"Erreur, il manque des parametres\n");
        return EXIT_FAILURE;
    }

    taille = atoi(argv[1]);
    nb_threads = atoi(argv[2]);

    if(taille % nb_threads != 0) {
        fprintf(stderr,"Erreur, la taille du tableau doit etre un multiple du nombre de thread.\n");
        return EXIT_FAILURE;
    }

    tab = malloc(taille * sizeof(int));
    res_tab = malloc(taille * sizeof(int));

    pthread_t threads[nb_threads];
    gsl_rng_env_setup();
    const gsl_rng_type *T = gsl_rng_default;
    gsl_rng *r = gsl_rng_alloc(T);

    for(size_t i = 0; i < taille; i++) {
        tab[i] = (gsl_rng_get(r) % 9) + 1;
    }

    for(size_t i = 0; i < taille; i++) {
        res_tab[i] = 0;
    }

    for(size_t i = 0; i < nb_threads; i++) {
        size_t* thread_id = malloc(sizeof(size_t));
        *thread_id = i; // Donne un ID pour chaque thread
        pthread_create(&threads[i],NULL,somme,thread_id);
    }

    int res_somme = 0;
    for(size_t i = 0; i < nb_threads; i++) {
        pthread_join(threads[i],NULL);
        res_somme += res_tab[i];
    }

    printf("Somme globale = %d\n",res_somme);

    free(tab);
    free(res_tab);
    gsl_rng_free(r);
    return EXIT_SUCCESS;
}