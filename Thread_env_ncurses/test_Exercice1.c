#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gsl/gsl_rng.h>
#include <sys/sysinfo.h>

int main(int argc, char* argv[]) {
    if(argc != 2) {
        fprintf(stderr,"Erreur, il manque des parametres\n");
        return EXIT_FAILURE;
    }

    int taille = atoi(argv[1]);
    int* tab = malloc(taille * sizeof(int));
    if(tab == NULL) {
        fprintf(stderr,"Erreur d'allocation mémoire\n");
        return EXIT_FAILURE;
    }

    gsl_rng_env_setup();
    const gsl_rng_type *T = gsl_rng_default;
    gsl_rng *r = gsl_rng_alloc(T);

    for(size_t i = 0; i < taille; i++) {
        tab[i] = (gsl_rng_get(r) % 9) + 1;
    }

    int somme_globale = 0;
    for(size_t i = 0; i < taille; i++) {
        somme_globale += tab[i];
    }

    printf("Somme globale = %d\n",somme_globale);

    free(tab);
    gsl_rng_free(r);
    return EXIT_SUCCESS;
}