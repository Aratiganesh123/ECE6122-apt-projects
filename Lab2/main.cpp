/*

Author: Arati Ganesh
Class: ECE6122 
Last Date Modified: 10/09/2023
Description: OpenMP implementation of Electric Field Calculation using user input # of threads

*/

#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <cmath>
#include <limits>
#include <iomanip>

#include "ECE_ElectricFieldUtils.h"

constexpr double K = 9e9;             // Coulomb's Constant
constexpr double microC_to_C = 1e-6;  // Conversion factor from µC to C

// Function to calculate the electric field at a point (x, y, z) due to a charge at (px, py, pz)
void electricFieldCalculation(double x, double y, double z, double px, double py, double pz, double q,
                              double& Ex, double& Ey, double& Ez) {

    double r = pow(pow(x - px, 2) + pow(y - py, 2) + pow(z - pz, 2), 1.5);
    Ex = K * q * microC_to_C *(x - px) / r;
    Ey = K * q * microC_to_C * (y - py) / r;
    Ez = K * q * microC_to_C * (z - pz) / r;
}

int main() {
    bool continueCalculations = true;
    int numThreads;
    std::cout << "Please enter the number of concurrent threads to use:";
    std::cin >> numThreads;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    while (continueCalculations) {
        int N, M;
        std::vector<int> gridDim;
        getInput<int>("Please enter the number of rows and columns (N x M): ", gridDim, 2, "Grid Dimensions should be natural numbers!", validateBounds);
        N = gridDim[0];
        M = gridDim[1];

        double xSeparation, ySeparation;
        std::vector<double> separationDist;
        getInput("Please enter the x and y separation distances in meters: ", separationDist, 2, "(N x M) separation distance values must be > 0!", validateBounds);
        xSeparation = separationDist[0];
        ySeparation = separationDist[1];

        double chargeVal;
        std::vector<double> charges;
        getInput("Please enter the common charge on the points in micro C: ", charges, 1, "(N x M) separation distance values must be > 0!", validateBounds);
        chargeVal = charges[0];

        auto grid = calculateGridCoordinates(N, M, xSeparation, ySeparation);
        //printGrid(grid , N , M);

        Point3D eFieldPoint;
        std::vector<double> electricFieldPoint;
        getInput<double>("Please enter the location in space to determine the electric field (x y z) in meters: ", electricFieldPoint, 3, "Overlap detected with the user-provided electric field point.", validateOverlap, grid);
        eFieldPoint.x = electricFieldPoint[0];
        eFieldPoint.y = electricFieldPoint[1];
        eFieldPoint.z = electricFieldPoint[2];

        double totalEx = 0.0;
        double totalEy = 0.0;
        double totalEz = 0.0;
        double E = 0.0;
        
        std::chrono::high_resolution_clock::time_point start, stop;
        std::chrono::microseconds duration;

        // OpenMP parallel region
        #pragma omp parallel num_threads(numThreads)
        {
            // Measure start time (only the master thread)
            #pragma omp master
            {
                start = std::chrono::high_resolution_clock::now();
            }

            // Parallel for loop with reduction clause
            #pragma omp for reduction(+:totalEx, totalEy, totalEz) schedule(static)
            for (int i = 0; i < M; i++) {       // Iterate through rows (M rows)
                for (int j = 0; j < N; j++) {  // Iterate through columns (N columns)
                    double Ex, Ey, Ez;
                    electricFieldCalculation(grid[i][j].x, grid[i][j].y, grid[i][j].z, eFieldPoint.x, eFieldPoint.y, eFieldPoint.z, chargeVal, Ex, Ey, Ez);
                    totalEx += Ex;
                    totalEy += Ey;
                    totalEz += Ez;
                }
            }

            // Measure end time (only the master thread)
            #pragma omp master
            {
                E = sqrt(totalEx * totalEx + totalEy * totalEy + totalEz * totalEz);
                stop = std::chrono::high_resolution_clock::now();
                duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
                std::cout << "The calculation took " << duration.count() << " microseconds!" << std::endl;
            }
        }

        int precision = 4;
        printScientificNotation("Ex", totalEx, precision);
        printScientificNotation("Ey", totalEy, precision);
        printScientificNotation("Ez", totalEz, precision);
        printScientificNotation("|E|", E, precision);
        std::cout << "The calculation took " << duration.count() << " microseconds!" << std::endl;

        char continueChoice;
        std::cout << "Do you want to enter a new location (Y/N)? ";
        std::cin >> continueChoice;

        // Clear any remaining characters in the input buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (continueChoice != 'Y' && continueChoice != 'y') {
            continueCalculations = false;
        }
    }

    return 0;
}
