#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <iomanip>
#include <fstream>
#include <queue>
using namespace std;

    // define representation of a graph using the adjancency matrix
class Graph {
private:
    int numNodes;
    vector<vector<double>> adjMatrix;

public:
    Graph(int numNodes) : numNodes(numNodes) {
        adjMatrix.resize(numNodes, vector<double>(numNodes, -1.0));
    }

    // Constructor to read graph from a file
    Graph(const string& filename) {
        ifstream inputFile(filename);
        if (!inputFile.is_open()) {
            cerr << "Error: Could not open file " << filename << endl;
            exit(EXIT_FAILURE);
        }

        inputFile >> numNodes;
        adjMatrix.resize(numNodes, vector<double>(numNodes, -1.0));

        int u, v;
        double distance;
        while (inputFile >> u >> v >> distance) {
            if (u >= numNodes || v >= numNodes || u < 0 || v < 0) {
                cerr << "Error: Invalid node index in the file." << endl;
                exit(EXIT_FAILURE);
            }

            adjMatrix[u][v] = distance;
            adjMatrix[v][u] = distance;
        }

        inputFile.close();
    }

    void addEdge(int u, int v, double distance) {
        adjMatrix[u][v] = distance;
        adjMatrix[v][u] = distance;
    }

    double getDistance(int u, int v) const {
        return adjMatrix[u][v];
    }

    int getNumNodes() const {
        return numNodes;
    }

    vector<pair<int, int>> primMST() const {
        vector<bool> visited(numNodes, false);
        vector<pair<int, int>> minSpanningTree;

        priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
        pq.push({0, 0});

        while (!pq.empty()) {
            pair<double, int> top = pq.top();
            pq.pop();

            int u = top.second;
            double weight = top.first;

            if (visited[u])
                continue;

            visited[u] = true;

            if (u != 0)
                minSpanningTree.push_back({u, u});

            for (int v = 0; v < numNodes; ++v) {
                if (adjMatrix[u][v] >= 0 && !visited[v]) {
                    pq.push({adjMatrix[u][v], v});
                }
            }
        }

        return minSpanningTree;
    }
};

void generateRandomGraph(Graph& graph, double density, double minDistance, double maxDistance) {
    srand(time(0));
    int numNodes = graph.getNumNodes();

    for (int i = 0; i < numNodes; ++i) {
        for (int j = i + 1; j < numNodes; ++j) {
            double randProb = static_cast<double>(rand()) / RAND_MAX;

            if (randProb < density) {
                double randDistance = minDistance + static_cast<double>(rand()) / RAND_MAX * (maxDistance - minDistance);
                graph.addEdge(i, j, randDistance);
            }
        }
    }
}

double computeAverageShortestPath(const Graph& graph, int startNode) {
    int numNodes = graph.getNumNodes();
    double totalPathLength = 0.0;
    int validPathsCount = 0;

    for (int i = 0; i < numNodes; ++i) {
        if (i != startNode) {
            double distance = graph.getDistance(startNode, i);

            if (distance >= 0) {
                totalPathLength += distance;
                validPathsCount++;
            }
        }
    }

    if (validPathsCount == 0)
        return 0.0;

    return totalPathLength / validPathsCount;
}

int main() {
    const int numNodes = 50;
    const double minDistance = 1.0;
    const double maxDistance = 10.0;
    const double densities[] = {0.2, 0.4};
    const int numDensities = sizeof(densities) / sizeof(densities[0]);

    for (int d = 0; d < numDensities; ++d) {
        double density = densities[d];
        double averagePathLengthSum = 0.0;

        cout << "Density: " << fixed << setprecision(2) << density << "\n";

        for (int i = 0; i < numNodes; ++i) {
            Graph graph(numNodes);
            generateRandomGraph(graph, density, minDistance, maxDistance);

            double averagePathLength = computeAverageShortestPath(graph, 0);
            averagePathLengthSum += averagePathLength;
        }

        double averagePathLength = averagePathLengthSum / numNodes;
        cout << "Average Shortest Path Length: " << fixed << setprecision(2) << averagePathLength << "\n\n";
    }

    // Example usage of reading a graph from a file and computing MST
    Graph graphFromFile("example_graph.txt"); //add your file here
    cout << "Minimum Spanning Tree (Prim's Algorithm):\n";
    vector<pair<int, int>> mst = graphFromFile.primMST();
    double mstCost = 0;
    for (const auto& edge : mst) {
        cout << "Edge: " << edge.first << " - " << edge.second << "\n";
        mstCost += graphFromFile.getDistance(edge.first, edge.second);
    }
    cout << "Total MST Cost: " << mstCost << "\n";

    return 0;
}
