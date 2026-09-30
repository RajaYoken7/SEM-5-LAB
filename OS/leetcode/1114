import java.util.concurrent.Semaphore;

class Foo {
    private final Semaphore sem2 = new Semaphore(0);
    private final Semaphore sem3 = new Semaphore(0);

    public Foo() {}

    public void first(Runnable printFirst) throws InterruptedException {
        printFirst.run();
        sem2.release();
    }

    public void second(Runnable printSecond) throws InterruptedException {
        sem2.acquire();
        printSecond.run();
        sem3.release();
    }

    public void third(Runnable printThird) throws InterruptedException {
        sem3.acquire();
        printThird.run();
    }
}
