import java.util.concurrent.Semaphore;
import java.util.concurrent.CyclicBarrier;

class H2O {
    private final Semaphore hSem = new Semaphore(2);
    private final Semaphore oSem = new Semaphore(1);
    private final CyclicBarrier barrier = new CyclicBarrier(3);

    public H2O() {}

    public void hydrogen(Runnable releaseHydrogen) throws InterruptedException {
        hSem.acquire();
        try {
            barrier.await();
        } catch (Exception e) {
            Thread.currentThread().interrupt();
        }
        releaseHydrogen.run();
        hSem.release();
    }

    public void oxygen(Runnable releaseOxygen) throws InterruptedException {
        oSem.acquire();
        try {
            barrier.await();
        } catch (Exception e) {
            Thread.currentThread().interrupt();
        }
        releaseOxygen.run();
        oSem.release();
    }
}
