/*
Author: Arati Ganesh
Class: ECE6122 
Last Date Modified: 24/09/2023
Description: Contains functions to compute and get Electric Field 
*/

#include "ECE_PointCharge.h"
#include "ECE_ElectricField.h"
#include <math.h>

constexpr double K = 9e9;  

// Constructor for ECE_ElectricField class
ECE_ElectricField::ECE_ElectricField(double x, double y, double z, double q)
    : ECE_PointCharge(x, y, z, q), Ex(0.0), Ey(0.0), Ez(0.0) {
}

// Compute the electric field at a specified point in space
void ECE_ElectricField::computeFieldAt(double x, double y, double z) {
    // Convert charge to Micro Coulombs
    double q_C = q * 1e-6;  

    // Calculate electric field components
    Ex = K * q_C * (x - this->x) / pow(pow(x - this->x, 2) + pow(y - this->y, 2) + pow(z - this->z, 2), 1.5);
    Ey = K * q_C * (y - this->y) / pow(pow(x - this->x, 2) + pow(y - this->y, 2) + pow(z - this->z, 2), 1.5);
    Ez = K * q_C * (z - this->z) / pow(pow(x - this->x, 2) + pow(y - this->y, 2) + pow(z - this->z, 2), 1.5);
}

// Get the computed electric field components
void ECE_ElectricField::getElectricField(double &Ex, double &Ey, double &Ez) {
    Ex = this->Ex;
    Ey = this->Ey;
    Ez = this->Ez;
}
