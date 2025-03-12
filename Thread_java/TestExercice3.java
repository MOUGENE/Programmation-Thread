package TP.TP4;

public class TestExercice3 {
    public static void main(String[] args) {
        // Création de la boite de message
        BoiteMsg boite = new BoiteMsg();

        // Message à déposer par le producteur
        String[] messages = {
            "Le ","stage ","est ","obligatoire ","pour ","valider ","la ","L3 ","Informatique ","alors ","n'attendez ","pas ","le ","mois ","de ","mars ","pour ","chercher !"
        };

        // Création du producteur et du consommateur
        Producteur prod = new Producteur(boite, messages);
        Consommateur cons = new Consommateur(boite);
        // Création des threads pour le producteur et le consommateur
        Thread producteur = new Thread(prod); // ---> Ici on créer le thread producteur
        Thread consommateur = new Thread(cons); // ---> Ici on créer le thread consommateur

        // Démarre des threads
        producteur.start();
        consommateur.start();

        try {
            producteur.join();
            consommateur.join();
        } catch(InterruptedException e) {
            e.printStackTrace();
        }
    }
}
