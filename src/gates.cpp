#include "gates.h"
#include "globals.h"
#include <iostream>

// ------------------------------------------
//                 H
// ------------------------------------------

// Hadamard gate on multiple qubits
void h(std::vector<int> qubits) {
    std::string qubitString = "";
    for (size_t i = 0; i < qubits.size(); i++) {
        qubitString += " q[" + std::to_string(qubits[i]) + "]";
        if (i < qubits.size()-1) {
            qubitString += ",";
        }
    }
    output.push_back("h" + qubitString + ";");
}

// Hadamard gate on a single qubit
void h(int qubit) {
    std::vector<int> qubits = {qubit};
    h(qubits);
}

// Hadamard gate applied to a whole register
void h(std::string registerName) {
    std::vector<int> qubits;
    for (int i = qubitList[registerName].start; i < qubitList[registerName].end; i++) {
        qubits.push_back(i);
    }
    h(qubits);
}

// ------------------------------------------
//                 X
// ------------------------------------------

// X gate on multiple qubits
void x(std::vector<int> qubits) {
    std::string qubitString = "";
	for (size_t i = 0; i < qubits.size(); i++) {
        qubitString += " q[" + std::to_string(qubits[i]) + "]";
        if (i < qubits.size()-1) {
            qubitString += ",";
        }
	}
    output.push_back("x" + qubitString + ";");
}

// X gate on a single qubit
void x(int qubit) {
    std::vector<int> qubits = {qubit};
	x(qubits);
}

// X gate applied to a whole register
void x(std::string registerName) {
    std::vector<int> qubits;
	for (int i = qubitList[registerName].start; i < qubitList[registerName].end; i++) {
        qubits.push_back(i);
	}
    x(qubits);
}

// X gate applied to many registers
void x(std::vector<std::string> registerNames) {
    std::vector<int> qubits;
	for (size_t j = 0; j < registerNames.size(); j++) {
        for (int i = qubitList[registerNames[j]].start; i < qubitList[registerNames[j]].end; i++) {
            qubits.push_back(i);
        }
    }
    x(qubits);
}

// ------------------------------------------
//                   CX
// ------------------------------------------

// Controlled X gate on multiple controls and multiple targets
void cx(std::vector<int> controls, std::vector<int> targets) {
	std::string gateName = "x";
    std::string controlString = "";
    std::string targetString = "";
	for (size_t i = 0; i < controls.size(); i++) {
		gateName = "ctrl @ " + gateName;
		controlString += " q[" + std::to_string(controls[i]) + "]";
        if (i < controls.size()-1 || targets.size() > 0) {
            controlString += ",";
        }
	}
    for (size_t i = 0; i < targets.size(); i++) {
        targetString += " q[" + std::to_string(targets[i]) + "]";
        if (i < targets.size()-1) {
            targetString += ",";
        }
    }
    if (targets.size() > 0) {
        output.push_back(gateName + controlString + targetString + ";");
    }
}

// Controlled X gate on a single qubit
void cx(int control, int target) {
    std::vector<int> controls = {control};
    std::vector<int> targets = {target};
	cx(controls, targets);
}

// Controlled X gate on multiple targets
void cx(int control, std::vector<int> targets) {
    std::vector<int> controls = {control};
    cx(controls, targets);
}

// Controlled X gate on multiple controls 
void cx(std::vector<int> controls, int target) {
    std::vector<int> targets = {target};
    cx(controls, targets);
}

// Controlled X gate on a whole register
void cx(int control, std::string registerName) {
    std::vector<int> controls = {control};
    std::vector<int> targets;
	for (int i = qubitList[registerName].start; i < qubitList[registerName].end; i++) {
        targets.push_back(i);
	}
    cx(controls, targets);
}

// Controlled X gate on a whole register as the control
void cx(std::string registerName, int target) {
    std::vector<int> targets = {target};
    std::vector<int> controls;
    for (int i = qubitList[registerName].start; i < qubitList[registerName].end; i++) {
        controls.push_back(i);
    }
    cx(controls, targets);
}

// Controlled X gate on a whole register as the control and whole register as the target
void cx(std::string controlRegister, std::string targetRegister) {
    std::vector<int> controls;
    std::vector<int> targets;
    for (int i = qubitList[controlRegister].start; i < qubitList[controlRegister].end; i++) {
        controls.push_back(i);
    }
	for (int i = qubitList[targetRegister].start; i < qubitList[targetRegister].end; i++) {
	    targets.push_back(i);
	}
    cx(controls, targets);
}

// Controlled X gate on a register as the control and multiple targets
void cx(std::string controlRegister, std::vector<int> targets) {
    std::vector<int> controls;
    for (int i = qubitList[controlRegister].start; i < qubitList[controlRegister].end; i++) {
        controls.push_back(i);
    }
    cx(controls, targets);
}

// Controlled X gate on a register as the target and multiple controls
void cx(std::vector<int> controls, std::string targetRegister) {
    std::vector<int> targets;
    for (int i = qubitList[targetRegister].start; i < qubitList[targetRegister].end; i++) {
        targets.push_back(i);
    }
    cx(controls, targets);
}

// Controlled X gate on several registers as the control and a single target
void cx(std::vector<std::string> controlRegisters, std::string targetRegister) {
    std::vector<int> controls;
    std::vector<int> targets;
    for (size_t i = 0; i < controlRegisters.size(); i++) {
        for (int j = qubitList[controlRegisters[i]].start; j < qubitList[controlRegisters[i]].end; j++) {
            controls.push_back(j);
        }
    }
    for (int i = qubitList[targetRegister].start; i < qubitList[targetRegister].end; i++) {
        targets.push_back(i);
    }
    cx(controls, targets);
}

// ------------------------------------------
//                 SWAPS
// ------------------------------------------

// Swap gate between two qubits
void swap(int qubit1, int qubit2) {
    output.push_back("ctrl @ x q[" + std::to_string(qubit1) + "], q[" + std::to_string(qubit2) + "];");
    output.push_back("ctrl @ x q[" + std::to_string(qubit2) + "], q[" + std::to_string(qubit1) + "];");
    output.push_back("ctrl @ x q[" + std::to_string(qubit1) + "], q[" + std::to_string(qubit2) + "];");
}

// Swap gate with a single control
void cswap(int control, int target1, int target2) {
    output.push_back("ctrl @ ctrl @ x q[" + std::to_string(control) + "], q[" + std::to_string(target1) + "], q[" + std::to_string(target2) + "];");
    output.push_back("ctrl @ ctrl @ x q[" + std::to_string(control) + "], q[" + std::to_string(target2) + "], q[" + std::to_string(target1) + "];");
    output.push_back("ctrl @ ctrl @ x q[" + std::to_string(control) + "], q[" + std::to_string(target1) + "], q[" + std::to_string(target2) + "];");
}

// Swap gate with multiple controls
void cswap(std::vector<int> controls, int target1, int target2) {
    std::string gateName = "ctrl @ x";
    for (size_t i = 0; i < controls.size(); i++) {
        gateName = "ctrl @ " + gateName;
        gateName += " q[" + std::to_string(controls[i]) + "],";
    }
    output.push_back(gateName + " q[" + std::to_string(target1) + "], q[" + std::to_string(target2) + "];");
    output.push_back(gateName + " q[" + std::to_string(target2) + "], q[" + std::to_string(target1) + "];");
    output.push_back(gateName + " q[" + std::to_string(target1) + "], q[" + std::to_string(target2) + "];");
}

// Swap gate with register as the control
void cswap(std::string controlRegister, int target1, int target2) {
    std::string gateName = "ctrl @ x";
    for (int i = qubitList[controlRegister].start; i < qubitList[controlRegister].end; i++) {
        gateName = "ctrl @ " + gateName;
        gateName += " q[" + std::to_string(i) + "],";
    }
    output.push_back(gateName + " q[" + std::to_string(target1) + "], q[" + std::to_string(target2) + "];");
    output.push_back(gateName + " q[" + std::to_string(target2) + "], q[" + std::to_string(target1) + "];");
    output.push_back(gateName + " q[" + std::to_string(target1) + "], q[" + std::to_string(target2) + "];");
}

// Swap gate between two registers
void swap(std::string register1, std::string register2) {
    if (qubitList[register1].amount != qubitList[register2].amount) {
        std::cout << "Error: registers must be the same size to swap" << std::endl;
        exit(1);
    }
    for (int i = 0; i < qubitList[register1].amount; i++) {
        swap(qubitList[register1].start + i, qubitList[register2].start + i);
    }
}

