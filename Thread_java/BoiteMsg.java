package TP.TP4;
import java.util.concurrent.ArrayBlockingQueue;

// Pour la question 3 sans ArrayBlockingQueue

/*public class BoiteMsg {
    private String message;
    private boolean vide;
    
    public BoiteMsg() {
        this.message = "";
        vide = true;
    }

    // Dépose un message dans la boite
    public synchronized void deposer_message(String msg) {
        while(!vide) {
            try {
                wait();
            } catch(InterruptedException e) {
                //Thread.currentThread().interrupt();
                System.err.println("Thread interrompu");
            }  
        }
        message = msg;
        vide = false;
        System.out.println("Message recu : " +msg);
        notifyAll(); // Réveille le consommateur si nécessaire
    }

    // Récupère un message dans la boite
    public synchronized String recuperer_message() { // -----> Synchronized représente le vérou en Java
        while(vide) { // On attend si la boite est vide
            try {
                wait();
            } catch(InterruptedException e) {
                //Thread.currentThread().interrupt();
                System.err.println("Thread interrompu");
            }
        }
        System.out.println("Message recupere : " + message);
        vide = true;
        notifyAll(); // Réveille le producteur si nécessaire
        return message;
    }
}*/

// Pour la question 4 avec ArrayBlockingQueue
public class BoiteMsg {
    private ArrayBlockingQueue<String> file; // ----> File de message

    public BoiteMsg(int capacite) {
        this.file = new ArrayBlockingQueue<>(capacite);
    }

    public void deposer_message(String message, int idProducteur) {
        try {
            file.put(message);
            System.out.println("Message depose par le producteur : " + idProducteur + ": " + message + " - " + (file.remainingCapacity()) + " places restante dans la file");
        } catch(InterruptedException e) {
            System.err.println("Thread interrompu");
        }

    }

    public String recuperer_message(int idConsommateur) {
        try {
            String message = file.take();
            System.out.println("Message recu par le consommateur : " + idConsommateur + ": " + message + " - " + (file.remainingCapacity() + 1) + " places restantes dans la file");
            return message;
        } catch (InterruptedException e) {
            System.err.println("Thread interrompu");
            return null;
        }
    }
}

