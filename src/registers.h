#pragma once
#include <vector>
#include <string>

// Init various values in the qubit list
void initQubitList();

// Output the qubit list
void printQubitList();

// Output every ancilla name
void printAncillas();

// Given a string, return a string with the first letter capitalized
std::string toCap(std::string str);

// Return the number of ancilla qubits used
int getNumAncillaUsed();

// Lock a section of ancilla qubits and return an identifier
std::string ancillaLock(int numRequired, std::string name = "");

// Free a section of ancilla qubits
void ancillaFree(std::string ancName);

// Create a subregister of a register
std::string subreg(std::string regName, int start, int end);

// Create a subregister of a register with a name
std::string subreg(std::string regName, int start, int end, std::string name);

// Clear all subregs for memory reasons
void clearAllSubregs();

// Start recording for the purposes of later uncomputation
int startRecording();

// Stop recording 
void stopRecording(int ind);

// Get the subroutine name from the index (invert the map)
std::string getSubroutineNameFromIndex(int ind);

// Start the definition of a subroutine
void beginSubroutine(std::string name);

// Finish the definition of a subroutine
void endSubroutine();

// Create the inverse of a subroutine
void createInverse(int ind);

// When told to uncompute the recording
void uncomputeRecording(int ind);

// Call a subroutine
void callSubroutine(std::string name);

// Call a subroutine conditioned on a register
void callSubroutineIf(std::string name, std::string regName);

// Call a subroutine conditioned on a several registers
void callSubroutineIfAnd(std::string name, std::vector<std::string> regNames);

// Call a subroutine conditioned on a register
void callSubroutineIf(std::string name, int controlQubit);

// When told to start recording peak ancilla usage
void startPeakAncilla();

// When told to stop recording peak ancilla usage
int stopPeakAncilla();

// Count the number of qubits used in a register
int countUsed(std::string name);

