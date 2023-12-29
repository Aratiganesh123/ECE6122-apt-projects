#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <cmath>


double integralFunction1(double x) {
    return x * x;
}


double integralFunction2(double x) {
    return std::exp(-x * x);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Check for proper argument count
    if (argc != 5) {
        if (world_rank == 0) {
            std::cerr << "Usage: " << argv[0] << " -P [1|2] -N <number_of_samples>\n";
        }
        MPI_Finalize();
        return 1;
    }

    // Parse command line arguments
    int P = -1, N = -1;
    for (int i = 1; i < argc; i += 2) {
        if (std::string(argv[i]) == "-P") {
            P = std::atoi(argv[i + 1]);
        } else if (std::string(argv[i]) == "-N") {
            N = std::atoi(argv[i + 1]);
        }
    }

    if ((P != 1 && P != 2) || N <= 0) {
        if (world_rank == 0) {
            std::cerr << "Invalid arguments. -P should be 1 or 2, -N should be positive.\n";
        }
        MPI_Finalize();
        return 1;
    }

    
    double local_sum = 0.0;
    srand(time(NULL) + world_rank); 

    // Monte Carlo simulation
    for (int i = world_rank; i < N; i += world_size) {
        double x = static_cast<double>(rand()) / RAND_MAX;
        local_sum += (P == 1) ? integralFunction1(x) : integralFunction2(x);
    }

    double global_sum;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

 
    if (world_rank == 0) {
        double integral = (global_sum / N);
        std::cout << "The estimate for integral " << P << " is " << integral << std::endl;
        std::cout << "Bye!" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
