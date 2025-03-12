#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ncurses.h>
#include <time.h>
#include "file_bloquante.h"
#include "requete.h"

int nanosleep(const struct timespec *duration, struct timespec * rem);

#define NB_LIGNES_SIM			22			/* Dimensions des fenetres du programme */
#define NB_COL_SIM				62
#define NB_LIGNES_MSG			16
#define NB_COL_MSG				50
#define NB_LIGNES_OUTILS		6
#define NB_COL_OUTILS			50

#define VIDE					0				/* Identifiants des elements pouvant etre */
#define OBSTACLE				1				/* places sur la grille de simulation */
#define FOURMI					2

typedef struct {								/* Description d'une case sur la grille de simulation */
	int element;								/* Ce qui est present sur la case : VIDE/OBSTACLE/FOURMI */
	pthread_t *fourmi;							/* Identifiant du thread de la fourmi presente sur la case */
	pthread_mutex_t mutex;						/* Protection de la case */
} case_grille;

file_bloquante f;
pthread_mutex_t mutex_affichage = PTHREAD_MUTEX_INITIALIZER;
int item_actif = VIDE;
int simulation_terminee = 0;

WINDOW *fen_sim;
WINDOW *fen_box_msg;
WINDOW *fen_msg;
WINDOW *fen_outils;

case_grille grille[NB_LIGNES_SIM][NB_COL_SIM];	/* Grille de simulation */
int nb_fourmis = 0;								/* Nombre de fourmis de la simulation*/

void ncurses_init() {
	initscr();									/* Demarre le mode ncurses */
	cbreak();									/* Pour les saisies clavier (desac. mise en buffer) */
	noecho();									/* Desactive l'affichage des caracteres saisis */
	keypad(stdscr, TRUE);						/* Active les touches specifiques */
	refresh();									/* Met a jour l'affichage */
	curs_set(FALSE);							/* Masque le curseur */
	mousemask(BUTTON1_CLICKED, NULL);			/* Active le clic gauche de la souris*/
}

void ncurses_stop() {
	endwin();
}

void creer_fenetres() {	
	fen_sim = newwin(NB_LIGNES_SIM, NB_COL_SIM, 0, 0);
	box(fen_sim, 0, 0);
	mvwprintw(fen_sim, 0, NB_COL_SIM / 2 - 5, "SIMULATION");	
	wrefresh(fen_sim);

	fen_box_msg = newwin(NB_LIGNES_MSG, NB_COL_MSG, 0, NB_COL_SIM);
	box(fen_box_msg, 0, 0);
	mvwprintw(fen_box_msg, 0, NB_COL_MSG / 2 - 4, "MESSAGES");

	fen_msg = newwin(NB_LIGNES_MSG - 2, NB_COL_MSG - 2, 1, NB_COL_SIM + 1);
	scrollok(fen_msg, TRUE);

	wrefresh(fen_box_msg);
	wrefresh(fen_msg);

	fen_outils = newwin(NB_LIGNES_OUTILS, NB_COL_OUTILS, NB_LIGNES_MSG, NB_COL_SIM);
	box(fen_outils, 0, 0);
	mvwprintw(fen_outils, 0, NB_COL_OUTILS / 2 - 3, "OUTILS");
	mvwprintw(fen_outils, 1, 3, "# Ajout obstacle");
	mvwprintw(fen_outils, 2, 3, "@ Ajout fourmi");
	mvwprintw(fen_outils, 3, 3, "  Destruction");
	mvwprintw(fen_outils, 1, 1, "X");
	wrefresh(fen_outils);
}

void detruire_fenetres() {
	delwin(fen_sim);
	delwin(fen_msg);
	delwin(fen_box_msg);
	delwin(fen_outils);
}

void simulation_init() {	
	// Initialise les cases de la grille de simulation
	// Les premieres/dernieres lignes/colonnes de la grille ne sont pas utilises
	// de sorte a faire correspondre les coordonnees (y, x) des evenements souris
	// avec les cases grille[y][x]

	for (size_t i = 1; i < NB_LIGNES_SIM - 1; ++i) {
		for (size_t j = 1; j < NB_COL_SIM - 1; ++j) {
			grille[i][j].element = VIDE;
			grille[i][j].fourmi = NULL;
			pthread_mutex_init(&grille[i][j].mutex, NULL);
		}
	}
}

void cri_de_mort(void *arg) {
	(void) arg;
	requete *r = creer_requete(REQUETE_CRI_MORT, creer_coord(0, 0), creer_coord(0, 0));
	enfiler(&f, r);
}

void *routine_fourmi(void *arg) {
	pthread_cleanup_push(cri_de_mort, NULL);

	coord c = *(coord *) arg;
	free(arg);
	struct timespec temps_sleep = {
		.tv_sec = 1.5,
		.tv_nsec = 500000000
	};
    int direction[4][2] = { {-1,0}, {1,0}, {0,1}, {0,-1} };
	wprintw(fen_msg, "Je suis la fourmi a la position y=%d, x=%d\n", c.y, c.x);
	while (1) {		
		int rd = rand() % 4;
        int new_y = c.y + direction[rd][0];
        int new_x = c.x + direction[rd][1];
        if(new_y > 0 && new_y < NB_LIGNES_SIM - 1 && new_x > 0 && new_x < NB_COL_SIM - 1 && new_x > 0) {
            requete* r = creer_requete(REQUETE_DEPLACEMENT_FOURMI, creer_coord(c.y,c.x), creer_coord(new_y, new_x));
			enfiler(&f,r);

			pthread_mutex_lock(&grille[new_y][new_x].mutex);
			if(grille[new_y][new_x].element != OBSTACLE) {
				c.y = new_y;
				c.x = new_x;
			}
			pthread_mutex_unlock(&grille[new_y][new_x].mutex);
        }
		nanosleep(&temps_sleep, NULL);

	}
	pthread_cleanup_pop(0);
	return NULL;
}

void* routine_affichage(void *arg) {
	file_bloquante *f = (file_bloquante*) arg;

	while(!simulation_terminee) {
		requete *req = defiler(f);
		switch(req->type_req){
			case REQUETE_ACTIVATION_OUTIL_OBSTACLE:
				pthread_mutex_lock(&mutex_affichage);
				mvwprintw(fen_outils, 1, 1, "X");
				mvwprintw(fen_outils, 2, 1, " ");
				mvwprintw(fen_outils, 3, 1, " ");
				wrefresh(fen_outils);
				wprintw(fen_msg, "Outil obstacle active\n");
				wrefresh(fen_msg);
				pthread_mutex_unlock(&mutex_affichage);
				break;
			case REQUETE_ACTIVATION_OUTIL_FOURMI:
				pthread_mutex_lock(&mutex_affichage);
				mvwprintw(fen_outils, 1, 1, " ");
				mvwprintw(fen_outils, 2, 1, "X");
				mvwprintw(fen_outils, 3, 1, " ");
				wrefresh(fen_outils);
				wprintw(fen_msg, "Outil fourmi active\n");
				wrefresh(fen_msg);
				pthread_mutex_unlock(&mutex_affichage);
				break;
			case REQUETE_ACTIVATION_OUTIL_DESTRUCTION:
				pthread_mutex_lock(&mutex_affichage);
				mvwprintw(fen_outils, 1, 1, " ");
				mvwprintw(fen_outils, 2, 1, " ");
				mvwprintw(fen_outils, 3, 1, "X");
				wrefresh(fen_outils);
				wprintw(fen_msg, "Outil destruction active\n");
				wrefresh(fen_msg);
				pthread_mutex_unlock(&mutex_affichage);
				break;
			case REQUETE_AFFICHAGE_OBSTACLE:
				pthread_mutex_lock(&grille[req->c1.y][req->c1.x].mutex);
				if(grille[req->c1.y][req->c1.x].element == VIDE){
					grille[req->c1.y][req->c1.x].element = OBSTACLE;
					pthread_mutex_lock(&mutex_affichage);
					mvwprintw(fen_sim, req->c1.y, req->c1.x, "#");
					wprintw(fen_msg, "Ajout d'un obstacle a la position %d %d\n", req->c1.y, req->c1.x);
					wrefresh(fen_msg);
					wrefresh(fen_sim);
					pthread_mutex_unlock(&mutex_affichage);
				}
				pthread_mutex_unlock(&grille[req->c1.y][req->c1.x].mutex);
				break;
			case REQUETE_AFFICHAGE_FOURMI:
				pthread_mutex_lock(&grille[req->c1.y][req->c1.x].mutex);
				if(grille[req->c1.y][req->c1.x].element == VIDE){
					grille[req->c1.y][req->c1.x].fourmi = malloc(sizeof(pthread_t));
					coord* c = malloc(sizeof(coord));
					*c = creer_coord(req->c1.y, req->c1.x);
					pthread_create(grille[req->c1.y][req->c1.x].fourmi, NULL, routine_fourmi, c);
					grille[req->c1.y][req->c1.x].element = FOURMI;
					nb_fourmis++;
					pthread_mutex_lock(&mutex_affichage);
					wprintw(fen_msg, "Nombre de fourmis : %d\n", nb_fourmis);
					mvwprintw(fen_sim, req->c1.y, req->c1.x, "@");
					wprintw(fen_msg, "Ajout d'une fourmi a la position %d %d\n", req->c1.y, req->c1.x);
					wrefresh(fen_sim);
					wrefresh(fen_msg);
					pthread_mutex_unlock(&mutex_affichage);
				}
				pthread_mutex_unlock(&grille[req->c1.y][req->c1.x].mutex);
				break;
			case REQUETE_AFFICHAGE_DESTRUCTION:
				pthread_mutex_lock(&grille[req->c1.y][req->c1.x].mutex);
				pthread_mutex_lock(&mutex_affichage);
				if (grille[req->c1.y][req->c1.x].element == FOURMI) {
					pthread_cancel(*grille[req->c1.y][req->c1.x].fourmi);
					pthread_join(*grille[req->c1.y][req->c1.x].fourmi, NULL);
					free(grille[req->c1.y][req->c1.x].fourmi);
					grille[req->c1.y][req->c1.x].fourmi = NULL;
					grille[req->c1.y][req->c1.x].element = VIDE;
					nb_fourmis--;
					mvwprintw(fen_sim, req->c1.y, req->c1.x, " ");
					wprintw(fen_msg, "Nombre de fourmis : %d\n", nb_fourmis);
				}
				else if (grille[req->c1.y][req->c1.x].element == OBSTACLE) {
					grille[req->c1.y][req->c1.x].element = VIDE;
					mvwprintw(fen_sim, req->c1.y, req->c1.x, " ");
					wprintw(fen_msg, "Destruction de l'obstacle a la position %d %d\n", req->c1.y, req->c1.x);
				}
				wrefresh(fen_sim);
				wrefresh(fen_msg);
				pthread_mutex_unlock(&mutex_affichage);
				pthread_mutex_unlock(&grille[req->c1.y][req->c1.x].mutex);
				break;
			case REQUETE_DEPLACEMENT_FOURMI:
				pthread_mutex_lock(&grille[req->c1.y][req->c1.x].mutex);
				pthread_mutex_lock(&grille[req->c2.y][req->c2.x].mutex);
				if(grille[req->c2.y][req->c2.x].element != OBSTACLE) {
					pthread_mutex_lock(&mutex_affichage);
					mvwprintw(fen_sim, req->c2.y, req->c2.x,"@");
					mvwprintw(fen_sim, req->c1.y, req->c1.x, " ");
					if(grille[req->c2.y][req->c2.x].element == FOURMI){
						pthread_cancel(*grille[req->c2.y][req->c2.x].fourmi);
						pthread_join(*grille[req->c2.y][req->c2.x].fourmi,NULL);
						nb_fourmis--;
						wprintw(fen_msg, "Nombre de fourmis : %d\n", nb_fourmis);
					}
					grille[req->c2.y][req->c2.x].element = FOURMI;
					grille[req->c2.y][req->c2.x].fourmi = grille[req->c1.y][req->c1.x].fourmi;
					grille[req->c1.y][req->c1.x].fourmi = NULL;
					grille[req->c1.y][req->c1.x].element = VIDE;
					
					wrefresh(fen_sim);
					wrefresh(fen_msg);
					pthread_mutex_unlock(&mutex_affichage);
				}
				pthread_mutex_unlock(&grille[req->c2.y][req->c2.x].mutex);
				pthread_mutex_unlock(&grille[req->c1.y][req->c1.x].mutex);
				break;
			case REQUETE_CLIC_POSITION:
				pthread_mutex_lock(&mutex_affichage);
				wprintw(fen_msg, "Clic a la position %d %d de l'ecran\n", req->c1.y, req->c1.x);
				wrefresh(fen_msg);
				pthread_mutex_unlock(&mutex_affichage);
				break;
			case REQUETE_CRI_MORT:
				pthread_mutex_lock(&mutex_affichage);
				wprintw(fen_msg, "AAAAAAH JE MEURS\n");
				wrefresh(fen_msg);
				if(nb_fourmis == 1) {
					wprintw(fen_msg, "\n*** THE END ***\nLa dernière fourmi a gagné !\n");
                    wrefresh(fen_msg);
                    simulation_terminee = 1;
					requete *r = creer_requete(REQUETE_FIN_SIMULATION, creer_coord(0, 0), creer_coord(0, 0));
                    enfiler(f, r);
				}
				pthread_mutex_unlock(&mutex_affichage);
				break;
			case REQUETE_FIN_SIMULATION:
				pthread_mutex_lock(&mutex_affichage);
				struct timespec temps_attente = {
					.tv_sec = 3,
					.tv_nsec = 0
				};
				nanosleep(&temps_attente, NULL);
				wprintw(fen_msg, "FIN DE SIMULATION\n");
				pthread_mutex_unlock(&mutex_affichage);
				break;
			default:
				break;
			
		}
		if(req != NULL){
			free(req);
		}
	}
	return NULL;
}

int main() {
	initialiser_file(&f,100);
	MEVENT event;
	int ch; 
	item_actif = OBSTACLE;
	srand(time(NULL));	
	ncurses_init();
	simulation_init();
	creer_fenetres();
	pthread_t thread_affichage;
	pthread_create(&thread_affichage,NULL,routine_affichage,&f);
	mvprintw(LINES - 1, 0, "Tapez F2 pour quitter");
	wrefresh(stdscr);
	while((ch = getch()) != KEY_F(2) && !simulation_terminee) {
		switch(ch) {
			case KEY_MOUSE :
				if (getmouse(&event) == OK) {
					requete *r = creer_requete(REQUETE_CLIC_POSITION,creer_coord(event.y,event.x), creer_coord(0,0));
					enfiler(&f,r);
					if (event.y == 17 && event.x >= 63 && event.x <= 80) {
						item_actif = OBSTACLE;
						requete *r = creer_requete(REQUETE_ACTIVATION_OUTIL_OBSTACLE,creer_coord(0,0),creer_coord(0,0));
						enfiler(&f,r);
					}
					else if (event.y == 18 && event.x >=63 && event.x <= 78) {
						item_actif = FOURMI;
						requete *r = creer_requete(REQUETE_ACTIVATION_OUTIL_FOURMI,creer_coord(0,0),creer_coord(0,0));
						enfiler(&f,r);
					}
					else if (event.y == 19 && event.x >=63 && event.x <= 77) {
						item_actif = VIDE;
						requete *r = creer_requete(REQUETE_ACTIVATION_OUTIL_DESTRUCTION, creer_coord(0,0), creer_coord(0,0));
						enfiler(&f,r);
					}
					else if (event.y > 0 && event.y < NB_LIGNES_SIM - 1 && event.x > 0 && event.x < NB_COL_SIM - 1) {
						requete* r;
						switch (item_actif) {
							case OBSTACLE :
								r = creer_requete(REQUETE_AFFICHAGE_OBSTACLE,creer_coord(event.y, event.x), creer_coord(0,0));
								enfiler(&f,r);
								break;
							case FOURMI :
								r = creer_requete(REQUETE_AFFICHAGE_FOURMI,creer_coord(event.y,event.x), creer_coord(0,0));
								enfiler(&f,r);
								break;
							case VIDE :
								r = creer_requete(REQUETE_AFFICHAGE_DESTRUCTION,creer_coord(event.y,event.x), creer_coord(0,0));
								enfiler(&f,r);
								break;
						}
					}
				}
			break;
		}
	}

	detruire_fenetres();
	ncurses_stop();

	return EXIT_SUCCESS;
}