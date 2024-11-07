#include "debug.h"
#include "globals.h"
#include "registers.h"
#include <algorithm>
#include <fstream>

// Tell the simulator to output the current value of a register, converted to an int
void outputReg(std::string text, std::string registerName, std::string mode) {
    if (forceNoOutput) {
        return;
    }
    std::vector<int> valsToWrite = {1, int(text.size())};
    if (mode == "binary") {
        valsToWrite[0] = 2;
    } else if (mode == "denaryTC") {
        valsToWrite[0] = 3;
    } else if (mode == "binaryIf") {
        valsToWrite[0] = 4;
    }
	std::string gateName = "#o";
    for (size_t i = 0; i < text.size(); i++) {
        valsToWrite.push_back(int(text[i]));
    }
    if (registerName != "") {
        for (int i = qubitList[registerName].start; i < qubitList[registerName].end; i++) {
            valsToWrite.push_back(i);
        }
    }
    for (size_t i = 0; i < valsToWrite.size(); i++) {
        gateName += " q[" + std::to_string(valsToWrite[i]) + "]";
    }
	output.push_back(gateName + ";");
}

// Output all the values of the ancillas at the current moment
void outputAncillas(std::string text, std::string mode) {
    outputReg("----------" + text + "----------");
    for (auto& [key, value] : qubitList) {
        if (key.substr(0, 7) == "ancilla" && key != "ancilla") {
            int startInd = qubitList[key].start - qubitList["ancilla"].start;
            int endInd = qubitList[key].end - qubitList["ancilla"].start;
            outputReg(key + "(" + std::to_string(startInd) + "-" + std::to_string(endInd) + "): ", key, mode);
        }
    }
    outputReg("----------------------------");
}

// Output all the values of the registers at the current moment
void outputAll(std::string text, std::string mode) {
    outputReg("----------" + text + "----------");
    for (auto& [key, value] : qubitList) {
        if (key != "ancilla") {
            int startInd = qubitList[key].start;
            int endInd = qubitList[key].end;
            outputReg(key + "(" + std::to_string(startInd) + "-" + std::to_string(endInd) + "): ", key, mode);
        }
    }
    outputReg("----------------------------");
}

// Add a comment to the output
void comment(std::string comment) {
    output.push_back("# " + comment);
}

// After a section of code, output the number of gates used
void finishSection(std::string name) {
    int gateCount = output.size();
    std::cout << name << ": " << gateCount - currentGateCount << std::endl;
    currentGateCount = gateCount;
}

// Read the output lines in the same way as the simulator
std::vector<int> extractInfo(std::string line) {
    std::vector<int> qubits = {0};
    if (line[0] == 'x' || line[0] == 'c') {
        std::string currentNum = "";
        for (size_t i = 0; i < line.size(); i++) {
            if (line[i] >= '0' && line[i] <= '9') {currentNum += line[i];}
            else if (line[i] == 'c') {qubits[0]++;}
            else if ((line[i] == ']' || line[i] == ' ' || line[i] == ';') && currentNum != "") {
                qubits.push_back(std::stoi(currentNum));
                currentNum = "";
            }
        }
    }
    return qubits;
}

// Check for similar gates that can be combined or removed
void optimize() {

    // Optimizing each function
    int gatesProcessed = 0;
    int gatesRemoved = 0;
    for (size_t l = 0; l < subroutineLines.size(); l++) {
        std::cout << "Optimizing function " << l << " / " << subroutineLines.size() << "...         \r" << std::flush;
        bool inParallel = false;
        for (int i = 1; i < int(subroutineLines[l].size())-2; i++) {

            // Ignore parallel sections
            if (subroutineLines[l][i].size() >= 2 && subroutineLines[l][i][1] == 'j') {
                inParallel = true;
            }
            if (subroutineLines[l][i].size() >= 2 && subroutineLines[l][i][1] == 'w') {
                inParallel = false;
            }
            if (inParallel) {
                continue;
            }
            
            // Parse the same as the simulator
            std::vector<int> qubitsThis = extractInfo(subroutineLines[l][i]);
            std::vector<int> qubitsNext = extractInfo(subroutineLines[l][i+1]);

            // If they have the same number of controls
            if (qubitsThis.size() > 1 && qubitsNext.size() > 1 && qubitsThis[0] == qubitsNext[0]) {

                // Check if the have the same controls
                bool allSame = true;
                for (int j = 1; j < 1+qubitsThis[0]; j++) {
                    if (qubitsThis[j] != qubitsNext[j]) {
                        allSame = false;
                        break;
                    }
                }

                // If so, combine the targets
                if (allSame) {
                    std::vector<int> combined = qubitsThis;
                    for (size_t j = 1+qubitsThis[0]; j < qubitsNext.size(); j++) {
                        auto loc = std::find(combined.begin()+1+qubitsThis[0], combined.end(), qubitsNext[j]);
                        if (loc == combined.end()) {
                            combined.push_back(qubitsNext[j]);
                        } else {
                            combined.erase(loc);
                        }
                    }

                    // If it's non-trivial, output the new line
                    if (combined.size() > 1 && int(combined.size()) > combined[0]+1) {
                        bool lineContainsBracket = std::find(subroutineLines[l][i].begin(), subroutineLines[l][i].end(), '[') != subroutineLines[l][i].end();
                        std::string newLine = "x";
                        for (int j = 0; j < combined[0]; j++) {
                            newLine = "ctrl @ " + newLine;
                        }
                        if (lineContainsBracket) {
                            for (size_t j = 1; j < combined.size(); j++) {
                                newLine += " q[" + std::to_string(combined[j]) + "]";
                                if (j < combined.size()-1) {
                                    newLine += ",";
                                }
                            }
                        } else {
                            for (size_t j = 1; j < combined.size(); j++) {
                                newLine += " q" + std::to_string(combined[j]);
                                if (j < combined.size()-1) {
                                    newLine += ",";
                                }
                            }
                        }
                        newLine += ";";
                        subroutineLines[l][i] = newLine;
                        subroutineLines[l].erase(subroutineLines[l].begin()+i+1);
                        i--;
                        gatesRemoved++;

                    // If it's trivial, remove both lines
                    } else {
                        subroutineLines[l].erase(subroutineLines[l].begin()+i, subroutineLines[l].begin()+i+2);
                        gatesRemoved += 2;
                        i -= 2;

                    }

                }

            }

            // Keep track of the total number of gates processed
            gatesProcessed++;

        }
    }
    std::cout << std::endl;

    // Output how good the reduction was
    std::cout << "Num gates removed: " << gatesRemoved << std::endl;
    std::cout << "As percentage: " << float(gatesRemoved) / float(gatesProcessed) * 100 << "%" << std::endl;

}

// Write the output to a file
int writeCircuit(std::string filename) {

    // Ensure all ancilla are freed
    int numInUse = getNumAncillaUsed();
    int numReservedFunctions = 0;
    for (auto const& [name, val] : qubitList) {
        if (name.substr(0, 15) == "ancillaFunction") {
            numReservedFunctions += val.amount;
            if (ancillaCheck) {
                outputReg("ERROR - function ancilla '" + name + "' freed but not uncomputed: ", name, "binaryIf");
            }
        }
    }
    std::cout << "Ancilla qubits still in use: " << numInUse << std::endl;
    std::cout << "Ancilla qubits reserved for functions: " << numReservedFunctions << std::endl;
    if (numInUse - numReservedFunctions > 0) {
        std::cout << "ERROR: Ancillas not all freed" << std::endl;
        printAncillas();
        return 1;
    }

    // Output some stats about the circuit
    int gateCount = output.size();
    std::cout << "--------------------------------" << std::endl;
    int numDrawPixel = 0;
    int numDrawSprite = 0;
    for (auto const& [name, val] : subroutineNameToIndex) {
        if (name.substr(0, 9) == "drawPixel") {
            numDrawPixel++;
            if (numDrawPixel > 5) {
                continue;
            }
        }
        if (name.substr(0, 10) == "drawSprite") {
            numDrawSprite++;
            if (numDrawSprite > 5) {
                continue;
            }
        }
        if (subroutineLines[val].size() > 0) {
            std::cout << "Subroutine G" << val << " (" << name << ") has " << subroutineLines[val].size() << " lines" << std::endl;
        }
    }
    std::cout << "--------------------------------" << std::endl;
	std::cout << "Total qubits: " << qubitsNeeded << "\n";
	std::cout << "Total qubits (no screen): " << qubitsNeeded - (320*200) << "\n";
	std::cout << "Total gate count: " << gateCount << "\n";
    std::cout << "Num garbage used: " << countUsed("garbage") << "\n";
	std::cout << "Peak ancilla use: " << peakAncillaUsed << "\n";
    std::cout << "--------------------------------" << std::endl;
    int numLines = 0;
    for (size_t i=0; i<subroutineLines.size(); i++) {
        numLines += subroutineLines[i].size();
    }
    numLines += output.size();
    std::cout << "Writing file with " << numLines << " lines" << std::endl;
    std::cout << "Estimated file size in GB: " << numLines*0.00000007 << std::endl;
    std::cout << "--------------------------------" << std::endl;

    // Write the file
    std::string filedata = "";
    filedata += "#q total[" + std::to_string(qubitsNeeded) + "] saved[" + std::to_string(qubitsToSave) + "] nonScreen[" + std::to_string(qubitsNeeded - (320*200)) + "] screen[320][200] cores[" + std::to_string(numCores) + "]\n";
    if (qasmCompliant) {
        filedata += "# To make this file QASM-compliant, replace all \"ALLQUBITS\" with q[0] q[1] q[2] ... q[" + std::to_string(qubitsNeeded) + "]\n";
    }
    filedata += "OPENQASM 3.0;\n";
    filedata += "qreg q[" + std::to_string(qubitsNeeded) + "];\n";
    for (size_t i=0; i<subroutineLines.size(); i++) {
        if (subroutineLines[i].size() > 2) {
            for (size_t j = 0; j < subroutineLines[i].size(); j++) {
                filedata += subroutineLines[i][j] + "\n";
            }
        }
    }
    for (size_t i = 0; i < output.size(); i++) {
        if (output[i].size() > 0) {
            filedata += output[i] + "\n";
        }
    }
	std::ofstream file(filename);
    file << filedata;
	file.close();

    return 0;

}

// Start a parallel block
void beginParallel() {
    if (numCores == 1) {
        return;
    }

    // Make a note of where we are
    parallelStart = output.size();

    // Add a placeholder for the jump gates
    for (int i = 0; i < numCores; i++) {
        output.push_back("#js");
    }

    sectionStarts = {int(output.size())};
    
}

// Finish a parallel section
void sectionParallel() {
    if (numCores == 1) {
        return;
    }

    // Make a note of this location
    output.push_back("#je");
    sectionStarts.push_back(output.size());
    
}

// End a parallel block
void endParallel() {
    if (numCores == 1) {
        return;
    }

    // Make a note of where we are
    parallelEnd = output.size();

    // Stuff for debugging
    std::cout << "Parallel block: " << parallelStart << " to " << parallelEnd << std::endl;
    std::cout << "Section starts: ";
    for (int i = 0; i < numCores; i++) {
        std::cout << sectionStarts[i] << " ";
    }
    std::cout << std::endl;

    // Go back and fill in the jump gates
    for (int i = 0; i < numCores; i++) {
        if (output[parallelStart+i] != "#js") {
            std::cout << "ERROR: Jump gate already filled" << std::endl;
        }
        output[parallelStart+i] = "#j " + std::to_string(i+1) + " " + std::to_string(sectionStarts[i]-parallelStart-i-1) + " ";
    }
    for (int i = 0; i < numCores-1; i++) {
        if (output[sectionStarts[i+1]-1] != "#je") {
            std::cout << "ERROR: End gate already filled" << std::endl;
        }
        output[sectionStarts[i+1]-1] = "#j " + std::to_string(i+1) + " " + std::to_string(parallelEnd-sectionStarts[i+1]) + " ";
    }
    output[parallelEnd-1] = "#j " + std::to_string(numCores) + " " + std::to_string(0) + " ";

    // Add a barrier to ensure all cores are synced at the end
    output.push_back("#w");
    
}

// Start a parallel block in which only the main core is active
void beginMainOnly() {
    if (numCores == 1) {
        return;
    }
    beginParallel();
}

// End a parallel block in which only the main core is active
void endMainOnly() {
    if (numCores == 1) {
        return;
    }
    for (int i = 0; i < numCores; i++) {
        sectionParallel();
    }
    endParallel();
}

// Check for functions that are never called
void checkForUnusedFunctions() {
    std::cout << "Checking for unused functions..." << std::endl;
    std::set<int> usedFunctions;

    // Go through main looking for calls
    for (size_t i = 0; i < output.size(); i++) {
        for (size_t j = 0; j < output[i].size(); j++) {
            if (output[i][j] == 'G') {
                std::string funcName = "";
                for (size_t k = j+1; k < output[i].size(); k++) {
                    if (output[i][k] == ' ' || output[i][k] == ']') {
                        break;
                    }
                    funcName += output[i][k];
                }
                usedFunctions.insert(std::stoi(funcName));
            }
        }
    }

    // Go through all functions looking for calls
    for (size_t i = 0; i < subroutineLines.size(); i++) {
        for (size_t j = 0; j < subroutineLines[i].size(); j++) {
            for (size_t k = 0; k < subroutineLines[i][j].size(); k++) {
                if (subroutineLines[i][j][k] == 'G') {
                    std::string funcName = "";
                    for (size_t l = k+1; l < subroutineLines[i][j].size(); l++) {
                        if (subroutineLines[i][j][l] == ' ' || subroutineLines[i][j][l] == ']') {
                            break;
                        }
                        funcName += subroutineLines[i][j][l];
                    }
                    usedFunctions.insert(std::stoi(funcName));
                }
            }
        }
    }

    // Go through all functions and check if they are used
    for (auto const& [name, val] : subroutineNameToIndex) {
        if (subroutineLines[val].size() > 0 && usedFunctions.find(val) == usedFunctions.end()) {
            std::cout << "Warning - function " << name << " is never called (size " << subroutineLines[val].size() << ")" << std::endl;
        }
    }

}

// Check for unused qubits
void checkForUnusedQubits() {
    std::cout << "Checking for unused qubits..." << std::endl;
    std::set<int> usedQubits;

    // Go through main looking for qubits
    for (size_t i = 0; i < output.size(); i++) {
        for (size_t j = 0; j < output[i].size(); j++) {
            if (output[i][j] == 'q') {
                std::string qubitName = "";
                for (size_t k = j+1; k < output[i].size(); k++) {
                    if (output[i][k] == ' ' || output[i][k] == ']') {
                        break;
                    }
                    if (output[i][k] != '[') {
                        qubitName += output[i][k];
                    }
                }
                usedQubits.insert(std::stoi(qubitName));
            }
        }
    }

    // Go through all functions looking for qubits
    for (size_t i = 0; i < subroutineLines.size(); i++) {
        for (size_t j = 0; j < subroutineLines[i].size(); j++) {
            for (size_t k = 0; k < subroutineLines[i][j].size(); k++) {
                if (subroutineLines[i][j][k] == 'q') {
                    std::string qubitName = "";
                    for (size_t l = k+1; l < subroutineLines[i][j].size(); l++) {
                        if (subroutineLines[i][j][l] == ' ' || subroutineLines[i][j][l] == ']') {
                            break;
                        }
                        if (subroutineLines[i][j][l] != '[') {
                            qubitName += subroutineLines[i][j][l];
                        }
                    }
                    usedQubits.insert(std::stoi(qubitName));
                }
            }
        }
    }

    // Go through all qubits and check if they are used
    for (auto const& [name, val] : qubitList) {
        if (name.find("frameCounter") != std::string::npos) {
            continue;
        }
        if (name.find("subreg") != std::string::npos) {
            continue;
        }
        if (usedQubits.find(val.start) == usedQubits.end()) {
            std::cout << "Warning - start of reg " << name << " is never used" << std::endl;
        }
        if (usedQubits.find(val.end-1) == usedQubits.end()) {
            std::cout << "Warning - end of reg " << name << " is never used" << std::endl;
        }
    }

}

