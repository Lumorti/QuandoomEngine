#include "logic.h"
#include "draw.h"
#include "registers.h"
#include "gates.h"
#include "binary.h"
#include "debug.h"

// Function that runs all the logic for a room 
void functionRoomLogic(std::vector<Room> rooms) {

    // Keep track of ancilla usage then reserve after
    startPeakAncilla();

    // Reserve some ancillas here to prevent ancilla overlap later
    std::string actionValid = ancillaLock(1, "actionValid");

    // For each room
    for (Room room : rooms) {

        // Function moving an enemy towards the player
        for (size_t j=0; j<room.enemies.size(); j++) {
            int i = room.enemies[j];
            beginSubroutine("enemyMove" + std::to_string(i));

                // If we should forbid diagonal movement
                if (enemies[i].behaviour == "cardinal") {

                    // Ancilla needed
                    std::string xDiff = ancillaLock(qubitList["playerX"].amount, "xDiff");
                    std::string yDiff = ancillaLock(qubitList["playerY"].amount, "yDiff");
                    std::string xDiffAbs = ancillaLock(qubitList["playerX"].amount, "xDiffAbs");
                    std::string yDiffAbs = ancillaLock(qubitList["playerY"].amount, "yDiffAbs");
                    std::string deltaX = ancillaLock(2, "deltaX");
                    std::string deltaY = ancillaLock(2, "deltaY");
                    std::string deltaXShifted = ancillaLock(qubitList["playerX"].amount, "deltaXShifted");
                    std::string deltaYShifted = ancillaLock(qubitList["playerY"].amount, "deltaYShifted");
                    std::string xIsBigger = ancillaLock(1, "xIsBigger");

                    // Calculate the differences in x and y
                    int rec = startRecording();
                    copy("playerX", xDiff);
                    copy("playerY", yDiff);
                    subtractTC(xDiff, "enemy" + std::to_string(i) + "X");
                    subtractTC(yDiff, "enemy" + std::to_string(i) + "Y");

                    // Pick the one with the largest magnitude
                    copyAbs(xDiff, xDiffAbs);
                    copyAbs(yDiff, yDiffAbs);

                    // Move with the sign of the one with largest magnitude
                    flipIfLessOrEqualTC(xIsBigger, yDiffAbs, xDiffAbs);
                    stopRecording(rec);
                    copySignIf(xDiff, deltaX, xIsBigger);
                    x(xIsBigger);
                    copySignIf(yDiff, deltaY, xIsBigger);
                    x(xIsBigger);

                    // Uncompute everything, leaving only the deltas
                    uncomputeRecording(rec);

                    // Perform the move
                    copyTC(deltaX, deltaXShifted);
                    copyTC(deltaY, deltaYShifted);
                    shiftLeftTC(deltaXShifted, enemyMoveShift);
                    shiftLeftTC(deltaYShifted, enemyMoveShift);
                    add("enemy" + std::to_string(i) + "X", deltaXShifted);
                    add("enemy" + std::to_string(i) + "Y", deltaYShifted);
                    
                    // Clean up
                    shiftRightTC(deltaXShifted, enemyMoveShift);
                    shiftRightTC(deltaYShifted, enemyMoveShift);
                    copyTC(deltaX, deltaXShifted);
                    copyTC(deltaY, deltaYShifted);
                    clear(deltaX);
                    clear(deltaY);

                    // Free ancillas
                    ancillaFree(xDiff);
                    ancillaFree(yDiff);
                    ancillaFree(xDiffAbs);
                    ancillaFree(yDiffAbs);
                    ancillaFree(deltaX);
                    ancillaFree(deltaY);
                    ancillaFree(deltaXShifted);
                    ancillaFree(deltaYShifted);
                    ancillaFree(xIsBigger);

                } else {

                    // Ancilla needed
                    std::string xDiff = ancillaLock(qubitList["playerX"].amount, "xDiff");
                    std::string yDiff = ancillaLock(qubitList["playerY"].amount, "yDiff");
                    std::string deltaX = ancillaLock(2, "deltaX");
                    std::string deltaY = ancillaLock(2, "deltaY");
                    std::string deltaXShifted = ancillaLock(qubitList["playerX"].amount, "deltaXShifted");
                    std::string deltaYShifted = ancillaLock(qubitList["playerY"].amount, "deltaYShifted");

                    // Calculate the differences in x and y
                    int rec = startRecording();
                    copy("playerX", xDiff);
                    copy("playerY", yDiff);
                    subtractTC(xDiff, "enemy" + std::to_string(i) + "X");
                    subtractTC(yDiff, "enemy" + std::to_string(i) + "Y");
                    stopRecording(rec);
                    copySign(xDiff, deltaX);
                    copySign(yDiff, deltaY);

                    // Uncompute everything, leaving only the deltas
                    uncomputeRecording(rec);

                    // Perform the move
                    copyTC(deltaX, deltaXShifted);
                    copyTC(deltaY, deltaYShifted);
                    shiftLeftTC(deltaXShifted, enemyMoveShift);
                    shiftLeftTC(deltaYShifted, enemyMoveShift);
                    add("enemy" + std::to_string(i) + "X", deltaXShifted);
                    add("enemy" + std::to_string(i) + "Y", deltaYShifted);
                    
                    // Clean up
                    shiftRightTC(deltaXShifted, enemyMoveShift);
                    shiftRightTC(deltaYShifted, enemyMoveShift);
                    copyTC(deltaX, deltaXShifted);
                    copyTC(deltaY, deltaYShifted);
                    clear(deltaX);
                    clear(deltaY);

                    // Free ancillas
                    ancillaFree(xDiff);
                    ancillaFree(yDiff);
                    ancillaFree(deltaX);
                    ancillaFree(deltaY);
                    ancillaFree(deltaXShifted);
                    ancillaFree(deltaYShifted);

                }

            endSubroutine();

        }

        // Function attacking the player
        for (size_t j=0; j<room.enemies.size(); j++) {
            int i = room.enemies[j];
            beginSubroutine("enemyShoot" + std::to_string(i));

                // See if armor is positive
                int rec = startRecording();
                std::string hitCheck = ancillaLock(1, "hitCheck");
                std::string positiveArmor = ancillaLock(1, "positiveArmor");
                flipIfGreaterOrEqual(positiveArmor, "armor", 1);

                // Only on certain frames
                std::string frameValid = ancillaLock(1, "frameValid");
                flipIfEqual(frameValid, "frameCounter3", 2);

                // With some chance
                std::string hitChance = ancillaLock(1, "hitChance");
                flipWithChance(hitChance, 0.6);
                stopRecording(rec);

                // If it's a bullet from a zombieman (15 damage)
                if (enemies[i].name == "zombieman") {

                    // If the player has armor
                    flipIfAnd(hitCheck, {positiveArmor, hitChance, frameValid});
                    subtractIf("health", 10, hitCheck);
                    subtractIf("armor", 5, hitCheck);
                    flipIfAnd(hitCheck, {positiveArmor, hitChance, frameValid});

                    // If the player doesn't have armor
                    x(positiveArmor);
                    flipIfAnd(hitCheck, {positiveArmor, hitChance, frameValid});
                    subtractIf("health", 15, hitCheck);
                    flipIfAnd(hitCheck, {positiveArmor, hitChance, frameValid});
                    x(positiveArmor);

                // If it's a fireball from an imp (20 damage)
                } else if (enemies[i].name == "imp") {

                    // If the player has armor
                    flipIfAnd(hitCheck, {positiveArmor, hitChance, frameValid});
                    subtractIf("health", 14, hitCheck);
                    subtractIf("armor", 6, hitCheck);
                    flipIfAnd(hitCheck, {positiveArmor, hitChance, frameValid});

                    // If the player doesn't have armor
                    x(positiveArmor);
                    flipIfAnd(hitCheck, {positiveArmor, hitChance, frameValid});
                    subtractIf("health", 20, hitCheck);
                    flipIfAnd(hitCheck, {positiveArmor, hitChance, frameValid});
                    x(positiveArmor);

                }

                // Uncompute
                uncomputeRecording(rec);
                ancillaFree(hitCheck);
                ancillaFree(positiveArmor);
                ancillaFree(hitChance);
                ancillaFree(frameValid);

            endSubroutine();
        }

        // The function applying the z map for that room
        beginSubroutine("roomZMap" + std::to_string(room.id));

            // Find the floors
            if (room.walls.size() > 0) {

                // Create an ancilla for each bounds check
                std::vector<std::string> floorChecks(room.walls.size());
                for (size_t j=0; j<room.walls.size(); j++) {
                    int i = room.walls[j];
                    if (walls[i].startLoc.size() > 0 && walls[i].type == "floor") {
                        floorChecks[j] = ancillaLock(1, "ancillaFloorCheck"+std::to_string(j));
                    }
                }

                // Check whether the player is within the bounds for each one
                int recFloor = startRecording();
                for (size_t j=0; j<room.walls.size(); j++) {
                    int i = room.walls[j];
                    if (walls[i].startLoc.size() > 0 && walls[i].type == "floor") {
                        checkCollision("playerX", "playerY", walls[i], floorChecks[j]);
                    }
                }
                stopRecording(recFloor);

                // Output every floor check
                //for (size_t j=0; j<room.walls.size(); j++) {
                    //int i = room.walls[j];
                    //if (walls[i].startLoc.size() > 0 && walls[i].type == "floor") {
                        //outputReg("floor check " + std::to_string(i) + " (height " + std::to_string(walls[i].startLoc[2]) + ") = ", floorChecks[j], "binary");
                    //}
                //}

                // Get an index list sorted by the z value of each floor
                std::vector<int> sortedFloors;
                for (size_t j=0; j<room.walls.size(); j++) {
                    int i = room.walls[j];
                    if (walls[i].startLoc.size() > 0 && walls[i].type == "floor") {
                        sortedFloors.push_back(j);
                    }
                }
                //std::sort(sortedFloors.begin(), sortedFloors.end(), [&](int a, int b) {
                    //return std::abs(walls[room.walls[a]].startLoc[2]) > std::abs(walls[room.walls[b]].startLoc[2]);
                //});

                // Output the player's z value
                //outputReg("playerZ = ", "playerZ", "denary");

                // Set the z value to the lowest floor that the player is on
                for (size_t j=0; j<sortedFloors.size(); j++) {
                    int i = room.walls[sortedFloors[j]];
                    std::vector<std::string> regsToCheck;
                    for (size_t k=0; k<j; k++) {
                        x(floorChecks[sortedFloors[k]]);
                        regsToCheck.push_back(floorChecks[sortedFloors[k]]);
                    }
                    regsToCheck.push_back(floorChecks[sortedFloors[j]]);
                    setValTCIfAnd("playerZ", playerStartZ, regsToCheck);
                    setValTCIfAnd("playerZ", playerStartZ+walls[i].startLoc[2], regsToCheck);
                    for (size_t k=0; k<j; k++) {
                        x(floorChecks[sortedFloors[k]]);
                    }
                }

                // Output the player's z value
                //outputReg("playerZ = ", "playerZ", "denary");

                // Uncompute the checks
                uncomputeRecording(recFloor);
                for (size_t j=0; j<room.walls.size(); j++) {
                    int i = room.walls[j];
                    if (walls[i].startLoc.size() > 0 && walls[i].type == "floor") {
                        ancillaFree(floorChecks[j]);
                    }
                }

            }

            // Head bob
            std::string deltaZ = ancillaLock(qubitList["playerX"].amount, "deltaZ");
            std::string aboveHalf = ancillaLock(1, "aboveHalf");
            int recBob = startRecording();
            flipIfGreaterOrEqual(aboveHalf, "headBob", std::pow(2, qubitList["headBob"].amount-1));
            copy("headBob", deltaZ);
            negIf(deltaZ, aboveHalf);
            addIf(deltaZ, std::pow(2, qubitList["headBob"].amount-1), aboveHalf);
            stopRecording(recBob);
            add("playerZ", deltaZ);
            uncomputeRecording(recBob);
            ancillaFree(deltaZ);
            ancillaFree(aboveHalf);

        endSubroutine();

        // The function controlling the logic for that room
        beginSubroutine("roomLogic" + std::to_string(room.id));

            // Collision check
            if (room.walls.size() > 0) {
                std::vector<std::string> wallChecks(room.walls.size());
                for (size_t j=0; j<room.walls.size(); j++) {
                    int i = room.walls[j];
                    if (walls[i].startLoc.size() > 0 && walls[i].checkCollision) {
                        wallChecks[j] = ancillaLock(1, "ancillaWallCheck"+std::to_string(j));
                    }
                }
                std::string needToUndoMovement = ancillaLock(1, "ancillaNeedToUndoMovement");
                int recWall = startRecording();
                for (size_t j=0; j<room.walls.size(); j++) {
                    int i = room.walls[j];
                    if (walls[i].startLoc.size() > 0 && walls[i].checkCollision) {
                        checkCollision("playerX", "playerY", walls[i], wallChecks[j]);
                    }
                }
                stopRecording(recWall);
                flipIfOr(needToUndoMovement, wallChecks);
                //outputReg("need to undo movement = ", needToUndoMovement, "binary");
                uncomputeRecording(recWall);
                callSubroutineIf("reverseMovement", needToUndoMovement);
                clear(needToUndoMovement);
                for (size_t j=0; j<room.walls.size(); j++) {
                    int i = room.walls[j];
                    if (walls[i].startLoc.size() > 0 && walls[i].checkCollision) {
                        ancillaFree(wallChecks[j]);
                    }
                }
                ancillaFree(needToUndoMovement);
            }

            // Item pickup
            if (room.pickups.size() > 0) {
                std::string inLocForPickup = ancillaLock(1, "ancillaInLocForPickup");
                std::string pickedUpReady = ancillaLock(1, "ancillaPickedUpReady");
                std::string pickedUpReadyToDelete = ancillaLock(room.pickups.size(), "ancillaPickedUpReadyToDelete");
                for (size_t j=0; j<room.pickups.size(); j++) {
                    int i = room.pickups[j];
                    if (pickups[i].name.size() > 0 && !staticPickups.count(pickups[i].name)) {
                        
                        // Check if the user is in the location of the pickup and it's still there
                        std::string collectedReg = "pickup" + std::to_string(i) + "Collected";
                        int recPickup = startRecording();
                        checkCollision("playerX", "playerY", pickups[i], inLocForPickup);
                        x(collectedReg);
                        stopRecording(recPickup);

                        // To be cleared later
                        flipIfAnd(pickedUpReady, {inLocForPickup, collectedReg});

                        // Health
                        if (pickups[i].name == "health") {
                            addIf("health", 1, pickedUpReady);

                        // Med pack
                        } else if (pickups[i].name == "health2") {
                            addIf("health", 10, pickedUpReady);

                        // Armor
                        } else if (pickups[i].name == "armor") {
                            addIf("armor", 1, pickedUpReady);

                        // Security armor
                        } else if (pickups[i].name == "armor2") {
                            addIf("armor", 100, pickedUpReady);

                        // Mega armor
                        } else if (pickups[i].name == "armor3") {
                            addIf("armor", 200, pickedUpReady);

                        // Clip
                        } else if (pickups[i].name == "clip") {
                            addIf("ammoBullets", 10, pickedUpReady);

                        // Shells
                        } else if (pickups[i].name == "shells") {
                            addIf("ammoShells", 10, pickedUpReady);

                        // Shotgun
                        } else if (pickups[i].name == "shotgun") {
                            clearIf("hasShotgun", pickedUpReady);
                            flipIf("hasShotgun", pickedUpReady);

                        // Just in case
                        } else {
                            std::cout << "Error - unknown pickup type: " << pickups[i].name << std::endl;

                        }

                        // Note it as collected
                        flipIf(collectedReg, pickedUpReady);
                        std::string toDelete = subreg(pickedUpReadyToDelete, j, j+1);
                        swap(pickedUpReady, toDelete);

                        // Uncompute the pickup check
                        uncomputeRecording(recPickup);

                    }
                }
                clear(pickedUpReadyToDelete);
                ancillaFree(inLocForPickup);
                ancillaFree(pickedUpReady);
                ancillaFree(pickedUpReadyToDelete);
            }

            // Some constants
            int doorMin = 1;

            // Interact
            if (room.interactables.size() > 0) {
                std::string inLoc = ancillaLock(1, "ancillaInLocForPickup");
                std::string extraCheck = ancillaLock(1, "extraCheck");
                for (size_t j=0; j<room.interactables.size(); j++) {
                    int i = room.interactables[j];
                    int rec = startRecording();

                    // If it's a door toggle
                    std::string regToToggle;
                    if (interactables[i].type == "door") {
                        int doorID = interactables[i].idToAffect;
                        int doorMax = std::abs(walls[doorID].endLoc[2] - walls[doorID].startLoc[2])-5;
                        regToToggle = "door" + std::to_string(doorID) + "IsOpening";
                        flipIfGreaterOrEqualTC(extraCheck, "door" + std::to_string(doorID) + "Open", doorMax-1);
                        flipIfLessOrEqualTC(extraCheck, "door" + std::to_string(doorID) + "Open", doorMin+1);
                    } else {
                        x(extraCheck);
                    }

                    // Check if we're in the right place
                    checkCollision("playerX", "playerY", interactables[i], inLoc);
                    stopRecording(rec);

                    // If it's the final elevator
                    if (interactables[i].type == "elevator") {
                        regToToggle = "elevatorPressed";
                    }

                    // Do the check
                    flipIfAnd(regToToggle, {inLoc, "inputSpace", extraCheck});

                    // Uncompute
                    uncomputeRecording(rec);

                }
                ancillaFree(inLoc);
                ancillaFree(extraCheck);
            }

            // Door movement
            if (room.walls.size() > 0) {
                for (size_t j=0; j<room.walls.size(); j++) {
                    int i = room.walls[j];
                    if (walls[i].type == "door") {

                        // The regs that we need
                        std::string regAnd = ancillaLock(1, "regAnd");
                        std::string doorAtEnd = ancillaLock(1, "doorAtEnd");
                        std::string doorRegOpen = "door" + std::to_string(i) + "Open";
                        std::string doorRegIsOpening = "door" + std::to_string(i) + "IsOpening";
                        int doorMax = std::abs(walls[i].endLoc[2] - walls[i].startLoc[2])-5;

                        // Add/remove from the open amount
                        addIf(doorRegOpen, doorSpeed, doorRegIsOpening);
                        x(doorRegIsOpening);
                        subtractIf(doorRegOpen, doorSpeed, doorRegIsOpening);
                        x(doorRegIsOpening);

                        // Check if we're at the end
                        flipIfGreaterOrEqualTC(doorAtEnd, doorRegOpen, doorMax+1);
                        flipIfLessOrEqualTC(doorAtEnd, doorRegOpen, doorMin-1);

                        // Undo the previous move if we're at the end
                        flipIfAnd(regAnd, std::vector<std::string>{doorAtEnd, doorRegIsOpening});
                        subtractIf(doorRegOpen, doorSpeed, regAnd);
                        flipIfAnd(regAnd, std::vector<std::string>{doorAtEnd, doorRegIsOpening});
                        x(doorRegIsOpening);
                        flipIfAnd(regAnd, std::vector<std::string>{doorAtEnd, doorRegIsOpening});
                        addIf(doorRegOpen, doorSpeed, regAnd);
                        flipIfAnd(regAnd, std::vector<std::string>{doorAtEnd, doorRegIsOpening});
                        x(doorRegIsOpening);

                        // Clear the reg
                        clear(doorAtEnd);
                        ancillaFree(doorAtEnd);
                        ancillaFree(regAnd);

                        //outputReg("door is opening = ", doorRegIsOpening, "binary");
                        //outputReg("door open progress = ", doorRegOpen, "denary");

                    }
                }
            }

            // Update the enemy action
            if (room.enemies.size() > 0 && !simpleRooms) {
                for (size_t j=0; j<room.enemies.size(); j++) {
                    int i = room.enemies[j];

                    // If it's been long enough
                    std::string frameCounterForActions = "frameCounter3";
                    std::string shouldChangeAction = ancillaLock(1, "shouldChangeAction");
                    std::string shouldBeWalk = ancillaLock(1, "shouldBeWalk");
                    std::string moveIsStationary = ancillaLock(1, "moveIsStationary");
                    int rec = startRecording();
                    add(frameCounterForActions, 3*j);
                    flipIfEqual(shouldChangeAction, frameCounterForActions, 0);

                    // Roll the dice
                    if (enemies[i].name == "imp") {
                        flipWithChance(shouldBeWalk, 0.85);
                    } else {
                        flipWithChance(shouldBeWalk, 0.7);
                    }

                    // If there are two enemies, one should be stationary
                    if (enemies[i].behaviour == "stationary") {
                        x(moveIsStationary);
                    } 
                    stopRecording(rec);

                    // If we're changing action, clear the current action
                    clearIf("enemy" + std::to_string(i) + "Action", shouldChangeAction);

                    // If we're moving, but stationary
                    setValIfAnd("enemy" + std::to_string(i) + "Action", 0, {shouldChangeAction, shouldBeWalk, moveIsStationary});

                    // If we're moving, but not stationary
                    x(moveIsStationary);
                    setValIfAnd("enemy" + std::to_string(i) + "Action", 1, {shouldChangeAction, shouldBeWalk, moveIsStationary});
                    x(moveIsStationary);

                    // Otherwise, shoot
                    x(shouldBeWalk);
                    setValIfAnd("enemy" + std::to_string(i) + "Action", 4, {shouldChangeAction, shouldBeWalk});
                    x(shouldBeWalk);

                    // Print the action for debugging
                    //outputReg("enemy " + std::to_string(i) + " action = ", "enemy" + std::to_string(i) + "Action", "denary");

                    // Uncompute and free ancillas
                    uncomputeRecording(rec);
                    ancillaFree(shouldChangeAction);
                    ancillaFree(shouldBeWalk);
                    ancillaFree(moveIsStationary);

                }
            }

            // Determine where in the grid the player is
            int rec3 = startRecording();
            int gridQubits = std::ceil(std::log2(rayCastSections));
            std::string playerGridX = ancillaLock(gridQubits, "playerGridX");
            std::string playerGridY = ancillaLock(gridQubits, "playerGridY");
            std::string playerGridLess = ancillaLock(1, "playerGridLess");
            std::string playerGridMore = ancillaLock(1, "playerGridMore");
            for (size_t i=0; i<room.zonesX.size(); i++) {
                flipIfGreaterOrEqualTC(playerGridMore, "playerX", room.zonesX[i].first);
                flipIfLessOrEqualTC(playerGridLess, "playerX", room.zonesX[i].second);
                setValIfAnd(playerGridX, i, {playerGridMore, playerGridLess});
                flipIfLessOrEqualTC(playerGridLess, "playerX", room.zonesX[i].second);
                flipIfGreaterOrEqualTC(playerGridMore, "playerX", room.zonesX[i].first);
            }
            for (size_t i=0; i<room.zonesY.size(); i++) {
                flipIfGreaterOrEqualTC(playerGridMore, "playerY", room.zonesY[i].first);
                flipIfLessOrEqualTC(playerGridLess, "playerY", room.zonesY[i].second);
                setValIfAnd(playerGridY, i, {playerGridMore, playerGridLess});
                flipIfLessOrEqualTC(playerGridLess, "playerY", room.zonesY[i].second);
                flipIfGreaterOrEqualTC(playerGridMore, "playerY", room.zonesY[i].first);
            }

            // Stuff for the player shooting
            std::string frameIsGood = ancillaLock(1, "frameIsGood");
            std::string frameIsNew = ancillaLock(1, "frameIsNew");
            std::string angleGreater = ancillaLock(1, "angleGreater");
            std::string angleLess = ancillaLock(1, "angleLess");
            std::string weaponCorrect = ancillaLock(1, "weaponCorrect");
            std::string ammoPositive = ancillaLock(1, "ammoPositive");
            std::string distanceCorrect = ancillaLock(1, "distanceCorrect");
            std::string shootAllGood = ancillaLock(1, "shootAllGood");
            flipIfEqual(frameIsGood, "weaponFrame", 1);
            flipIfEqual(frameIsNew, "weaponFrame", 0);
            stopRecording(rec3);
            flipIfGreaterOrEqual(ammoPositive, "ammoBullets", 1);

            // If we shot, start the animation
            flipIfEqual(weaponCorrect, "currentWeapon", 1);
            flipIfAnd("weaponFrameI", {"inputCtrl", frameIsNew, weaponCorrect, ammoPositive});
            flipIfEqual(weaponCorrect, "currentWeapon", 1);
            flipIfEqual(weaponCorrect, "currentWeapon", 0);
            flipIfAnd("weaponFrameI", {"inputCtrl", frameIsNew, weaponCorrect});
            flipIfEqual(weaponCorrect, "currentWeapon", 0);

            // If we shot, remove an ammo
            flipIfEqual(weaponCorrect, "currentWeapon", 1);
            flipIfAnd(shootAllGood, {"inputCtrl", frameIsGood, weaponCorrect, ammoPositive});
                subtractIf("ammoBullets", 1, shootAllGood);
            flipIfAnd(shootAllGood, {"inputCtrl", frameIsGood, weaponCorrect, ammoPositive});
            flipIfEqual(weaponCorrect, "currentWeapon", 1);

            //outputReg("player grid x = ", playerGridX, "denary");
            //outputReg("player grid y = ", playerGridY, "denary");

            // If they can't see the player, reset to standing
            for (size_t j=0; j<room.enemies.size(); j++) {
                int i = room.enemies[j];

                // Determine where in the grid the enemy is
                int rec = startRecording();
                std::string enemyGridX = ancillaLock(gridQubits, "enemyGridX");
                std::string enemyGridY = ancillaLock(gridQubits, "enemyGridY");
                //outputReg("enemy " + std::to_string(i) + " x = ", "enemy" + std::to_string(i) + "X", "denary");
                //outputReg("enemy " + std::to_string(i) + " y = ", "enemy" + std::to_string(i) + "Y", "denary");
                for (size_t k=0; k<room.zonesX.size(); k++) {
                    flipIfGreaterOrEqualTC(playerGridMore, "enemy" + std::to_string(i) + "X", room.zonesX[k].first);
                    flipIfLessOrEqualTC(playerGridLess, "enemy" + std::to_string(i) + "X", room.zonesX[k].second);
                    setValIfAnd(enemyGridX, k, {playerGridMore, playerGridLess});
                    flipIfLessOrEqualTC(playerGridLess, "enemy" + std::to_string(i) + "X", room.zonesX[k].second);
                    flipIfGreaterOrEqualTC(playerGridMore, "enemy" + std::to_string(i) + "X", room.zonesX[k].first);
                }
                for (size_t k=0; k<room.zonesY.size(); k++) {
                    flipIfGreaterOrEqualTC(playerGridMore, "enemy" + std::to_string(i) + "Y", room.zonesY[k].first);
                    flipIfLessOrEqualTC(playerGridLess, "enemy" + std::to_string(i) + "Y", room.zonesY[k].second);
                    setValIfAnd(enemyGridY, k, {playerGridMore, playerGridLess});
                    flipIfLessOrEqualTC(playerGridLess, "enemy" + std::to_string(i) + "Y", room.zonesY[k].second);
                    flipIfGreaterOrEqualTC(playerGridMore, "enemy" + std::to_string(i) + "Y", room.zonesY[k].first);
                }

                // Ancilla for each
                std::string hasNoVision = ancillaLock(1, "hasNoVision");
                std::string enemyXCorrect = ancillaLock(1, "enemyXCorrect");
                std::string enemyYCorrect = ancillaLock(1, "enemyYCorrect");
                std::string playerXCorrect = ancillaLock(1, "playerXCorrect");
                std::string playerYCorrect = ancillaLock(1, "playerYCorrect");
                std::vector<std::string> noVisionChecks(room.noVisionZones.size());
                for (size_t k=0; k<room.noVisionZones.size(); k++) {
                    noVisionChecks[k] = ancillaLock(1, "noVisionCheck"+std::to_string(k));
                }

                // Check each no vision zone
                for (size_t k=0; k<room.noVisionZones.size(); k++) {
                    int rec2 = startRecording();
                    flipIfEqual(enemyXCorrect, enemyGridX, room.noVisionZones[k][0]);
                    flipIfEqual(enemyYCorrect, enemyGridY, room.noVisionZones[k][1]);
                    flipIfEqual(playerXCorrect, playerGridX, room.noVisionZones[k][2]);
                    flipIfEqual(playerYCorrect, playerGridY, room.noVisionZones[k][3]);
                    stopRecording(rec2);
                    flipIfAnd(noVisionChecks[k], {enemyXCorrect, enemyYCorrect, playerXCorrect, playerYCorrect});
                    uncomputeRecording(rec2);
                }

                // Set the action to 0 if the player is in the no vision zone
                flipIfOr(hasNoVision, noVisionChecks);
                stopRecording(rec);
                clearIf("enemy" + std::to_string(i) + "Action", hasNoVision);
                setValIf("enemy" + std::to_string(i) + "Action", 0, hasNoVision);

                // Check to see if the player is facing this enemy
                int recShoot = startRecording();
                std::string diffX = ancillaLock(qubitsPer, "ancillaDiffX");
                std::string diffY = ancillaLock(qubitsPer, "ancillaDiffY");
                std::string dX = ancillaLock(qubitsPer, "ancillaDX");
                std::string dY = ancillaLock(qubitsPer, "ancillaDY");
                std::string diffXCos = ancillaLock(qubitsPer, "ancillaDiffXCos");
                std::string diffXSin = ancillaLock(qubitsPer, "ancillaDiffXSin");
                std::string diffYCos = ancillaLock(qubitsPer, "ancillaDiffYCos");
                std::string diffYSin = ancillaLock(qubitsPer, "ancillaDiffYSin");
                std::string eYdX = ancillaLock(qubitsPer, "ancillaEYdX");
                std::string bX = ancillaLock(qubitsPer, "ancillaBX");
                std::string bY = ancillaLock(qubitsPer, "ancillaBY");
                std::string rem1 = ancillaLock(qubitsPer, "ancillaRem1");

                // diff = a - c
                copy("playerX", diffX);
                copy("playerY", diffY);
                subtractTC(diffX, "enemy" + std::to_string(i) + "X");
                subtractTC(diffY, "enemy" + std::to_string(i) + "Y");

                // dx = diffX * cos(theta) + diffY * sin(theta)
                multiply(diffX, "cosTheta", diffXCos);
                multiply(diffY, "sinTheta", diffYSin);
                copy(diffXCos, dX);
                add(dX, diffYSin);
                
                // dy = diffY * cos(theta) - diffX * sin(theta)
                multiply(diffY, "cosTheta", diffYCos);
                multiply(diffX, "sinTheta", diffXSin);
                copy(diffYCos, dY);
                subtract(dY, diffXSin);

                // bX = (eY*dX) / dY + eX
                float fovRad = fov * (M_PI / 180.0);
                int eX = 10 + screenWidth / 2;
                int eY = screenWidth / (2*std::tan(fovRad/2));
                multiply(dX, eY, eYdX);
                divideTC(eYdX, dY, bX, rem1);
                add(bX, eX);

                // see if bX is more of less central
                int upperLimit = screenWidth/2 + dispersion;
                int lowerLimit = screenWidth/2 - dispersion;
                flipIfGreaterOrEqual(angleGreater, bX, lowerLimit);
                flipIfLessOrEqual(angleLess, bX, upperLimit);

                // Stun chance is random
                std::string enemyStunnedRand = ancillaLock(1, "enemyStunnedRand");
                std::string enemyStunned = ancillaLock(1, "enemyStunned");
                flipWithChance(enemyStunnedRand, 0.7);

                // Small chance to miss
                std::string hitShot = ancillaLock(1, "missChance");
                flipWithChance(hitShot, 0.8);

                // Check the distance to this enemy
                std::string diffXAbs = ancillaLock(qubitList["playerX"].amount, "diffXAbs");
                std::string diffYAbs = ancillaLock(qubitList["playerY"].amount, "diffYAbs");
                std::string distance = ancillaLock(qubitList["playerX"].amount, "distance");
                copyAbs(diffX, diffXAbs);
                copyAbs(diffY, diffYAbs);
                copy(diffXAbs, distance);
                add(distance, diffYAbs);
                flipIfLessOrEqualTC(distanceCorrect, distance, meleeDistance);
                stopRecording(recShoot);

                // If enemy is dead and close enough, pick up ammo
                std::string newPickup = ancillaLock(1, "newPickup");
                x("enemy" + std::to_string(i) + "IsAlive");
                x("enemy" + std::to_string(i) + "PickedUp");
                flipIfAnd(newPickup, {"enemy" + std::to_string(i) + "IsAlive", distanceCorrect, "enemy" + std::to_string(i) + "PickedUp"});
                x("enemy" + std::to_string(i) + "PickedUp");
                x("enemy" + std::to_string(i) + "IsAlive");
                addIf("ammoBullets", 5, newPickup);
                flipIf("enemy" + std::to_string(i) + "PickedUp", newPickup);
                clear(newPickup);
                ancillaFree(newPickup);

                // DEBUG stuff
                //outputReg("enemy stunned rand = ", enemyStunnedRand, "binary");
                //outputReg("distance = ", distance, "denaryTC");
                //outputReg("frame is good = ", frameIsGood, "binary");
                //outputReg("angle correct 1 = ", angleLess, "binary");
                //outputReg("angle correct 2 = ", angleGreater, "binary");
                //outputReg("distance correct = ", distanceCorrect, "binary");
                //outputReg("current weapon = ", "currentWeapon", "denary");
                //outputReg("ammo positive = ", ammoPositive, "binary");

                // Pistol shot
                x(hasNoVision);
                flipIfEqual(weaponCorrect, "currentWeapon", 1);
                flipIfAnd(shootAllGood, {"inputCtrl", frameIsGood, angleLess, angleGreater, weaponCorrect, ammoPositive, hitShot, hasNoVision});
                    //outputReg("hit pistol = ", shootAllGood, "binaryIf");
                    subtractIf("enemy" + std::to_string(i) + "Health", 10, shootAllGood);
                    flipIfAnd(enemyStunned, {shootAllGood, enemyStunnedRand});
                        clearIf("enemy" + std::to_string(i) + "Action", enemyStunned);
                        setValIf("enemy" + std::to_string(i) + "Action", 5, enemyStunned);
                    flipIfAnd(enemyStunned, {shootAllGood, enemyStunnedRand});
                flipIfAnd(shootAllGood, {"inputCtrl", frameIsGood, angleLess, angleGreater, weaponCorrect, ammoPositive, hitShot, hasNoVision});
                flipIfEqual(weaponCorrect, "currentWeapon", 1);
                x(hasNoVision);

                // Punch
                x(hasNoVision);
                flipIfEqual(weaponCorrect, "currentWeapon", 0);
                flipIfAnd(shootAllGood, {"inputCtrl", frameIsGood, angleLess, angleGreater, weaponCorrect, distanceCorrect, hasNoVision});
                    //outputReg("hit fist = ", shootAllGood, "binaryIf");
                    subtractIf("enemy" + std::to_string(i) + "Health", 10, shootAllGood);
                    flipIfAnd(enemyStunned, {shootAllGood, enemyStunnedRand});
                        clearIf("enemy" + std::to_string(i) + "Action", enemyStunned);
                        setValIf("enemy" + std::to_string(i) + "Action", 5, enemyStunned);
                    flipIfAnd(enemyStunned, {shootAllGood, enemyStunnedRand});
                flipIfAnd(shootAllGood, {"inputCtrl", frameIsGood, angleLess, angleGreater, weaponCorrect, distanceCorrect, hasNoVision});
                flipIfEqual(weaponCorrect, "currentWeapon", 0);
                x(hasNoVision);

                // Free shooting ancillas
                uncomputeRecording(recShoot);
                ancillaFree(diffX);
                ancillaFree(diffY);
                ancillaFree(diffXAbs);
                ancillaFree(diffYAbs);
                ancillaFree(distance);
                ancillaFree(dX);
                ancillaFree(dY);
                ancillaFree(diffXCos);
                ancillaFree(diffXSin);
                ancillaFree(diffYCos);
                ancillaFree(diffYSin);
                ancillaFree(eYdX);
                ancillaFree(bX);
                ancillaFree(bY);
                ancillaFree(rem1);
                ancillaFree(enemyStunnedRand);
                ancillaFree(enemyStunned);
                ancillaFree(hitShot);

                // Check if enemy is dead
                int recDeath = startRecording();
                std::string deathCheck0 = ancillaLock(1, "deathCheck0");
                std::string deathCheckNeg = ancillaLock(1, "deathCheckNeg");
                std::string deathCheck = ancillaLock(1, "deathCheck");
                flipIfEqual(deathCheck0, "enemy" + std::to_string(i) + "Health", 0);
                flipIfGreaterOrEqual(deathCheckNeg, "enemy" + std::to_string(i) + "Health", 200);
                flipIfOr(deathCheck, {deathCheck0, deathCheckNeg});
                stopRecording(recDeath);
                clearIf("enemy" + std::to_string(i) + "IsAlive", deathCheck);   
                uncomputeRecording(recDeath);
                ancillaFree(deathCheck0);
                ancillaFree(deathCheckNeg);
                ancillaFree(deathCheck);

                // If enemy is dead, set the action to 6 or 5 (if picked up)
                //outputReg("picked up = ", "enemy" + std::to_string(i) + "PickedUp", "binary");
                x("enemy" + std::to_string(i) + "IsAlive");
                clearIf("enemy" + std::to_string(i) + "Action", "enemy" + std::to_string(i) + "IsAlive");
                setValIfAnd("enemy" + std::to_string(i) + "Action", 5, {"enemy" + std::to_string(i) + "IsAlive", "enemy" + std::to_string(i) + "PickedUp"});
                x("enemy" + std::to_string(i) + "PickedUp");
                setValIfAnd("enemy" + std::to_string(i) + "Action", 6, {"enemy" + std::to_string(i) + "IsAlive", "enemy" + std::to_string(i) + "PickedUp"});
                x("enemy" + std::to_string(i) + "PickedUp");
                x("enemy" + std::to_string(i) + "IsAlive");

                // If enemy is dead and animation not finished, increase animation counter
                x("enemy" + std::to_string(i) + "IsAlive");
                x("enemy" + std::to_string(i) + "DeathAnimFinished");
                std::string increaseCounter = ancillaLock(1, "increaseCounter");
                flipIfAnd(increaseCounter, {"enemy" + std::to_string(i) + "IsAlive", "enemy" + std::to_string(i) + "DeathAnimFinished"});
                addIf("enemy" + std::to_string(i) + "DeathAnim", 1, increaseCounter);
                flipIfAnd(increaseCounter, {"enemy" + std::to_string(i) + "IsAlive", "enemy" + std::to_string(i) + "DeathAnimFinished"});
                ancillaFree(increaseCounter);
                x("enemy" + std::to_string(i) + "DeathAnimFinished");
                x("enemy" + std::to_string(i) + "IsAlive");

                // If animation is at the end, note that we've finished
                std::string deathAnimEnd = ancillaLock(1, "deathAnimEnd");
                int finalVal = std::pow(2, qubitList["enemy" + std::to_string(i) + "DeathAnim"].amount)-1;
                flipIfEqual(deathAnimEnd, "enemy" + std::to_string(i) + "DeathAnim", finalVal);
                clearIf("enemy" + std::to_string(i) + "DeathAnimFinished", deathAnimEnd);
                setValIf("enemy" + std::to_string(i) + "DeathAnimFinished", 1, deathAnimEnd);
                flipIfEqual(deathAnimEnd, "enemy" + std::to_string(i) + "DeathAnim", finalVal);
                ancillaFree(deathAnimEnd);

                // DEBUG
                //outputReg("enemy " + std::to_string(i) + " health = ", "enemy" + std::to_string(i) + "Health", "denary");
                //outputReg("enemy " + std::to_string(i) + " action = ", "enemy" + std::to_string(i) + "Action", "denary");
                //outputReg("enemy " + std::to_string(i) + " is alive = ", "enemy" + std::to_string(i) + "IsAlive", "binary");
                //outputReg("enemy " + std::to_string(i) + " death anim = ", "enemy" + std::to_string(i) + "DeathAnim", "denary");
                //outputReg("enemy " + std::to_string(i) + " death anim finished = ", "enemy" + std::to_string(i) + "DeathAnimFinished", "binary");

                // Free ancillas
                uncomputeRecording(rec);
                ancillaFree(hasNoVision);
                ancillaFree(enemyXCorrect);
                ancillaFree(enemyYCorrect);
                ancillaFree(playerXCorrect);
                ancillaFree(playerYCorrect);
                ancillaFree(enemyGridX);
                ancillaFree(enemyGridY);
                for (size_t k=0; k<room.noVisionZones.size(); k++) {
                    ancillaFree(noVisionChecks[k]);
                }

            }

            // Free the player specific ancillas
            clear(ammoPositive);
            uncomputeRecording(rec3);
            ancillaFree(frameIsGood);
            ancillaFree(frameIsNew);
            ancillaFree(angleGreater);
            ancillaFree(angleLess);
            ancillaFree(weaponCorrect);
            ancillaFree(ammoPositive);
            ancillaFree(distanceCorrect);
            ancillaFree(shootAllGood);
            ancillaFree(playerGridX);
            ancillaFree(playerGridY);
            ancillaFree(playerGridLess);
            ancillaFree(playerGridMore);

            // Based on the enemy action, do something
            for (size_t j=0; j<room.enemies.size(); j++) {
                int i = room.enemies[j];

                // If action is 1, it's walking towards the player
                flipIfEqual(actionValid, "enemy" + std::to_string(i) + "Action", 1);
                callSubroutineIf("enemyMove" + std::to_string(i), actionValid);
                flipIfEqual(actionValid, "enemy" + std::to_string(i) + "Action", 1);

                // If action is 4, it's shooting
                flipIfEqual(actionValid, "enemy" + std::to_string(i) + "Action", 4);
                if (!playerImmortal) {
                    callSubroutineIf("enemyShoot" + std::to_string(i), actionValid);
                }
                flipIfEqual(actionValid, "enemy" + std::to_string(i) + "Action", 4);

            }

        endSubroutine(); 

    }

    // Free ancillas declared outside the loop
    ancillaFree(actionValid);

    // Reserve ancillas
    int ancillasUsedInFunc = stopPeakAncilla();
    std::cout << "Peak ancillas used in room logic: " << ancillasUsedInFunc << std::endl;
    if (ancillasUsedInFunc > 0) {
        ancillaLock(ancillasUsedInFunc, "ancillaFunctionRoomLogic");
    }

}

// Given a room object, render it if the user is in it
void processRoomIfInside(Room room) {

    // Setup
    std::string xGreaterThanMin = ancillaLock(1, "ancillaXGreaterThanMin");
    std::string xLessThanMax = ancillaLock(1, "ancillaXLessThanMax");
    std::string yGreaterThanMin = ancillaLock(1, "ancillaYGreaterThanMin");
    std::string yLessThanMax = ancillaLock(1, "ancillaYLessThanMax");
    int rec = startRecording();

    // Check if the player is in the room
    flipIfGreaterOrEqualTC(xGreaterThanMin, "playerX", room.minX);
    flipIfLessOrEqualTC(xLessThanMax, "playerX", room.maxX);
    flipIfGreaterOrEqualTC(yGreaterThanMin, "playerY", room.minY);
    flipIfLessOrEqualTC(yLessThanMax, "playerY", room.maxY);
    stopRecording(rec);

    // Render the room if all conditions met
    callSubroutineIfAnd("roomLogic" + std::to_string(room.id), {xGreaterThanMin, xLessThanMax, yGreaterThanMin, yLessThanMax});
    int zMapRec = startRecording();
    callSubroutineIfAnd("roomZMap" + std::to_string(room.id), {xGreaterThanMin, xLessThanMax, yGreaterThanMin, yLessThanMax});
    stopRecording(zMapRec);
    callSubroutineIfAnd("renderRoom" + std::to_string(room.id), {xGreaterThanMin, xLessThanMax, yGreaterThanMin, yLessThanMax});

    // Cleanup
    uncomputeRecording(zMapRec);
    uncomputeRecording(rec);
    ancillaFree(xGreaterThanMin);
    ancillaFree(xLessThanMax);
    ancillaFree(yGreaterThanMin);
    ancillaFree(yLessThanMax);

}

// Check if the player is in a wall
void checkCollision(std::string playerX, std::string playerY, Wall wall, std::string regToFlip) {

    // If it's a cardinal wall
    int wallThickness = 5*moveSpeed;
    if (wall.startLoc[0] == wall.endLoc[0] || wall.startLoc[1] == wall.endLoc[1] || (wall.type == "floor" && wall.startLoc.size() <= 3)) {

        // Ancillas for each check
        std::string xLessMax = ancillaLock(1, "xLessMax");
        std::string xGreaterMin = ancillaLock(1, "xGreaterMin");
        std::string yLessMax = ancillaLock(1, "yLessMax");
        std::string yGreaterMin = ancillaLock(1, "yGreaterMin");
        std::string isSolid = ancillaLock(1, "isSolid");

        // The AABB
        int minX = std::min(wall.startLoc[0], wall.endLoc[0]);
        int maxX = std::max(wall.startLoc[0], wall.endLoc[0]);
        int minY = std::min(wall.startLoc[1], wall.endLoc[1]);
        int maxY = std::max(wall.startLoc[1], wall.endLoc[1]);

        // Make sure it has finite volume
        int extra = wallThickness;
        if (wall.type != "floor") {
            if (minX == maxX) {
                minX -= extra;
                maxX += extra;
            }
            if (minY == maxY) {
                minY -= extra;
                maxY += extra;
            }
        }

        // Check each side
        int rec = startRecording();
        flipIfLessOrEqualTC(xLessMax, playerX, maxX);
        flipIfGreaterOrEqualTC(xGreaterMin, playerX, minX);
        flipIfLessOrEqualTC(yLessMax, playerY, maxY);
        flipIfGreaterOrEqualTC(yGreaterMin, playerY, minY);

        // If it's a door, check if it's open
        x(isSolid);
        if (wall.type == "door") {
            cx("door" + std::to_string(wall.id) + "IsOpening", isSolid);
        }
        stopRecording(rec);

        // We're in the wall if all are true
        flipIfAnd(regToFlip, {xLessMax, xGreaterMin, yLessMax, yGreaterMin, isSolid});

        // Uncompute and free ancillas
        uncomputeRecording(rec);
        ancillaFree(xLessMax);
        ancillaFree(xGreaterMin);
        ancillaFree(yLessMax);
        ancillaFree(yGreaterMin);
        ancillaFree(isSolid);

    // Otherwise, need to rotate
    } else {

        // The start and end of the region
        std::vector<int> startLoc = wall.startLoc;
        std::vector<int> endLoc = wall.endLoc;
        int thickness = wallThickness;
        if (wall.type == "floor" && wall.startLoc.size() > 3) {
            int thicknessNew = std::sqrt(std::pow(wall.startLoc[0]-wall.startLoc[3], 2) + std::pow(wall.startLoc[1]-wall.startLoc[4], 2)) / 2;
            std::vector<int> startLocNew = {(wall.startLoc[0] + wall.startLoc[3])/2, (wall.startLoc[1] + wall.startLoc[4])/2, wall.startLoc[2]};
            std::vector<int> endLocNew = {startLocNew[0] + (wall.endLoc[0] - wall.startLoc[3]), startLocNew[1] + (wall.endLoc[1] - wall.startLoc[4]), wall.endLoc[2]};
            //std::cout << std::endl;
            //std::cout << "startLoc 1 = " << startLoc[0] << ", " << startLoc[1] << ", " << startLoc[2] << std::endl;
            //std::cout << "startLoc 2 = " << startLoc[3] << ", " << startLoc[4] << ", " << startLoc[5] << std::endl;
            //std::cout << "endLoc = " << endLoc[0] << ", " << endLoc[1] << ", " << endLoc[2] << std::endl;
            //std::cout << "new startLoc = " << startLocNew[0] << ", " << startLocNew[1] << ", " << startLocNew[2] << std::endl;
            //std::cout << "new endLoc = " << endLocNew[0] << ", " << endLocNew[1] << ", " << endLocNew[2] << std::endl;
            //std::cout << "new thickness = " << thicknessNew << std::endl;
            startLoc = startLocNew;
            endLoc = endLocNew;
            thickness = thicknessNew;
        }

        // The the line of the wall
        int xDiff = startLoc[0] - endLoc[0];
        int yDiff = endLoc[1] - startLoc[1];
        int gcd = std::abs(std::__gcd(xDiff, yDiff));
        xDiff /= gcd;
        yDiff /= gcd;
        int c = startLoc[0]*yDiff + startLoc[1]*xDiff;
        int extra = thickness*std::sqrt(xDiff*xDiff + yDiff*yDiff);

        // Get the line at a right angle
        int xDiffPerp = -yDiff;
        int yDiffPerp = xDiff;
        int cPerpStart = startLoc[0]*yDiffPerp + startLoc[1]*xDiffPerp;
        int cPerpEnd = endLoc[0]*yDiffPerp + endLoc[1]*xDiffPerp;

        // Ancillas for each check
        std::string xLessMax = ancillaLock(1, "xLessMax");
        std::string xGreaterMin = ancillaLock(1, "xGreaterMin");
        std::string yLessMax = ancillaLock(1, "yLessMax");
        std::string yGreaterMin = ancillaLock(1, "yGreaterMin");
        std::string isSolid = ancillaLock(1, "isSolid");

        // Registers for the scaled x and y
        int extraQubits = 0;
        std::string scaledX1 = ancillaLock(qubitList[playerX].amount + extraQubits, "scaledX1");
        std::string scaledX2 = ancillaLock(qubitList[playerX].amount + extraQubits, "scaledX2");
        std::string scaledY1 = ancillaLock(qubitList[playerY].amount + extraQubits, "scaledY1");
        std::string scaledY2 = ancillaLock(qubitList[playerY].amount + extraQubits, "scaledY2");
        std::string newCoord1 = ancillaLock(qubitList[playerX].amount + extraQubits, "newCoord1");
        std::string newCoord2 = ancillaLock(qubitList[playerX].amount + extraQubits, "newCoord2");

        // Compute the scaled x and y
        int rec = startRecording();
        multiplyTC(playerX, yDiff, scaledX1);
        multiplyTC(playerY, xDiff, scaledY1);
        copyTC(scaledX1, newCoord1);
        addTC(newCoord1, scaledY1);
        multiplyTC(playerX, yDiffPerp, scaledX2);
        multiplyTC(playerY, xDiffPerp, scaledY2);
        copyTC(scaledX2, newCoord2);
        addTC(newCoord2, scaledY2);

        // Check each side
        flipIfLessOrEqualTC(xLessMax, newCoord1, c + extra);
        flipIfGreaterOrEqualTC(xGreaterMin, newCoord1, c - extra);
        flipIfLessOrEqualTC(yLessMax, newCoord2, cPerpStart);
        flipIfGreaterOrEqualTC(yGreaterMin, newCoord2, cPerpEnd);

        // If it's a door, check if it's open
        x(isSolid);
        if (wall.type == "door") {
            cx("door" + std::to_string(wall.id) + "IsOpening", isSolid);
        }
        stopRecording(rec);

        //outputReg("xDiff = " + std::to_string(xDiff));
        //outputReg("yDiff = " + std::to_string(yDiff));
        //outputReg("xDiffPerp = " + std::to_string(xDiffPerp));
        //outputReg("yDiffPerp = " + std::to_string(yDiffPerp));
        //outputReg("scaledX1 = ", scaledX1, "denaryTC");
        //outputReg("scaledX2 = ", scaledX2, "denaryTC");
        //outputReg("scaledY1 = ", scaledY1, "denaryTC");
        //outputReg("scaledY2 = ", scaledY2, "denaryTC");
        //outputReg("newCoord1 = ", newCoord1, "denaryTC");
        //outputReg("newCoord2 = ", newCoord2, "denaryTC");
        //outputReg("cPerpStart = " + std::to_string(cPerpStart));
        //outputReg("cPerpEnd = " + std::to_string(cPerpEnd));
        //outputReg("c = " + std::to_string(c));
        //outputReg("extra = " + std::to_string(extra));
        //outputReg("equation 1 = " + std::to_string(yDiff) + " x  + " + std::to_string(xDiff) + " y >= " + std::to_string(c + extra));
        //outputReg("equation 2 = " + std::to_string(yDiff) + " x  + " + std::to_string(xDiff) + " y <= " + std::to_string(c - extra));
        //outputReg("equation 3 = " + std::to_string(yDiffPerp) + " x  + " + std::to_string(xDiffPerp) + " y <= " + std::to_string(cPerpStart));
        //outputReg("equation 4 = " + std::to_string(yDiffPerp) + " x  + " + std::to_string(xDiffPerp) + " y >= " + std::to_string(cPerpEnd));
        //outputReg("xLessMax = ", xLessMax, "binary");
        //outputReg("xGreaterMin = ", xGreaterMin, "binary");
        //outputReg("yLessMax = ", yLessMax, "binary");
        //outputReg("yGreaterMin = ", yGreaterMin, "binary");

        // We're in the wall if all are true
        flipIfAnd(regToFlip, {xLessMax, xGreaterMin, yLessMax, yGreaterMin, isSolid});

        // Free ancillas
        uncomputeRecording(rec);
        ancillaFree(scaledX1);
        ancillaFree(scaledX2);
        ancillaFree(scaledY1);
        ancillaFree(scaledY2);
        ancillaFree(newCoord1);
        ancillaFree(newCoord2);
        ancillaFree(xLessMax);
        ancillaFree(xGreaterMin);
        ancillaFree(yLessMax);
        ancillaFree(yGreaterMin);
        ancillaFree(isSolid);

    }

}

// Check if the player is touching a sprite
void checkCollision(std::string playerX, std::string playerY, Pickup sprite, std::string regToFlip) {

    // Ancillas for each check
    std::string xLessMax = ancillaLock(1, "xLessMax");
    std::string xGreaterMin = ancillaLock(1, "xGreaterMin");
    std::string yLessMax = ancillaLock(1, "yLessMax");
    std::string yGreaterMin = ancillaLock(1, "yGreaterMin");

    // The AABB
    int minX = sprite.loc[0] - pickupDistance;
    int maxX = sprite.loc[0] + pickupDistance;
    int minY = sprite.loc[1] - pickupDistance;
    int maxY = sprite.loc[1] + pickupDistance;

    // Check each side
    int rec = startRecording();
    flipIfLessOrEqualTC(xLessMax, playerX, maxX);
    flipIfGreaterOrEqualTC(xGreaterMin, playerX, minX);
    flipIfLessOrEqualTC(yLessMax, playerY, maxY);
    flipIfGreaterOrEqualTC(yGreaterMin, playerY, minY);
    stopRecording(rec);

    // We're in the wall if all are true
    flipIfAnd(regToFlip, {xLessMax, xGreaterMin, yLessMax, yGreaterMin});

    // Uncompute and free ancillas
    uncomputeRecording(rec);
    ancillaFree(xLessMax);
    ancillaFree(xGreaterMin);
    ancillaFree(yLessMax);
    ancillaFree(yGreaterMin);

}

// Check if the player is within range to interact
void checkCollision(std::string playerX, std::string playerY, Interactable object, std::string regToFlip) {

    // Ancillas for each check
    std::string xLessMax = ancillaLock(1, "xLessMax");
    std::string xGreaterMin = ancillaLock(1, "xGreaterMin");
    std::string yLessMax = ancillaLock(1, "yLessMax");
    std::string yGreaterMin = ancillaLock(1, "yGreaterMin");

    // The AABB
    int minX = object.loc[0] - object.range;
    int maxX = object.loc[0] + object.range;
    int minY = object.loc[1] - object.range;
    int maxY = object.loc[1] + object.range;

    // Check each side
    int rec = startRecording();
    flipIfLessOrEqualTC(xLessMax, playerX, maxX);
    flipIfGreaterOrEqualTC(xGreaterMin, playerX, minX);
    flipIfLessOrEqualTC(yLessMax, playerY, maxY);
    flipIfGreaterOrEqualTC(yGreaterMin, playerY, minY);
    stopRecording(rec);

    // We're in the wall if all are true
    flipIfAnd(regToFlip, {xLessMax, xGreaterMin, yLessMax, yGreaterMin});

    // Uncompute and free ancillas
    uncomputeRecording(rec);
    ancillaFree(xLessMax);
    ancillaFree(xGreaterMin);
    ancillaFree(yLessMax);
    ancillaFree(yGreaterMin);

}
