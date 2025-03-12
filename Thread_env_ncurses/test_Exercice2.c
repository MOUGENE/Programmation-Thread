#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ncurses.h>
#include <stdbool.h>
#include <time.h>
#include <gsl/gsl_rng.h>
#include <unistd.h>


// Partie fenêtre ncurses
WINDOW *matriceWindow;
WINDOW *vecteurWindow;
WINDOW *vecteurWinResultat;

// Variable globale
int **mat;
int *mat_stockage;
int *vec;
int *vecteur_resultat;
int N,M;
int elements_calcules = 0; // Nombre d'éléments calculés
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // mutex calcul
pthread_mutex_t mutex_affichage = PTHREAD_MUTEX_INITIALIZER; // mutex affichage
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; // condition pour l'affichage
int valeurs_calculees = 0;
int x = 2; // MAJ après 2 calculs

void ncurses_initialiser() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE); // F2 pour quitter la fenêtre
    refresh();
    curs_set(FALSE);
}

void ncurses_stopper() {
    endwin();
}

void creer_fenetres() {
    int matrice_height = M + 2;
    int vecteur_height = N + 2;
    int vecteur_res_height = M + 2;

    matriceWindow = newwin(matrice_height, 10 * N + 2, 0, 0);
    box(matriceWindow,0,0);
    mvwprintw(matriceWindow,0,(10 * N) / 2 - 4, "MATRICE");
    wrefresh(matriceWindow);

    vecteurWindow = newwin(vecteur_height, 10, 0, 10 * N + 4);
    box(vecteurWindow,0,0);
    mvwprintw(vecteurWindow,0,1, "VECTEUR");
    wrefresh(vecteurWindow);

    vecteurWinResultat = newwin(vecteur_res_height, 10, 0, 10 * N + 16);
    box(vecteurWinResultat,0,0);
    mvwprintw(vecteurWinResultat, 0, 1, "RESULTAT");
    wrefresh(vecteurWinResultat);
}

void detruire_fenetres() {
    delwin(matriceWindow);
    delwin(vecteurWindow);
    delwin(vecteurWinResultat);
}

void afficher_matrice() {
    wclear(matriceWindow);
    box(matriceWindow,0,0);
    mvwprintw(matriceWindow, 0, (10 * N) / 2 - 4, "MATRICE");
    for(int i = 0; i < M; ++i) {
        for(int j = 0; j < N; ++j) {
            mvwprintw(matriceWindow, i + 1, j * 10 + 1, "%d", mat[i][j]);
        }
    }
    wrefresh(matriceWindow);
}

void afficher_vecteur(WINDOW* window, int* vec, size_t n) {
    wclear(window);
    box(window,0,0);
    mvwprintw(vecteurWindow, 0, 1, "VECTEUR");
    for(size_t i = 0; i < n; ++i) {
        mvwprintw(window, i + 1, 1, "%d", vec[i]);
    }
    wrefresh(window);
}

void afficher_vecteur_resultat() {
    pthread_mutex_lock(&mutex_affichage);
    wclear(vecteurWinResultat);
    box(vecteurWinResultat,0,0);
    mvwprintw(vecteurWinResultat, 0, 1, "RESULTAT");
    for(int i = 0; i < M; ++i) {
        if(vecteur_resultat[i] == - 1){
            mvwprintw(vecteurWinResultat, i + 1, 1, " - ");
        }
        else {
            mvwprintw(vecteurWinResultat, i + 1, 1, "%d", vecteur_resultat[i]);
        }
    }
    wrefresh(vecteurWinResultat);
    pthread_mutex_unlock(&mutex_affichage);
}

void* calcul_bloc(void* args) {
    int* res_bloc = malloc(sizeof(int));
    int mult = 0;
    int op = (int)(long)(args);
    for(int j = 0; j < N; ++j) {
        mult += mat[op][j] * vec[j];
    }
    sleep(2);
    pthread_mutex_lock(&mutex);
    vecteur_resultat[op] = mult;
    valeurs_calculees++;
    if(valeurs_calculees % x == 0) {
        pthread_cond_signal(&cond);
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit((void*)res_bloc);
}

void* affichage(void* args) {
    (void)args;
    while(valeurs_calculees < M) {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        for(int i = 0; i < M; ++i) {
            if(vecteur_resultat[i] == -1) {
                mvwprintw(vecteurWinResultat, i + 1, 1, "%d", -1);
            }
            else {
                mvwprintw(vecteurWinResultat, i + 1, 1, "%d", vecteur_resultat[i]);
            }
        }
        wrefresh(vecteurWinResultat);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <M>\n", argv[0]);
        return EXIT_FAILURE;
    }

    M = atoi(argv[1]);
    N = M;

    ncurses_initialiser();
    creer_fenetres();
    mvprintw(LINES - 1, 0, "Taper F2 pour quitter");
    //wrefresh(stdscr);
    refresh();

    gsl_rng_env_setup();
    const gsl_rng_type *T = gsl_rng_default;
    gsl_rng *r = gsl_rng_alloc(T);
    gsl_rng_set(r,time(NULL));

    mat_stockage = malloc(sizeof(int) * M * N);
    mat = malloc(sizeof(int*) * M);
    vec = malloc(sizeof(int) * N);
    vecteur_resultat = malloc(sizeof(int) * M);

    for(int i = 0; i < M; ++i) {
        mat[i] = mat_stockage + N * i;
    }

    for(int i = 0; i < M; ++i) {
        for(int j = 0; j < N; ++j) {
            mat[i][j] = (gsl_rng_get(r) % 9) + 1;
        }
    }

    for(int i = 0; i < N; ++i) {
        vec[i] = (gsl_rng_get(r) % 9) + 1;
    }

    for(int i = 0; i < M; ++i) {
        vecteur_resultat[i] = -1;
    }

    afficher_matrice();
    afficher_vecteur(vecteurWindow, vec, N);

    pthread_t thread_affichage;
    pthread_create(&thread_affichage, NULL, affichage, NULL);

    pthread_t thread[M];
    for(int i = 0; i < M; ++i) {
        pthread_create(&thread[i], NULL, calcul_bloc, (void*)(long)i);
    }

    for(int i = 0; i < M; ++i) {
        int *res;
        pthread_join(thread[i],(void**)&res);
        free(res);
    }

    pthread_cond_signal(&cond);
    pthread_join(thread_affichage, NULL);

    afficher_vecteur_resultat();

    int ch;
    while((ch = getch()) != KEY_F(2)) {

    }

    detruire_fenetres();
    ncurses_stopper();

    free(vec);
    free(mat);
    free(mat_stockage);
    free(vecteur_resultat);
    gsl_rng_free(r);

    return EXIT_SUCCESS;
}
