#include <iostream>
#include <string>
#include <sstream>
#include <set>

/*
Author: Arati Ganesh
Class: ECE6122
Last Date Modified: 08/29/23
Description:
This code inputs a natural number from the user and outputs
the multiples of 3 or 5 less than the entered number and the 
sum of those multiples.
*/

/*
    This Functions finds all the multiples under the limit. 
    It stores the unique numbers and returns the sum of multiples.
*/

int findMultiplesAndSum(int lastNumber, int divisor, std::set<int>& multiples)
{
    int multipleSum = 0;
    std::cout << "The multiples of " << divisor << " below " << lastNumber << " are: ";
    for (int count = divisor; count < lastNumber; count += divisor)
    {
        std::cout << count << ", ";
        //Check if it exists in the set
        if (multiples.find(count) == multiples.end()) 
        {
            multipleSum += count;
            multiples.insert(count);
        }
    }
    //Delete the last comma
    if (multipleSum != 0) 
    {
        std::cout << "\b\b";  
    }

    std::cout << "." << std::endl;
    return multipleSum;
}

int main() 
{
    while (1) 
    {
        std::cout << "Please enter a natural number (0 to quit): ";
        std::string inputNumString;
        std::getline(std::cin, inputNumString);
        std::stringstream ss(inputNumString);

        int inputNum;
        if (ss >> inputNum && inputNum >= 0) {
            //Exit the program when 0 is encountered
            if (inputNum == 0) 
            {
                std::cout << "Program terminated." << std::endl;
                break;
            } 
            else 
            {
                int multipleSum = 0;
                // Use a Set to store only unique multiples to prevent double counting
                std::set<int> multiples;

                multipleSum += findMultiplesAndSum(inputNum, 3, multiples);
                multipleSum += findMultiplesAndSum(inputNum, 5, multiples);

                std::cout << "The sum of all multiples is: " << multipleSum << "." << std::endl;
            }
        } 
        else 
        {
            //Reprompt the user when the input not a number
            continue;
        }
    }

    std::cout << "Have a nice day!" << std::endl;
    return 0;
}
