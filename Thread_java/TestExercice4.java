package TP.TP4;

public class TestExercice4 {
    public static void main(String[] args) {
        int nombreProducteurs = 4;
        int nombreConsommateurs = 4;
        BoiteMsg boite = new BoiteMsg(5);

        String[] message = {"Le ","stage ","est ", "obligatoire ", "pour ", "valider ", "la ", "L3 ", "Informatique ", "alors ", "n'attendez ", "pas ", "le ", "mois ", "de ", "mars ", "pour ", "chercher !" };

        // Création et démarrage des producteurs
        for(int i = 0; i < nombreProducteurs; i++) {
            new Thread(new Producteur(boite, message, i + 1)).start();
        }

        // Création et démarrage des consommateurs
        for(int i = 0; i < nombreConsommateurs; i++) {
            new Thread(new Consommateur(boite, i + 1)).start();
        }
    }
}
