#pragma once
#include <vector>
#include <string>

// ------------------------------------------
//                 H
// ------------------------------------------

// Hadamard gate on multiple qubits
void h(std::vector<int> qubits);

// Hadamard gate on a single qubit
void h(int qubit);

// Hadamard gate applied to a whole register
void h(std::string registerName);

// ------------------------------------------
//                 X
// ------------------------------------------

// X gate on multiple qubits
void x(std::vector<int> qubits);

// X gate on a single qubit
void x(int qubit);

// X gate applied to a whole register
void x(std::string registerName);

// X gate applied to many registers
void x(std::vector<std::string> registerNames);

// ------------------------------------------
//                   CX
// ------------------------------------------

// Controlled X gate on multiple controls and multiple targets
void cx(std::vector<int> controls, std::vector<int> targets);
	
// Controlled X gate on a single qubit
void cx(int control, int target);

// Controlled X gate on multiple targets
void cx(int control, std::vector<int> targets);

// Controlled X gate on multiple controls 
void cx(std::vector<int> controls, int target);

// Controlled X gate on a whole register
void cx(int control, std::string registerName);

// Controlled X gate on a whole register as the control
void cx(std::string registerName, int target);

// Controlled X gate on a whole register as the control and whole register as the target
void cx(std::string controlRegister, std::string targetRegister);

// Controlled X gate on a register as the control and multiple targets
void cx(std::string controlRegister, std::vector<int> targets);

// Controlled X gate on a register as the target and multiple controls
void cx(std::vector<int> controls, std::string targetRegister);

// Controlled X gate on several registers as the control and a single target
void cx(std::vector<std::string> controlRegisters, std::string targetRegister);

// ------------------------------------------
//                 SWAPS
// ------------------------------------------

// Swap gate between two qubits
void swap(int qubit1, int qubit2);

// Swap gate with a single control
void cswap(int control, int target1, int target2);

// Swap gate with multiple controls
void cswap(std::vector<int> controls, int target1, int target2);

// Swap gate with register as the control
void cswap(std::string controlRegister, int target1, int target2);

// Swap gate between two registers
void swap(std::string register1, std::string register2);

