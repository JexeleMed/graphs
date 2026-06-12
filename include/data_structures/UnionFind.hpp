#ifndef UNION_FIND_HPP
#define UNION_FIND_HPP

class UnionFind {
private:
    int* parent;
    int* rank;

public:
    explicit UnionFind(int n) {
        parent = new int[n];
        rank   = new int[n];

        for (int i = 0; i < n; ++i) {
            parent[i] = i;
            rank[i]   = 0;
        }
    }

    ~UnionFind() {
        delete[] parent;
        delete[] rank;
    }

    // Non-copyable: double free prevention
    UnionFind(const UnionFind&) = delete;
    UnionFind& operator=(const UnionFind&) = delete;

    // Path compression: flattens the tree on the way up
    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

    // Returns false if x and y are already connected (cycle detected)
    bool unite(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);

        if (rootX == rootY) return false;

        // Union by rank: attach shorter tree under taller one
        if (rank[rootX] < rank[rootY]) {
            parent[rootX] = rootY;
        } else if (rank[rootX] > rank[rootY]) {
            parent[rootY] = rootX;
        } else {
            parent[rootY] = rootX;
            rank[rootX]++;
        }

        return true;
    }

    bool connected(int x, int y) {
        return find(x) == find(y);
    }
};

#endif
