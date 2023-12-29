/*
Author: Arati Ganesh
Class: ECE6122 
Last Date Modified: 24/09/2023
Description: Contains functions related to point charge such as setting up the charge value, location etc.
*/

#pragma once

class ECE_PointCharge {
protected:
    double x; // x-coordinate.
    double y; // y-coordinate.
    double z; // z-coordinate.
    double q; // charge of the point.
    
public:
    // Constructor to initialize the point charge's position (x, y, z) and charge (q).
    ECE_PointCharge(double x, double y, double z, double q);

    // Set the location (x, y, z) of the point charge.
    void setLocation(double x, double y, double z);

    // Set the charge (q) of the point charge.
    void setCharge(double q);

    // Getter function to retrieve the x-coordinate of the point charge.
    double getX() const;

    // Getter function to retrieve the y-coordinate of the point charge.
    double getY() const;

    // Getter function to retrieve the z-coordinate of the point charge.
    double getZ() const;

    // Getter function to retrieve the charge (q) of the point charge.
    double getQ() const;
};
