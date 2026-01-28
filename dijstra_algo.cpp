#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <queue>
using namespace std;

// Define PriorityQueue class as per the provided abstraction
class PriorityQueue {
private:
    struct QueueElement {
        int vertex;
        double priority;

        QueueElement(int v, double p) : vertex(v), priority(p) {}
        bool operator>(const QueueElement& other) const {
            return priority > other.priority;
        }
    };

    std::priority_queue<QueueElement, std::vector<QueueElement>, std::greater<QueueElement>> pq;

public:
    void insert(int vertex, double priority) {
        pq.push(QueueElement(vertex, priority));
    }

    int getTop() {
        return pq.top().vertex;
    }

    double getTopPriority() {
        return pq.top().priority;
    }

    void pop() {
        pq.pop();
    }

    bool empty() const {
        return pq.empty();
    }
};

class Graph {
private:
    int num_vertices;
    vector<vector<double>> adjacency_matrix;

public:
    Graph(int vertices) {
        num_vertices = vertices;
        adjacency_matrix.resize(vertices, vector<double>(vertices, numeric_limits<double>::infinity()));
        for (int i = 0; i < vertices; ++i)
            adjacency_matrix[i][i] = 0;
    }

    void addEdge(int x, int y, double distance) {
        if (x < 0 || x >= num_vertices || y < 0 || y >= num_vertices)
            throw out_of_range("Invalid vertex index");

        adjacency_matrix[x][y] = distance;
        adjacency_matrix[y][x] = distance;
    }

    void generateRandomGraph(double edge_density, double min_distance, double max_distance) {
        // Seed for random number generation
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<double> distance_distribution(min_distance, max_distance);
        uniform_real_distribution<double> density_distribution(0, 1);

        for (int i = 0; i < num_vertices; ++i) {
            for (int j = i + 1; j < num_vertices; ++j) {
                if (density_distribution(gen) < edge_density) {
                    double distance = distance_distribution(gen);
                    addEdge(i, j, distance);
                }
            }
        }
    }
        //dijstra algorythm
    vector<double> dijkstraShortestPath(int source) {
        vector<double> distance(num_vertices, numeric_limits<double>::infinity());
        vector<bool> visited(num_vertices, false);
        PriorityQueue pq;

        distance[source] = 0;
        pq.insert(source, 0);

        while (!pq.empty()) {
            int current_vertex = pq.getTop();
            double current_distance = pq.getTopPriority();
            pq.pop();

            if (visited[current_vertex])
                continue;

            visited[current_vertex] = true;

            for (int j = 0; j < num_vertices; ++j) {
                if (!visited[j] && adjacency_matrix[current_vertex][j] != numeric_limits<double>::infinity()) {
                    double new_distance = current_distance + adjacency_matrix[current_vertex][j];
                    if (new_distance < distance[j]) {
                        distance[j] = new_distance;
                        pq.insert(j, new_distance);
                    }
                }
            }
        }

        return distance;
    }
};

int main() {
    // Example usage
    int num_nodes = 50;
    double edge_density = 0.1;  // Example edge density
    double min_distance = 1.0;
    double max_distance = 10.0;

    Graph graph(num_nodes);
    graph.generateRandomGraph(edge_density, min_distance, max_distance);

    // Perform Dijkstra's algorithm on the graph
    int source_vertex = 0;  // Example source vertex
    vector<double> shortest_distances = graph.dijkstraShortestPath(source_vertex);

    // Print the shortest distances from the source vertex to all other vertices
    cout << "Shortest distances from vertex " << source_vertex << ":\n";
    for (int i = 0; i < num_nodes; ++i) {
        cout << "To vertex " << i << ": " << shortest_distances[i] << "\n";
    }

    return 0;
}
