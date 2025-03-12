package TP.TP4;

// Pour la question 3 sans ArrayBlockingQueue

/*public class Producteur implements Runnable {
    private BoiteMsg boite;
    private String[] message;

    public Producteur(BoiteMsg boite, String[] message) {
        this.boite = boite;
        this.message = message;
    }

    @Override
    public void run() {
        for(String msg : message) {
            try {
                // Pause entre chaque dépot
                boite.deposer_message(msg);
                Thread.sleep((long) (Math.random() * 4000) + 1000);
            } catch (InterruptedException e) {
                //Thread.currentThread().interrupt();
                System.err.println("Thread interrompu");
            }
        }
        // Dépose un message "termine" pour indiquer la fin
        boite.deposer_message("Termine");
    }
}*/

// Pour la question 4 avec ArrayBlockingQueue

public class Producteur implements Runnable {
    private BoiteMsg boite;
    private String[] message;
    private int idProducteur;

    public Producteur(BoiteMsg boite, String[] message, int idProducteur) {
        this.boite = boite;
        this.message = message;
        this.idProducteur = idProducteur;
    }

    @Override
    public void run() {
        for(String msg : message) {
            try {
                boite.deposer_message(msg, idProducteur);
                Thread.sleep((long) (Math.random() * 4000) + 1000);
            } catch (InterruptedException e) {
                System.err.println("Thread interrompu");
            }
        }
        boite.deposer_message("Termine", idProducteur);
    }
}


