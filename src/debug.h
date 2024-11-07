#pragma once
#include <vector>
#include <string>
#include <iostream>

// Tell the simulator to output the current value of a register, converted to an int
void outputReg(std::string text="", std::string registerName="", std::string mode="denary");

// Output all the values of the ancillas at the current moment
void outputAncillas(std::string text="ancillas", std::string mode="denary");
    
// Output all the values of the registers at the current moment
void outputAll(std::string text="registers", std::string mode="denary");
    
// Add a comment to the output
void comment(std::string comment);

// After a section of code, output the number of gates used
void finishSection(std::string name);
    
// Overload the output of vectors
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    os << "{";
    for (size_t i = 0; i < v.size(); i++) {
        os << v[i];
        if (i != v.size()-1) {
            os << ", ";
        }
    }
    os << "}";
    return os;
}

// Read the output lines in the same way as the simulator
std::vector<int> extractInfo(std::string line);
    
// Check for similar gates that can be combined or removed
void optimize();

// Write the circuit to a file
int writeCircuit(std::string filename);

// Parallel stuff
void beginParallel();
void endParallel();
void sectionParallel();
void beginMainOnly();
void endMainOnly();

// Checks
void checkForUnusedFunctions();
void checkForUnusedQubits();
