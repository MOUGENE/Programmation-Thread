package TP.TP4;
// Pour la question 3 sans ArrayBlockingQueue

/*public class Consommateur implements Runnable {
    private BoiteMsg boite;

    public Consommateur(BoiteMsg boite) {
        this.boite = boite;
    }

    @Override
    public void run() {
        String msg = "";
        while(!(msg = boite.recuperer_message()).equals("termine")) {
            msg = boite.recuperer_message();
            //System.out.println("Message recupere : " + msg);
            try {    
                // Pause entre chaque récupération de message
                Thread.sleep((long) (Math.random() * 4000) + 1000);
            } catch(InterruptedException e) {
                //Thread.currentThread().interrupt();
                System.err.println("Thread interrompue");
            }
        }
    }
}*/

// Pour la question 4 avec ArrayBlockingQueue

public class Consommateur implements Runnable {
    private BoiteMsg boite;
    private int idConsommateur;

    public Consommateur(BoiteMsg boite, int idConsommateur) {
        this.boite = boite;
        this.idConsommateur = idConsommateur;
    }

    @Override
    public void run() {
        String message;
        do {
            message = boite.recuperer_message(idConsommateur);
            try {
                Thread.sleep((long) (Math.random() * 4000) + 1000 );
            } catch (InterruptedException e) {
                System.err.println("Thread interrompu");
            }
        } while(!"Termine".equals(message));
        System.out.println("Consommateur : " + idConsommateur + "termine");
    }
}


