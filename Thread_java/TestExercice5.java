package TP.TP4;
import java.util.concurrent.Executor;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class TestExercice5 {
    public static void main(String[] args) {
        int nombreProducteurs = 4;
        int nombreConsommateurs = 4;
        BoiteMsg boite = new BoiteMsg(5);

        String[] message = {"Le ","stage ","est ", "obligatoire ", "pour ", "valider ", "la ", "L3 ", "Informatique ", "alors ", "n'attendez ", "pas ", "le ", "mois ", "de ", "mars ", "pour ", "chercher !" };
        
        // Création d'un ExecutorService avec un pool de 8 threads
        ExecutorService executeur = Executors.newFixedThreadPool(8);

        // Soumission des producteurs à l'ExecutorService
        for(int i = 0; i < nombreProducteurs; i++) {
            executeur.execute(new Producteur(boite, message, i + 1));
        }

        // Soumission des consommateurs à l'ExecutorService
        for(int i = 0; i < nombreConsommateurs; i++) {
            executeur.execute(new Consommateur(boite, i + 1));
        }

        // Fermeture de l'ExecutorService après utilisation
        executeur.shutdown();
    }
}
