/*
Author: Arati Ganesh
Class: ECE6122 
Last Date Modified: 24/09/2023
Description: Contains functions related to point charge such as setting up the charge value, location etc.
*/


#include "ECE_PointCharge.h"

// Constructor to initialize the point charge's position (x, y, z) and charge (q).
ECE_PointCharge::ECE_PointCharge(double x, double y, double z, double q)
    : x(x), y(y), z(z), q(q) {
}

// Set the location (x, y, z) of the point charge.
void ECE_PointCharge::setLocation(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

// Set the charge (q) of the point charge.
void ECE_PointCharge::setCharge(double q) {
    this->q = q;
}

// Getter function to retrieve the x-coordinate of the point charge.
double ECE_PointCharge::getX() const {
    return x;
}

// Getter function to retrieve the y-coordinate of the point charge.
double ECE_PointCharge::getY() const {
    return y;
}

// Getter function to retrieve the z-coordinate of the point charge.
double ECE_PointCharge::getZ() const {
    return z;
}

// Getter function to retrieve the charge (q) of the point charge.
double ECE_PointCharge::getQ() const {
    return q;
}
