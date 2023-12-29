cmake_minimum_required(VERSION 2.8)
project(APT_HW1)


set(CMAKE_CXX_STANDARD 14)

# Define source files
set(SOURCE_FILES
    
    main_threading.cpp
    #main_threadpool.cpp
    ECE_ElectricField.cpp
    ECE_PointCharge.cpp
    ECE_ElectricFieldUtils.cpp
)

add_executable(electric_field ${SOURCE_FILES})
target_link_libraries(electric_field pthread)

# Enable O3 optimization
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")