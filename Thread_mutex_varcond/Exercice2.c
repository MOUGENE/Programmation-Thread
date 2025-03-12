#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <ncurses.h>

#define NB_GUICHETS 3
#define CAPACITE 15
#define INCREMENT 5

typedef struct {
    int nombre;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} SalleAttente;

SalleAttente salle_attente = {0,PTHREAD_MUTEX_INITIALIZER,PTHREAD_COND_INITIALIZER};
int nanosleep(const struct timespec *duration, struct timespec * rem);

#define NB_LIGNES_FEN_G1		4			/* Dimensions des fenetres du programme */
#define NB_COL_FEN_G1			24
#define NB_LIGNES_FEN_G2		4
#define NB_COL_FEN_G2			24
#define NB_LIGNES_FEN_G3		4
#define NB_COL_FEN_G3			24
#define NB_LIGNES_FEN_SALLE		4
#define NB_COL_FEN_SALLE		30
#define NB_LIGNES_FEN_BOUTONS	4
#define NB_COL_FEN_BOUTONS		30

WINDOW *fen_g1;
WINDOW *fen_g2;
WINDOW *fen_g3;
WINDOW *fen_salle;
WINDOW *fen_boutons;

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
	fen_g1 = newwin(NB_LIGNES_FEN_G1, NB_COL_FEN_G1, 0, 0);
	box(fen_g1, 0, 0);
	mvwprintw(fen_g1, 0, NB_COL_FEN_G1 / 2 - 4, "GUICHET 1");	
	wrefresh(fen_g1);

	fen_g2 = newwin(NB_LIGNES_FEN_G2, NB_COL_FEN_G2, 0, NB_COL_FEN_G1);
	box(fen_g2, 0, 0);
	mvwprintw(fen_g2, 0, NB_COL_FEN_G2 / 2 - 4, "GUICHET 2");	
	wrefresh(fen_g2);

	fen_g3 = newwin(NB_LIGNES_FEN_G3, NB_COL_FEN_G3, 0, NB_COL_FEN_G1 + NB_COL_FEN_G2);
	box(fen_g3, 0, 0);
	mvwprintw(fen_g3, 0, NB_COL_FEN_G3 / 2 - 4, "GUICHET 3");	
	wrefresh(fen_g3);

	fen_salle = newwin(NB_LIGNES_FEN_SALLE, NB_COL_FEN_SALLE, NB_LIGNES_FEN_G1,
					  (NB_COL_FEN_G1 + NB_COL_FEN_G2 + NB_COL_FEN_G3) / 2 - (NB_COL_FEN_SALLE / 2));
	box(fen_salle, 0, 0);
	mvwprintw(fen_salle, 0, NB_COL_FEN_SALLE / 2 - 8, "SALLE D'ATTENTE");	
	wrefresh(fen_salle);

	fen_boutons = newwin(NB_LIGNES_FEN_BOUTONS, NB_COL_FEN_BOUTONS, NB_LIGNES_FEN_G1 + NB_LIGNES_FEN_SALLE,
						(NB_COL_FEN_G1 + NB_COL_FEN_G2 + NB_COL_FEN_G3) / 2 - (NB_COL_FEN_BOUTONS / 2));
	box(fen_boutons, 0, 0);
	mvwprintw(fen_boutons, 0, NB_COL_FEN_BOUTONS / 2 - 4, "BOUTONS");
	mvwprintw(fen_boutons, 2, NB_COL_FEN_BOUTONS / 2 - 13, "Faire entrer des etrangers");
	wrefresh(fen_boutons);
}

void detruire_fenetres() {
	delwin(fen_g1);
	delwin(fen_g2);
	delwin(fen_g3);
	delwin(fen_salle);
	delwin(fen_boutons);
}

void* guichet_thread(void* arg) {
    int guichet_id = *(int*) arg;
    WINDOW* fen_guichet = (guichet_id == 0) ? fen_g1 : (guichet_id == 1) ? fen_g2 : fen_g3;
    while(1) {
        pthread_mutex_lock(&salle_attente.mutex);

        // Attendre s'il n'y a personne
        while(salle_attente.nombre == 0) {
            pthread_cond_wait(&salle_attente.cond, &salle_attente.mutex);
        }

        // Traitement d'une personne
        salle_attente.nombre--;
        mvwprintw(fen_guichet, 1, 1, "Etranger en traitement");
        wrefresh(fen_guichet);
        mvwprintw(fen_salle, 1, 14, "%d ", salle_attente.nombre);
        wrefresh(fen_salle);

        pthread_mutex_unlock(&salle_attente.mutex);

        nanosleep((const struct timespec[]){{0, 500000000L}}, NULL);
        
        mvwprintw(fen_guichet, 1, 1, "                       ");
        wrefresh(fen_guichet);
    }
    pthread_exit(NULL);
}

void ajouter_personnes() {
    pthread_mutex_lock(&salle_attente.mutex);

    if (salle_attente.nombre < CAPACITE || salle_attente.nombre + INCREMENT <= CAPACITE) {
        salle_attente.nombre += INCREMENT;
        if (salle_attente.nombre > CAPACITE) salle_attente.nombre = CAPACITE;
        mvwprintw(fen_salle, 1, 14, "%d ", salle_attente.nombre);
        wrefresh(fen_salle);
        
        // Signal aux threads de guichets
        pthread_cond_broadcast(&salle_attente.cond);
    }

    pthread_mutex_unlock(&salle_attente.mutex);
}


int main() {
	MEVENT event;
	int ch;
    pthread_t threads[NB_GUICHETS];
    int guichet_ids[NB_GUICHETS] = {0,1,2};
	//size_t i;
	
	ncurses_initialiser();
	creer_fenetres();

    // On créer les threads des guichets
    for(int i = 0; i < NB_GUICHETS; i++) {
        pthread_create(&threads[i],NULL,guichet_thread, &guichet_ids[i]);
    }
	
	mvprintw(LINES - 1, 0, "Tapez F2 pour quitter");
	wrefresh(stdscr);

	//i = 0;
	//mvwprintw(fen_salle, 1, 14, "%zu ", i);
	//wrefresh(fen_salle);
	
	while((ch = getch()) != KEY_F(2)) {
		switch(ch) {
			case KEY_MOUSE :
				if (getmouse(&event) == OK) {
					if (event.y == 10 && event.x >= 23 && event.x <= 48) {
						//i+= 5;
						//mvwprintw(fen_salle, 1, 14, "%zu ", i);
						//wrefresh(fen_salle);
                        ajouter_personnes();
					}
				}
			break;
		}
	}

	detruire_fenetres();
	ncurses_stopper();

	return EXIT_SUCCESS;
}