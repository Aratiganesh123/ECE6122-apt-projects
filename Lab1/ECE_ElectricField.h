/*
Author: Arati Ganesh
Class: ECE6122 
Last Date Modified: 24/09/2023
Description: Contains functions to compute and get Electric Field 
*/


#pragma once

#include "ECE_PointCharge.h"

class ECE_ElectricField : public ECE_PointCharge {
protected:
    double Ex; // X-component of the electric field.
    double Ey; // Y-component of the electric field.
    double Ez; // Z-component of the electric field.

public:
    ECE_ElectricField(double x, double y, double z, double q);
    
    // Compute the electric field at a specified point.
    void computeFieldAt(double x, double y, double z);

    // Get the electric field components.
    void getElectricField(double &Ex, double &Ey, double &Ez);
};
