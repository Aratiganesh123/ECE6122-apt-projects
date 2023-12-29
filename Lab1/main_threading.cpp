/*
Author: Arati Ganesh
Class: ECE6122 
Last Date Modified: 24/09/2023
Description: Contains the driver code. Prompts the user to enter inputs, calculates grid coordinates, distributes work between threads and launches them.
*/

#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <mutex>

#include "ECE_ElectricField.h"
#include "ECE_PointCharge.h"
#include "ECE_ElectricFieldUtils.h"


std::mutex mtx;
// Function to calculate electric fields in parallel for a range of points
void calculateElectricFields(int threadIndex, std::vector<ECE_ElectricField> electricFields, Point3D eFieldPoint, int startIdx, int endIdx, std::vector<ThreadResult>& partialResult) {
    for (int i = startIdx; i < endIdx; ++i) {
        electricFields[i].computeFieldAt(eFieldPoint.x, eFieldPoint.y, eFieldPoint.z);
        double Ex, Ey, Ez;
        electricFields[i].getElectricField(Ex, Ey, Ez);
        partialResult[threadIndex].Ex += Ex;
        partialResult[threadIndex].Ey += Ey;
        partialResult[threadIndex].Ez += Ez;
    }
}

int main() {
    bool continueCalculations = true;
    int numThreads = std::thread::hardware_concurrency();
    while (continueCalculations) {
        

        std::cout << "Your computer supports " << numThreads << " concurrent threads" << std::endl;
       

        int N, M;
        std::vector<int> gridDim;
        getInput<int>("Please enter the number of rows and columns in the N x M array: ", gridDim, 2, "Grid Dimensions should be natural numbers!", validateBounds);
        N = gridDim[0];
        M = gridDim[1];

        double xSeparation, ySeparation;
        std::vector<double> separationDist;
        getInput<double>("Please enter the x and y separation distances in meters: ", separationDist, 2, "(N x M) separation distance values must be > 0!", validateBounds);
        xSeparation = separationDist[0];
        ySeparation = separationDist[1];

        double chargeVal;
        std::vector<double> charges;
        getInput<double>("Please enter the common charge on the points in micro C: ", charges, 1, "Point Charge value should be > 0.0!", validateBounds);
        chargeVal = charges[0];

        auto grid = calculateGridCoordinates(N, M, xSeparation, ySeparation);
        //printGrid(grid , N , M);
        auto threadRanges = calculateDataDistribution(N*M , numThreads);

        Point3D eFieldPoint;
        std::vector<double> electricFieldPoint;

        // Calculate data distribution for threads
        std::vector<ECE_ElectricField> electricFields;
        for (const auto& point : grid) {
            ECE_ElectricField electricField(point.x, point.y, point.z, chargeVal);
            electricFields.push_back(electricField);
        }

        getInput<double>("Please enter the location in space to determine the electric field (x y z) in meters: ", electricFieldPoint, 3, "Overlap detected with the user-provided electric field point.", validateOverlap, grid);
        eFieldPoint.x = electricFieldPoint[0];
        eFieldPoint.y = electricFieldPoint[1];
        eFieldPoint.z = electricFieldPoint[2];

        auto start = std::chrono::high_resolution_clock::now();
        // Create and run threads
        std::vector<std::thread> threads(threadRanges.size());
        std::vector<ThreadResult> threadResults(threadRanges.size());

        for (int i = 0; i < threadRanges.size(); ++i) {
            threads[i] = std::thread(calculateElectricFields, i, electricFields, eFieldPoint,
                                threadRanges[i].start, threadRanges[i].end, std::ref(threadResults));
        }

        // Join threads and accumulate results
        double totalEx = 0.0;
        double totalEy = 0.0;
        double totalEz = 0.0;
        for (int i = 0; i < threads.size(); ++i) {
            threads[i].join();
            totalEx += threadResults[i].Ex;
            totalEy += threadResults[i].Ey;
            totalEz += threadResults[i].Ez;
        }

        // Calculate |E|
        double E = sqrt(totalEx * totalEx + totalEy * totalEy + totalEz * totalEz);

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

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

    
    // std::cout << "(N , M) : " << "(" << N <<  "," << M << ")" << std::endl;
    // std::cout << "(XSeparation , ySeparation) : " << "(" << xSeparation <<  "," << ySeparation << ")" << std::endl;
    // std::cout << "Charge in micro C : " << chargeVal << std::endl;
    // std::cout << "(Px , Py , Pz) : " << "(" << eFieldPoint.x << "," << eFieldPoint.y << "," << eFieldPoint.z << ")" << std::endl;

    }

    return 0;
}
