package TP.TP4;
import java.lang.Thread;

class Exercice1 implements Runnable {
    private int threadIndex;
    private int max_thread;

    public Exercice1(int threadIndex, int max_thread) {
        this.threadIndex = threadIndex;
        this.max_thread = max_thread;
    }

    @Override
    public void run() {
        // On récupère l'id et le nom du thread créer par Java
        long threadId = Thread.currentThread().threadId(); // ----> Ici on récupère l'id donné par Java
        String threadName = Thread.currentThread().getName(); // -----> Ici on récupère le nom donné par Java
        System.out.println("Bonjour " + threadIndex + "/" + max_thread + ", d'id " + threadId + " et de nom : " + threadName);
    }

    public int getThreadIndex() {
        return this.threadIndex;
    }

    public int getMax_thread() {
        return this.max_thread;
    }
}

public class TestExercice2 {
    public static void main(String []args) {
        int p = 8; // Nombre de thread a créée
        // Création des threads
        Thread[] threads = new Thread[p]; // ----> Ici on créer p Thread

        for(int i = 0; i < p; i++) {
            // Création d'un thread et de son index
            Exercice1 threadExercice1 = new Exercice1(i + 1, p);
            threads[i] = new Thread(threadExercice1); // ----> On attache l'objet Exercice1 au thread, commme si c'était une struct en C
            threads[i].start(); // ----> pour démarrer les threads, afin qu'ils lancent leurs méthodes runs
        }

        // Attend que tous les threads se terminent
        for(int i =  0; i < p; i++) {
            try {
                threads[i].join();
            } catch(InterruptedException e) { // -----> Au cas où il y'a une erreur lors de l'interruption des threads lors de l'execution
                e.printStackTrace();
            }
        }
    }
}