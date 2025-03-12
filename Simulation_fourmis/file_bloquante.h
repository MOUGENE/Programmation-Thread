#ifndef FILE_BLOQUANTE_H
#define FILE_BLOQUANTE_H

#include <pthread.h>
#include "requete.h"

typedef struct file_bloquante_t {
    size_t longeur; // Capacité de la file
    size_t tete;    // Index de la tete
    size_t queue;   // Index de la queue
    requete **t; // Tableau de requête
    pthread_mutex_t mutex;
    pthread_cond_t non_vide;
    pthread_cond_t non_pleine;
}file_bloquante;

// Fonctions associées à la file bloquante
void initialiser_file(file_bloquante *file, size_t longueur);
void detruire_file(file_bloquante *file);
void enfiler(file_bloquante *file, requete *r);
requete* defiler(file_bloquante *file);
int file_vide(file_bloquante file);
int file_pleine(file_bloquante file);
int espaces_libres(file_bloquante file);

#endif // FILE_BLOQUANTE_H
