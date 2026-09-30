class Solution {
    public int distanceBetweenBusStops(int[] distance, int start, int destination) {
        if (start > destination) {
            int temp = start;
            start = destination;
            destination = temp;
        }
        
        int pathDistance = 0;
        int totalDistance = 0;
        
        for (int i = 0; i < distance.length; i++) {
            if (i >= start && i < destination) {
                pathDistance += distance[i];
            }
            totalDistance += distance[i];
        }
        
        return Math.min(pathDistance, totalDistance - pathDistance);
    }
}
