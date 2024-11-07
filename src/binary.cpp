#include "binary.h"
#include "gates.h"
#include "registers.h"
#include "debug.h"

// ------------------------------------------
//               CONVERTERS
// ------------------------------------------

// Convert to a binary array e.g. 32 = {1, 0, 0, 0, 0}
std::vector<int> toBinary(int val, int bits) {
    if (val < 0) {
        std::cout << "ERROR: Value " << val << " is negative, use a TC routine instead" << std::endl;
        exit(1);
    }
    if (val > pow(2, bits)) {
        std::cout << "ERROR: Value " << val << " too large for " << bits << " bits" << std::endl;
        exit(1);
    }
	std::vector<int> binaryVal;
	while (val > 0) {
		binaryVal.push_back(val % 2);
		val /= 2;
	}
	while (int(binaryVal.size()) < bits) {
		binaryVal.push_back(0);
	}
	std::reverse(binaryVal.begin(), binaryVal.end());
	return binaryVal;
}

// Convert to a denary value from a binary array, two's complement
int toDenaryTC(std::vector<int> val) {
	int denaryVal= 0;
    for (size_t i = 0; i < val.size(); i++) {
        if (i == 0) {
            denaryVal -= val[i] * pow(2, val.size()-i-1);
        } else {
            denaryVal += val[i] * pow(2, val.size()-i-1);
        }
    }
	return denaryVal;
}

// Convert to a binary array in two's complement e.g. -16 = {1, 1, 0, 0, 0}
std::vector<int> toBinaryTC(int val, int bits) {
    if (val > pow(2, bits-1) || val < -pow(2, bits-1)) {
        std::cout << "ERROR: Value " << val << " too large for " << bits << " bits" << std::endl;
        exit(1);
    }
	std::vector<int> binaryVal;
    bool isNeg = val < 0;
    if (isNeg) {
        val += pow(2, bits-1);
    }
	while (val > 0) {
		binaryVal.push_back(val % 2);
		val /= 2;
	}
    if (isNeg) {
        while (int(binaryVal.size()) < bits-1) {
            binaryVal.push_back(0);
        }
        binaryVal.push_back(1);
    } else {
        while (int(binaryVal.size()) < bits) {
            binaryVal.push_back(0);
        }
    }
	std::reverse(binaryVal.begin(), binaryVal.end());
	return binaryVal;
}

// Convert to a denary array e.g. 32 = {10, 10, 3, 2} (10 is used as padding)
std::vector<int> toDenary(int val, int bits) {
	std::vector<int> denaryVal;
	while (val > 0) {
		denaryVal.push_back(val % 10);
		val /= 10;
	}
	if (denaryVal.size() == 0) {
		denaryVal.push_back(0);
	}
	while (int(denaryVal.size()) < bits) {
		denaryVal.push_back(10);
	}
	std::reverse(denaryVal.begin(), denaryVal.end());
	return denaryVal;
}

// Convert to a denary array e.g. 32 = {3, 2} (no padding)
std::vector<int> toDenary(int val) {
	std::vector<int> denaryVal;
	while (val > 0) {
		denaryVal.push_back(val % 10);
		val /= 10;
	}
	if (denaryVal.size() == 0) {
		denaryVal.push_back(0);
	}
	std::reverse(denaryVal.begin(), denaryVal.end());
	return denaryVal;
}

// Convert to a denary value from a binary array
int toDenary(std::vector<int> val) {
	int denaryVal= 0;
    for (size_t i = 0; i < val.size(); i++) {
        denaryVal += val[i] * pow(2, val.size()-i-1);
    }
	return denaryVal;
}

// ------------------------------------------
//           SETTERS / FLIPPERS
// ------------------------------------------

// Set a blank register to a certain value
void setVal(std::string destQubit, int val) {
	std::vector<int> binaryVal = toBinary(val, qubitList[destQubit].amount);
    std::vector<int> qubits = {};
	for (size_t i = 0; i < binaryVal.size(); i++) {
		if (binaryVal[i] == 1) {
            qubits.push_back(qubitList[destQubit].start + i);
		}
	}
    x(qubits);
}

// Set a blank register to a certain value, two's complement
void setValTC(std::string destQubit, int val) {
	std::vector<int> binaryVal = toBinaryTC(val, qubitList[destQubit].amount);
    std::vector<int> qubits = {};
	for (size_t i = 0; i < binaryVal.size(); i++) {
		if (binaryVal[i] == 1) {
            qubits.push_back(qubitList[destQubit].start + i);
		}
	}
    x(qubits);
}

// Flip an entire register if a register is all ones
void flipIf(std::string regToFlip, std::string regToCheck) {
	cx(regToCheck, regToFlip);
}

// Flip an entire register if several registers are all ones
void flipIfAnd(std::string regToFlip, std::vector<std::string> regsToCheck) {
	cx(regsToCheck, regToFlip);
}

// Flip an entire register if several registers are non-zero
void flipIfOr(std::string regToFlip, std::vector<std::string> regsToCheck) {
	x(regsToCheck);
    cx(regsToCheck, regToFlip);
    x(regsToCheck);
    x(regToFlip);
}

// Flip an entire register if a register has a certain value
void flipIfEqual(std::string regToFlip, std::string regToCheck, int checkVal) {
	std::vector<int> binaryVal = toBinary(checkVal, qubitList[regToCheck].amount);
    std::vector<int> qubits = {};
	for (size_t i = 0; i < binaryVal.size(); i++) {
		if (binaryVal[i] == 0) {
            qubits.push_back(qubitList[regToCheck].start + i);
		}
	}
    x(qubits);
	cx(regToCheck, regToFlip);
    x(qubits);
}

// Flip an entire register if a register has a certain value, if another reg is all true
void flipIfEqualIf(std::string regToFlip, std::string regToCheck, int checkVal, std::string regIf) {
	std::vector<int> binaryVal = toBinary(checkVal, qubitList[regToCheck].amount);
    std::vector<int> qubits = {};
	for (size_t i = 0; i < binaryVal.size(); i++) {
		if (binaryVal[i] == 0) {
            qubits.push_back(qubitList[regToCheck].start + i);
		}
	}
    std::vector<int> controls = {};
    for (int i = qubitList[regIf].start; i < qubitList[regIf].end; i++) {
        controls.push_back(i);
    }
    for (int i = qubitList[regToCheck].start; i < qubitList[regToCheck].end; i++) {
        controls.push_back(i);
    }
    x(qubits);
	cx(controls, regToFlip);
    x(qubits);
}

// Flip an entire register if two registers have the same value
void flipIfEqual(std::string regToFlip, std::string regToCheck, std::string regToCheck2) {
    int recordInd = startRecording();
    if (qubitList[regToCheck].amount >= qubitList[regToCheck2].amount) {
        std::string areEqualPer = ancillaLock(qubitList[regToCheck].amount, "areEqualPer");
        int biggest = qubitList[regToCheck].amount;
        int smallest = qubitList[regToCheck2].amount;
        int diff = biggest - smallest;
        for (int i = 0; i < diff; i++) {
            x(qubitList[regToCheck].start + i);
            cx(qubitList[regToCheck].start + i, qubitList[areEqualPer].start + i);
            x(qubitList[regToCheck].start + i);
        }
        for (int i = 0; i < smallest; i++) {
            cx(qubitList[regToCheck].start + diff + i, qubitList[areEqualPer].start + diff + i);
            cx(qubitList[regToCheck2].start + i, qubitList[areEqualPer].start + diff + i);
            x(qubitList[areEqualPer].start + diff + i);
        }
        stopRecording(recordInd);
        std::vector<int> controls;
        for (int i = 0; i < qubitList[areEqualPer].amount; i++) {
            controls.push_back(qubitList[areEqualPer].start + i);
        }
        cx(controls, regToFlip);
        uncomputeRecording(recordInd);
        ancillaFree(areEqualPer);
    } else {
        std::string areEqualPer = ancillaLock(qubitList[regToCheck2].amount, "areEqualPer");
        int biggest = qubitList[regToCheck2].amount;
        int smallest = qubitList[regToCheck].amount;
        int diff = biggest - smallest;
        for (int i = 0; i < diff; i++) {
            x(qubitList[regToCheck2].start + i);
            cx(qubitList[regToCheck2].start + i, qubitList[areEqualPer].start + i);
            x(qubitList[regToCheck2].start + i);
        }
        for (int i = 0; i < smallest; i++) {
            cx(qubitList[regToCheck2].start + diff + i, qubitList[areEqualPer].start + diff + i);
            cx(qubitList[regToCheck].start + i, qubitList[areEqualPer].start + diff + i);
            x(qubitList[areEqualPer].start + diff + i);
        }
        stopRecording(recordInd);
        std::vector<int> controls;
        for (int i = 0; i < qubitList[areEqualPer].amount; i++) {
            controls.push_back(qubitList[areEqualPer].start + i);
        }
        cx(controls, regToFlip);
        uncomputeRecording(recordInd);
        ancillaFree(areEqualPer);
    }
}

// Flip an entire register if a register has a value less than another register
void flipIfLessOrEqual(std::string regToFlip, std::string regToCheck, std::string regToCheck2) {
    int recordInd = startRecording();
    std::string firstBigger = ancillaLock(qubitList[regToCheck].amount, "firstBigger");
    std::string secondBigger = ancillaLock(qubitList[regToCheck2].amount, "secondBigger");
    std::string areEqual = ancillaLock(1, "areEqual");
    flipIfEqual(areEqual, regToCheck, regToCheck2);
    if (qubitList[regToCheck].amount >= qubitList[regToCheck2].amount) {
        int biggest = qubitList[regToCheck].amount;
        int smallest = qubitList[regToCheck2].amount;
        int diff = biggest - smallest;
        for (int i = 0; i < diff; i++) {
            cx(qubitList[regToCheck].start + i, qubitList[firstBigger].start + i);
        }
        for (int i = 0; i < smallest; i++) {
            std::vector<int> controls = {qubitList[regToCheck2].start + i, qubitList[regToCheck].start + diff + i};
            x(qubitList[regToCheck2].start + i);
            cx(controls, qubitList[firstBigger].start + diff + i);
            x(qubitList[regToCheck2].start + i);
            x(qubitList[regToCheck].start + diff + i);
            cx(controls, qubitList[secondBigger].start + i);
            x(qubitList[regToCheck].start + diff + i);
        }
        stopRecording(recordInd);
        for (int i = 0; i < smallest; i++) {
            std::vector<int> controls = {qubitList[secondBigger].start + i, qubitList[areEqual].start};
            for (int j = 0; j < diff + i; j++) {
                controls.push_back(qubitList[firstBigger].start + j);
            }
            for (int j = 0; j < i; j++) {
                controls.push_back(qubitList[secondBigger].start + j);
            }
            for (size_t j = 1; j < controls.size(); j++) {
                x(controls[j]);
            }
            cx(controls, regToFlip);
            for (size_t j = 1; j < controls.size(); j++) {
                x(controls[j]);
            }
        }
    } else {
        int biggest = qubitList[regToCheck2].amount;
        int smallest = qubitList[regToCheck].amount;
        int diff = biggest - smallest;
        for (int i = 0; i < diff; i++) {
            cx(qubitList[regToCheck2].start + i, qubitList[secondBigger].start + i);
        }
        for (int i = 0; i < smallest; i++) {
            std::vector<int> controls = {qubitList[regToCheck].start + i, qubitList[regToCheck2].start + diff + i};
            x(qubitList[regToCheck].start + i);
            cx(controls, qubitList[secondBigger].start + diff + i);
            x(qubitList[regToCheck].start + i);
            x(qubitList[regToCheck2].start + diff + i);
            cx(controls, qubitList[firstBigger].start + i);
            x(qubitList[regToCheck2].start + diff + i);
        }
        stopRecording(recordInd);
        for (int i = 0; i < smallest; i++) {
            std::vector<int> controls = {qubitList[firstBigger].start + i, qubitList[areEqual].start};
            for (int j = 0; j < diff + i; j++) {
                controls.push_back(qubitList[secondBigger].start + j);
            }
            for (int j = 0; j < i; j++) {
                controls.push_back(qubitList[firstBigger].start + j);
            }
            for (size_t j = 1; j < controls.size(); j++) {
                x(controls[j]);
            }
            cx(controls, regToFlip);
            for (size_t j = 1; j < controls.size(); j++) {
                x(controls[j]);
            }
        }
    }
    cx(areEqual, regToFlip);
    uncomputeRecording(recordInd);
    ancillaFree(firstBigger);
    ancillaFree(secondBigger);
    ancillaFree(areEqual);
}

// Flip a register if another register has a value less than a value
void flipIfLessOrEqualTC(std::string regToFlip, std::string regToCheck, int checkVal) {
    std::string diffReg = ancillaLock(qubitList[regToCheck].amount+3, "diffRegLess");
    std::string isNeg = subreg(diffReg, 0, 1);
    int rec = startRecording();
    copyTC(regToCheck, diffReg);
    subtractTC(diffReg, checkVal+1);
    stopRecording(rec);
    flipIf(regToFlip, isNeg);
    uncomputeRecording(rec);
    ancillaFree(diffReg);
}

// Flip a register if another register has a value greater than a value
void flipIfGreaterOrEqualTC(std::string regToFlip, std::string regToCheck, int checkVal) {
    std::string diffReg = ancillaLock(qubitList[regToCheck].amount+3, "diffRegGreater");
    std::string isNeg = subreg(diffReg, 0, 1);
    int rec = startRecording();
    copyTC(regToCheck, diffReg);
    subtractTC(diffReg, checkVal);
    x(isNeg);
    stopRecording(rec);
    flipIf(regToFlip, isNeg);
    uncomputeRecording(rec);
    ancillaFree(diffReg);
}

// Flip a register if another register has a value less than a value
void flipIfLessOrEqual(std::string regToFlip, std::string regToCheck, int checkVal) {
    std::string diffReg = ancillaLock(qubitList[regToCheck].amount+3, "diffRegLess");
    std::string isNeg = subreg(diffReg, 0, 1);
    int rec = startRecording();
    copy(regToCheck, diffReg);
    subtractTC(diffReg, checkVal+1);
    stopRecording(rec);
    flipIf(regToFlip, isNeg);
    uncomputeRecording(rec);
    ancillaFree(diffReg);
}

// Flip a register if another register has a value greater than a value
void flipIfGreaterOrEqual(std::string regToFlip, std::string regToCheck, int checkVal) {
    std::string diffReg = ancillaLock(qubitList[regToCheck].amount+3, "diffRegGreater");
    std::string isNeg = subreg(diffReg, 0, 1);
    int rec = startRecording();
    copy(regToCheck, diffReg);
    subtractTC(diffReg, checkVal);
    x(isNeg);
    stopRecording(rec);
    flipIf(regToFlip, isNeg);
    uncomputeRecording(rec);
    ancillaFree(diffReg);
}

// Flip a register if a value is negative
void flipIfNegTC(std::string toFlip, std::string toCheck) {
    std::string isNeg = subreg(toCheck, 0, 1);
    flipIf(toFlip, isNeg);
}

// Set a blank register to a certain value if another register is all ones
void setValIf(std::string regToSet, int newVal, std::string regToCheck) {
	std::vector<int> binaryValToSet = toBinary(newVal, qubitList[regToSet].amount);
    std::vector<int> qubits = {};
	for (size_t i = 0; i < binaryValToSet.size(); i++) {
		if (binaryValToSet[i] == 1) {
            qubits.push_back(qubitList[regToSet].start + i);
		}
	}
    cx(regToCheck, qubits);
}

// Set a blank register to a certain value if several registers are all ones
void setValIfAnd(std::string regToSet, int newVal, std::vector<std::string> regsToCheck) {
	std::vector<int> binaryValToSet = toBinary(newVal, qubitList[regToSet].amount);
    std::vector<int> qubits = {};
	for (size_t i = 0; i < binaryValToSet.size(); i++) {
		if (binaryValToSet[i] == 1) {
            qubits.push_back(qubitList[regToSet].start + i);
		}
	}
    std::vector<int> controls = {};
    for (size_t i = 0; i < regsToCheck.size(); i++) {
        for (int j = qubitList[regsToCheck[i]].start; j < qubitList[regsToCheck[i]].end; j++) {
            controls.push_back(j);
        }
    }
    cx(controls, qubits);
}

// Set a blank register to a certain value if another register has a certain value
void setValIfEqual(std::string regToSet, int newVal, std::string regToCheck, int checkVal) {
	std::vector<int> binaryVal = toBinary(checkVal, qubitList[regToCheck].amount);
	std::vector<int> binaryValToSet = toBinary(newVal, qubitList[regToSet].amount);
    std::vector<int> qubitsToCheck = {};
    std::vector<int> qubitsToSet = {};
	for (size_t i = 0; i < binaryVal.size(); i++) {
		if (binaryVal[i] == 0) {
            qubitsToCheck.push_back(qubitList[regToCheck].start + i);
		}
	}
	for (size_t i = 0; i < binaryValToSet.size(); i++) {
		if (binaryValToSet[i] == 1) {
            qubitsToSet.push_back(qubitList[regToSet].start + i);
		}
	}
    x(qubitsToCheck);
    cx(regToCheck, qubitsToSet);
    x(qubitsToCheck);
}

// Set a blank register to a certain TC value if another register has a certain value
void setValTCIfEqual(std::string regToSet, int newVal, std::string regToCheck, int checkVal) {
	std::vector<int> binaryVal = toBinary(checkVal, qubitList[regToCheck].amount);
	std::vector<int> binaryValToSet = toBinaryTC(newVal, qubitList[regToSet].amount);
    std::vector<int> qubitsToCheck = {};
    std::vector<int> qubitsToSet = {};
	for (size_t i = 0; i < binaryVal.size(); i++) {
		if (binaryVal[i] == 0) {
            qubitsToCheck.push_back(qubitList[regToCheck].start + i);
		}
	}
	for (size_t i = 0; i < binaryValToSet.size(); i++) {
		if (binaryValToSet[i] == 1) {
            qubitsToSet.push_back(qubitList[regToSet].start + i);
		}
	}
    x(qubitsToCheck);
    cx(regToCheck, qubitsToSet);
    x(qubitsToCheck);
}

// Set a blank register to a certain TC value if another register is true
void setValTCIf(std::string regToSet, int newVal, std::string regToCheck) {
	std::vector<int> binaryValToSet = toBinaryTC(newVal, qubitList[regToSet].amount);
    std::vector<int> qubitsToCheck = {};
    std::vector<int> qubitsToSet = {};
    for (int j = qubitList[regToCheck].start; j < qubitList[regToCheck].end; j++) {
        qubitsToCheck.push_back(j);
    }
	for (size_t i = 0; i < binaryValToSet.size(); i++) {
		if (binaryValToSet[i] == 1) {
            qubitsToSet.push_back(qubitList[regToSet].start + i);
		}
	}
    cx(qubitsToCheck, qubitsToSet);
}

// Set a blank register to a certain TC value if some other registers are true
void setValTCIfAnd(std::string regToSet, int newVal, std::vector<std::string> regsToCheck) {
	std::vector<int> binaryValToSet = toBinaryTC(newVal, qubitList[regToSet].amount);
    std::vector<int> qubitsToCheck = {};
    std::vector<int> qubitsToSet = {};
	for (size_t i = 0; i < regsToCheck.size(); i++) {
        for (int j = qubitList[regsToCheck[i]].start; j < qubitList[regsToCheck[i]].end; j++) {
            qubitsToCheck.push_back(j);
        }
	}
	for (size_t i = 0; i < binaryValToSet.size(); i++) {
		if (binaryValToSet[i] == 1) {
            qubitsToSet.push_back(qubitList[regToSet].start + i);
		}
	}
    cx(qubitsToCheck, qubitsToSet);
}

// Copy from one register to a blank
void copy(std::string regToCopy, std::string regToSet) {
    int minAmount = std::min(qubitList[regToCopy].amount, qubitList[regToSet].amount);
    for (int i = 0; i < minAmount; i++) {
        cx(qubitList[regToCopy].end - i - 1, qubitList[regToSet].end - i - 1);
    }
}

// Copy from one register to a blank, if another register is all true
void copyIf(std::string regToCopy, std::string regToSet, std::string regIf) {
    std::vector<int> controls = {};
    for (int i = qubitList[regIf].start; i < qubitList[regIf].end; i++) {
        controls.push_back(i);
    }
    int minAmount = std::min(qubitList[regToCopy].amount, qubitList[regToSet].amount);
    for (int i = 0; i < minAmount; i++) {
        std::vector<int> qubits = controls;
        qubits.push_back(qubitList[regToCopy].end - i - 1);
        cx(qubits, qubitList[regToSet].end - i - 1);
    }
}

// Copy from one register to a blank, if the left bit is one then extend the sign
void copyTC(std::string regToCopy, std::string regToSet) {
    int minAmount = std::min(qubitList[regToCopy].amount, qubitList[regToSet].amount);
    for (int i = 0; i < minAmount-1; i++) {
        cx(qubitList[regToCopy].end - i - 1, qubitList[regToSet].end - i - 1);
    }
    for (int i = minAmount-1; i < qubitList[regToSet].amount; i++) {
        cx(qubitList[regToCopy].start, qubitList[regToSet].end - i - 1);
    }
}

// Copy from one register to a blank, dealing with TC, if another register is all true
void copyTCIf(std::string regToCopy, std::string regToSet, std::string regIf) {
    std::vector<int> controls = {};
    for (int i = qubitList[regIf].start; i < qubitList[regIf].end; i++) {
        controls.push_back(i);
    }
    int minAmount = std::min(qubitList[regToCopy].amount, qubitList[regToSet].amount);
    for (int i = 0; i < minAmount-1; i++) {
        std::vector<int> qubits = controls;
        qubits.push_back(qubitList[regToCopy].end - i - 1);
        cx(qubits, qubitList[regToSet].end - i - 1);
    }
    for (int i = minAmount-1; i < qubitList[regToSet].amount; i++) {
        std::vector<int> qubits = controls;
        qubits.push_back(qubitList[regToCopy].start);
        cx(qubits, qubitList[regToSet].end - i - 1);
    }
}

// Copy from one TC reg, taking the absolute value
void copyAbs(std::string regToCopy, std::string regToSet) {
    std::string isNeg = subreg(regToCopy, 0, 1);
    copy(regToCopy, regToSet);
    negIf(regToSet, isNeg);
}

// Copy the sign from a reg
void copySign(std::string regToCopy, std::string regToSet) {
    std::string signBit = subreg(regToCopy, 0, 1);
    setValTCIf(regToSet, -1, signBit);
    x(signBit);
    setValTCIf(regToSet, 1, signBit);
    x(signBit);
}

// Copy the sign from a reg, if another reg is all true
void copySignIf(std::string regToCopy, std::string regToSet, std::string regIf) {
    std::string signBit = subreg(regToCopy, 0, 1);
    setValTCIfAnd(regToSet, -1, {signBit, regIf});
    x(signBit);
    setValTCIfAnd(regToSet, 1, {signBit, regIf});
    x(signBit);
}

// ------------------------------------------
//                ARITHMETIC
// ------------------------------------------

// Given a register, add a fixed value to it
void add(std::string reg, int val) {
    std::vector<int> binaryVal = toBinary(val, qubitList[reg].amount);
    for (int i = 0; i < int(binaryVal.size()); i++) {
        if (binaryVal[i] == 1) {
            for (int j = 0; j < i; j++) {
                std::vector<int> qubits = {};
                for (int k = j+1; k <= i; k++) {
                    qubits.push_back(qubitList[reg].start + k);
                }
                cx(qubits, qubitList[reg].start + j);
            }
            x(qubitList[reg].start + i);
        }
    }
}

// Given a register, add a fixed value to it
void addTC(std::string reg, int val) {
    std::vector<int> binaryVal = toBinaryTC(val, qubitList[reg].amount);
    for (int i = 0; i < int(binaryVal.size()); i++) {
        if (binaryVal[i] == 1) {
            for (int j = 0; j < i; j++) {
                std::vector<int> qubits = {};
                for (int k = j+1; k <= i; k++) {
                    qubits.push_back(qubitList[reg].start + k);
                }
                cx(qubits, qubitList[reg].start + j);
            }
            x(qubitList[reg].start + i);
        }
    }
}

// Given a register, add a fixed value to it if another register is all ones
void addIf(std::string reg, int val, std::string regIf) {
    std::vector<int> binaryVal = toBinary(val, qubitList[reg].amount);
    std::vector<int> controls = {};
    for (int i = qubitList[regIf].start; i < qubitList[regIf].end; i++) {
        controls.push_back(i);
    }
    for (int i = 0; i < int(binaryVal.size()); i++) {
        if (binaryVal[i] == 1) {
            for (int j = 0; j < i; j++) {
                std::vector<int> qubits = controls;
                for (int k = j+1; k <= i; k++) {
                    qubits.push_back(qubitList[reg].start + k);
                }
                cx(qubits, qubitList[reg].start + j);
            }
            cx(controls, qubitList[reg].start + i);
        }
    }
}

// Given two registers, add the latter to the former
void add(std::string reg1, std::string reg2) {
    if (qubitList[reg1].amount != qubitList[reg2].amount) {
        std::cout << "ERROR: Adding registers of different sizes " << qubitList[reg1].amount << " / " << qubitList[reg2].amount << std::endl;
        exit(1);
    }
    for (int i = 0; i < std::min(qubitList[reg2].amount, qubitList[reg1].amount); i++) {
        for (int j = qubitList[reg1].amount - 1; j >= i + 1; j--) {
            std::vector<int> qubits = {qubitList[reg2].end - i - 1};
            for (int k = 0; k < j - i; k++) {
                qubits.push_back(qubitList[reg1].end - j - 1 + k + 1);
            }
            cx(qubits, qubitList[reg1].end - j - 1);
        }
        cx(qubitList[reg2].end - i - 1, qubitList[reg1].end - i - 1);
    }
}

// Given two registers, add the latter to the former (TC with diff)
void addTC(std::string reg1, std::string reg2) {

    // If the second register is smaller
    if (qubitList[reg2].amount <= qubitList[reg1].amount) {

        // Standard addition
        for (int i = 0; i < qubitList[reg2].amount; i++) {
            for (int j = qubitList[reg1].amount - 1; j >= i + 1; j--) {
                std::vector<int> qubits = {qubitList[reg2].end - i - 1};
                for (int k = 0; k < j - i; k++) {
                    qubits.push_back(qubitList[reg1].end - j - 1 + k + 1);
                }
                cx(qubits, qubitList[reg1].end - j - 1);
            }
            cx(qubitList[reg2].end - i - 1, qubitList[reg1].end - i - 1);
        }

        // add assuming the rest of reg2 is all the same as its sign bit
        for (int i = qubitList[reg2].amount; i < qubitList[reg1].amount; i++) {
            for (int j = qubitList[reg1].amount - 1; j >= i + 1; j--) {
                std::vector<int> qubits = {qubitList[reg2].start};
                for (int k = 0; k < j - i; k++) {
                    qubits.push_back(qubitList[reg1].end - j - 1 + k + 1);
                }
                cx(qubits, qubitList[reg1].end - j - 1);
            }
            cx(qubitList[reg2].start, qubitList[reg1].end - i - 1);
        }

    // Can't be bothered to do the other case
    } else {
        std::cout << "ERROR: Subtracting TC registers where the second is bigger " << qubitList[reg1].amount << " / " << qubitList[reg2].amount << std::endl;
        exit(1);
    }

}

// Given two registers, add the latter to the former, shifted 
void addShift(std::string reg1, std::string reg2, int shift) {
    if (qubitList[reg1].amount != qubitList[reg2].amount) {
        std::cout << "ERROR: Adding registers of different sizes " << qubitList[reg1].amount << " / " << qubitList[reg2].amount << std::endl;
        exit(1);
    }
    for (int i = qubitList[reg2].amount - 1; i >= shift; i--) {
        for (int j = shift; j < i; j++) {
            std::vector<int> qubits = {qubitList[reg2].start + i};
            for (int k = j+1; k <= i; k++) {
                qubits.push_back(qubitList[reg1].start + k - shift);
            }
            cx(qubits, qubitList[reg1].start + j - shift);
        }
        std::vector<int> qubits2 = {qubitList[reg2].start + i};
        cx(qubits2, qubitList[reg1].start + i - shift);
    }
}

// Given two registers, add the latter to the former, shifted and controlled
void addShiftIf(std::string reg1, std::string reg2, int shift, int control) {
    if (qubitList[reg1].amount != qubitList[reg2].amount) {
        std::cout << "ERROR: Adding registers of different sizes " << qubitList[reg1].amount << " / " << qubitList[reg2].amount << std::endl;
        exit(1);
    }
    for (int i = qubitList[reg2].amount - 1; i >= shift; i--) {
        for (int j = shift; j < i; j++) {
            std::vector<int> qubits = {control, qubitList[reg2].start + i};
            for (int k = j+1; k <= i; k++) {
                qubits.push_back(qubitList[reg1].start + k - shift);
            }
            cx(qubits, qubitList[reg1].start + j - shift);
        }
        std::vector<int> qubits2 = {control, qubitList[reg2].start + i};
        cx(qubits2, qubitList[reg1].start + i - shift);
    }
}

// Given two registers, add the latter to the former, controlled
void addIf(std::string reg1, std::string reg2, std::string control) {
    if (qubitList[reg1].amount != qubitList[reg2].amount) {
        std::cout << "ERROR: Adding registers of different sizes " << qubitList[reg1].amount << " / " << qubitList[reg2].amount << std::endl;
        exit(1);
    }
    std::vector<int> controls = {};
    for (int i = qubitList[control].start; i < qubitList[control].end; i++) {
        controls.push_back(i);
    }
    for (int i = qubitList[reg2].amount - 1; i >= 0; i--) {
        for (int j = 0; j < i; j++) {
            std::vector<int> qubits = controls;
            qubits.push_back(qubitList[reg2].start + i);
            for (int k = j+1; k <= i; k++) {
                qubits.push_back(qubitList[reg1].start + k);
            }
            cx(qubits, qubitList[reg1].start + j);
        }
        std::vector<int> qubits2 = controls;
        qubits2.push_back(qubitList[reg2].start + i);
        cx(qubits2, qubitList[reg1].start + i);
    }
}

// Add a value to a register if another register is all ones, wrapped
void addWrappedIf(std::string reg, int val, std::string regIf, int minVal, int maxVal) {

    // Add if
    std::vector<int> controls = {};
    for (int i = qubitList[regIf].start; i < qubitList[regIf].end; i++) {
        controls.push_back(i);
    }
    std::vector<int> binaryVal = toBinary(val, qubitList[reg].amount);
    for (int i = 0; i < int(binaryVal.size()); i++) {
        if (binaryVal[i] == 1) {
            for (int j = 0; j < i; j++) {
                std::vector<int> qubits = controls;
                for (int k = j+1; k <= i; k++) {
                    qubits.push_back(qubitList[reg].start + k);
                }
                cx(qubits, qubitList[reg].start + j);
            }
            cx(controls, qubitList[reg].start + i);
        }
    }

    // If we've reached the max, go to the min
    std::string binIf = ancillaLock(2, "binIf");
    std::string regIfAllTrue = subreg(binIf, 0, 1);
    std::string regIsAtMax = subreg(binIf, 1, 2);
    cx(controls, regIfAllTrue);
    flipIfEqualIf(regIsAtMax, reg, maxVal, regIfAllTrue);
    setValIf(reg, maxVal, binIf);
    setValIf(reg, minVal+1, binIf);
    flipIfEqualIf(regIsAtMax, reg, minVal+1, regIfAllTrue);
    cx(controls, regIfAllTrue);
    ancillaFree(binIf);

}

// Take the negative of a register
void neg(std::string reg) {
    for (int i = 0; i < qubitList[reg].amount; i++) {
        x(qubitList[reg].start + i);
    }
    add(reg, 1);
}

// Take the negative of a register, controlled
void negIf(std::string reg, std::string regIf) {
    for (int i = 0; i < qubitList[reg].amount; i++) {
        cx(regIf, qubitList[reg].start + i);
    }
    addIf(reg, 1, regIf);
}

// Subtract a value from a register if another register is all ones
void subtractIf(std::string reg, int val, std::string regIf) {
    std::vector<int> controls = {};
    for (int i = qubitList[regIf].start; i < qubitList[regIf].end; i++) {
        controls.push_back(i);
    }
    std::vector<int> binaryVal = toBinary(val, qubitList[reg].amount);
    for (int i = 0; i < int(binaryVal.size()); i++) {
        if (binaryVal[i] == 1) {
            cx(controls, qubitList[reg].start + i);
            for (int j = i-1; j >= 0; j--) {
                std::vector<int> qubits = controls;
                for (int k = j+1; k <= i; k++) {
                    qubits.push_back(qubitList[reg].start + k);
                }
                cx(qubits, qubitList[reg].start + j);
            }
        }
    }
}


// Subtract a value from a register if another register is all ones, wrapped
void subtractWrappedIf(std::string reg, int val, std::string regIf, int minVal, int maxVal) {

    // Subtract if
    std::vector<int> controls = {};
    for (int i = qubitList[regIf].start; i < qubitList[regIf].end; i++) {
        controls.push_back(i);
    }
    std::vector<int> binaryVal = toBinary(val, qubitList[reg].amount);
    for (int i = 0; i < int(binaryVal.size()); i++) {
        if (binaryVal[i] == 1) {
            cx(controls, qubitList[reg].start + i);
            for (int j = i-1; j >= 0; j--) {
                std::vector<int> qubits = controls;
                for (int k = j+1; k <= i; k++) {
                    qubits.push_back(qubitList[reg].start + k);
                }
                cx(qubits, qubitList[reg].start + j);
            }
        }
    }

    // If we've reached the min, go to the max
    std::string binIf = ancillaLock(2, "binIf");
    std::string regIfAllTrue = subreg(binIf, 0, 1);
    std::string regIsAtMin = subreg(binIf, 1, 2);
    cx(controls, regIfAllTrue);
    flipIfEqualIf(regIsAtMin, reg, minVal, regIfAllTrue);
    setValIf(reg, minVal, binIf);
    setValIf(reg, maxVal-1, binIf);
    flipIfEqualIf(regIsAtMin, reg, maxVal-1, regIfAllTrue);
    cx(controls, regIfAllTrue);
    ancillaFree(binIf);

}

// Multiply a register by another register, storing in a third
void multiply(std::string reg1, std::string reg2, std::string reg3) {
    if (qubitList[reg1].amount != qubitList[reg2].amount || qubitList[reg1].amount != qubitList[reg3].amount) {
        std::cout << "ERROR: Multiplying registers of different sizes " << qubitList[reg1].amount << " / " << qubitList[reg2].amount << std::endl;
        exit(1);
    }
    for (int i = qubitList[reg2].amount - 1; i >= 0; i--) {
        addShiftIf(reg3, reg1, qubitList[reg2].amount-i-1, qubitList[reg2].start + i);
    }
}

// Multiply a register by an int, storing in a third 
void multiply(std::string reg1, int val, std::string reg3) {
    std::vector<int> binaryVal = toBinary(val, qubitList[reg1].amount);
    for (int i = 0; i < int(binaryVal.size()); i++) {
        if (binaryVal[i] == 1) {
            addShift(reg3, reg1, qubitList[reg1].amount-i-1);
        }
    }
}

// Multiply a register by an int, storing in a third 
void multiplyTC(std::string reg1, int val, std::string reg3) {
    std::vector<int> binaryVal = toBinaryTC(val, qubitList[reg1].amount);
    for (int i = 0; i < int(binaryVal.size()); i++) {
        if (binaryVal[i] == 1) {
            addShift(reg3, reg1, qubitList[reg1].amount-i-1);
        }
    }
}

// Shift a register left by a certain amount
void shiftLeft(std::string reg, int shift) {
    if (shift > qubitList[reg].amount - 1) {
        std::cout << "ERROR: Shifting left overflow " << shift << " / " << qubitList[reg].amount << std::endl;
        exit(1);
    }
    for (int i = 0; i < shift; i++) {
        for (int j = 0; j < qubitList[reg].amount - 1; j++) {
            swap(qubitList[reg].start + j, qubitList[reg].start + j + 1);
        }
    }
}

// Shift a register right by a certain amount
void shiftRight(std::string reg, int shift) {
    if (shift > qubitList[reg].amount - 1) {
        std::cout << "ERROR: Shifting right overflow " << shift << " / " << qubitList[reg].amount << std::endl;
        exit(1);
    }
    for (int i = 0; i < shift; i++) {
        for (int j = qubitList[reg].amount - 2; j >= 0; j--) {
            swap(qubitList[reg].start + j, qubitList[reg].start + j + 1);
        }
    }
}

// Shift a register left by a certain amount, but not the leftmost bit
void shiftLeftTC(std::string reg, int shift) {
    if (shift > qubitList[reg].amount - 2) {
        std::cout << "ERROR: Shifting left overflow " << shift << " / " << qubitList[reg].amount << std::endl;
        exit(1);
    }
    for (int i = 0; i < shift; i++) {
        for (int j = 1; j < qubitList[reg].amount - 1; j++) {
            swap(qubitList[reg].start + j, qubitList[reg].start + j + 1);
        }
    }
    for (int i = 0; i < shift; i++) {
        cx(qubitList[reg].start, qubitList[reg].end - i - 1);
    }
}

// Shift a register right by a certain amount, but not the leftmost bit
void shiftRightTC(std::string reg, int shift) {
    if (shift > qubitList[reg].amount - 2) {
        std::cout << "ERROR: Shifting right overflow " << shift << " / " << qubitList[reg].amount << std::endl;
        exit(1);
    }
    for (int i = 0; i < shift; i++) {
        for (int j = qubitList[reg].amount - 2; j >= 1; j--) {
            swap(qubitList[reg].start + j, qubitList[reg].start + j + 1);
        }
    }
    for (int i = 0; i < shift; i++) {
        cx(qubitList[reg].start, qubitList[reg].start + i + 1);
    }
}

// Given a register, subtract a fixed value from it
void subtract(std::string reg, int val) {
    std::vector<int> binaryVal = toBinary(val, qubitList[reg].amount);
    for (int i = 0; i < int(binaryVal.size()); i++) {
        if (binaryVal[i] == 1) {
            x(qubitList[reg].start + i);
            for (int j = i-1; j >= 0; j--) {
                std::vector<int> qubits = {};
                for (int k = j+1; k <= i; k++) {
                    qubits.push_back(qubitList[reg].start + k);
                }
                cx(qubits, qubitList[reg].start + j);
            }
        }
    }
}

// Given a register, subtract a fixed value from it
void subtractTC(std::string reg, int val) {
    std::vector<int> binaryVal = toBinaryTC(val, qubitList[reg].amount);
    for (int i = 0; i < int(binaryVal.size()); i++) {
        if (binaryVal[i] == 1) {
            x(qubitList[reg].start + i);
            for (int j = i-1; j >= 0; j--) {
                std::vector<int> qubits = {};
                for (int k = j+1; k <= i; k++) {
                    qubits.push_back(qubitList[reg].start + k);
                }
                cx(qubits, qubitList[reg].start + j);
            }
        }
    }
}

// Given two registers, subtract the latter from the former
void subtract(std::string reg1, std::string reg2, bool checkDiff) {
    if (qubitList[reg1].amount != qubitList[reg2].amount && checkDiff) {
        std::cout << "ERROR: Subtracting registers of different sizes " << qubitList[reg1].amount << " / " << qubitList[reg2].amount << std::endl;
        exit(1);
    }
    for (int i = 0; i < std::min(qubitList[reg2].amount, qubitList[reg1].amount); i++) {
        cx(qubitList[reg2].end - i - 1, qubitList[reg1].end - i - 1);
        for (int j = i + 1; j < qubitList[reg1].amount; j++) {
            std::vector<int> qubits = {qubitList[reg2].end - i - 1};
            for (int k = 0; k < j - i; k++) {
                qubits.push_back(qubitList[reg1].end - j - 1 + k + 1);
            }
            cx(qubits, qubitList[reg1].end - j - 1);
        }
    }
}

// Given two registers, subtract the latter from the former (TC with diff)
void subtractTC(std::string reg1, std::string reg2) {

    // If the second register is smaller
    if (qubitList[reg2].amount <= qubitList[reg1].amount) {

        // Standard subtraction
        for (int i = 0; i < qubitList[reg2].amount; i++) {
            cx(qubitList[reg2].end - i - 1, qubitList[reg1].end - i - 1);
            for (int j = i + 1; j < qubitList[reg1].amount; j++) {
                std::vector<int> qubits = {qubitList[reg2].end - i - 1};
                for (int k = 0; k < j - i; k++) {
                    qubits.push_back(qubitList[reg1].end - j - 1 + k + 1);
                }
                cx(qubits, qubitList[reg1].end - j - 1);
            }
        }

        // Subtract assuming the rest of reg2 is all the same as its sign bit
        for (int i = qubitList[reg2].amount; i < qubitList[reg1].amount; i++) {
            cx(qubitList[reg2].start, qubitList[reg1].end - i - 1);
            for (int j = i + 1; j < qubitList[reg1].amount; j++) {
                std::vector<int> qubits = {qubitList[reg2].start};
                for (int k = 0; k < j - i; k++) {
                    qubits.push_back(qubitList[reg1].end - j - 1 + k + 1);
                }
                cx(qubits, qubitList[reg1].end - j - 1);
            }
        }

    // Can't be bothered to do the other case
    } else {
        std::cout << "ERROR: Subtracting TC registers where the second is bigger " << qubitList[reg1].amount << " / " << qubitList[reg2].amount << std::endl;
        exit(1);
    }

}

// Given two registers, subtract the latter from the former, controlled
void subtractIf(std::string reg1, std::string reg2, std::string controlReg) {
    if (qubitList[reg1].amount != qubitList[reg2].amount) {
        std::cout << "ERROR: Subtracting registers of different sizes " << qubitList[reg1].amount << " / " << qubitList[reg2].amount << std::endl;
        exit(1);
    }
    for (int i = qubitList[reg2].amount - 1; i >= 0; i--) {
        std::vector<int> qubits = {qubitList[reg2].start + i};
        for (int j = qubitList[controlReg].start; j < qubitList[controlReg].end; j++) {
            qubits.push_back(j);
        }
        cx(qubits, qubitList[reg1].start + i);
        for (int j = i-1; j >= 0; j--) {
            std::vector<int> qubits2 = {qubitList[reg2].start + i};
            for (int k = qubitList[controlReg].start; k < qubitList[controlReg].end; k++) {
                qubits2.push_back(k);
            }
            for (int k = j+1; k <= i; k++) {
                qubits2.push_back(qubitList[reg1].start + k);
            }
            cx(qubits2, qubitList[reg1].start + j);
        }
    }
}

// Given two registers, subtract the latter from the former, shifted and controlled
void subtractShiftIf(std::string reg1, std::string reg2, int shift, int control) {
    if (qubitList[reg1].amount != qubitList[reg2].amount) {
        std::cout << "ERROR: Subtracting registers of different sizes " << qubitList[reg1].amount << " / " << qubitList[reg2].amount << std::endl;
        exit(1);
    }
    for (int i = shift; i < qubitList[reg2].amount; i++) {
        std::vector<int> qubits2 = {control, qubitList[reg2].start + i};
        cx(qubits2, qubitList[reg1].start + i - shift);
        for (int j = i-1; j >= shift; j--) {
            std::vector<int> qubits = {control, qubitList[reg2].start + i};
            cx(qubits, qubitList[reg1].start + j - shift);
            for (int k = i-1; k >= j+1; k--) {
                qubits.push_back(qubitList[reg1].start + k - shift);
            }
        }
    }
}

// Divide a register by another register, storing in a third
void divide(std::string divideThis, std::string byThis, std::string result, std::string remainder) {
    if (qubitList[divideThis].amount != qubitList[byThis].amount || qubitList[divideThis].amount != qubitList[result].amount || qubitList[remainder].amount != qubitList[result].amount) {
        std::cout << "ERROR: Dividing registers of different sizes " << qubitList[divideThis].amount << ", " << qubitList[byThis].amount << ", " << qubitList[result].amount << ", " << qubitList[remainder].amount << std::endl;
        exit(1);
    }
    copy(divideThis, remainder);
    for (int i=0; i<qubitList[divideThis].amount; i++) {
        std::string subReg = subreg(remainder, 0, i+1);
        std::string subRegBin = subreg(result, i, i+1);
        std::string subRegToSub = subreg(byThis, qubitList[byThis].amount-i-1, qubitList[byThis].amount);
        flipIfLessOrEqual(subRegBin, byThis, subReg);
        subtractIf(subReg, subRegToSub, subRegBin);
    }
}

// Flip a register if another register has a value less than another register
void flipIfLessOrEqualTC(std::string regToFlip, std::string regToCheck, std::string regToCheck2) {
    if (qubitList[regToCheck].amount >= qubitList[regToCheck2].amount) {
        std::string diffReg = ancillaLock(qubitList[regToCheck].amount, "diffReg");
        std::string isNeg = subreg(diffReg, 0, 1);
        int rec = startRecording();
        copyTC(regToCheck2, diffReg);
        subtract(diffReg, regToCheck);
        stopRecording(rec);
        flipIf(regToFlip, isNeg);
        x(regToFlip);
        uncomputeRecording(rec);
        ancillaFree(diffReg);
    } else {
        std::string diffReg = ancillaLock(qubitList[regToCheck2].amount, "diffReg");
        std::string isNeg = subreg(diffReg, 0, 1);
        int rec = startRecording();
        copyTC(regToCheck, diffReg);
        subtract(diffReg, regToCheck2);
        neg(diffReg);
        stopRecording(rec);
        flipIf(regToFlip, isNeg);
        x(regToFlip);
        uncomputeRecording(rec);
        ancillaFree(diffReg);
    }
}

// Flip a register if another register has a value greater than another register
void flipIfGreaterOrEqualTC(std::string regToFlip, std::string regToCheck, std::string regToCheck2) {
    if (qubitList[regToCheck].amount >= qubitList[regToCheck2].amount) {
        std::string diffReg = ancillaLock(qubitList[regToCheck].amount, "diffReg");
        std::string isNeg = subreg(diffReg, 0, 1);
        int rec = startRecording();
        copyTC(regToCheck2, diffReg);
        subtract(diffReg, regToCheck);
        neg(diffReg);
        stopRecording(rec);
        flipIf(regToFlip, isNeg);
        x(regToFlip);
        uncomputeRecording(rec);
        ancillaFree(diffReg);
    } else {
        std::string diffReg = ancillaLock(qubitList[regToCheck2].amount, "diffReg");
        std::string isNeg = subreg(diffReg, 0, 1);
        int rec = startRecording();
        copyTC(regToCheck, diffReg);
        subtract(diffReg, regToCheck2);
        stopRecording(rec);
        flipIf(regToFlip, isNeg);
        x(regToFlip);
        uncomputeRecording(rec);
        ancillaFree(diffReg);
    }
}

// Divide a register by another register, storing in a third, dealing with negatives
void divideTC(std::string divideThis, std::string byThis, std::string result, std::string remainder) {
    if (qubitList[divideThis].amount != qubitList[byThis].amount || qubitList[divideThis].amount != qubitList[result].amount || qubitList[remainder].amount != qubitList[result].amount) {
        std::cout << "ERROR: Dividing registers of different sizes " << qubitList[divideThis].amount << ", " << qubitList[byThis].amount << ", " << qubitList[result].amount << ", " << qubitList[remainder].amount << std::endl;
        exit(1);
    }
    std::string firstIsNeg = ancillaLock(1, "firstIsNeg");
    std::string secondIsNeg = ancillaLock(1, "secondIsNeg");
    std::string resultIsNeg = ancillaLock(1, "resultIsNeg");
    flipIfNegTC(firstIsNeg, divideThis);
    flipIfNegTC(secondIsNeg, byThis);
    cx(firstIsNeg, resultIsNeg);
    cx(secondIsNeg, resultIsNeg);
    negIf(divideThis, firstIsNeg);
    negIf(byThis, secondIsNeg);
    copy(divideThis, remainder);
    for (int i=0; i<qubitList[divideThis].amount; i++) {
        std::string subReg = subreg(remainder, 0, i+1);
        std::string subRegBin = subreg(result, i, i+1);
        std::string subRegToSub = subreg(byThis, qubitList[byThis].amount-i-1, qubitList[byThis].amount);
        flipIfLessOrEqualTC(subRegBin, byThis, subReg);
        subtractIf(subReg, subRegToSub, subRegBin);
    }
    negIf(result, resultIsNeg);
    negIf(byThis, secondIsNeg);
    negIf(divideThis, firstIsNeg);
    cx(secondIsNeg, resultIsNeg);
    cx(firstIsNeg, resultIsNeg);
    flipIfNegTC(secondIsNeg, byThis);
    flipIfNegTC(firstIsNeg, divideThis);
    ancillaFree(firstIsNeg);
    ancillaFree(secondIsNeg);
    ancillaFree(resultIsNeg);
}

// ------------------------------------------
//                GARBAGE
// ------------------------------------------

// Clear values by swapping them to garbage
int clear(std::string regToDelete) {
    int numInUse = countUsed("garbage");
    if (numInUse + qubitList[regToDelete].amount > qubitList["garbage"].amount) {
        std::cout << "ERROR: Garbage register overflow " << numInUse + qubitList[regToDelete].amount << " / " << qubitList["garbage"].amount << std::endl;
        exit(1);
    }
    for (int i = 0; i < qubitList[regToDelete].amount; i++) {
        swap(qubitList[regToDelete].start + i, qubitList["garbage"].start + numInUse + i);
        qubitList["garbage"].used[numInUse + i] = true;
    }
    return qubitList["garbage"].start + numInUse;
}

// Clear values by swapping them to garbage, if another register is all ones
int clearIf(std::string regToDelete, std::string regToCheck) {
    int numInUse = countUsed("garbage");
    if (numInUse + qubitList[regToDelete].amount > qubitList["garbage"].amount) {
        std::cout << "ERROR: Garbage register overflow " << numInUse + qubitList[regToDelete].amount << " / " << qubitList["garbage"].amount << std::endl;
        exit(1);
    }
    for (int i = 0; i < qubitList[regToDelete].amount; i++) {
        cswap(regToCheck, qubitList[regToDelete].start + i, qubitList["garbage"].start + numInUse + i);
        qubitList["garbage"].used[numInUse + i] = true;
    }
    return qubitList["garbage"].start + numInUse;
}

// ------------------------------------------
//                 TRIG
// ------------------------------------------

// Take the cosine of a reg and put in another, shifted by some number of bits 
void setCos(std::string reg, std::string val, int extraBits) {
    int extraAmount = pow(2, extraBits);
    for (int i = 0; i < 361; i++) {
        int cosVal = int(extraAmount*cos(i * M_PI / 180.0));
        setValTCIfEqual(reg, cosVal, val, i);
    }
}

// Take the sine of a reg and put in another, shifted by some number of bits 
void setSin(std::string reg, std::string val, int extraBits) {
    int extraAmount = pow(2, extraBits);
    for (int i = 0; i < 361; i++) {
        int sinVal = int(extraAmount*sin(i * M_PI / 180.0));
        setValTCIfEqual(reg, sinVal, val, i);
    }
}

// ------------------------------------------
//                 RANDOM
// ------------------------------------------

// Use the random register to return true or false
void flipWithChance(std::string reg, double prob) {

    // Add random x gates to the random register
    for (int i = 0; i < qubitList["random"].amount; i++) {
        if (rand() % 2) {
            x(qubitList["random"].start + i);
        }
    }

    // Compare the random register to the probability
    int compareInt = int(prob * pow(2, qubitList["random"].amount) - pow(2, qubitList["random"].amount - 1));
    flipIfLessOrEqualTC(reg, "random", compareInt);

}

// ------------------------------------------
//                 MISC
// ------------------------------------------

// Add offsets to each register (so that the zero state is the starting state)
void addOffsets() {
	for (auto& [key, value] : qubitList) {
		if (value.offset != 0) {
            if (value.isTC) {
                setValTC(key, value.offset);
            } else {
                setVal(key, value.offset);
            }
		}
	}
}

// Create a domregister of a register
std::string domreg(std::string regName, int newSize) {
    std::string newRegName = "ancillaDom" + regName + "to" + std::to_string(newSize);
    ancillaLock(newSize, newRegName);
    copyTC(regName, newRegName);
    return newRegName;
}

