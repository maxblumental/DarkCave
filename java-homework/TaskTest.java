import org.junit.Test;

import java.util.Random;

/**
 * Created by Maxim on 12/10/2015.
 */
public class TaskTest {
    @Test
    public void testMultithreadedTaskUsage() {
        int threadNumber = 10;
        Task<Integer> task = new Task<>(()-> new Random().nextInt());
        Thread[] threads = new Thread[threadNumber];
        for (int i = 0; i < threadNumber; i++) {
            threads[i] = new Thread(() -> task.get());
        }

        for (Thread thread : threads) {
            thread.start();
        }

        for (Thread thread : threads) {
            try {
                thread.join();
            } catch (InterruptedException ignored) {
            }
        }
    }
}