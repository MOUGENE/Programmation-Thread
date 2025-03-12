#ifndef REQUETE_H
#define REQUETE_H

/* Types de requêtes disponibles */
#define REQUETE_ACTIVATION_OUTIL_OBSTACLE 3
#define REQUETE_ACTIVATION_OUTIL_FOURMI 4
#define REQUETE_ACTIVATION_OUTIL_DESTRUCTION 5
#define REQUETE_DEPLACEMENT_FOURMI 6  /* Requête pour déplacer une fourmi */
#define REQUETE_AFFICHAGE_OBSTACLE 7    /* Requête pour mettre à jour l'affichage */
#define REQUETE_AFFICHAGE_FOURMI 8    /* Requête pour mettre à jour l'affichage */
#define REQUETE_AFFICHAGE_DESTRUCTION 9 /* Requête pour mettre à jour l'affichage */
#define REQUETE_CLIC_POSITION 10
#define REQUETE_CRI_MORT 11
#define REQUETE_FIN_SIMULATION 12
/**
 * Structure représentant des coordonnées 2D
 */
typedef struct {
    int y;  /* Position verticale */
    int x;  /* Position horizontale */
} coord;

/**
 * Structure représentant une requête
 */
typedef struct {
    int type_req;   /* Type de la requête (REQUETE_DEPLACEMENT ou REQUETE_AFFICHAGE) */
    coord c1;       /* Coordonnée source */
    coord c2;       /* Coordonnée destination */
} requete;

/**
 * Crée une nouvelle coordonnée
 * @param y Position verticale
 * @param x Position horizontale
 * @return Structure coord initialisée
 */
coord creer_coord(int y, int x);

/**
 * Crée une nouvelle requête
 * @param type Type de la requête (REQUETE_DEPLACEMENT ou REQUETE_AFFICHAGE)
 * @param c1 Coordonnée source
 * @param c2 Coordonnée destination
 * @return Pointeur vers la requête créée, NULL si échec d'allocation
 */
requete* creer_requete(int type, coord c1, coord c2);

#endif