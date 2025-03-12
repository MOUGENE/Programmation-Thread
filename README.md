# Programmation Thread

Ce projet contient plusieurs exercices et simulations liés à la programmation multithread en C et en Java. Les exercices couvrent divers aspects de la programmation concurrente, y compris l'utilisation de threads, mutex, variables de condition, et la bibliothèque ncurses pour l'affichage.

## Structure du projet

Le projet est organisé en plusieurs dossiers, chacun contenant des exercices spécifiques :

- `Simulation_fourmis/`
  - `file_bloquante.c` : Implémentation d'une file bloquante.
  - `file_bloquante.h` : Déclaration des fonctions et structures pour la file bloquante.
  - `requete.c` : Implémentation des requêtes pour la simulation.
  - `requete.h` : Déclaration des types et fonctions pour les requêtes.
  - `test_fourmis.c` : Programme principal pour la simulation des fourmis.

- `Thread_basique/`
  - `Exercice1.c` : Création et gestion de threads simples.
  - `Exercice2.c` : Utilisation de mutex pour protéger une section critique.
  - `Exercice3.c` : Calcul parallèle de la suite de Fibonacci.

- `Thread_env_ncurses/`
  - `Exercice1.c` : Calcul parallèle de la somme d'un tableau avec affichage ncurses.
  - `Exercice2.c` : Multiplication matrice-vecteur avec affichage ncurses.
  - `main.c` : Programme principal pour la multiplication matrice-vecteur avec affichage ncurses.
  - `test.c` : Test de la disponibilité des processeurs.
  - `test_Exercice1.c` : Test de la somme d'un tableau.
  - `test_Exercice2.c` : Test de la multiplication matrice-vecteur.

- `Thread_java/`
  - `BoiteMsg.java` : Implémentation d'une boîte de messages avec ArrayBlockingQueue.
  - `Consommateur.java` : Consommateur de messages.
  - `Producteur.java` : Producteur de messages.
  - `TestExercice2.java` : Test de création de threads en Java.
  - `TestExercice3.java` : Test de la boîte de messages avec un producteur et un consommateur.
  - `TestExercice4.java` : Test de la boîte de messages avec plusieurs producteurs et consommateurs.
  - `TestExercice5.java` : Utilisation d'un ExecutorService pour gérer les threads.

- `Thread_mutex_varcond/`
  - `Exercice1.c` : Utilisation de mutex et variables de condition pour synchroniser des threads.
  - `Exercice2.c` : Simulation de guichets avec une salle d'attente, utilisant mutex et variables de condition.

## Compilation et exécution

### C

Pour compiler les fichiers C, utilisez `gcc`. Par exemple, pour compiler `test_fourmis.c` :

```sh
gcc -o test_fourmis test_fourmis.c file_bloquante.c requete.c -lpthread -lncurses
