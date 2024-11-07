#pragma once
#include "globals.h"

// Function per room
void functionRoomLogic(std::vector<Room> room);

// Given a room object, render it if the user is in it
void processRoomIfInside(Room room);

// Check collision with a wall
void checkCollision(std::string playerX, std::string playerY, Wall wall, std::string regToFlip);

// Check collision with a pickup
void checkCollision(std::string playerX, std::string playerY, Pickup sprite, std::string regToFlip);
    
// Check collision with an interactable
void checkCollision(std::string playerX, std::string playerY, Interactable object, std::string regToFlip);
