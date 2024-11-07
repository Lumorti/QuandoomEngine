#pragma once
#include "globals.h"
#include <unordered_map>
#include <set>

// Convert a pixel coordinate to a qubit number
int screenToQubit(int x, int y);

// Draw a pixel, optionally only if an ancilla is true
void drawPixel(int xCoord, int yCoord, std::string ancillaIf="");

// Draw a pixel, optionally only if an ancilla is true
void drawPixels(std::vector<std::pair<int, int>> points, std::string ancillaIf="");
    
// Draw a pixel, optionally only if an ancilla is true
void drawPixels(std::set<std::pair<int, int>> points, std::string ancillaIf="");

// Draw a line known at compile time
void drawLine(int x1, int y1, int x2, int y2);

// Draw a sprite, optionally only if an ancilla is true
void drawSprite(std::vector<std::vector<bool>> sprite, int x, int y, std::string ancillaIf="");

// Draw a sprite, optionally only if an ancilla is true
void drawSprite(std::string spriteName, int x, int y, std::string ancillaIf="");

// Load a sprite from a PPM file
// Based on this http://josiahmanson.com/prose/optimize_ppm/
void ignoreComment(std::ifstream &f);
std::vector<std::vector<bool>> loadPPM(const std::string &name);

// Load a sprite from a PPM file and store it in the global sprite list
void loadPPM(std::string filename, std::string nameToStore);

// Load a spritesheet from a PPM file and store it in the global sprite list
void loadPPMSheet(std::string filename, std::string nameToStore, int spriteWidth=60, int spriteHeight=60, int gap=1);

// Draw a number, known at compile time, optionally only if an ancilla is true
void drawNumber(int num, int x, int y, bool isBig=true, std::string ancilla="");
	
// Draw one of the big numbers based on the value of a register
void drawNumber(std::string num, int x, int y, bool isBig);
	
// Create a function to draw a pixel, top level
void functionDrawPixel(std::string xReg, std::string yReg, std::string regName, bool peakAncilla=true);

// Draw a pixel based on the values of some registers
void drawPixel(std::string name);

// Draw a pixel based on the values of some registers if another register
void drawPixelIf(std::string name, std::string regToCheck);

// Draw a pixel based on the values of some registers if the values are in range
void drawPixelIfValid(std::string name, std::string xCoord, int minX, int maxX, std::string yCoord, int minY, int maxY);

// The functions called whenever a line is drawn
void functionDrawLine(std::string startX, std::string startY, std::string endX, std::string endY);

// Draw a line based on the values of some registers
void drawLine();

// Draw a line based on the values of some registers
void drawLineIf(std::string reg);

// Draw a line based on the values of some registers
void drawLineIfValid(std::string reg, int minX, int maxX, int minY, int maxY, std::string x1RegName, std::string y1RegName, std::string x2RegName, std::string y2RegName);
    
// Given a fixed coord, project it onto the screen relative to the player
// https://en.wikipedia.org/wiki/3D_projection
void projectCoord(std::vector<int> coord, std::string xReg, std::string yReg);
   
// Given a point in world coords, render the pixel relative to the player
void renderPoint(std::vector<int> point);

// Given a start and end in world coords, draw a line relative to the player
void renderLine(std::vector<int> start, std::vector<int> end);
void functionRenderLine(std::string xStart, std::string yStart, std::string zStart, std::string xEnd, std::string yEnd, std::string zEnd);
    
// Function to render a sprite
void functionRenderSprite(std::string spriteName, std::string xCoord, std::string yCoord, std::string zCoord);

// Given a sprite name and world location, draw it (just flat with scaling)
void renderSprite(std::string spriteName, std::string xCoord, std::string yCoord, std::string zCoord);
void renderSpriteIf(std::string spriteName, std::string xCoord, std::string yCoord, std::string zCoord, std::string regToCheck);

// Given a enemy name and world location, draw it (just flat with scaling)
void functionRenderEnemy(std::string spriteName, std::string xCoord, std::string yCoord, std::string zCoord, std::vector<int> limits = {1,16,1,16});
void renderEnemy(std::string spriteName, std::string xCoord, std::string yCoord, std::string zCoord, std::string action, std::string frame);

// Given a room object, render it if the user is in it
void functionRenderRoom(Room room);
void renderRoomIfInside(Room room);

