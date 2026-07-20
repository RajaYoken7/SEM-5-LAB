class Solution(object):
    def findMaximumXOR(self, numbers):
        max_bits = max(numbers).bit_length()
        prefix_tree = [[-1, -1]]

        def add_number(num):
            current = 0
            for bit_pos in range(max_bits - 1, -1, -1):
                bit_val = (num >> bit_pos) & 1
                if prefix_tree[current][bit_val] == -1:
                    prefix_tree[current][bit_val] = len(prefix_tree)
                    prefix_tree.append([-1, -1])
                current = prefix_tree[current][bit_val]

        def query_max_xor(num):
            current = 0
            xor_accum = 0
            for bit_pos in range(max_bits - 1, -1, -1):
                bit_val = (num >> bit_pos) & 1
                opposite_bit = 1 - bit_val
                if prefix_tree[current][opposite_bit] != -1:
                    xor_accum |= (1 << bit_pos)
                    current = prefix_tree[current][opposite_bit]
                else:
                    current = prefix_tree[current][bit_val]
            return xor_accum

        for n in numbers:
            add_number(n)

        max_xor = 0
        for n in numbers:
            max_xor = max(max_xor, query_max_xor(n))

        return max_xor
