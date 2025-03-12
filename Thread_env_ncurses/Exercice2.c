#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <gsl/gsl_rng.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <ncurses.h>

#define M	6
#define N	6

int **mat;
int *mat_stockage;
int *vec;
int *vec_resultat;

void afficher_matrice(int **mat, size_t m, size_t n) {
	printf("Matrice******\n");
	for (size_t i = 0; i < m; ++i){
		for (size_t j = 0; j < n; ++j)
			printf("%d ", mat[i][j]);
		printf("\n");
	}
	printf("**************\n");
}

void afficher_vecteur(int *vec, size_t n) {
	printf("Vecteur******\n");
	for (size_t i = 0; i < n; ++i)
		printf("%d ", vec[i]);
	printf("\n");
	printf("*************\n");
}

int main() {
	gsl_rng_env_setup();
	const gsl_rng_type *T = gsl_rng_default;
	gsl_rng *r = gsl_rng_alloc (T);
	gsl_rng_set(r, time(NULL));

	mat_stockage = malloc(sizeof(int[M * N]));
	mat = malloc(sizeof(int *[M]));
	vec = malloc(sizeof(int[N]));
	for (size_t i = 0; i < M; ++i)
		mat[i] = mat_stockage + N * i;
	for (size_t i = 0; i < M; ++i)
		for (size_t j = 0; j < N; ++j)
			mat[i][j] = (gsl_rng_get(r) % 5) + 1;

	for (size_t i = 0; i < N; ++i)
		vec[i] = (gsl_rng_get(r) % 5) + 1;

	vec_resultat = malloc(sizeof(int[M]));
	for (size_t i = 0; i < M; ++i)
		vec_resultat[i] = -1;	

	afficher_matrice(mat, M, N);
	afficher_vecteur(vec, N);

	for (size_t i = 0; i < M; ++i)	{
		vec_resultat[i] = 0;
		for (size_t j = 0; j < N; ++j)
			vec_resultat[i] = vec_resultat[i] + mat[i][j] * vec[j];
	}

	afficher_vecteur(vec_resultat, M);

	free(vec);
	free(vec_resultat);
	free(mat_stockage);
	free(mat);
	gsl_rng_free (r);

	return EXIT_SUCCESS;
}