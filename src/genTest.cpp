#include <iostream>
#include <fstream>

// Local files
#include "globals.h"
#include "debug.h"
#include "registers.h"
#include "gates.h"
#include "binary.h"
#include "draw.h"
#include "levels.h"
#include "logic.h"

// Entry point
int main(int argc, char** argv) {

	initQubitList();

    beginSubroutine("testSub");
        setValTC("health", 100);
        setValTC("health", 100);
        outputReg("health = ", "health", "denaryTC");
    endSubroutine();
    callSubroutine("testSub");
    
    //std::string lessThan400 = ancillaLock(1);
    //std::string greaterThan400 = ancillaLock(1);
    //std::string lessThan500 = ancillaLock(1);
    //std::string greaterThan300 = ancillaLock(1);
    //std::string lessThan399 = ancillaLock(1);
    //std::string greaterThan401 = ancillaLock(1);
    //beginSubroutine("testSub");
        //int rec = startRecording();
        //setValTC("health", 200);
        //flipIfLessOrEqualTC(lessThan400, "health", 200);
        //flipIfGreaterOrEqualTC(greaterThan400, "health", 200);
        //flipIfLessOrEqualTC(lessThan500, "health", 300);
        //flipIfGreaterOrEqualTC(greaterThan300, "health", 100);
        //flipIfLessOrEqualTC(lessThan399, "health", 199);
        //flipIfGreaterOrEqualTC(greaterThan401, "health", 201);
        //stopRecording(rec);
        //outputReg("health = ", "health", "denaryTC");
        //outputReg("lessThan200 = ", lessThan400, "binary");
        //outputReg("greaterThan200 = ", greaterThan400, "binary");
        //outputReg("lessThan300 = ", lessThan500, "binary");
        //outputReg("greaterThan100 = ", greaterThan300, "binary");
        //outputReg("lessThan199 = ", lessThan399, "binary");
        //outputReg("greaterThan201 = ", greaterThan401, "binary");
        //uncomputeRecording(rec);
    //endSubroutine();
    //ancillaFree(lessThan400);
    //ancillaFree(greaterThan400);
    //ancillaFree(lessThan500);
    //ancillaFree(greaterThan300);
    //ancillaFree(lessThan399);
    //ancillaFree(greaterThan401);

    optimize();

    return writeCircuit("test.qasm");
	
}

