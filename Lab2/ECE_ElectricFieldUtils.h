/*
Author: Arati Ganesh
Class: ECE6122 
Last Date Modified: 10/09/2023
Description: Contains utility functions related to E.Field Calculation such as grid creation, overlap checking, thread work distribution, and validation functions.
*/

#pragma once

#include <vector>
#include <iostream>
#include <sstream>
#include <functional>
#include <cmath>
#include <iomanip>
#include <mutex>


// Structure to represent a 3D point
struct Point3D {
    double x;
    double y;
    double z;
};

// Structure to represent a range of data points assigned to a thread
struct ThreadRange {
    int start;
    int end;
};

// Structure to accumulate the Electric Field results
struct ThreadResult {
    double Ex;
    double Ey;
    double Ez;
};



// Function to split a string into a vector of values
template<typename T>
bool splitInputs(const std::string& s, char delimiter, std::vector<T>& results) {
    bool conversionSuccessful = true;

    if (s.empty()) {
        return false;  // Input string is empty
    }

    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        for (char c : token) {
            if (!std::isdigit(c) && c != '-' && c != '.') {
                conversionSuccessful = false;
                std::cerr << "[ERROR] Enter a valid numeric type!" << std::endl;
                return false;
            }
        }

        try {
            results.push_back(static_cast<T>(std::stod(token)));
        } catch (const std::exception& e) {
            // Handle the exception (e.g., by throwing or logging an error)
            conversionSuccessful = false;
            std::cerr << "[ERROR] Invalid conversion: " << token << std::endl;
        }
    }

    return conversionSuccessful;
}


// Function to validate that values are greater than zero
template<typename T>
bool validateBounds(const std::vector<T>& values)
{
    for (auto val : values)
    {
        if (val <= 0)
            return false;
    }
    return true;
}

// Function to get user input with validation
template <typename T>
void getInput(const std::string& prompt, std::vector<T>& values, int expectedNumber, const std::string& error, bool (*validationFunction)(const std::vector<T>&))
{
    bool endLoop = false;
    do
    {
        std::cout << prompt;
        std::string inputLine;
        std::getline(std::cin, inputLine);
        std::vector<T> results;
        if (!splitInputs(inputLine, ' ', results))
        {
            continue;
        }

        if (results.size() != expectedNumber)
        {
            std::cout << "[ERROR] Invalid Number of Arguments Entered! " << std::endl;
            continue;
        }

        if (!validationFunction(results))
        {
            std::cout << "[ERROR] " << error << std::endl;
            continue;
        }

        values = results;

        endLoop = true;

    } while (!endLoop);
}

// Function to get user input with point validation
template <typename T>
void getInput(const std::string& prompt, std::vector<T>& values, int expectedNumber, const std::string& error,  bool (*validationFunction)(const Point3D&, const std::vector<std::vector<Point3D>>& gridPoints), const std::vector<std::vector<Point3D>>& gridPoints)
{
    bool endLoop = false;
    do
    {
        std::cout << prompt;
        std::string inputLine;
        std::getline(std::cin, inputLine);
        std::vector<T> results;
        if (!splitInputs(inputLine, ' ', results))
        {
            continue;
        }

        if (results.size() != expectedNumber)
        {
            std::cout << "[ERROR] Invalid Number of Arguments Entered! " << std::endl;
            continue;
        }

        Point3D pointsToValidate = { results[0], results[1], results[2] };
        if (validationFunction(pointsToValidate, gridPoints))
        {
            std::cout << "[ERROR] " << error << std::endl;
            continue;
        }

        values = results;

        endLoop = true;

    } while (!endLoop);
}

// Calculate grid coordinates based on parameters
std::vector<std::vector<Point3D>> calculateGridCoordinates(int N, int M, double x_sep, double y_sep);

// Print the grid coordinates
void printGrid(const std::vector<std::vector<Point3D>>& coordinates, int N, int M);

// Check if a user-defined point overlaps with any grid points
bool validateOverlap(const Point3D& userPoint, const std::vector<std::vector<Point3D>>& gridPoints);

// Print a value in scientific notation
void printScientificNotation(const std::string& label, double value, int precision);




