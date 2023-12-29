# Electric Field Calculation

**Author:** Arati Ganesh
**Last Date Modified:** 24/09/2023

## Description

This project calculates the electric field at a given point in space due to a grid of point charges. It utilizes multi-threading to parallelize the computation, improving performance.

## Prerequisites

- CMake (2.8)
- C++ Compiler (e.g., g++)

## Steps to Execute

1. Open `CMakeLists.txt` and verify if `main_threading.cpp` is added and uncommented.

2. Navigate to the project folder in your terminal:

   ```
   cd path/to/your/project
   ```
3. Create a build directory and navigate into it

    ```
    mkdir build
    cd build
    ```
4. Generate the build files with CMake:

    ```
    cmake ..
    ```
5. Build the executable:

    ```
    make -j12

    ```
6. Run the executable:

    ```
    ./electric_field

    ```
    
## Note:

1. main_threading.cpp is the final version of the code with better timing results.
2. An alternate implementation using the thread pool design pattern is also provided (main_threadpool.cpp), but the timing results do not benchmark on par with main_threading.cpp.
3. To test main_threadpool.cpp, uncomment main_threadpool.cpp and comment main_threading.cpp in CMakeLists.txt, and then follow steps 2 to 6 accordingly.
