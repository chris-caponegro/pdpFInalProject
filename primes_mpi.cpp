#include <mpi.h>
#include <iostream>
#include <cmath>
#include <chrono>  // For timing

using namespace std;
using namespace chrono;

// Function to check if a number is prime
bool is_prime(int n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (int i = 3; i <= sqrt(n); i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

int main(int argc, char** argv) {
    int rank, size;
    int n = 1000000;
    int local_count = 0, total_count = 0;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    auto start_time = high_resolution_clock::now();

    // Calculate the range for each node
    int start = (rank * n / size) + (rank == 0 ? 2 : 1);
    int end = (rank + 1) * n / size;

    // Each node counts primes in its range
    for (int i = start; i <= end; i++) {
        if (is_prime(i)) {
            local_count++;
        }
    }

    // Reduce local counts to the master node (rank 0)
    MPI_Reduce(&local_count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // End the timer (only on the master node)
    if (rank == 0) {
        auto end_time = high_resolution_clock::now();
        double total_duration = duration<double>(end_time - start_time).count();

        // Output the results
        cout << "Total number of primes up to " << n << ": " << total_count << endl;
        cout << "Total execution time: " << total_duration << " seconds" << endl;
    }

    MPI_Finalize();
    return 0;
}
