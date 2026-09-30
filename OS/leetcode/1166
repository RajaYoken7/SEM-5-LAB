import java.util.*;

class FileSystem {
    private final Map<String, Integer> paths = new HashMap<>();

    public FileSystem() {
        paths.put("", -1);
    }

    public boolean createPath(String path, int value) {
        if (path == null || path.isEmpty() || path.equals("/") || paths.containsKey(path)) {
            return false;
        }
        
        int lastSlashIndex = path.lastIndexOf('/');
        String parent = path.substring(0, lastSlashIndex);
        
        if (!paths.containsKey(parent)) {
            return false;
        }
        
        paths.put(path, value);
        return true;
    }

    public int get(String path) {
        return paths.getOrDefault(path, -1);
    }
}
