class DiningPhilosophers {
    private final Object[] forks = new Object[5];

    public DiningPhilosophers() {
        for (int i = 0; i < 5; i++) {
            forks[i] = new Object();
        }
    }

    public void wantsToEat(int philosopher,
                           Runnable pickLeftFork,
                           Runnable pickRightFork,
                           Runnable eat,
                           Runnable putLeftFork,
                           Runnable putRightFork) throws InterruptedException {
        
        int leftFork = philosopher;
        int rightFork = (philosopher + 1) % 5;

        int firstFork = Math.min(leftFork, rightFork);
        int secondFork = Math.max(leftFork, rightFork);

        synchronized (forks[firstFork]) {
            synchronized (forks[secondFork]) {
                if (firstFork == leftFork) {
                    pickLeftFork.run();
                    pickRightFork.run();
                } else {
                    pickRightFork.run();
                    pickLeftFork.run();
                }
                
                eat.run();
                
                if (firstFork == leftFork) {
                    putRightFork.run();
                    putLeftFork.run();
                } else {
                    putLeftFork.run();
                    putRightFork.run();
                }
            }
        }
    }
}
