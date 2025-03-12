#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <gsl/gsl_rng.h>
#include <pthread.h>
#include <ncurses.h>
#include <unistd.h> 


WINDOW *matriceWindow;
WINDOW *vecteurWindow;
WINDOW *vec_resWindow;

int **mat;
int *mat_stockage;
int *vec;
int *vec_resultat;
int N, M;
int elements_calcules = 0;  // Nombre d'éléments calculés
pthread_mutex_t mutex_affichage = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_vecteur = PTHREAD_MUTEX_INITIALIZER;
int valeurs_calculees = 0;
int x = 2; // Par exemple, mettre à jour après chaque 2 calculs
pthread_cond_t cond_affichage = PTHREAD_COND_INITIALIZER;

void ncurses_initialiser() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);  // Activer les touches spéciales comme F2
    refresh();
    curs_set(FALSE);
}

void ncurses_stopper() {
    endwin();
}

void creer_fenetres() {
    int matrice_height = M + 2;
    int vecteur_height = N + 2;
    int vec_res_height = M + 2;

    matriceWindow = newwin(matrice_height, 10 * N + 2, 0, 0);
    box(matriceWindow, 0, 0);
    mvwprintw(matriceWindow, 0, (10 * N) / 2 - 4, "MATRICE");
    wrefresh(matriceWindow);

    vecteurWindow = newwin(vecteur_height, 10, 0, 10 * N + 4);
    box(vecteurWindow, 0, 0);
    mvwprintw(vecteurWindow, 0, 1, "VECTEUR");
    wrefresh(vecteurWindow);

    vec_resWindow = newwin(vec_res_height, 10, 0, 10 * N + 16);
    box(vec_resWindow, 0, 0);
    mvwprintw(vec_resWindow, 0, 1, "VEC RES");
    wrefresh(vec_resWindow);
}

void detruire_fenetres() {
    delwin(matriceWindow);
    delwin(vecteurWindow);
    delwin(vec_resWindow);
}

void afficher_matrice_ncurses() {
    wclear(matriceWindow);
    box(matriceWindow, 0, 0);
    //Pour adapter la taille de la fenetre on fais 10*N avec N le nombre d'éléments dans la matrices MN
    mvwprintw(matriceWindow, 0, (10 * N) / 2 - 4, "MATRICE");

    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            //affichage des valeurs dans le tableau 
            mvwprintw(matriceWindow, i + 1, j * 10 + 1, "%d", mat[i][j]);
        }
    }
    wrefresh(matriceWindow);
}

void afficher_vecteur_ncurses(WINDOW *window, int *vec, size_t n) {
    wclear(window);
    box(window, 0, 0);
    mvwprintw(window, 0, 1, "VECTEUR");

    for (size_t i = 0; i < n; ++i) {
        mvwprintw(window, i + 1, 1, "%d", vec[i]);
    }
    wrefresh(window);
}

void afficher_vec_res_ncurses() {
    pthread_mutex_lock(&mutex_affichage);

    wclear(vec_resWindow);
    box(vec_resWindow, 0, 0);
    mvwprintw(vec_resWindow, 0, 1, "VEC RES");

    for (int i = 0; i < M; ++i) {
        if (vec_resultat[i] == -1)
            mvwprintw(vec_resWindow, i + 1, 1, " - ");
        else
            mvwprintw(vec_resWindow, i + 1, 1, "%d", vec_resultat[i]);
    }
    wrefresh(vec_resWindow);

    pthread_mutex_unlock(&mutex_affichage);
}

void* calcul_bloc(void* args){
    int *resultat_bloc = malloc(sizeof(int));   
    int mult = 0;
    int op = (int)(long)(args);

    for(int j=0; j<N; ++j){
        mult += mat[op][j] * vec[j];
    }

    sleep(2);
    pthread_mutex_lock(&mutex_vecteur);
    vec_resultat[op] = mult;
    valeurs_calculees++;

    if (valeurs_calculees % x == 0) {
        pthread_cond_signal(&cond_affichage);
    }

    pthread_mutex_unlock(&mutex_vecteur);
    pthread_exit((void*)resultat_bloc);
}


void* thread_affichage(void* args) {
    (void)args;
    while (valeurs_calculees < M) {
        pthread_mutex_lock(&mutex_vecteur);
        pthread_cond_wait(&cond_affichage, &mutex_vecteur);

        // Mettre à jour la fenêtre vec_resWindow avec les valeurs calculées
        for (int i = 0; i < M; ++i) {
            if (vec_resultat[i] == -1) {
                mvwprintw(vec_resWindow, i + 1, 1, "%d", -1);
            } else {
                mvwprintw(vec_resWindow, i + 1, 1, "%d", vec_resultat[i]);
            }
        }
        wrefresh(vec_resWindow);
        pthread_mutex_unlock(&mutex_vecteur);
        sleep(1);
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <M>\n", argv[0]);
        return EXIT_FAILURE;
    }

    M = atoi(argv[1]);
    N = M;

    ncurses_initialiser();
    creer_fenetres();
    mvprintw(LINES - 1, 0, "Tapez F2 pour quitter");
    refresh();

    gsl_rng_env_setup();
    const gsl_rng_type *T = gsl_rng_default;
    gsl_rng *r = gsl_rng_alloc(T);
    gsl_rng_set(r, time(NULL));

    mat_stockage = malloc(sizeof(int) * M * N);
    mat = malloc(sizeof(int*) * M);
    vec = malloc(sizeof(int) * N);
    vec_resultat = malloc(sizeof(int) * M);

    for (int i = 0; i < M; ++i)
        mat[i] = mat_stockage + N * i;

    for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j)
            mat[i][j] = (gsl_rng_get(r) % 5) + 1;

    for (int i = 0; i < N; ++i)
        vec[i] = (gsl_rng_get(r) % 5) + 1;

    for (int i = 0; i < M; ++i)
        vec_resultat[i] = -1;

    afficher_matrice_ncurses();
    afficher_vecteur_ncurses(vecteurWindow, vec, N);

    pthread_t affichage_thread;
    pthread_create(&affichage_thread, NULL, thread_affichage, NULL);

    pthread_t TabThread[M];
    for (int i = 0; i < M; ++i) {
        pthread_create(&TabThread[i], NULL, calcul_bloc, (void*)(long)i);
    }

    for (int i = 0; i < M; ++i) {
        int *result;
        pthread_join(TabThread[i], (void**)&result);
        free(result);
    }

    pthread_cond_signal(&cond_affichage); // Pour terminer le thread d'affichage
    pthread_join(affichage_thread, NULL);

    afficher_vec_res_ncurses();

    int ch;
    while ((ch = getch()) != KEY_F(2)) {
        // Attendre F2 pour quitter
    }

    detruire_fenetres();
    ncurses_stopper();

    free(vec);
    free(vec_resultat);
    free(mat_stockage);
    free(mat);
    gsl_rng_free(r);

    return EXIT_SUCCESS;
}