#ifndef SORT_HPP
#define SORT_HPP

namespace Sort {

    // Restore max-heap property going down from "root" (iterative to avoid deep recursion)
    template <typename T>
    void siftDown(T* data, int size, int root) {
        while (true) {
            int largest = root;
            int left = 2 * root + 1;
            int right = 2 * root + 2;

            if (left < size && data[largest] < data[left]) largest = left;
            if (right < size && data[largest] < data[right]) largest = right;

            if (largest == root) break;

            T temp = data[root];
            data[root] = data[largest];
            data[largest] = temp;

            root = largest;
        }
    }

    // In-place heapsort, O(n log n) worst case; requires operator< on T
    template <typename T>
    void heapSort(T* data, int size) {
        // Build max-heap bottom-up (Floyd's method, O(n))
        for (int i = size / 2 - 1; i >= 0; --i) {
            siftDown(data, size, i);
        }

        // Repeatedly move current max behind the shrinking heap
        for (int end = size - 1; end > 0; --end) {
            T temp = data[0];
            data[0] = data[end];
            data[end] = temp;

            siftDown(data, end, 0);
        }
    }
}

#endif
