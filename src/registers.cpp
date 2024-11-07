#include "registers.h"
#include "gates.h"
#include "globals.h"
#include "debug.h"
#include <set>
#include <iostream>

// Init various values in the qubit list
void initQubitList() {

    // Define the qubit list
    qubitList = {

        // Input qubits
        {"inputLeft",        {.saved = true, .amount = 1, .buffer = 0}},
        {"inputRight",       {.saved = true, .amount = 1, .buffer = 0}},
        {"inputUp",          {.saved = true, .amount = 1, .buffer = 0}},
        {"inputDown",        {.saved = true, .amount = 1, .buffer = 0}},
        {"inputSpace",       {.saved = true, .amount = 1, .buffer = 0}},
        {"inputAlt",         {.saved = true, .amount = 1, .buffer = 0}},
        {"inputCtrl",        {.saved = true, .amount = 1, .buffer = 0}},
        {"input1",           {.saved = true, .amount = 1, .buffer = 0}},
        {"input2",           {.saved = true, .amount = 1, .buffer = 0}},
        {"input3",           {.saved = true, .amount = 1, .buffer = 0}},

        // Player info
        {"health",	     {.saved = true, .amount = 9, .buffer = 0, .offset = 100, .min = 0, .max = 200}},
        {"armor",            {.saved = true, .amount = 9, .buffer = 0, .offset = 0, .min = 0, .max = 200}},
        {"ammoBullets",      {.saved = true, .amount = 8, .buffer = 0, .offset = 50, .min = 0, .max = 200}},
        {"ammoShells",       {.saved = true, .amount = 8, .buffer = 0, .offset = 0, .min = 0, .max = 50}},
        {"currentWeapon",    {.saved = true, .amount = 2, .offset = 1, .min = 0, .max = 2}},
        {"hasShotgun",       {.saved = true, .amount = 1, .buffer = 0, .offset = 0}},
        {"playerX",          {.saved = true, .amount = 23, .buffer = 0, .offset = playerStartX, .isTC=true}},
        {"playerY",          {.saved = true, .amount = 23, .buffer = 0, .offset = playerStartY, .isTC=true}},
        {"playerZ",          {.saved = true, .amount = 23, .buffer = 0, .offset = playerStartZ, .isTC=true}},
        {"playerAngle",      {.saved = true, .amount = 9, .buffer = 0, .offset = playerStartAngle}}, // never 0!
        {"headBob",          {.saved = true, .amount = 3, .buffer = 0, .offset = 0}},

        // Keeping track of the screen
        {"elevatorPressed",  {.saved = true, .amount = 1, .buffer = 0, .offset = 0}},
        {"postIntro",        {.saved = true, .amount = 1, .buffer = 0, .offset = 0}},
        {"postMain",         {.saved = true, .amount = 1, .buffer = 0, .offset = 0}},
        {"hasDied",          {.saved = true, .amount = 1, .buffer = 0, .offset = 0}},
        {"postLife",         {.saved = true, .amount = 1, .buffer = 0, .offset = 0}},

        // Animation stuff
        {"frameCounter",     {.saved = true, .amount = 50, .buffer = 0}},
        {"enemyDrawAction",  {.saved = true, .amount = 4, .buffer = 0, .offset = 0}},
        {"enemyDrawFrame",   {.saved = true, .amount = 3, .buffer = 0, .offset = 0}},
        {"weaponFrame",      {.saved = true, .amount = 3, .buffer = 0, .offset = 0}},
        {"weaponFrameI",     {.saved = true, .amount = 1, .buffer = 0, .offset = 0}},
        {"weaponFrameS",     {.saved = true, .amount = 1, .buffer = 0, .offset = 0}},

        // The source of all randomness
        {"random",           {.saved = true, .amount = 10, .buffer = 0}},

        // For rendering
        {"lineXWorldStart",  {.saved = true, .amount = 16, .buffer = 0}},
        {"lineYWorldStart",  {.saved = true, .amount = 16, .buffer = 0}},
        {"lineZWorldStart",  {.saved = true, .amount = 16, .buffer = 0}},
        {"lineXWorldEnd",    {.saved = true, .amount = 16, .buffer = 0}},
        {"lineYWorldEnd",    {.saved = true, .amount = 16, .buffer = 0}},
        {"lineZWorldEnd",    {.saved = true, .amount = 16, .buffer = 0}},
        {"lineXScreenStart", {.saved = true, .amount = 10, .buffer = 0}},
        {"lineYScreenStart", {.saved = true, .amount = 10, .buffer = 0}},
        {"lineXScreenEnd",   {.saved = true, .amount = 10, .buffer = 0}},
        {"lineYScreenEnd",   {.saved = true, .amount = 10, .buffer = 0}},
        {"pixelToDrawX",     {.saved = true, .amount = 10, .buffer = 0}},
        {"pixelToDrawY",     {.saved = true, .amount = 10, .buffer = 0}},
        {"enemyDrawX",       {.saved = true, .amount = 23, .buffer = 0, .offset = 0, .isTC=true}},
        {"enemyDrawY",       {.saved = true, .amount = 23, .buffer = 0, .offset = 0, .isTC=true}},
        {"enemyDrawZ",       {.saved = true, .amount = 23, .buffer = 0, .offset = 0, .isTC=true}},

        // Caching for speed
        {"cosTheta",         {.saved = true, .amount = 23, .buffer = 0, .isTC=true}},
        {"sinTheta",         {.saved = true, .amount = 23, .buffer = 0, .isTC=true}},
        {"cosThetaSmall",    {.saved = true, .amount = 23, .buffer = 0, .isTC=true}},
        {"sinThetaSmall",    {.saved = true, .amount = 23, .buffer = 0, .isTC=true}},

        // Output and other qubits
        {"ancilla",          {.saved = true, .amount = 7200, .buffer = 0}},
        {"garbage",          {.saved = false, .amount = screenWidth, .buffer = 0}},
        {"outputPixels",     {.saved = false, .amount = screenWidth*screenHeight, .buffer = 0}},

    };

    // Create enemy info registers
    std::set<int> enemyIds;
    for (auto& room : level) {
        for (auto& enemyId : room.enemies) {
            enemyIds.insert(enemyId);
        }
    }
    for (auto& enemyId : enemyIds) {
        std::string id = std::to_string(enemyId);
        int x = enemies[enemyId].startLoc[0];
        int y = enemies[enemyId].startLoc[1];
        int z = enemies[enemyId].startLoc[2];
        qubitList["enemy" + id + "X"] = {.saved = true, .amount = 23, .buffer = 0, .offset=x, .isTC=true};
        qubitList["enemy" + id + "Y"] = {.saved = true, .amount = 23, .buffer = 0, .offset=y, .isTC=true};
        qubitList["enemy" + id + "Z"] = {.saved = true, .amount = 23, .buffer = 0, .offset=z, .isTC=true};
        qubitList["enemy" + id + "Action"] = {.saved = true, .amount = 4, .buffer = 0, .offset = 0};
        qubitList["enemy" + id + "IsAlive"] = {.saved = true, .amount = 1, .buffer = 0, .offset = 1};
        qubitList["enemy" + id + "DeathAnim"] = {.saved = true, .amount = 2+frameDuplication, .buffer = 0, .offset = 0};
        qubitList["enemy" + id + "DeathAnimFinished"] = {.saved = true, .amount = 1, .buffer = 0, .offset = 0};
        qubitList["enemy" + id + "PickedUp"] = {.saved = true, .amount = 1, .buffer = 0, .offset = 0};
        int health = 0;
        if (enemies[enemyId].name == "imp") {
            health = 60;
        } else if (enemies[enemyId].name == "zombieman") {
            health = 20;
        }
        qubitList["enemy" + id + "Health"] = {.saved = true, .amount = 8, .buffer = 3, .offset = health, .min = 0, .max = 200};
    }

    // Create pickup info registers
    std::set<int> pickupIds;
    for (auto& room : level) {
        for (auto& pickupId : room.pickups) {
            pickupIds.insert(pickupId);
        }
    }
    for (auto& pickupId : pickupIds) {
        std::string id = std::to_string(pickupId);
        qubitList["pickup" + id + "Collected"] = {.saved = true, .amount = 1, .buffer = 0, .offset=0};
    }

    // Create registers for each door
    std::set<int> doorIds;
    for (auto& room : level) {
        for (auto& wallID : room.walls) {
            if (walls[wallID].type == "door") {
                doorIds.insert(wallID);
            }
        }
    }
    for (auto& doorId : doorIds) {
        std::string id = std::to_string(doorId);
        qubitList["door" + id + "IsOpening"] = {.saved = true, .amount = 1, .buffer = 0, .offset=0};
        qubitList["door" + id + "Open"] = {.saved = true, .amount = doorQubits, .buffer = 0, .offset=doorSpeed+1};
    }

	// Get the list of keys
	std::vector<std::string> keys = {
		"inputLeft",
		"inputRight",
		"inputUp",
		"inputDown",
		"inputSpace",
		"inputAlt",
		"inputCtrl",
		"input1",
		"input2",
		"input3",
	};
	for (auto& [key, value] : qubitList) {
		if (key.substr(0, 5) != "input" && key.substr(0, 6) != "output") {
			keys.push_back(key);
		}
	}
	keys.push_back("outputPixels");

	// First pass: set the start and end values of the saved qubits
	int currentPos = 0;
	for (auto& key : keys) {
		if (qubitList[key].saved) {
			qubitList[key].start = currentPos;
			qubitList[key].end = currentPos + qubitList[key].amount;
			currentPos += qubitList[key].amount;
		}
	}
	qubitsToSave = currentPos;

	// Then the buffers for the saved qubits
	for (auto& key : keys) {
		if (qubitList[key].saved) {
			qubitList[key].bufferStart = currentPos;
			qubitList[key].bufferEnd = currentPos + qubitList[key].buffer;
			currentPos += qubitList[key].buffer;
		}
	}
	
	// Then all the non-saved qubits
	for (auto& key : keys) {
		if (!qubitList[key].saved && key.substr(0, 7) != "garbage") {
			qubitList[key].start = currentPos;
			qubitList[key].end = currentPos + qubitList[key].amount;
			currentPos += qubitList[key].amount;
		}
	}

    // Garbage qubits are part of the screen
    qubitList["garbage"].start = qubitList["outputPixels"].end - screenWidth;
    qubitList["garbage"].end = qubitList["outputPixels"].end;

    // Some qubit lists keep track of which qubits are currently active
    for (auto& [key, value] : qubitList) {
		if (key.substr(0, 7) == "ancilla" || key.substr(0, 7) == "garbage") {
            value.used.resize(value.amount, false);
        } else {
            value.used.resize(value.amount, true);
        }
    }

	// Store the total number of qubits needed
	qubitsNeeded = currentPos;
	nonScreenQubits = qubitsNeeded - screenWidth*screenHeight;

    // Duplicate the frame counts 
    int scaling = std::pow(2, frameDuplication);
    for (auto& [key, value] : framesPerAction) {
        for (size_t i=0; i<value.size(); i++) {
            value[i] *= scaling;
        }
    }

    // Create subregs for each frame counter
    for (int i=qubitList["frameCounter"].amount; i>0; i--) {
        subreg("frameCounter", qubitList["frameCounter"].amount-i, qubitList["frameCounter"].amount, "frameCounter" + std::to_string(i));
    }
 
}

// Output the qubit list
void printQubitList() {
    std::cout << "Qubit list:" << std::endl;
    for (auto& [key, value] : qubitList) {
        std::cout << key << ": " << value.start << " to " << value.end << " (" << value.amount << ")" << std::endl;
    }
}

// Output every ancilla name
void printAncillas() {
    std::cout << "Current ancilla registers:" << std::endl;
    for (auto& [key, value] : qubitList) {
        if (key.substr(0, 7) == "ancilla" && key != "ancilla") {
            std::cout << key << ": " << value.amount << " (" << value.start << " to " << value.end << ")" << std::endl;
        }
    }
}

// Given a string, return a string with the first letter capitalized
std::string toCap(std::string str) {
    if (str.size() == 0) {
        return str;
    }
    str[0] = std::toupper(str[0]);
    return str;
}

// Return the number of ancilla qubits used
int getNumAncillaUsed() {
    int numUsed = 0;
    for (int i = 0; i < qubitList["ancilla"].amount; i++) {
        if (qubitList["ancilla"].used[i]) {
            numUsed++;
        }
    }
    return numUsed;
}

// Lock a section of ancilla qubits and return an identifier
std::string ancillaLock(int numRequired, std::string name) {

    // Make sure it's not zero or fewer
    if (numRequired <= 0) {
        std::cout << "WARNING: Skipping ancilla register creation with zero or fewer qubits - " << name << std::endl;
        return "";
    }

    // Search for a suitable section of unused ancilla qubits
    int ancStart = -1;
    for (int i = 0; i < qubitList["ancilla"].amount - numRequired; i++) {
        bool spotFound = true;
        for (int j = i; j < i + numRequired; j++) {
            if (qubitList["ancilla"].used[j]) {
                spotFound = false;
                break;
            }
        }
        if (spotFound) {
            ancStart = i;
            break;
        }
    }

    // Ensure we have enough space
    if (ancStart <= -1) {
        std::cout << "ERROR: Not enough ancilla qubits" << std::endl;
        std::cout << "Requested: " << numRequired << std::endl;
        std::cout << "Total available: " << qubitList["ancilla"].amount << std::endl;
        int numInUse = 0;
        for (int i = 0; i < qubitList["ancilla"].amount; i++) {
            if (qubitList["ancilla"].used[i]) {
                numInUse++;
            }
        }
        std::cout << "Num in use: " << numInUse << std::endl;
        printAncillas();
        exit(1);
    }
    int ancEnd = ancStart + numRequired;

    // Get the name
    std::string ancName = "ancilla" + std::to_string(ancStart) + "to" + std::to_string(ancEnd);
    if (name != "") {
        ancName = name;
        if (ancName.substr(0, 7) != "ancilla") {
            ancName = "ancilla" + toCap(ancName);
        }
        while (qubitList.find(ancName) != qubitList.end()) {
            ancName += "+";
        }
    }

    // Create the ancilla register
    qubitList[ancName] = {.saved = true, .start = qubitList["ancilla"].start + ancStart, .end = qubitList["ancilla"].start + ancEnd, .amount = numRequired};

    // Set these qubits as used
    for (int i = ancStart; i < ancEnd; i++) {
        qubitList["ancilla"].used[i] = true;
    }

    // Check if we've used more ancilla qubits than ever before
    int numUsed = getNumAncillaUsed();
    if (numUsed > peakAncillaUsed) {
        peakAncillaUsed = numUsed;
    }
    if (recordingPeakAncilla && numUsed > peakAncillaUsedTemp) {
        peakAncillaUsedTemp = numUsed;
    }

    // Sanity check to make sure it's zero when initialized
    if (ancillaCheck) {
        outputReg("ERROR - ancilla '" + ancName + "' created from " + std::to_string(ancStart) + " to " + std::to_string(ancEnd) + " with a non-zero value: ", ancName, "binaryIf");
    }

    return ancName;

}

// Free a section of ancilla qubits
void ancillaFree(std::string ancName) {
    if (qubitList.find(ancName) == qubitList.end()) {
        std::cout << "ERROR: Ancilla register does not exist: " << ancName << std::endl;
        exit(1);
    }
    if (ancillaCheck) {
        outputReg("ERROR - ancilla '" + ancName + "' freed but not uncomputed: ", ancName, "binaryIf");
    }
    for (int i = qubitList[ancName].start; i < qubitList[ancName].end; i++) {
        qubitList["ancilla"].used[i - qubitList["ancilla"].start] = false;
    }
    qubitList.erase(ancName);
}

// Create a subregister of a register
std::string subreg(std::string regName, int start, int end) {
    std::string newRegName = "sub" + regName + "from" + std::to_string(start) + "to" + std::to_string(end);
    qubitList[newRegName] = {.saved = true, .start = qubitList[regName].start + start, .end = qubitList[regName].start + end, .amount = end-start};
    return newRegName;
}

// Create a subregister of a register with a specific name
std::string subreg(std::string regName, int start, int end, std::string newRegName) {
    qubitList[newRegName] = {.saved = true, .start = qubitList[regName].start + start, .end = qubitList[regName].start + end, .amount = end-start};
    return newRegName;
}

// Clear all subregs for memory reasons
void clearAllSubregs() {
    std::vector<std::string> toDelete = {};
    for (auto& [key, value] : qubitList) {
        if (key.substr(0, 3) == "sub") {
            toDelete.push_back(key);
        }
    }
    for (auto& key : toDelete) {
        qubitList.erase(key);
    }
}

// Start recording for the purposes of later uncomputation
int startRecording() {
    recordings.push_back({output.size(), -1});
    return recordings.size() - 1;
}

// Stop recording 
void stopRecording(int ind) {
    recordings[ind].second = output.size();
}

// Get the subroutine name from the index (invert the map)
std::string getSubroutineNameFromIndex(int ind) {
    for (auto& [key, value] : subroutineNameToIndex) {
        if (value == ind) {
            return key;
        }
    }
    return "";
}

// Start the definition of a subroutine
void beginSubroutine(std::string name) {
    if (currentSubroutineStart != -1) {
        std::cout << "ERROR: Subroutine already in progress" << std::endl;
        exit(1);
    }
    if (subroutineNameToIndex.find(name) != subroutineNameToIndex.end()) {
        std::cout << "ERROR: Subroutine already exists" << std::endl;
        exit(1);
    }
    currentSubroutineStart = output.size();
    subroutineNameToIndex[name+"Inv"] = subroutineLines.size();
    subroutineNameToIndex[name] = subroutineLines.size()+1;
    subroutineLines.push_back({});
    subroutineLines.push_back({});
    subroutineCallString.push_back("");
}

// Finish the definition of a subroutine
void endSubroutine() {

    // Ensure a subroutine is in progress
    if (currentSubroutineStart == -1) {
        std::cout << "ERROR: No subroutine in progress" << std::endl;
        exit(1);
    }

    // Move lines into the subroutine
    int ind = subroutineLines.size() - 1;
    for (int i = currentSubroutineStart; i < int(output.size()); i++) {
        subroutineLines[ind].push_back(output[i]);
    }
    output.erase(output.begin()+currentSubroutineStart, output.end());

    // Check this line for any q[...] references
    std::set<int> qubitsUsed;
    if (qasmCompliant) {
        for (size_t i = 0; i < subroutineLines[ind].size(); i++) {
            std::string lineWithoutBrackets = "";
            for (size_t j = 0; j < subroutineLines[ind][i].size(); j++) {
                //if (subroutineLines[ind][i][j] == 'q') {
                    //std::string qubitNumString = "";
                    //for (size_t k = j+1; k < subroutineLines[ind][i].size(); k++) {
                        //if (subroutineLines[ind][i][k] == ']' || subroutineLines[ind][i][k] == ' ') {
                            //break;
                        //}
                        //if (subroutineLines[ind][i][k] != '[') {
                            //qubitNumString += subroutineLines[ind][i][k];
                        //}
                    //}
                    //int qubitNum = std::stoi(qubitNumString);
                    //if (qubitNum <= qubitsNeeded) {
                        //qubitsUsed.insert(qubitNum);
                    //}
                //}
                if (subroutineLines[ind][i][j] != '[' && subroutineLines[ind][i][j] != ']') {
                    lineWithoutBrackets += subroutineLines[ind][i][j];
                }
            }
            subroutineLines[ind][i] = lineWithoutBrackets;
        }
    }

    // Construct the call string and the function header
    std::string callString = "";
    std::string functionHeader = "gate G" + std::to_string(ind);
    if (qasmCompliant) {
        //int count = 0;
        //for (auto& qubitNum : qubitsUsed) {
            //callString += " q[" + std::to_string(qubitNum) + "]";
            //functionHeader += " q" + std::to_string(qubitNum);
            //if (count < int(qubitsUsed.size())-1) {
                //functionHeader += ",";
                //callString += ",";
            //}
            //count++;
        //}
        callString += " ALLQUBITS";
        functionHeader += " ALLQUBITS";
    }

    // Set all the globals
    subroutineLines[ind].insert(subroutineLines[ind].begin(), functionHeader + " {");
    subroutineLines[ind].push_back("}");
    subroutineCallString.push_back(callString);
    currentSubroutineStart = -1;

}

// Create the inverse of a subroutine
void createInverse(int ind) {

    // Create the new subroutine
    if (subroutineLines[ind-1].size() > 0) {
        return;
    }
    subroutineCallString[ind-1] = subroutineCallString[ind];
    std::vector<std::string> newLines = {};

    // Copy the lines in reverse order, changing the gate name
    int locG = 5;
    int endLoc = -1;
    for (size_t j = int(locG)+1; j < subroutineLines[ind][0].size(); j++) {
        endLoc = j;
        if (subroutineLines[ind][0][j] == ' ') {
            break;
        }
    }
    std::string newFirstLine = "gate G" + std::to_string(ind-1) + subroutineLines[ind][0].substr(endLoc);
    newLines.push_back(newFirstLine);
    for (int i = int(subroutineLines[ind].size())-2; i > 0; i--) {
        if (subroutineLines[ind][i].find("#o") == std::string::npos) {
            newLines.push_back(subroutineLines[ind][i]);
        }
    }
    newLines.push_back(subroutineLines[ind][subroutineLines[ind].size()-1]);
    subroutineLines[ind-1] = newLines;

}

// When told to uncompute the recording
void uncomputeRecording(int ind) {

    // For each line in the recording
    for (int i = recordings[ind].second-1; i >= recordings[ind].first; i--) {

        // Make sure it's not a debug output
        if (output[i].find("#o") == std::string::npos) {
            std::string lineToAdd = output[i];

            // Check for subroutine calls
            auto locG = output[i].find("G");
            if (locG != std::string::npos) {

                // Get the number of the subroutine
                std::string funcNumString = "";
                int endLoc = -1;
                for (size_t j = int(locG)+1; j < output[i].size(); j++) {
                    endLoc = j;
                    if (output[i][j] == ' ') {
                        break;
                    }
                    funcNumString += output[i][j];
                }
                int funcNum = std::stoi(funcNumString);

                // Create a new subroutine which is the inverse of the old one
                std::string newSubName = getSubroutineNameFromIndex(funcNum) + "Inv";
                if (newSubName.find("drawPixel") != std::string::npos) {
                    continue;
                }
                createInverse(funcNum);
                int newSubNum = subroutineNameToIndex[newSubName];

                // Replace the subroutine call with the new subroutine call
                lineToAdd = lineToAdd.substr(0, locG) + "G" + std::to_string(newSubNum) + lineToAdd.substr(endLoc);

            }

            // Add the uncomputed
            output.push_back(lineToAdd);

        }

    }
}

// Count the number of used qubits for a register
int countUsed(std::string regName) {
    int count = 0;
    for (int i = 0; i < qubitList[regName].amount; i++) {
        if (qubitList[regName].used[i]) {
            count++;
        }
    }
    return count;
}

// Call a subroutine
void callSubroutine(std::string name) {
    if (subroutineNameToIndex.find(name) == subroutineNameToIndex.end()) {
        std::cout << "ERROR: Subroutine does not exist: " << name << std::endl;
        exit(1);
    }
    output.push_back("G" + std::to_string(subroutineNameToIndex[name]) + subroutineCallString[subroutineNameToIndex[name]] + ";");
}

// Call a subroutine conditioned on a register
void callSubroutineIf(std::string name, std::string regName) {
    if (subroutineNameToIndex.find(name) == subroutineNameToIndex.end()) {
        std::cout << "ERROR: Subroutine does not exist: " << name << std::endl;
        exit(1);
    }
    std::string gateName = "G" + std::to_string(subroutineNameToIndex[name]);
    for (int i = qubitList[regName].start; i < qubitList[regName].end; i++) {
        gateName = "ctrl @ " + gateName + " q[" + std::to_string(i) + "], ";
    }
    output.push_back(gateName + subroutineCallString[subroutineNameToIndex[name]] + ";");
}

// Call a subroutine conditioned on a several registers
void callSubroutineIfAnd(std::string name, std::vector<std::string> regNames) {
    if (subroutineNameToIndex.find(name) == subroutineNameToIndex.end()) {
        std::cout << "ERROR: Subroutine does not exist: " << name << std::endl;
        exit(1);
    }
    std::string gateName = "G" + std::to_string(subroutineNameToIndex[name]);
    for (auto& regName : regNames) {
        for (int i = qubitList[regName].start; i < qubitList[regName].end; i++) {
            gateName = "ctrl @ " + gateName + " q[" + std::to_string(i) + "], ";
        }
    }
    output.push_back(gateName + subroutineCallString[subroutineNameToIndex[name]] + ";");
}

// Call a subroutine conditioned on a register
void callSubroutineIf(std::string name, int controlQubit) {
    if (subroutineNameToIndex.find(name) == subroutineNameToIndex.end()) {
        std::cout << "ERROR: Subroutine does not exist: " << name << std::endl;
        exit(1);
    }
    std::string gateName = "G" + std::to_string(subroutineNameToIndex[name]);
    gateName = "ctrl @ " + gateName + " q[" + std::to_string(controlQubit) + "], ";
    output.push_back(gateName + subroutineCallString[subroutineNameToIndex[name]] + ";");
}

// When told to start recording peak ancilla usage
void startPeakAncilla() {
    if (recordingPeakAncilla) {
        std::cout << "ERROR: Already recording peak ancilla usage" << std::endl;
        exit(1);
    }
    peakAncillaUsedPrev = getNumAncillaUsed();
    peakAncillaUsedTemp = peakAncillaUsedPrev;
    recordingPeakAncilla = true;
}

// When told to stop recording peak ancilla usage
int stopPeakAncilla() {
    recordingPeakAncilla = false;
    return peakAncillaUsedTemp - peakAncillaUsedPrev;
}


