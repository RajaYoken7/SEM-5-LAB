def riddle(arr):
    n = len(arr)
    
    # 1. Previous strictly smaller element
    left = [-1] * n
    stack = []
    for i in range(n):
        while stack and arr[stack[-1]] >= arr[i]:
            stack.pop()
        left[i] = stack[-1] if stack else -1
        stack.append(i)
    
    # 2. Next smaller-or-equal element
    right = [n] * n
    stack = []
    for i in range(n - 1, -1, -1):
        while stack and arr[stack[-1]] > arr[i]:
            stack.pop()
        right[i] = stack[-1] if stack else n
        stack.append(i)
    
    # 3. ans[w] = max element that is minimum of a window of size exactly w
    ans = [0] * (n + 2)
    for i in range(n):
        window_size = right[i] - left[i] - 1
        ans[window_size] = max(ans[window_size], arr[i])
    
    # 4. Fill gaps from right to left
    # If no element is min for window w, use the answer from w+1
    for w in range(n - 1, 0, -1):
        ans[w] = max(ans[w], ans[w + 1])
    
    return ans[1:n + 1]


# Input reading
n = int(input())
arr = list(map(int, input().split()))
print(' '.join(map(str, riddle(arr))))
