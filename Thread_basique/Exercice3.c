#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

/*
À partir de quelle valeur de n ce programme prend-t-il plusieurs secondes de calcul ?
A partir de la valeur n = 40
*/

unsigned long fib(int n) {
	unsigned long x, y;
	if (n <= 1)
		return n;
	else {
		x = fib(n - 1);
		y = fib(n - 2);
		return x + y;
	}
}

/*
La fonction fib doit être déclarer avant le thread
*/

void* thread_fib(void* arg) {
    int n = *((int*) arg);
    int* res;
    res = malloc(sizeof(int));
    *res = fib(n);
    pthread_exit((void*)res);
}
/*
On cast la valeur de retour en void*
*/

int main(int argc, char *argv[]) {
	unsigned long f,e,resultat;
	int n, diff_temps;
	time_t temps_1, temps_2;
    int* val1; // fib de n-1
    int* val2; // fib de n-2
    void* res; // void* pour le retour de la valeur du thread

    pthread_t threads_fib1;
    pthread_t threads_fib2;

	if (argc != 2) {
		printf("Usage : ./fib_seq n\n");
		printf("n : no du terme de la suite de fibonacci a calculer\n");
		exit(EXIT_SUCCESS);
	}
	n = atoi(argv[1]);
    temps_1 = time(NULL);

    // Création du premier thread
    val1 = malloc(sizeof(int));
    *val1 = n - 1;
    if(pthread_create(&threads_fib1,NULL,thread_fib,val1) != 0){
        fprintf(stderr,"Erreur lors de la création du thread");
        exit(EXIT_FAILURE);
    }

    // Création du deuxième thread
    val2 = malloc(sizeof(int));
    *val2 = n - 2;
    if(pthread_create(&threads_fib2,NULL,thread_fib,val2) != 0) {
        fprintf(stderr,"Erreur lors de la création du thread");
        exit(EXIT_FAILURE);
    }

	// Attente de retour du premier thread
    if(pthread_join(threads_fib1,&res) != 0) {
        fprintf(stderr,"Erreur lors de l'attente du thread");
        exit(EXIT_FAILURE);
    }
    /*
    On récupère la valeur de retour en passant un void* en paramètre qu'on
    re-cast en int après
    */
    f = *((int*)res);
    free(res);
    // Attente de retour du deuxième thread
    if(pthread_join(threads_fib2,&res) != 0) {
        fprintf(stderr,"Erreur lors de l'attente du thread");
        exit(EXIT_FAILURE);
    }
    e = *((int*)res);
    resultat = f + e;

    free(res);

    temps_2 = time(NULL);
	diff_temps = (int) temps_2 - (int) temps_1;
	printf("Valeur du %d-ieme nombre de Fibonacci : %lu\n", n,resultat);
	printf("Temps : %d secondes\n", diff_temps);
    free(val1);
    free(val2);
	
	return EXIT_SUCCESS;
}