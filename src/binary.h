#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <math.h>
#include "globals.h"

// ------------------------------------------
//               CONVERTERS
// ------------------------------------------

// Convert to a binary array e.g. 32 = {1, 0, 0, 0, 0}
std::vector<int> toBinary(int val, int bits);

// Convert to a denary value from a binary array, two's complement
int toDenaryTC(std::vector<int> val);

// Convert to a binary array in two's complement e.g. -16 = {1, 1, 0, 0, 0}
std::vector<int> toBinaryTC(int val, int bits);
    
// Convert to a denary array e.g. 32 = {10, 10, 3, 2} (10 is used as padding)
std::vector<int> toDenary(int val, int bits);

// Convert to a denary array e.g. 32 = {3, 2} (no padding)
std::vector<int> toDenary(int val);
	
// Convert to a denary value from a binary array
int toDenary(std::vector<int> val);
	
// ------------------------------------------
//           SETTERS / FLIPPERS
// ------------------------------------------

// Set a blank register to a certain value
void setVal(std::string destQubit, int val);

// Set a blank register to a certain value, two's complement
void setValTC(std::string destQubit, int val);

// Set a blank register to a certain value, two's complement, if another register is true
void setValTCIfAnd(std::string regToSet, int newVal, std::string regToCheck);

// Set a blank register to a certain value, two's complement, if another register is all ones
void setValTCIfAnd(std::string regToSet, int newVal, std::vector<std::string> regsToCheck);

// Flip an entire register if a register is all ones
void flipIf(std::string regToFlip, std::string regToCheck);

// Flip an entire register if several registers are all ones
void flipIfAnd(std::string regToFlip, std::vector<std::string> regsToCheck);

// Flip an entire register if several registers are non-zero
void flipIfOr(std::string regToFlip, std::vector<std::string> regsToCheck);

// Flip an entire register if a register has a certain value
void flipIfEqual(std::string regToFlip, std::string regToCheck, int checkVal);

// Flip an entire register if a register has a certain value, if another reg is all true
void flipIfEqualIf(std::string regToFlip, std::string regToCheck, int checkVal, std::string regIf);

// Flip an entire register if two registers have the same value
void flipIfEqual(std::string regToFlip, std::string regToCheck, std::string regToCheck2);

// Flip an entire register if a register has a value less than another register
void flipIfLessOrEqual(std::string regToFlip, std::string regToCheck, std::string regToCheck2);
    
// Flip a register if a value is negative
void flipIfNegTC(std::string toFlip, std::string toCheck);

// Set a blank register to a certain value if another register is all ones
void setValIf(std::string regToSet, int newVal, std::string regToCheck);

// Set a blank register to a certain value if several registers are all ones
void setValIfAnd(std::string regToSet, int newVal, std::vector<std::string> regsToCheck);

// Set a blank register to a certain value if another register has a certain value
void setValIfEqual(std::string regToSet, int newVal, std::string regToCheck, int checkVal);

// Set a blank register to a certain TC value if another register has a certain value
void setValTCIfEqual(std::string regToSet, int newVal, std::string regToCheck, int checkVal);
	
// Copy from one register to a blank
void copy(std::string regToCopy, std::string regToSet);
    
// Copy from one register to a blank, if another register is all true
void copyIf(std::string regToCopy, std::string regToSet, std::string regIf);
    
// Copy from one register to a blank, if the left bit is one then extend the sign
void copyTC(std::string regToCopy, std::string regToSet);
    
// Copy from one register to a blank, dealing with TC, if another register is all true
void copyTCIf(std::string regToCopy, std::string regToSet, std::string regIf);
    
// Copy from one TC reg, taking the absolute value
void copyAbs(std::string regToCopy, std::string regToSet);

// Copy the sign from a reg
void copySign(std::string regToCopy, std::string regToSet);

// Copy the sign from a reg, if another reg is all true
void copySignIf(std::string regToCopy, std::string regToSet, std::string regIf);

// ------------------------------------------
//                ARITHMETIC
// ------------------------------------------

// Given a register, add a fixed value to it
void add(std::string reg, int val);
    
// Given a register, add a fixed value to it
void addTC(std::string reg, int val);
    
// Given a register, add a fixed value to it if another register is all ones
void addIf(std::string reg, int val, std::string regIf);
    
// Given two registers, add the latter to the former
void add(std::string reg1, std::string reg2);
    
// Given two registers, add the latter to the former (TC with diff)
void addTC(std::string reg1, std::string reg2);

// Given two registers, add the latter to the former, shifted 
void addShift(std::string reg1, std::string reg2, int shift);
    
// Given two registers, add the latter to the former, shifted and controlled
void addShiftIf(std::string reg1, std::string reg2, int shift, int control);
    
// Given two registers, add the latter to the former, controlled
void addIf(std::string reg1, std::string reg2, std::string control);
    
// Add a value to a register if another register is all ones, wrapped
void addWrappedIf(std::string reg, int val, std::string regIf, int minVal, int maxVal);

// Take the negative of a register
void neg(std::string reg);

// Take the negative of a register, controlled
void negIf(std::string reg, std::string regIf);

// Subtract a value from a register if another register is all ones
void subtractIf(std::string reg, int val, std::string regIf);

// Subtract a value from a register if another register is all ones, wrapped
void subtractWrappedIf(std::string reg, int val, std::string regIf, int minVal, int maxVal);
    
// Multiply a register by another register, storing in a third
void multiply(std::string reg1, std::string reg2, std::string reg3);
    
// Multiply a register by an int, storing in a third 
void multiply(std::string reg1, int val, std::string reg3);
    
// Multiply a register by an int, storing in a third (TC)
void multiplyTC(std::string reg1, int val, std::string reg3);
    
// Shift a register left by a certain amount
void shiftLeft(std::string reg, int shift);
    
// Shift a register left by a certain amount, but not the leftmost bit
void shiftLeftTC(std::string reg, int shift);
    
// Shift a register right by a certain amount
void shiftRight(std::string reg, int shift);
    
// Shift a register right by a certain amount, but not the rightmost bit
void shiftRightTC(std::string reg, int shift);
    
// Given a register, subtract a fixed value from it
void subtract(std::string reg, int val);
   
// Given a register, subtract a fixed value from it
void subtractTC(std::string reg, int val);
    
// Given two registers, subtract the latter from the former
void subtract(std::string reg1, std::string reg2, bool checkDiff = true);
    
// Given two registers, subtract the latter from the former (TC with diff)
void subtractTC(std::string reg1, std::string reg2);

// Given two registers, subtract the latter from the former, controlled
void subtractIf(std::string reg1, std::string reg2, std::string controlReg);
    
// Given two registers, subtract the latter from the former, shifted and controlled
void subtractShiftIf(std::string reg1, std::string reg2, int shift, int control);
    
// Divide a register by another register, storing in a third
void divide(std::string divideThis, std::string byThis, std::string result, std::string remainder);
    
// Flip a register if another register has a value less than another register
void flipIfLessOrEqualTC(std::string regToFlip, std::string regToCheck, std::string regToCheck2);
    
// Flip a register if another register has a value less than a value
void flipIfLessOrEqual(std::string regToFlip, std::string regToCheck, int checkVal);

// Flip a register if another register has a value greater than another register
void flipIfGreaterOrEqualTC(std::string regToFlip, std::string regToCheck, std::string regToCheck2);
    
// Flip a register if another register has a value less than a value
void flipIfGreaterOrEqual(std::string regToFlip, std::string regToCheck, int checkVal);

// Divide a register by another register, storing in a third, dealing with negatives
void divideTC(std::string divideThis, std::string byThis, std::string result, std::string remainder);
    
// ------------------------------------------
//                GARBAGE
// ------------------------------------------

// Clear values by swapping them to garbage
int clear(std::string regToDelete);
   
// Clear values by swapping them to garbage, if another register is all ones
int clearIf(std::string regToDelete, std::string regToCheck);
    
// ------------------------------------------
//                 TRIG
// ------------------------------------------

// Take the cosine of a reg and put in another, shifted by some number of bits 
void setCos(std::string reg, std::string val, int extraBits=0);
   
// Take the sine of a reg and put in another, shifted by some number of bits 
void setSin(std::string reg, std::string val, int extraBits=0);

// ------------------------------------------
//                 RANDOM
// ------------------------------------------

// Use the random register to return true or false
void flipWithChance(std::string reg, double prob);

// ------------------------------------------
//                 MISC
// ------------------------------------------

// Flip a register if another register has a value less than a value
void flipIfLessOrEqualTC(std::string regToFlip, std::string regToCheck, int checkVal);
   
// Flip a register if another register has a value greater than a value
void flipIfGreaterOrEqualTC(std::string regToFlip, std::string regToCheck, int checkVal);
    
// Add offsets to each register (so that the zero state is the starting state)
void addOffsets();
	
// Create a domregister of a register
std::string domreg(std::string regName, int newSize);
