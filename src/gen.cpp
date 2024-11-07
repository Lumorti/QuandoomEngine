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

	// Load the level
    std::cout << "--------------------------------" << std::endl;
    initLevel();
    checkLevel();

    // Init the circuit
	initQubitList();
	addOffsets();
    h("random");
    //outputReg("random = ", "random", "binary");
    finishSection("circuit init");

    // Incremement the counters
    add("frameCounter", 1);
    //outputReg("frameCounter = ", "frameCounter", "denary");

    // Which enemies and pickups we have
    std::set<std::string> enemyTypes;
    for (auto enemy : enemies) {
        if (enemy.name.size() > 0) {
            enemyTypes.insert(enemy.name);
        }
    }
    bool pickupsExist = false;
    for (auto pickup : pickups) {
        if (pickup.name.size() > 0) {
            pickupsExist = true;
            break;
        }
    }

    // Sprite loading
    if (!simpleRooms) {
        for (auto enemyType : enemyTypes) {
            for (int i=1; i<=maxSpriteHeight; i++) {
                loadPPMSheet("sprites/"+enemyType+"/"+enemyType+"-"+std::to_string(i)+".ppm", enemyType+"-"+std::to_string(i), i, i, 1);
            }
        }
    }
    for (int i=1; i<=8; i++) {
        loadPPM("sprites/weapons/fists/fists" + std::to_string(i) + ".ppm", "fists-" + std::to_string(i));
    }
    for (int i=1; i<=8; i++) {
        loadPPM("sprites/weapons/pistol/pistol" + std::to_string(i) + ".ppm", "pistol-" + std::to_string(i));
    }
    loadPPM("sprites/ui/intro.ppm", "intro");
    loadPPM("sprites/ui/death.ppm", "death");
    loadPPM("sprites/ui/endings/ending1.ppm", "ending1");
    loadPPM("sprites/ui/endings/ending2.ppm", "ending2");
    loadPPM("sprites/ui/endings/ending3.ppm", "ending3");
    loadPPM("sprites/ui/endings/ending4.ppm", "ending4");
    loadPPM("sprites/ui/ui.ppm", "ui");
    loadPPMSheet("sprites/numbers/numbers.ppm", "numbers", 20, 20, 1);
    if (pickupsExist && !simpleRooms) {
        for (int i=1; i<=maxSpriteHeight; i++) {
            loadPPMSheet("sprites/pickups/pickups-"+std::to_string(i)+".ppm", "pickups-"+std::to_string(i), i, i, 1);
        }
    }
    finishSection("sprite loading");

    // Checking for multiple key presses
    std::string leftAndNotAlt = ancillaLock(1, "ancillaLeftAndNotAlt");
    std::string rightAndNotAlt = ancillaLock(1, "ancillaRightAndNotAlt");
    std::string leftAndAlt = ancillaLock(1, "ancillaLeftAndAlt");
    std::string rightAndAlt = ancillaLock(1, "ancillaRightAndAlt");
    int recAlt = startRecording();
    flipIfAnd(leftAndAlt, {"inputLeft", "inputAlt"});
    flipIfAnd(rightAndAlt, {"inputRight", "inputAlt"});
    x("inputAlt");
    flipIfAnd(leftAndNotAlt, {"inputLeft", "inputAlt"});
    flipIfAnd(rightAndNotAlt, {"inputRight", "inputAlt"});
    x("inputAlt");
    stopRecording(recAlt);
    finishSection("multiple key presses");

    // Ancilla definition that needs to be before main
    std::string screenCheck = ancillaLock(1, "ancillaScreenCheck");

    // Function/gate defs
    functionDrawPixel("pixelToDrawX", "pixelToDrawY", "Main");
    functionRenderLine("lineXWorldStart", "lineYWorldStart", "lineZWorldStart", "lineXWorldEnd", "lineYWorldEnd", "lineZWorldEnd");
    if (!simpleRooms) {
        for (auto enemyType : enemyTypes) {
            std::cout << "creating function for enemy type " << enemyType << std::endl;
            functionRenderEnemy(enemyType, "enemyDrawX", "enemyDrawY", "enemyDrawZ", {0,maxFrame,0,maxAction});
        }
        if (pickupsExist) {
            functionRenderEnemy("pickups", "enemyDrawX", "enemyDrawY", "enemyDrawZ", {0,7,0,2});
        }
    }
    finishSection("function defs");

    // Wrap the intro in a function
    beginSubroutine("intro");
        beginMainOnly();
            drawSprite("intro", 0, 0);
        endMainOnly();
    endSubroutine();

    // Wrap the death screen in a function
    beginSubroutine("death");
        beginMainOnly();
            //outputReg("drawing death screen", "ancillaScreenCheck", "binary");
            drawSprite("death", 0, 0);
        endMainOnly();
    endSubroutine();

    // Wrap the ending in a function
    beginSubroutine("ending");
        beginMainOnly();

            // Pick one of the endings at "random"
            std::string ancillaEnding = ancillaLock(1);
            //outputReg("playerX = ", "playerX", "binary");
            //outputReg("playerY = ", "playerY", "binary");
            std::string rand1 = subreg("playerX", qubitList["playerX"].amount-1, qubitList["playerX"].amount);
            std::string rand2 = subreg("playerY", qubitList["playerY"].amount-1, qubitList["playerY"].amount);
            //outputReg("rand1 = ", rand1, "binary");
            //outputReg("rand2 = ", rand2, "binary");

            // Ending 11
            flipIfAnd(ancillaEnding, {rand1, rand2});
            drawSprite("ending1", 0, 0, ancillaEnding);
            flipIfAnd(ancillaEnding, {rand1, rand2});

            // Ending 01
            x(rand1);
            flipIfAnd(ancillaEnding, {rand1, rand2});
            drawSprite("ending2", 0, 0, ancillaEnding);
            flipIfAnd(ancillaEnding, {rand1, rand2});
            x(rand1);

            // Ending 10
            x(rand2);
            flipIfAnd(ancillaEnding, {rand1, rand2});
            drawSprite("ending3", 0, 0, ancillaEnding);
            flipIfAnd(ancillaEnding, {rand1, rand2});
            x(rand2);

            // Ending 00
            x(rand1);
            x(rand2);
            flipIfAnd(ancillaEnding, {rand1, rand2});
            drawSprite("ending4", 0, 0, ancillaEnding);
            flipIfAnd(ancillaEnding, {rand1, rand2});
            x(rand2);
            x(rand1);

            // Free the ancilla
            ancillaFree(ancillaEnding);

        endMainOnly();
    endSubroutine();

    // Subroutine to reverse the movement
    beginSubroutine("reverseMovement");
        for (int i=0; i<moveSpeed; i++) {
            addIf("playerX", "sinThetaSmall", "inputUp");
            subtractIf("playerY", "cosThetaSmall", "inputUp");
            subtractIf("playerX", "sinThetaSmall", "inputDown");
            addIf("playerY", "cosThetaSmall", "inputDown");
            addIf("playerY", "sinThetaSmall", leftAndAlt);
            addIf("playerX", "cosThetaSmall", leftAndAlt);
            subtractIf("playerY", "sinThetaSmall", rightAndAlt);
            subtractIf("playerX", "cosThetaSmall", rightAndAlt);
        }
    endSubroutine();

    // For each room, create the function and then run if them if inside
    for (size_t i=0; i<level.size(); i++) {
        std::cout << "Generating render function for room with id " << level[i].id << " at loc " << i << std::endl;
        functionRenderRoom(level[i]);
    }
    functionRoomLogic(level);

    // Wrap main in a function
    beginSubroutine("main");

        // Turning
        for (int i=0; i<turnSpeed; i++) {
            addWrappedIf("playerAngle", 1, leftAndNotAlt, 0, 361);
            subtractWrappedIf("playerAngle", 1, rightAndNotAlt, 0, 361);
        }
        finishSection("turning");

        // Update the cos and sin of the angle
        int leftShift = 5;
        int leftShiftSmall = 2;
        setCos("cosTheta", "playerAngle", leftShift);
        setSin("sinTheta", "playerAngle", leftShift);
        setCos("cosThetaSmall", "playerAngle", leftShiftSmall);
        setSin("sinThetaSmall", "playerAngle", leftShiftSmall);
        finishSection("sin and cos calc");

        // Movement
        for (int i=0; i<moveSpeed; i++) {
            subtractIf("playerX", "sinThetaSmall", "inputUp");
            addIf("playerY", "cosThetaSmall", "inputUp");
            addIf("playerX", "sinThetaSmall", "inputDown");
            subtractIf("playerY", "cosThetaSmall", "inputDown");
            subtractIf("playerY", "sinThetaSmall", leftAndAlt);
            subtractIf("playerX", "cosThetaSmall", leftAndAlt);
            addIf("playerY", "sinThetaSmall", rightAndAlt);
            addIf("playerX", "cosThetaSmall", rightAndAlt);
        }
        finishSection("movement");

        // Head bob
        std::string someMovement = ancillaLock(1);
        flipIfOr(someMovement, {"inputUp", "inputDown", leftAndAlt, rightAndAlt});
        addIf("headBob", 1, someMovement);
        flipIfOr(someMovement, {"inputUp", "inputDown", leftAndAlt, rightAndAlt});
        ancillaFree(someMovement);

        // Debug output
        if (debugRender) {
            outputReg("----------------------------------------------");
            outputReg("playerAngle = ", "playerAngle", "denary");
            outputReg("playerX = ", "playerX", "denaryTC");
            outputReg("playerY = ", "playerY", "denaryTC");
            outputReg("playerZ = ", "playerZ", "denaryTC");
        }

        // Weapon frame update
        int maxVal = std::pow(2, qubitList["weaponFrame"].amount)-1;
        std::string ancillaWeaponFrame = ancillaLock(1, "ancillaWeaponFrame");
        flipIfEqual(ancillaWeaponFrame, "weaponFrame", maxVal);
        clearIf("weaponFrameS", ancillaWeaponFrame);
        cx(ancillaWeaponFrame, "weaponFrameS");
        flipIfEqual(ancillaWeaponFrame, "weaponFrame", maxVal);
        ancillaFree(ancillaWeaponFrame);
        clearIf("weaponFrameI", "weaponFrameS");
        addIf("weaponFrame", 1, "weaponFrameS");
        addIf("weaponFrame", 1, "weaponFrameI");
        //outputReg("weaponFrame = ", "weaponFrame", "denary");
        //outputReg("weaponFrameI = ", "weaponFrameI", "binary");
        //outputReg("weaponFrameS = ", "weaponFrameS", "binary");
        clear("weaponFrameS");

        // Change weapon
        std::string shouldClear = ancillaLock(1, "ancillaShouldClearWeapon");
        std::string ancillaBinaryShotgun = ancillaLock(1, "ancillaBinaryShotgun");
        std::string switchToFist = ancillaLock(1, "ancillaSwitchToFirst");
        std::string switchToPistol = ancillaLock(1, "ancillaSwitchToPistol");
        std::string switchToShotgun = ancillaLock(1, "ancillaSwitchToShotgun");
        int recWeapon = startRecording();
        flipIfAnd(ancillaBinaryShotgun, {"hasShotgun", "input3"});
        flipIfOr(shouldClear, {"input1", "input2", ancillaBinaryShotgun});
        flipIfAnd(switchToFist, {"input1"});
        x("input1");
        flipIfAnd(switchToPistol, {"input1", "input2"});
        x("input2");
        flipIfAnd(switchToShotgun, {"input1", "input2", ancillaBinaryShotgun});
        x("input2");
        x("input1");
        stopRecording(recWeapon);
        clearIf("currentWeapon", shouldClear);
        setValIf("currentWeapon", 0, switchToFist);
        setValIf("currentWeapon", 1, switchToPistol);
        setValIf("currentWeapon", 2, switchToShotgun);
        uncomputeRecording(recWeapon);
        ancillaFree(ancillaBinaryShotgun);
        ancillaFree(shouldClear);
        ancillaFree(switchToFist);
        ancillaFree(switchToPistol);
        ancillaFree(switchToShotgun);
        finishSection("weapon swapping");

        // For each room, create the function and then run if them if inside
        for (size_t i=0; i<level.size(); i++) {
            std::cout << "Processing room with id " << level[i].id << " at loc " << i << std::endl;
            processRoomIfInside(level[i]);
        }
        finishSection("room drawing and logic");

        // Check if the player's health is negative
        //outputReg("health = ", "health", "denary");
        std::string ancillaBinaryHealth = ancillaLock(1);
        flipIfGreaterOrEqual(ancillaBinaryHealth, "health", 300);
        //outputReg("ancillaBinaryHealth = ", ancillaBinaryHealth, "binary");
        clearIf("hasDied", ancillaBinaryHealth);
        cx(ancillaBinaryHealth, "hasDied");
        flipIfGreaterOrEqual(ancillaBinaryHealth, "health", 300);
        ancillaFree(ancillaBinaryHealth);

        // Make sure the player's armour is at least 0
        std::string ancillaBinaryArmor = ancillaLock(1);
        flipIfGreaterOrEqual(ancillaBinaryArmor, "armor", 300);
        //outputReg("armor = ", "armor", "denary");
        //outputReg("ancillaBinaryArmor = ", ancillaBinaryArmor, "binary");
        clearIf("armor", ancillaBinaryArmor);
        clear(ancillaBinaryArmor);
        ancillaFree(ancillaBinaryArmor);

        // All static draw calls on the main core
        beginMainOnly();

            // Draw the weapon
            std::string ancillaBinaryWeaponDraw = ancillaLock(1);
            std::string ancillaWeaponFrameCheck = ancillaLock(1);
            std::string ancillaWeaponOverall = ancillaLock(1);
            flipIfEqual(ancillaBinaryWeaponDraw, "currentWeapon", 0);
            for (int i=1; i<=8; i++) {
                flipIfEqual(ancillaWeaponFrameCheck, "weaponFrame", i-1);
                flipIfAnd(ancillaWeaponOverall, {ancillaBinaryWeaponDraw, ancillaWeaponFrameCheck});
                drawSprite("fists-"+std::to_string(i), 105, 126, ancillaWeaponOverall);
                flipIfAnd(ancillaWeaponOverall, {ancillaBinaryWeaponDraw, ancillaWeaponFrameCheck});
                flipIfEqual(ancillaWeaponFrameCheck, "weaponFrame", i-1);
            }
            flipIfEqual(ancillaBinaryWeaponDraw, "currentWeapon", 0);
            flipIfEqual(ancillaBinaryWeaponDraw, "currentWeapon", 1);
            for (int i=1; i<=8; i++) {
                flipIfEqual(ancillaWeaponFrameCheck, "weaponFrame", i-1);
                flipIfAnd(ancillaWeaponOverall, {ancillaBinaryWeaponDraw, ancillaWeaponFrameCheck});
                drawSprite("pistol-"+std::to_string(i), 135, 101, ancillaWeaponOverall);
                flipIfAnd(ancillaWeaponOverall, {ancillaBinaryWeaponDraw, ancillaWeaponFrameCheck});
                flipIfEqual(ancillaWeaponFrameCheck, "weaponFrame", i-1);
            }
            flipIfEqual(ancillaBinaryWeaponDraw, "currentWeapon", 1);
            ancillaFree(ancillaBinaryWeaponDraw);
            ancillaFree(ancillaWeaponFrameCheck);
            ancillaFree(ancillaWeaponOverall);
            finishSection("weapon drawing");

            // Draw the ui
            drawSprite("ui", 0, 169);
            drawNumber("ammoBullets", 37, 169, true);  
            drawNumber("health", 72, 169, true);
            drawNumber("armor", 170, 169, true);
            drawNumber("ammoBullets", 235, 166, false);
            drawNumber("ammoShells", 235, 172, false);
            finishSection("ui");

        endMainOnly();

        // Uncompute the cos and sin of the angle
        setCos("cosTheta", "playerAngle", leftShift);
        setSin("sinTheta", "playerAngle", leftShift);
        setCos("cosThetaSmall", "playerAngle", leftShiftSmall);
        setSin("sinThetaSmall", "playerAngle", leftShiftSmall);
        finishSection("cos sin uncompute");

    endSubroutine();

    // Leave the intro if we've ever pressed space
    clearIf("postIntro", "inputSpace");
    flipIf("postIntro", "inputSpace");

    // Finish if we've ever pressed the elevator button
    clearIf("postMain", "elevatorPressed");
    flipIf("postMain", "elevatorPressed");

    // Finish if we've ever died
    clearIf("postLife", "hasDied");
    flipIf("postLife", "hasDied");

    // Run the intro if not post intro, not died, not post main
    x("postIntro");
    x("postMain");
    x("postLife");
    flipIfAnd(screenCheck, {"postIntro", "postMain", "postLife"});
    callSubroutineIf("intro", "ancillaScreenCheck");
    flipIfAnd(screenCheck, {"postIntro", "postMain", "postLife"});
    x("postLife");
    x("postMain");
    x("postIntro");

    // Run the ending if we're post main, post intro, not died
    x("postLife");
    flipIfAnd(screenCheck, {"postIntro", "postMain", "postLife"});
    callSubroutineIf("ending", "ancillaScreenCheck");
    flipIfAnd(screenCheck, {"postIntro", "postMain", "postLife"});
    x("postLife");

    // Run the main routine if we're post intro, not died, not post main
    x("postMain");
    x("postLife");
    flipIfAnd(screenCheck, {"postIntro", "postMain", "postLife"});
    callSubroutineIf("main", "ancillaScreenCheck");
    flipIfAnd(screenCheck, {"postIntro", "postMain", "postLife"});
    x("postLife");
    x("postMain");

    // Run the death screen if we've died, past the intro, not post main
    x("postMain");
    flipIfAnd(screenCheck, {"postIntro", "postMain", "postLife"});
    callSubroutineIf("death", "ancillaScreenCheck");
    flipIfAnd(screenCheck, {"postIntro", "postMain", "postLife"});
    x("postMain");
    
    // Uncompute the multiple key checks
    uncomputeRecording(recAlt);
    ancillaFree(leftAndAlt);
    ancillaFree(rightAndAlt);
    ancillaFree(leftAndNotAlt);
    ancillaFree(rightAndNotAlt);
    ancillaFree(screenCheck);
    finishSection("uncompute multiple key presses");

	// Final uncomputation
	addOffsets();
    clear("random");
    finishSection("final uncomputation");

    // Run optimisations
    if (optimizeGates) {
        optimize();
    }

    // Run checks
    if (unusedChecks) {
        checkForUnusedFunctions();
        checkForUnusedQubits();
    }
    
	return writeCircuit("quandoom.qasm");

}

