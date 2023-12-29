/*

Author: Arati Ganesh
Class: ECE6122 
Last Date Modified: 24/09/2023
Description: Contains all the utility functions related to E.Field Calculation such as 
grid creation, Checking for overlap , Calculating the thread work distribution etc

*/

#include "ECE_ElectricFieldUtils.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>



// Calculate grid coordinates based on parameters
std::vector<Point3D> calculateGridCoordinates(int N, int M, double x_sep, double y_sep) {
    // Calculate the upper left corner coordinates
    double upper_left_x = -(N - 1) * x_sep / 2;
    double upper_left_y = (M - 1) * y_sep / 2;

    // Initialize a vector to store the coordinates of all points
    std::vector<Point3D> coordinates;

    // Iterate rows
    for (int i = 0; i < M; i++) {
        double y = upper_left_y - i * y_sep;
        // Iterate columns
        for (int j = 0; j < N; j++) {
            double x = upper_left_x + j * x_sep;
            Point3D point = {x, y, 0.0}; // Initialize z-coordinate to 0
            coordinates.push_back(point);
        }
    }

    return coordinates;
}

// Print the grid coordinates
void printGrid(const std::vector<Point3D>& coordinates, int N, int M) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            int index = i * M + j;
            std::cout << "(" << coordinates[index].x << "  " << coordinates[index].y << "  " << coordinates[index].z << ") ";
        }
        std::cout << std::endl;
    }
}

// Check if a user-defined point overlaps with any grid points
bool validateOverlap(const Point3D& userPoint, const std::vector<Point3D>& gridPoints) {
    for (const auto& gridPoint : gridPoints) {
        if (userPoint.x == gridPoint.x && userPoint.y == gridPoint.y && userPoint.z == gridPoint.z) {
            std::cout << gridPoint.x << " " << gridPoint.y << " " << gridPoint.z << std::endl;
            return true; // Found an overlap
        }
    }
    return false; // No overlap found
}

// Print a value in scientific notation
void printScientificNotation(const std::string& label, double value, int precision) {
    if (value == 0.0) {
        std::cout << label << " = " << std::fixed << std::setprecision(precision) << value << " * 10^0" << std::endl;
    } else {
        double absValue = fabs(value);
        int exponent = static_cast<int>(log10(absValue));
        double mantissa = absValue / pow(10, exponent);
        std::cout << label << " = " << std::fixed << std::setprecision(precision) << mantissa << " * 10^" << exponent << std::endl;
    }
}

// Calculate data distribution for parallel processing
std::vector<ThreadRange> calculateDataDistribution(int totalDataPoints, int maxConcurrency) {
    int numThreads = std::min(maxConcurrency, totalDataPoints);

    // Calculate the number of data points per thread and the remaining data points
    int dataPointsPerThread = totalDataPoints / numThreads;
    int remainingDataPoints = totalDataPoints % numThreads;

    // Create a vector to store the start and end indices for each thread
    std::vector<ThreadRange> threadRanges(numThreads);

    int currentIndex = 0;

    for (int i = 0; i < numThreads; ++i) {
        threadRanges[i].start = currentIndex;

        // Distribute the remaining data points from the top
        int extraDataPoints = (i < remainingDataPoints) ? 1 : 0;
        currentIndex += dataPointsPerThread + extraDataPoints;

        threadRanges[i].end = currentIndex;
    }

    return threadRanges;
}
