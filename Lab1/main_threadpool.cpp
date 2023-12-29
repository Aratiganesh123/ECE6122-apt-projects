/*
Author: Arati Ganesh
Class: ECE6122 
Last Date Modified: 24/09/2023
Description: Creates a pool of threads which are created only once.
Prompts the user for input and calculates grid points.
Pushes each point into a queue and prompts the user
*/

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <random>
#include <chrono>

#include "ECE_ElectricField.h"
#include "ECE_PointCharge.h"
#include "ECE_ElectricFieldUtils.h"

class ThreadPool {
public:
    // Constructor to initialize the thread pool with a specified number of threads.
    ThreadPool(size_t numThreads);

    // Destructor to clean up the thread pool and join all threads.
    ~ThreadPool();

    // Enqueues a task to be processed by the threads in the pool.
    void enqueueTask(const Point3D& point, double chargeVal);

    // Getters to retrieve the accumulated electric field components.
    double getResultEx() const;
    double getResultEy() const;
    double getResultEz() const;

    // Check if the thread pool was stopped.
    bool wasStopped() const;

    // Wait until all tasks in the queue are completed.
    void waitUntilEmpty();

    // Set the stop flag to terminate the thread pool.
    void setStopFlag(bool flag);

    // Check if the thread pool has reached the end of its tasks.
    bool EOS();

    // Set the electric field point for calculation.
    void setElectricFieldPoint(const Point3D& eFieldPoint);

private:
    // Function executed by worker threads to process tasks.
    void workerThreadFunction();

    // Member variables to store thread pool state and results.
    Point3D eFieldPoint;
    std::vector<std::thread> threads;
    std::vector<ECE_ElectricField> tasks;
    std::mutex taskMutex;
    std::condition_variable taskCondition;
    bool stop;
    double resultEx;
    double resultEy;
    double resultEz;
    size_t completedTasks;  // Tracks completed tasks
    size_t totalTasks;      // Total number of tasks
    std::mutex completionMutex;
    std::condition_variable completionCondition;
};

// Constructor definition for ThreadPool class.
ThreadPool::ThreadPool(size_t numThreads)
    : stop(false), resultEx(0), resultEy(0), resultEz(0), eFieldPoint({0,0,0}), completedTasks(0), totalTasks(0) {
    for (size_t i = 0; i < numThreads; ++i) {
        threads.emplace_back(&ThreadPool::workerThreadFunction, this);
    }
}

// Function executed by worker threads.
void ThreadPool::workerThreadFunction() {
    while (true) {
        ECE_ElectricField task(0, 0, 0, 0);

        {
            std::unique_lock<std::mutex> lock(taskMutex);
            taskCondition.wait(lock, [this]() { return !tasks.empty() || stop; });
            if (stop && tasks.empty()) {
                return;
            }
            task = tasks.back();
            tasks.pop_back();
        }

        task.computeFieldAt(eFieldPoint.x, eFieldPoint.y, eFieldPoint.z);

        {
            double Ex, Ey, Ez;
            task.getElectricField(Ex, Ey, Ez);
            std::lock_guard<std::mutex> lock(taskMutex);
            resultEx += Ex;
            resultEy += Ey;
            resultEz += Ez;
            ++completedTasks;
            if (completedTasks == totalTasks) {
                completionCondition.notify_all();
            }
        }
    }
}

// Destructor definition for ThreadPool class.
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(taskMutex);
        stop = true;
    }
    taskCondition.notify_all();

    for (std::thread& thread : threads) {
        thread.join();
    }
}

// Enqueues a task with the specified point and charge value.
void ThreadPool::enqueueTask(const Point3D& point, double chargeVal) {
    ECE_ElectricField task(point.x, point.y, point.z, chargeVal);
    {
        std::unique_lock<std::mutex> lock(taskMutex);
        tasks.push_back(task);
        ++totalTasks;
    }
    taskCondition.notify_one();
}

// Getter functions to retrieve the accumulated electric field components.
double ThreadPool::getResultEx() const {
    return resultEx;
}

double ThreadPool::getResultEy() const {
    return resultEy;
}

double ThreadPool::getResultEz() const {
    return resultEz;
}

// Check if the thread pool was stopped.
bool ThreadPool::wasStopped() const {
    return stop;
}

// Check if the thread pool has reached the end of its tasks.
bool ThreadPool::EOS() {
    return (tasks.empty() && stop);
}

// Wait until all tasks in the queue are completed.
void ThreadPool::waitUntilEmpty() {
    {
        std::unique_lock<std::mutex> lock(completionMutex);
        completionCondition.wait(lock, [this]() { return completedTasks == totalTasks; });
    }
}

// Set the stop flag to terminate the thread pool.
void ThreadPool::setStopFlag(bool flag) {
    {
        std::unique_lock<std::mutex> lock(taskMutex);
        stop = flag;
    }
    taskCondition.notify_all();
}

void ThreadPool::setElectricFieldPoint(const Point3D& eFieldPoint) {
    this->eFieldPoint = eFieldPoint;
}

int main() {
    int numThreads = std::thread::hardware_concurrency();
    ThreadPool pool(numThreads);

    int N = 0, M = 0;
    double xSeparation = 0.0, ySeparation = 0.0;
    double chargeVal = 0;

    std::vector<int> gridDim;
    std::vector<double> separationDist;
    std::vector<double> charges;
    std::vector<double> electricFieldPoint;
    Point3D eFieldPoint;
    bool continueCalculations = true;

    while (continueCalculations) {

        std::cout << "Your computer supports " << numThreads << " concurrent threads.\n";

        getInput<int>("Please enter the number of rows and columns in the N x M array: ", gridDim, 2, "Grid Dimensions should be natural numbers!", validateBounds);
        N = gridDim[0];
        M = gridDim[1];


        getInput("Please enter the x and y separation distances in meters: ", separationDist, 2, "(N x M) separation distance values must be > 0!", validateBounds);
        xSeparation = separationDist[0];
        ySeparation = separationDist[1];

        getInput("Please enter the common charge on the points in micro C: ", charges, 1, "Point Charge value should be > 0.0!!", validateBounds);
        chargeVal = charges[0];

        auto grid = calculateGridCoordinates(N, M, xSeparation, ySeparation);

        getInput("Please enter the location in space to determine the electric field (x y z) in meters: ", electricFieldPoint, 3, "Overlap detected with the user-provided electric field point.", validateOverlap, grid);
        eFieldPoint.x = electricFieldPoint[0];
        eFieldPoint.y = electricFieldPoint[1];
        eFieldPoint.z = electricFieldPoint[2];

        pool.setElectricFieldPoint(eFieldPoint);

        auto start = std::chrono::high_resolution_clock::now();

        // Enqueue tasks for grid points and calculate electric field.
        for (const auto& point : grid) {
            pool.enqueueTask(point, chargeVal);
        }

        // Wait for all tasks to complete.
        pool.waitUntilEmpty();

        // Calculate total electric field magnitude.
        double totalResultEx = pool.getResultEx();
        double totalResultEy = pool.getResultEy();
        double totalResultEz = pool.getResultEz();

        // Calculate |E|
        double E = sqrt(totalResultEx * totalResultEx + totalResultEy * totalResultEy + totalResultEz * totalResultEz);

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

        // Output results
        int precision = 4;
        printScientificNotation("Ex", totalResultEx, precision);
        printScientificNotation("Ey", totalResultEy, precision);
        printScientificNotation("Ez", totalResultEz, precision);
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

