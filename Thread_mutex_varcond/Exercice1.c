#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <ncurses.h>
#include <stdbool.h>

int nanosleep(const struct timespec *duration, struct timespec * rem);

#define NB_LIGNES_FEN_C1		4			/* Dimensions des fenetres du programme */
#define NB_COL_FEN_C1			24
#define NB_LIGNES_FEN_C2		4
#define NB_COL_FEN_C2			24
#define NB_LIGNES_FEN_C3		4
#define NB_COL_FEN_C3			24
#define NB_LIGNES_FEN_BOUTONS	11
#define NB_COL_FEN_BOUTONS		72

WINDOW *fen_c1;
WINDOW *fen_c2;
WINDOW *fen_c3;
WINDOW *fen_boutons;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Pour le bool
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER; // Pour le compteur
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_cond_t condition2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t condition3 = PTHREAD_COND_INITIALIZER;
bool enigma = false;

void ncurses_initialiser() {
	initscr();								/* Demarre le mode ncurses */
	cbreak();								/* Pour les saisies clavier (desac. mise en buffer) */
	noecho();								/* Desactive l'affichage des caracteres saisis */
	keypad(stdscr, TRUE);					/* Active les touches specifiques */
	refresh();								/* Met a jour l'affichage */
	curs_set(FALSE);						/* Masque le curseur */
	mousemask(BUTTON1_CLICKED, NULL);		/* Active le clic gauche de la souris*/
}

void ncurses_stopper() {
	endwin();
}

void creer_fenetres() {	
	fen_c1 = newwin(NB_LIGNES_FEN_C1, NB_COL_FEN_C1, 0, 0);
	box(fen_c1, 0, 0);
	mvwprintw(fen_c1, 0, NB_COL_FEN_C1 / 2 - 4, "COMPTEUR 1");	
	wrefresh(fen_c1);

	fen_c2 = newwin(NB_LIGNES_FEN_C2, NB_COL_FEN_C2, 0, NB_COL_FEN_C1);
	box(fen_c2, 0, 0);
	mvwprintw(fen_c2, 0, NB_COL_FEN_C2 / 2 - 4, "COMPTEUR 2");	
	wrefresh(fen_c2);

	fen_c3 = newwin(NB_LIGNES_FEN_C3, NB_COL_FEN_C3, 0, NB_COL_FEN_C1 + NB_COL_FEN_C2);
	box(fen_c3, 0, 0);
	mvwprintw(fen_c3, 0, NB_COL_FEN_C3 / 2 - 4, "COMPTEUR 3");	
	wrefresh(fen_c3);

	fen_boutons = newwin(NB_LIGNES_FEN_BOUTONS, NB_COL_FEN_BOUTONS, NB_LIGNES_FEN_C1, 0);
	box(fen_boutons, 0, 0);
	mvwprintw(fen_boutons, 0, NB_COL_FEN_BOUTONS / 2 - 4, "BOUTONS");
	mvwprintw(fen_boutons, 2, NB_COL_FEN_BOUTONS / 2 - 5, "Executer");
	mvwprintw(fen_boutons, 4, NB_COL_FEN_BOUTONS / 2 - 5, "Suspendre");
	mvwprintw(fen_boutons, 6, 1, "  Executer compteur 1     Executer compteur 2     Executer compteur 3");
	mvwprintw(fen_boutons, 8, 1, "  Suspendre compteur 1    Suspendre compteur 2    Suspendre compteur 3");
	wrefresh(fen_boutons);
}

void detruire_fenetres() {
	delwin(fen_c1);
	delwin(fen_c2);
	delwin(fen_c3);
	delwin(fen_boutons);
}

// Ici on créer les 3 routines de threads
void* thread_compt1(void* arg) {
    (void) arg;
    size_t i = 0;
    struct timespec temps_sleep_thread1 = {
		.tv_sec = 0,
		.tv_nsec = 500000000
	};
    while(true) {
        if(pthread_mutex_lock(&mutex) != 0) {
            fprintf(stderr,"Erreur mutex_lock compteur");
            exit(EXIT_FAILURE);
        }
        while(!enigma) {
            pthread_cond_wait(&condition,&mutex);
        }
        if(pthread_mutex_unlock(&mutex) != 0) {
            fprintf(stderr,"Erreur mutex_unlock compteur");
            exit(EXIT_FAILURE);
        }
        if(pthread_mutex_lock(&mutex2) != 0) {
            fprintf(stderr,"Erreur mutex_lock compteur");
            exit(EXIT_FAILURE);
        }
        ++i;
        mvwprintw(fen_c1, 1, 12, "%zu", i);
        wrefresh(fen_c1);
        if(pthread_mutex_unlock(&mutex2) != 0) {
            fprintf(stderr,"Erreur mutex_unlock compteur");
            exit(EXIT_FAILURE);
        }
        nanosleep(&temps_sleep_thread1, NULL);
    }
}

void* thread_compt2(void* arg) {
    (void) arg;
    size_t i = 0;
    struct timespec temps_sleep_thread2 = {
		.tv_sec = 0,
		.tv_nsec = 500000000
	};
    while(true) {
        if(pthread_mutex_lock(&mutex) != 0) {
            fprintf(stderr,"Erreur mutex_lock compteur");
            exit(EXIT_FAILURE);
        }
        while(!enigma) {
            pthread_cond_wait(&condition2,&mutex);
        }
        if(pthread_mutex_unlock(&mutex) != 0) {
            fprintf(stderr,"Erreur mutex_unlock compteur");
            exit(EXIT_FAILURE);
        }
        if(pthread_mutex_lock(&mutex2) != 0) {
            fprintf(stderr,"Erreur mutex_lock compteur");
            exit(EXIT_FAILURE);
        }
        ++i;    
        mvwprintw(fen_c2, 1, 12, "%zu", i);
        wrefresh(fen_c2);
        if(pthread_mutex_unlock(&mutex2) != 0) {
            fprintf(stderr,"Erreur mutex_unlock compteur");
            exit(EXIT_FAILURE);
        }
        nanosleep(&temps_sleep_thread2, NULL);
    }
}

void* thread_compt3(void* arg) {
    (void) arg;
    size_t i = 0;
    struct timespec temps_sleep_thread3 = {
		.tv_sec = 0,
		.tv_nsec = 500000000
	};
    
    while(true) {
        if(pthread_mutex_lock(&mutex) != 0) {
            fprintf(stderr,"Erreur mutex_lock compteur");
            exit(EXIT_FAILURE);
        }
        while(!enigma) {
            pthread_cond_wait(&condition3,&mutex);
        }
        if(pthread_mutex_unlock(&mutex) != 0) {
            fprintf(stderr,"Erreur mutex_unlock compteur");
            exit(EXIT_FAILURE);
        }
        if(pthread_mutex_lock(&mutex2) != 0) {
                fprintf(stderr,"Erreur mutex_lock compteur");
                exit(EXIT_FAILURE);
        }
        ++i;    
        mvwprintw(fen_c3, 1, 12, "%zu", i);
        wrefresh(fen_c3);
        if(pthread_mutex_unlock(&mutex2) != 0) {
            fprintf(stderr,"Erreur mutex_unlock compteur");
            exit(EXIT_FAILURE);
        }
        nanosleep(&temps_sleep_thread3, NULL);
    }
}

int main() {
	MEVENT event;
	int ch;
    pthread_t thread_1;
    pthread_t thread_2;
    pthread_t thread_3;
	struct timespec temps_sleep = {
		.tv_sec = 0,
		.tv_nsec = 500000000
	};
	//size_t i;
	
	ncurses_initialiser();
	creer_fenetres();
	
	mvprintw(LINES - 1, 0, "Tapez F2 pour quitter");
	wrefresh(stdscr);
    //i = 0;

    // Premier thread
    if(pthread_create(&thread_1,NULL,thread_compt1,NULL) != 0) {
        fprintf(stderr,"Erreur lors de la création du thread");
        exit(EXIT_FAILURE);
    }

    // Deuxième thread
    if(pthread_create(&thread_2,NULL,thread_compt2,NULL) != 0) {
        fprintf(stderr,"Erreur lors de la création du thread");
        exit(EXIT_FAILURE);
    }

    // Troisième thread
    if(pthread_create(&thread_3,NULL,thread_compt3,NULL) != 0) {
        fprintf(stderr,"Erreur lors de la création du thread");
        exit(EXIT_FAILURE);
    }

    // Ici pour les 3 routines de threads
	/*while (i < 6) {
		++i;
		mvwprintw(fen_c1, 1, 12, "%zu", i);
		wrefresh(fen_c1);
		nanosleep(&temps_sleep, NULL);
	}
	i = 0;
	while (i < 6) {
		++i;
		mvwprintw(fen_c2, 1, 12, "%zu", i);
		wrefresh(fen_c2);
		nanosleep(&temps_sleep, NULL);
	}
	i = 0;
	while (i < 6) {
		++i;
		mvwprintw(fen_c3, 1, 12, "%zu", i);
		wrefresh(fen_c3);
		nanosleep(&temps_sleep, NULL);
	}
    */

	while((ch = getch()) != KEY_F(2)) {
		switch(ch) {
			case KEY_MOUSE :
				if (getmouse(&event) == OK) {
                    if(pthread_mutex_lock(&mutex) != 0) {
                        fprintf(stderr,"Erreur mutex_lock compteur");
                        exit(EXIT_FAILURE);
                    }
					if (event.y == 6 && event.x >= 31 && event.x <= 38) {
                        enigma = true;
                        pthread_cond_broadcast(&condition);
                        pthread_cond_broadcast(&condition2);
                        pthread_cond_broadcast(&condition3);
						mvwprintw(fen_c1, 2, 8, "Execution");
						mvwprintw(fen_c2, 2, 8, "Execution");
						mvwprintw(fen_c3, 2, 8, "Execution");
						wrefresh(fen_c1);
						wrefresh(fen_c2);
						wrefresh(fen_c3);
					}
					else if (event.y == 8 && event.x >= 31 && event.x <= 39) {
                        enigma = false;
                        pthread_cond_broadcast(&condition);
                        pthread_cond_broadcast(&condition2);
                        pthread_cond_broadcast(&condition3);
						mvwprintw(fen_c1, 2, 8, "  Pause  ");
						mvwprintw(fen_c2, 2, 8, "  Pause  ");
						mvwprintw(fen_c3, 2, 8, "  Pause  ");
						wrefresh(fen_c1);
						wrefresh(fen_c2);
						wrefresh(fen_c3);
					}
					else if (event.y == 10 && event.x >= 3 && event.x <= 21) {
                        enigma = true;
                        pthread_cond_signal(&condition);
                        mvwprintw(fen_c1, 2, 8, "Execution");
						wrefresh(fen_c1);
					}
					else if (event.y == 10 && event.x >= 27 && event.x <= 45) {
                        enigma = true;
                        pthread_cond_signal(&condition2);
						mvwprintw(fen_c2, 2, 8, "Execution");
						wrefresh(fen_c2);
					}
					else if (event.y == 10 && event.x >= 51 && event.x <= 69) {
                        enigma = true;
                        pthread_cond_signal(&condition3);
						mvwprintw(fen_c3, 2, 8, "Execution");
						wrefresh(fen_c3);
					} 
					else if (event.y == 12 && event.x >= 3 && event.x <= 22) {
                        enigma = false;
                        pthread_cond_signal(&condition);
						mvwprintw(fen_c1, 2, 8, "  Pause  ");
						wrefresh(fen_c1);
					}
					else if (event.y == 12 && event.x >= 27 && event.x <= 46) {
                        enigma = false;
                        pthread_cond_signal(&condition2);
						mvwprintw(fen_c2, 2, 8, "  Pause  ");
						wrefresh(fen_c2);
					}
					else if (event.y == 12 && event.x >= 51 && event.x <= 70) {
                        enigma = false;
                        pthread_cond_signal(&condition3);
						mvwprintw(fen_c3, 2, 8, "  Pause  ");
						wrefresh(fen_c3);
					}
				}
            if(pthread_mutex_unlock(&mutex) != 0) {
                fprintf(stderr,"Erreur mutex_unlock compteur");
                exit(EXIT_FAILURE);
            }
			break;
		}
	}

	detruire_fenetres();
	ncurses_stopper();

	return EXIT_SUCCESS;
}

/*
Pas besoin de join dans ce code car le thread main tourne en boucle et quand on appuie sur F2 celui-ci se termine ainsi que les autres threads
*/