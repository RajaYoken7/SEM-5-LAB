import java.util.PriorityQueue;

class Solution {
    public int[] assignTasks(int[] servers, int[] tasks) {
        int n = servers.length;
        int m = tasks.length;
        int[] ans = new int[m];
        
        // freeServers Heap: sorted by [weight, index]
        PriorityQueue<int[]> freeServers = new PriorityQueue<>((a, b) -> {
            if (a[0] != b[0]) return Integer.compare(a[0], b[0]);
            return Integer.compare(a[1], b[1]);
        });
        
        // busyServers Heap: sorted by [freeTime, weight, index]
        PriorityQueue<int[]> busyServers = new PriorityQueue<>((a, b) -> {
            if (a[2] != b[2]) return Integer.compare(a[2], b[2]); // freeTime
            if (a[0] != b[0]) return Integer.compare(a[0], b[0]); // weight
            return Integer.compare(a[1], b[1]);                   // index
        });
        
        // Initialize all servers as free. Array structure: [weight, index, freeTime]
        for (int i = 0; i < n; i++) {
            freeServers.offer(new int[]{servers[i], i, 0});
        }
        
        int time = 0;
        for (int i = 0; i < m; i++) {
            // The time cannot be behind the task arrival index 'i'
            time = Math.max(time, i);
            
            // If no servers are available, fast forward time to the earliest freeing server
            if (freeServers.isEmpty()) {
                time = busyServers.peek()[2];
            }
            
            // Release all servers that finished processing at or before the current time
            while (!busyServers.isEmpty() && busyServers.peek()[2] <= time) {
                freeServers.offer(busyServers.poll());
            }
            
            // Grab the best available free server
            int[] currentServer = freeServers.poll();
            ans[i] = currentServer[1]; // Store server index for the ith task
            
            // Update the server's free time and move it to busy
            currentServer[2] = time + tasks[i];
            busyServers.offer(currentServer);
        }
        
        return ans;
    }
}
