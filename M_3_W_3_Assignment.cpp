#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <iomanip>
using namespace std;

        // define representation of a graph using the adjancency matrix
class Graph {
private:
    int numNodes;
    vector<vector<double>> adjMatrix;

        // constructor
public:
    Graph(int numNodes) : numNodes(numNodes) {
        adjMatrix.resize(numNodes, vector<double>(numNodes, -1.0));
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
};
        // generates a random graph
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
        //Computes the average of the shortest path between nodes of graph
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

        // Densities to be tested
    const double densities[] = { 0.2, 0.4 };
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

    return 0;
}
