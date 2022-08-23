/*
You are given a map of a labyrinth, and your task is to find a path from start to end.
You can walk left, right, up and down.
 
INPUT:
The first input line has two integers n and m: the height and width of the map.

Then there are n lines of m characters describing the labyrinth.
Each character is . (floor), # (wall), A (start), or B (end).
There is exactly one A and one B in the input.
 
OUTPUT:

First print "YES", if there is a path, and "NO" otherwise.

If there is a path, print the length of the shortest such path
and its description as a string consisting of characters L (left), R (right), U (up), and D (down).
You can print any valid solution.
 
CONSTRAINTS:
1 <= n,m  <= 1000

EXAMPLE:

Input:
5 8
########
#..#...#
####.#.#
#..#...#
########

Output:
YES
9
LDDRRRRRU

SOLUTION: 

VERIFY:

*/

#include <iostream>
#include <vector>
#include <array>
#include <set>
#include <string>
#include <tuple>
#include <stack>
#include <map>
#include <queue>
#include <limits>
#include <cmath>

using namespace std;

struct hashFunction {
    inline size_t operator()(const pair<int,int> & U) const {
        return U.first * 9113 + U.second;
    }
};

template <typename T>
inline void PrintOut(T value, string const& delimeter = "") {
	cout << value << delimeter;
}

inline bool CheckBounds(const size_t size) {
    return (size < 1 || size > 1000);
}

inline bool CheckInput(const char cell) {
    return (cell != '.' && cell != '#' && cell != 'A' && cell != 'B');
}

struct BitVector {
public:
    explicit BitVector(const int sequenceLength) :
    bitvector(ComputeSizeOfBitvector(sequenceLength))
    {}

    inline void SetBit(const int position) {
        const int bitix  = position / DIV;
        const int shift  = ComputeShift(position);
        bitvector[bitix] |= (ONE << shift);
    }

    inline size_t GetBit(const int position) const {
        const int bitix = position / DIV;
        const int shift = ComputeShift(position);
        return ((bitvector[bitix] & (ONE << shift)) >> shift);
    }

    inline int ComputeSizeOfBitvector(const int sequenceLength) const {
        return (sequenceLength + DIV - 1) / DIV;
    }

    inline uint64_t GetBits(const int position) const {
        return bitvector[position];
    }

private:
    static const long long DIV = 64L;
    static const long long ONE = 1L;

    inline int ComputeShift(const int position) const {
        return DIV - position % DIV - 1;
    }

    vector<uint64_t> bitvector;
};

class Graph {
public:
    explicit Graph(const int n, const int m) : 
    height(n),
    width(m),
    size(n * m),
    adjacentEdges(n * m),
    visitedNodes(n * m),
    distances(n * m)
    {
        if (CheckBounds(n) || CheckBounds(m))
            throw invalid_argument("Incorrect dimensions!");
    }

    void Init() {
    	for(int i = 0; i < height; i++) {
    		for(int j = 0; j < width; j++) {
                    char cell;
                    cin >> cell; //use macro here

                    AssignCellValue(cell, i, j);
            }
    	}
    }

    void SearchShortestPath() {
        InitializeDistances();

        AssignDistanceFromFirstNode(
            startingNode.second,
            startingNode.first,
            0
        );

        PutNodeToPriorityQueue(
            0,
            {
                startingNode.first,
                startingNode.second
            }
        );

        FindShortestPathWithDijkstraAlgorithm();
    }

    void InitializeDistances() {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (CellIsWall(i, j))
                    distances[LinearIndex(i, j)] = -1;
                else
                    distances[LinearIndex(i, j)] = height * width + 1;
            }
        }
    }

    inline void AssignDistanceFromFirstNode(const int y, const int x, int distance){
        distances[LinearIndex(y, x)] = distance;
    }

    inline void PutNodeToPriorityQueue(int distance, pair<int, int> node) {
        adjacentDistances.push(
            {
                distance,
                node
            }
        );
    }

    void FindShortestPathWithDijkstraAlgorithm() {
        while (!adjacentDistances.empty()) {
            auto currentNode = adjacentDistances.top();
            adjacentDistances.pop();

            auto x = currentNode.second.first;
            auto y = currentNode.second.second;

            if (NodeHasBeenVisited(y, x)) continue;
            VisitNode(y, x);

            const auto neighbourNodes = ComputeNeighbouringNodes(y, x);

            for (const auto& [x0, y0] : neighbourNodes) {

                if (CellIsWall(y0, x0)) continue;
                if (distances[LinearIndex(y, x)] + 1 < distances[LinearIndex(y0, x0)]) {
                    
                    distances[LinearIndex(y0, x0)] = distances[LinearIndex(y, x)] + 1;

                    adjacentDistances.push(
                        {
                            -distances[LinearIndex(y0, x0)],
                            {
                                x0,
                                y0
                            }
                        }
                    );
                }
            }
        }
    }

    void DisplayGrid() const {
    	for(int i = 0; i < height; i++) {
    		for(int j = 0; j < width; j++) {
    			PrintOut(distances[LinearIndex(i, j)], " ");
    		}
    		PrintOut("\n");
    	}
    }

    void Reconstruct() {
        int x = endingNode.first;
        int y = endingNode.second;

        map<int, char> directions({
            {0, 'R'},
            {1, 'L'},
            {2, 'D'},
            {3, 'U'}
        });

        while(x != startingNode.first || y != startingNode.second) {
            //const auto neighbourNodes = ComputeNeighbouringNodes(y, x);

            array<pair<int, int>, 4> neighbourNodes = {
                {
                    {
                        ComputeMinimumHorizontalLeftBound(x),
                        y
                    },

                    {
                        ComputeMaximumHorizontalRightBound(x),
                        y
                    },

                    {
                        x,
                        ComputeMaximumVerticalUpperBound(y)
                    },

                    {
                        x,
                        ComputeMinimumVerticalLowerBound(y)
                    }
                }
            };

            for (int i = 0; i < 4; i++) {

                int x0 = neighbourNodes[i].first;
                int y0 = neighbourNodes[i].second;

                if (CellIsWall(y0, x0)) continue;
                if (distances[LinearIndex(y0, x0)] == distances[LinearIndex(y, x)] - 1) {
                    path.push(directions[i]);
                    x = x0;
                    y = y0;
 
                    break;
                }

            }
        }
    }

    void ShowPath() {
        while(!path.empty()){
            auto direction = path.top();
            path.pop();
            PrintOut(direction);
        }
    }

    void Check() {
        if (distances[LinearIndex(endingNode.second, endingNode.first)] != height*width + 1) {
            PrintOut("YES", "\n");
            PrintOut(distances[LinearIndex(endingNode.second, endingNode.first)], "\n");
            Reconstruct();
            ShowPath();
        }
        else
            PrintOut("NO");
    }

private:
    int height;
    int width;
    size_t size;

    BitVector adjacentEdges;
    BitVector visitedNodes;
    vector<int> distances;
    priority_queue<pair<int, pair<int, int>>> adjacentDistances;

    pair<int, int> startingNode;
    pair<int, int> endingNode;

    stack<char> path;

    inline size_t LinearIndex(const int y, const int x) const {
        return x + y * width;
    }

    void AssignCellValue(const char cell, const int y, const int x) {
        switch(cell) {
            case '.':
                MarkFreeSpaceOnTheMap(y, x);
                break;
            case 'A':
                MarkFreeSpaceOnTheMap(y, x);
                startingNode = {x, y};
                break;
            case 'B':
                MarkFreeSpaceOnTheMap(y, x);
                endingNode = {x, y};
                break;
            case '#':
                break;
            default:
                throw invalid_argument("Incorrect input!");
        }
    }

    inline void MarkFreeSpaceOnTheMap(const int y, const int x) {
        adjacentEdges.SetBit(LinearIndex(y, x));
    }

    inline size_t NodeHasBeenVisited(const int y, const int x) const {
        return visitedNodes.GetBit(LinearIndex(y, x));
    }

    inline size_t CellIsWall(const int y, const int x) const {
        return !adjacentEdges.GetBit(LinearIndex(y, x));
    }

    inline void VisitNode(const int y, const int x) {
        visitedNodes.SetBit(LinearIndex(y, x));;
    }

    inline const set<pair<int, int>> ComputeNeighbouringNodes(const int y, const int x) const {

        set<pair<int, int>> neighbourNodes(
            {
                {
                    ComputeMinimumHorizontalLeftBound(x),
                    y
                },

                {
                    ComputeMaximumHorizontalRightBound(x),
                    y
                },

                {
                    x,
                    ComputeMaximumVerticalUpperBound(y)
                },

                {
                    x,
                    ComputeMinimumVerticalLowerBound(y)
                }
            }
        );

        return neighbourNodes;
    }

    inline int ComputeMinimumHorizontalLeftBound(const int x) const {
        return max(x - 1, 0);
    }

    inline int ComputeMaximumHorizontalRightBound(const int x) const {
        return min(x + 1, width - 1);
    }

    inline int ComputeMaximumVerticalUpperBound(const int y) const {
        return max(y - 1, 0);
    }

    inline int ComputeMinimumVerticalLowerBound(const int y) const {
        return min(y + 1, height - 1);
    }
};

void run() {
    size_t n, m; 
    cin >> n >> m;

    Graph Grid(n, m);

    Grid.Init();
    Grid.SearchShortestPath();
    Grid.Check();
}

int main() {
    try {
        run();
    } catch (const invalid_argument& exception) {
        cerr << exception.what() << "\n";
        return -1;
    }

    return 0;
}