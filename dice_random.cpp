//the following program computes the probaability for dice possibilities

#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

const int sides = 6;

inline int r_sides() { return (rand() % sides + 1); }

int main() {
    const int n_dice = 2;
    int d1, d2;
    srand(time(0));  // Seed the random number generator with the current time

    cout << "Enter number of trials: ";
    int trials;
    cin >> trials;

    int* outcomes = new int[n_dice * sides + 1]();  // Initialize array to 0

    for (int j = 0; j < trials; ++j)
        outcomes[(d1 = r_sides()) + (d2 = r_sides())]++;

    cout << "Probability\n";

    for (int j = 2; j < n_dice * sides + 1; ++j)
        cout << "Sum = " << j << ", Probability = " << static_cast<double>(outcomes[j]) / trials << endl;

    // Release the allocated memory
    delete[] outcomes;

    return 0;
}
