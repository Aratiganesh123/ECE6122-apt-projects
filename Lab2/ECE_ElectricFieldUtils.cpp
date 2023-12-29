/*

Author: Arati Ganesh
Class: ECE6122 
Last Date Modified: 10/09/2023
Description: Contains all the utility functions related to E.Field Calculation such as 
grid creation, Checking for overlap , Calculating the thread work distribution etc

*/

#include "ECE_ElectricFieldUtils.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>



// Calculate grid coordinates based on parameters
std::vector<std::vector<Point3D>> calculateGridCoordinates(int N, int M, double x_sep, double y_sep) {
    // Calculate the upper left corner coordinates
    double upper_left_x = -(N - 1) * x_sep / 2;
    double upper_left_y = (M - 1) * y_sep / 2;

    // Initialize a vector to store the coordinates of all points
    std::vector<std::vector<Point3D>> coordinates(M, std::vector<Point3D>(N));  // Create an MxN grid

    // Iterate rows
    for (int i = 0; i < M; i++) {
        double y = upper_left_y - i * y_sep;
        // Iterate columns
        for (int j = 0; j < N; j++) {
            double x = upper_left_x + j * x_sep;
            Point3D point = {x, y, 0.0}; // Initialize z-coordinate to 0
            coordinates[i][j] = point;
        }
    }

    return coordinates;
}


// Print the grid coordinates
void printGrid(const std::vector<std::vector<Point3D>>& coordinates, int N, int M) {
    for (int i = 0; i < M; i++) {  // Iterate through rows (M rows)
        for (int j = 0; j < N; j++) {  // Iterate through columns (N columns)
            std::cout << "(" << coordinates[i][j].x << "  " << coordinates[i][j].y << "  " << coordinates[i][j].z << ") ";
        }
        std::cout << std::endl;  // Move to the next row
    }
}


// Check if a user-defined point overlaps with any grid points
bool validateOverlap(const Point3D& userPoint, const std::vector<std::vector<Point3D>>& gridPoints) {
    for (const auto& row : gridPoints) {
        for (const auto& gridPoint : row) {
            if (userPoint.x == gridPoint.x && userPoint.y == gridPoint.y) {
                std::cout << gridPoint.x << " " << gridPoint.y << std::endl;
                return true; // Found an overlap
            }
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

