#include <stdio.h>
#include <curand_kernel.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>

#define NORTH 0
#define SOUTH 1
#define EAST  2
#define WEST  3

// Kernel function for performing 2D random walks with directions
__global__ void random_walks(int* x_walks, int* y_walks, int num_walks, int num_steps, unsigned int seed) {

    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    curandState state;
    curand_init(seed, tid, 0, &state);  // Initialize random number generator state for each thread

    int x = 0;  // Start at the origin on the x-axis
    int y = 0;  // Start at the origin on the y-axis

    for (int step = 0; step < num_steps; step++) {
        int random_direction = curand(&state) % 4;  

        if (random_direction == NORTH) {
            y++;
        } else if (random_direction == SOUTH) {
            y--;
        } else if (random_direction == EAST) {
            x++;
        } else if (random_direction == WEST) {
            x--;
        }
    }

    x_walks[tid] = x;
    y_walks[tid] = y;
}

// Calculate the average distance of walkers from the origin
double calculateAverageDistance(int* x_walks, int* y_walks, int num_walks) {
    double total_distance = 0.0;

    for (int i = 0; i < num_walks; i++) {
        double distance = sqrt(x_walks[i] * x_walks[i] + y_walks[i] * y_walks[i]);
        total_distance += distance;
    }

    return total_distance / num_walks;
}

// Allocate memory and perform random walks using standard CUDA memory allocation
double normalMemoryAllocation(int num_walks, int num_steps, unsigned int seed, int threadsPerBlock, int blocksPerGrid) {
   
    int* d_x_walks;
    int* d_y_walks;
    int x_walks[num_walks];
    int y_walks[num_walks];

    // Allocate device memory for x and y coordinates
    cudaMalloc((void**)&d_x_walks, num_walks * sizeof(int));
    cudaMalloc((void**)&d_y_walks, num_walks * sizeof(int));

    // Launch the random walk kernel
    random_walks<<<blocksPerGrid, threadsPerBlock>>>(d_x_walks, d_y_walks, num_walks, num_steps, seed);

    // Copy the results back to the host
    cudaMemcpy(x_walks, d_x_walks, num_walks * sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(y_walks, d_y_walks, num_walks * sizeof(int), cudaMemcpyDeviceToHost);

    double average_distance = calculateAverageDistance(x_walks, y_walks, num_walks);

    // Free device memory
    cudaFree(d_x_walks);
    cudaFree(d_y_walks);

    return average_distance;
}

// Allocate memory and perform random walks using pinned CUDA memory allocation
double pinnedMemoryAllocation(int num_walks, int num_steps, unsigned int seed, int threadsPerBlock, int blocksPerGrid) {
   
    int* d_x_walks;
    int* d_y_walks;
    int* x_walks;
    int* y_walks;

    // Allocate pinned memory for x and y coordinates on the host
    cudaMallocHost((void**)&x_walks, num_walks * sizeof(int));
    cudaMallocHost((void**)&y_walks, num_walks * sizeof(int));

    // Allocate device memory for x and y coordinates
    cudaMalloc((void**)&d_x_walks, num_walks * sizeof(int));
    cudaMalloc((void**)&d_y_walks, num_walks * sizeof(int));

    // Launch the random walk kernel
    random_walks<<<blocksPerGrid, threadsPerBlock>>>(d_x_walks, d_y_walks, num_walks, num_steps, seed);

    // Copy the results back to the host
    cudaMemcpy(x_walks, d_x_walks, num_walks * sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(y_walks, d_y_walks, num_walks * sizeof(int), cudaMemcpyDeviceToHost);

    double average_distance = calculateAverageDistance(x_walks, y_walks, num_walks);

    // Free device memory
    cudaFree(d_x_walks);
    cudaFree(d_y_walks);

    // Free pinned memory on the host
    cudaFreeHost(x_walks);
    cudaFreeHost(y_walks);

    return average_distance;
}

// Allocate memory and perform random walks using unified CUDA memory allocation
double unifiedMemoryAllocation(int num_walks, int num_steps, unsigned int seed, int threadsPerBlock, int blocksPerGrid) {
    
    int* x_walks;
    int* y_walks;

    // Allocate unified memory for x and y coordinates
    cudaMallocManaged((void**)&x_walks, num_walks * num_steps * sizeof(int));
    cudaMallocManaged((void**)&y_walks, num_walks * num_steps * sizeof(int));

    // Launch the random walk kernel
    random_walks<<<blocksPerGrid, threadsPerBlock>>>(x_walks, y_walks, num_walks, num_steps, seed);

    // Ensure that the kernel has finished executing
    cudaDeviceSynchronize();

    double average_distance = calculateAverageDistance(x_walks, y_walks, num_walks);

    // Free unified memory (no need to differentiate between host and device)
    cudaFree(x_walks);
    cudaFree(y_walks);

    return average_distance;
}

int main(int argc, char* argv[]) {
    unsigned int seed = time(NULL);
    int numWalkers = 0;
    int totalSteps = 0;

    std::string programName = argv[0];
    std::vector<std::string> arguments(argv + 1, argv + argc);

    for (size_t i = 0; i < arguments.size(); i++) {
        if (arguments[i] == "-W" && i + 1 < arguments.size()) {
            numWalkers = std::stoi(arguments[i + 1]);
        } else if (arguments[i] == "-I" && i + 1 < arguments.size()) {
            totalSteps = std::stoi(arguments[i + 1]);
        } else if (arguments[i] == "-H") {
            std::cerr << "Usage: " << programName << " [-W <numWalkers>] [-I <totalSteps] [-H]" << std::endl;
            return 1;
        }
    }

    // Set default values if no input was given
    if (numWalkers == 0) {
        numWalkers = 1000;
    }
    if (totalSteps == 0) {
        totalSteps = 10000;
    }

    //std::cout << "Number of Walkers: " << numWalkers << ", Total Steps: " << totalSteps << std::endl;

    // Set the number of threads per block and the number of blocks
    int threadsPerBlock = 256;
    int blocksPerGrid = (numWalkers + threadsPerBlock - 1) / threadsPerBlock;

    //Warmup time 
    auto warmup_avg_time = normalMemoryAllocation(numWalkers, totalSteps, seed, threadsPerBlock, blocksPerGrid);

    // Timer for normal memory allocation
    auto start = std::chrono::high_resolution_clock::now();
    auto normal_avg_time = normalMemoryAllocation(numWalkers, totalSteps, seed, threadsPerBlock, blocksPerGrid);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // Output for Normal CUDA memory Allocation
    std::cout << "Normal CUDA memory Allocation:" << std::endl;
    std::cout << std::setw(4) << "    Time to calculate(microsec): " << elapsed.count() << std::endl;
    std::cout << std::setw(4) << "    Average distance from origin: " << normal_avg_time << std::endl;

    // Timer for pinned memory allocation
    start = std::chrono::high_resolution_clock::now();
    auto pinned_avg_time = pinnedMemoryAllocation(numWalkers, totalSteps, seed, threadsPerBlock, blocksPerGrid);
    end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // Output for Pinned CUDA memory Allocation
    std::cout << "Pinned CUDA memory Allocation:" << std::endl;
    std::cout << std::setw(4) << "    Time to calculate(microsec): " << elapsed.count() << std::endl;
    std::cout << std::setw(4) << "    Average distance from origin: " << pinned_avg_time << std::endl;

    // Timer for unified memory allocation
    start = std::chrono::high_resolution_clock::now();
    auto unified_avg_time = unifiedMemoryAllocation(numWalkers, totalSteps, seed, threadsPerBlock, blocksPerGrid);
    end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // Output for Managed CUDA memory Allocation
    std::cout << "Managed CUDA memory Allocation:" << std::endl;
    std::cout << std::setw(4) << "    Time to calculate(microsec): " << elapsed.count() << std::endl;
    std::cout << std::setw(4) << "    Average distance from origin: " << unified_avg_time << std::endl;

    std::cout << "Bye" << std::endl;

    return 0;
}
