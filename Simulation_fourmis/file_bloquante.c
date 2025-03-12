#include "file_bloquante.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Initialise une file bloquante
void initialiser_file(file_bloquante *f, size_t l) {
    f->longeur = l + 1; // On met un espace en plus pour distinguer vide/plein
    f->tete = 0;
    f->queue = 0;
    pthread_mutex_init(&f->mutex,NULL);
    pthread_cond_init(&f->non_vide,NULL);
    pthread_cond_init(&f->non_pleine,NULL);
    f->t = malloc(l * sizeof(requete*));
}

// Détruire la file bloquante
void detruire_file(file_bloquante *f) {
    pthread_mutex_destroy(&f->mutex);
    pthread_cond_destroy(&f->non_vide);
    pthread_cond_destroy(&f->non_pleine);
    free(f->t); // Libère la mémoire allouée pour la tableau de requêtes
}

// Enfiler une requête dans la file
void enfiler(file_bloquante *f, requete* r) {
    pthread_mutex_lock(&f->mutex);

    // Attendre si la file est pleine
    while(file_pleine(*f)) {
        pthread_cond_wait(&f->non_pleine, &f->mutex);
    }

    // Ajouter la requête à la file
    f->t[f->queue] = r;
    if(f->queue == f->longeur - 1) {
        f->queue = 0;
    }
    else {
        f->queue++;
    }

    // Signaler qu'il y'a des éléments dans la file
    pthread_cond_signal(&f->non_vide);
    pthread_mutex_unlock(&f->mutex);
}

// Défiler une requête de la file
requete* defiler(file_bloquante *f) {
    pthread_mutex_lock(&f->mutex);

    // Attendre si la file est vide
    while(file_vide(*f)) {
        pthread_cond_wait(&f->non_vide,&f->mutex);
    }

    // Retirer une requête de la file
    requete* r = f->t[f->tete];
    if(f->tete == f->longeur - 1) {
        f->tete = 0;
    }
    else {
        f->tete++;
    }

    // Signaler qu'il y'a de l'espace dans le file
    pthread_cond_signal(&f->non_pleine);
    pthread_mutex_unlock(&f->mutex);

    return r;
}

int file_vide(file_bloquante f) {
    return f.tete == f.queue;
}

int file_pleine(file_bloquante f) {
    return (f.queue + 1) % f.longeur == f.tete;
}

int espaces_libres(file_bloquante f) {
    int result = 0;
    if(f.tete < f.queue) {
        result = f.queue - f.tete - 1;
    }
    else {
        result = f.longeur - f.tete + f.queue - 1;
    }
    return result;
}