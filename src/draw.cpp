#include "draw.h"
#include "binary.h"
#include "debug.h"
#include "gates.h"
#include "registers.h"
#include <iostream>
#include <fstream>
#include <map>

// Calculated quantities
const float fovRad = fov * (M_PI / 180.0);
const int eX = 10 + screenWidth / 2;
const int eY = screenWidth / (2*std::tan(fovRad/2));
const int eZ = 10 + screenHeight / 2;
const int nearDist = 10 * (1 << leftShift);
const int farDist = renderDistance * (1 << leftShift);

// Convert a pixel coordinate to a qubit number
int screenToQubit(int x, int y) {
	if (x < 0 || x >= 320 || y < 0 || y >= 200) {
		std::cout << "Error: Tried to access pixel outside of screen: (" << x << ", " << y << ")\n";
	}
	return nonScreenQubits + x + y * 320;
}

// Round a double to an int
int roundInt(double x) {
    return int(x + 0.5);
}

// Draw a pixel, optionally only if an ancilla is true
void drawPixel(int xCoord, int yCoord, std::string ancillaIf) {
	if (ancillaIf.size() == 0) {
		x(screenToQubit(xCoord, yCoord));
	} else {
		cx(ancillaIf, screenToQubit(xCoord, yCoord));
	}
}

// Draw a pixel, optionally only if an ancilla is true
void drawPixels(std::vector<std::pair<int, int>> points, std::string ancillaIf) {
    std::vector<int> ancillaIfQubits;
    for (int i=qubitList[ancillaIf].start; i<qubitList[ancillaIf].end; i++) {
        ancillaIfQubits.push_back(i);
    }
    std::vector<int> pointsQubits;
    for (auto& [x, y] : points) {
        pointsQubits.push_back(screenToQubit(x, y));
    }
    if (ancillaIf.size() == 0) {
        x(pointsQubits);
    } else {
        cx(ancillaIfQubits, pointsQubits);
    }
}

// Draw a pixel, optionally only if an ancilla is true
void drawPixels(std::set<std::pair<int, int>> points, std::string ancillaIf) {
    std::vector<int> ancillaIfQubits;
    for (int i=qubitList[ancillaIf].start; i<qubitList[ancillaIf].end; i++) {
        ancillaIfQubits.push_back(i);
    }
    std::vector<int> pointsQubits;
    for (auto& [x, y] : points) {
        pointsQubits.push_back(screenToQubit(x, y));
    }
    if (ancillaIf.size() == 0) {
        x(pointsQubits);
    } else {
        cx(ancillaIfQubits, pointsQubits);
    }
}

// Draw a line known at compile time
void drawLine(int x1, int y1, int x2, int y2) {
	std::set<std::pair<int, int>> points;
	if (x2 == x1) {
		if (y2 > y1) {
			for (int y = y1; y <= y2; y++) {
                points.insert({x1, y});
			}
		} else {
			for (int y = y2; y <= y1; y++) {
                points.insert({x1, y});
			}
		}
	} else {
		double grad = double(y2 - y1) / double(x2 - x1);
		if (x2 > x1) {
			for (int x = x1; x <= x2; x++) {
				int y = y1 + grad * (x - x1);
				points.insert({x, y});
			}
		} else {
			for (int x = x2; x <= x1; x++) {
				int y = y2 + grad * (x - x2);
				points.insert({x, y});
			}
		}
	}
    drawPixels(points);
}

// Draw a sprite, optionally only if an ancilla is true
void drawSprite(std::vector<std::vector<bool>> sprite, int x, int y, std::string ancillaIf) {
    std::vector<std::pair<int, int>> points;
	for (size_t i = 0; i < sprite.size(); i++) {
		for (size_t j = 0; j < sprite[i].size(); j++) {
			if (sprite[i][j]) {
                points.push_back({x + i, y + j});
			}
		}
	}
    drawPixels(points, ancillaIf);
}

// Draw a sprite, optionally only if an ancilla is true
void drawSprite(std::string spriteName, int x, int y, std::string ancillaIf) {
    std::vector<std::pair<int, int>> points;
    if (spriteList.find(spriteName) == spriteList.end()) {
        std::cout << "Error: Sprite " << spriteName << " not found\n";
        return;
    }
    std::vector<std::vector<bool>> sprite = spriteList[spriteName];
	for (size_t i = 0; i < sprite.size(); i++) {
		for (size_t j = 0; j < sprite[i].size(); j++) {
			if (sprite[i][j]) {
                points.push_back({x + i, y + j});
			}
		}
	}
    drawPixels(points, ancillaIf);
}

// Load a sprite from a PPM file
// Based on this http://josiahmanson.com/prose/optimize_ppm/
void ignoreComment(std::ifstream &f) {
    char linebuf[1024];
    char ppp;
    while (ppp = f.peek(), ppp == '\n' || ppp == '\r') {
        f.get();
	}
    if (ppp == '#') {
        f.getline(linebuf, 1023);
	}
}
std::vector<std::vector<bool>> loadPPM(const std::string &name) {
    if (!std::ifstream(name.c_str())) {
        std::cout << "Error: File " << name << " not found\n";
        return {};
    }
	std::ifstream f(name.c_str(), std::ios::binary);
	std::string s;
	ignoreComment(f);
    f >> s;
	ignoreComment(f);
	int w = 0;
	int h = 0;
    int bits = 0;
    f >> w >> h;
	ignoreComment(f);
    f >> bits;
	ignoreComment(f);
	std::vector<bool> img(w*h, false);
	if (s == "P6") {
		for (size_t i = 0; i < img.size(); i++) {
			char r, g, b;
			f >> r >> g >> b;
			if (r != 0 || g != 0 || b != 0) {
				img[i] = true;
			}
		}
	} else {
		for (size_t i = 0; i < img.size(); i++) {
			int r, g, b;
			f >> r >> g >> b;
			if (r != 0 || g != 0 || b != 0) {
				img[i] = true;
			}
		}
	}
    f.close();
	std::vector<std::vector<bool>> img2D(w, std::vector<bool>(h, false));
	for (size_t i = 0; i < img.size(); i++) {
		img2D[i%w][i/w] = img[i];
	}
	return img2D;
}

// Load a sprite from a PPM file and store it in the global sprite list
void loadPPM(std::string filename, std::string nameToStore) {
    std::vector<std::vector<bool>> sprite = loadPPM(filename);
    spriteList[nameToStore] = sprite;
}

// Load a spritesheet from a PPM file and store it in the global sprite list
void loadPPMSheet(std::string filename, std::string nameToStore, int spriteWidth, int spriteHeight, int gap) {
    if (!std::ifstream(filename.c_str())) {
        std::cout << "Error: File " << filename << " not found\n";
        return;
    }
    std::vector<std::vector<bool>> sheet = loadPPM(filename);
    if (spriteWidth == 60 || nameToStore == "numbers") {
        std::cout << "processing spritesheet of size " << sheet.size() << " x " << sheet[0].size() << " with name " << nameToStore << std::endl;
        std::cout << "sprite size: " << spriteWidth << " x " << spriteHeight << std::endl;
        std::cout << "gap between sprites: " << gap << std::endl;
    }
    int numAdded = 0;
    for (size_t i = 0; i < sheet.size(); i += spriteWidth + gap) {
        for (size_t j = 0; j < sheet[i].size(); j += spriteHeight + gap) {
            std::vector<std::vector<bool>> sprite;
            for (int k = 0; k < spriteWidth; k++) {
                std::vector<bool> row;
                for (int l = 0; l < spriteHeight; l++) {
                    row.push_back(sheet[i+k][j+l]);
                }
                sprite.push_back(row);
            }
            if (spriteWidth > 60) {
                int numToRemove = spriteWidth - 60;
                int every = std::max(2, spriteWidth / numToRemove);
                for (int k = 0; k < spriteWidth; k++) {
                    for (int l = 0; l < spriteHeight; l++) {
                        if (k % every == 0 || l % every == 0) {
                            sprite[k][l] = false;
                        }
                    }
                }
            }
            int indX = i/(spriteWidth+gap);
            int indY = j/(spriteHeight+gap);
            if (nameToStore == "numbers") {
                std::string spriteName = nameToStore + "-" + std::to_string(indX) + "-" + std::to_string(indY);
                spriteList[spriteName] = sprite;
            } else {
                std::string nameWithoutSize = nameToStore.substr(0, nameToStore.find("-"));
                std::string sizeString = nameToStore.substr(nameToStore.find("-")+1);
                std::string spriteName = nameWithoutSize + "-" + std::to_string(indX) + "-" + std::to_string(indY) + "-" + sizeString;
                spriteList[spriteName] = sprite;
            }
            numAdded++;
        }
    }
    if (spriteWidth == 60 || nameToStore == "numbers") {
        std::cout << "loaded " << numAdded << " sprites from this sheet" << std::endl;
    }
}

// Draw a number, known at compile time, optionally only if an ancilla is true
void drawNumber(int num, int x, int y, bool isBig, std::string ancilla) {
	std::vector<int> digits = toDenary(num);
	for (size_t i = 0; i < digits.size(); i++) {
		std::string sprite;
        int width = 0;
		if (isBig) {
			sprite = "numbers-" + std::to_string(digits[i]) + "-0";
            width = 10;
		} else {
			sprite = "numbers-" + std::to_string(digits[i]) + "-1";
            width = 3;
		}
		drawSprite(sprite, x + i * (1 + width), y, ancilla);
	}
}

// Draw one of the numbers based on the value of a register
void drawNumber(std::string num, int x, int y, bool isBig) {
	int numDigits = std::ceil(std::log10(std::pow(2, qubitList[num].amount)));
    std::string binaryReg = ancillaLock(1);
    std::vector<std::string> smallRegs;
    for (int i = 0; i < numDigits; i++) {
        smallRegs.push_back(ancillaLock(4));
    }
    for (int i = qubitList[num].min; i < qubitList[num].max; i++) {
		std::vector<int> denaryVal = toDenary(i, numDigits);
		for (size_t j = 0; j < denaryVal.size(); j++) {
            setValIfEqual(smallRegs[j], denaryVal[j], num, i);
		}
	}
	for (int j = 0; j < numDigits; j++) {
		for (int i = 0; i <= 9; i++) {
            flipIfEqual(binaryReg, smallRegs[j], i);
            std::string sprite;
            int width;
            if (isBig) {
                sprite = "numbers-" + std::to_string(i) + "-0";
                width = 8;
            } else {
                sprite = "numbers-" + std::to_string(i) + "-1";
                width = 3;
            }
			drawSprite(sprite, x + j * (1 + width), y, binaryReg);
            flipIfEqual(binaryReg, smallRegs[j], i);
		}
	}
	for (int i = qubitList[num].min; i < qubitList[num].max; i++) {
		std::vector<int> denaryVal = toDenary(i, numDigits);
		for (size_t j = 0; j < denaryVal.size(); j++) {
            setValIfEqual(smallRegs[j], denaryVal[j], num, i);
		}
	}
    ancillaFree(binaryReg);
    for (int i = 0; i < numDigits; i++) {
        ancillaFree(smallRegs[i]);
    }
}

// Create a function to draw a pixel, top level
void functionDrawPixel(std::string xReg, std::string yReg, std::string regName, bool peakAncilla) {

    // Make sure it's only defined once
    if (subroutineNameToIndex.find("drawPixel"+regName) != subroutineNameToIndex.end()) {
        std::cout << "Function drawPixel already defined, skipping" << std::endl;
        return;
    }

    // Start recording the peak ancilla usage
    if (peakAncilla) {
        startPeakAncilla();
    }

    // Cache the indices used for the final level controls
    std::cout << "Creating draw pixel functions..." << std::endl;
    std::vector<int> controlQubits;
    for (int l=qubitList[xReg].start; l<qubitList[xReg].end; l++) {
        controlQubits.push_back(l);
    }
    for (int l=qubitList[yReg].start; l<qubitList[yReg].end; l++) {
        controlQubits.push_back(l);
    }

    // For each possible value of x and y, create a function to draw a pixel
    int totalWidth = qubitList[xReg].amount+qubitList[yReg].amount;
    std::string drawPixelBinaries = ancillaLock(totalWidth, "ancillaDrawPixelBinaries" + regName);
    int perLowestLevel = 3;
    for (int i=totalWidth-perLowestLevel; i>=0; i--) {
        for (int j=0; j<std::pow(2, i); j++) {

            // Determine the function names
            std::vector<int> asBinary = toBinary(j, i);
            std::string funcName = "drawPixel" + regName;
            for (size_t k=0; k<asBinary.size(); k++) {
                funcName += std::to_string(asBinary[k]);
            }
            std::string funcNameIfYes = funcName + "1";
            std::string funcNameIfNo = funcName + "0";

            // Determine which qubits to affect
            int qubitToCheck = qubitList[xReg].start + i;
            if (i >= qubitList[xReg].amount) {
                qubitToCheck = qubitList[yReg].start + i - qubitList[xReg].amount;
            }
            int qubitResult = qubitList[drawPixelBinaries].start + i;

            // Check the qubit, is yes call yes, if no call no
            if (i < totalWidth-perLowestLevel) {
                beginSubroutine(funcName);
                    cx(qubitToCheck, qubitResult);
                    callSubroutineIf(funcNameIfYes, qubitResult);
                    x(qubitResult);
                    callSubroutineIf(funcNameIfNo, qubitResult);
                    x(qubitResult);
                    cx(qubitToCheck, qubitResult);
                endSubroutine();
            } else {
                beginSubroutine(funcName);
                    for (int k=0; k<std::pow(2, perLowestLevel); k++) {
                        std::vector<int> extra = toBinary(k, perLowestLevel);
                        std::vector<int> combined = asBinary;
                        for (size_t l=0; l<extra.size(); l++) {
                            combined.push_back(extra[l]);
                        }
                        std::vector<int> xBits;
                        for (int l=0; l<qubitList[xReg].amount; l++) {
                            xBits.push_back(combined[l]);
                        }
                        std::vector<int> yBits;
                        for (int l=0; l<qubitList[yReg].amount; l++) {
                            yBits.push_back(combined[l+qubitList[xReg].amount]);
                        }
                        int xVal = toDenary(xBits);
                        int yVal = toDenary(yBits);
                        if (xVal >= 0 && xVal < screenWidth && yVal >= 0 && yVal < screenHeight - barHeight) {
                            int coordVal = screenToQubit(xVal, yVal);
                            for (size_t l=0; l<controlQubits.size(); l++) {
                                if (combined[l] == 0) {
                                    x(controlQubits[l]);
                                }
                            }
                            cx(controlQubits, coordVal);
                            for (size_t l=0; l<controlQubits.size(); l++) {
                                if (combined[l] == 0) {
                                    x(controlQubits[l]);
                                }
                            }
                        }
                    }
                endSubroutine();
            }
        }
    }

    // Free all the ancillas
    ancillaFree(drawPixelBinaries);

    // See how many peak ancillas we used during the function
    if (peakAncilla) {
        int ancillasUsedInFunc = stopPeakAncilla();
        std::cout << "Peak ancillas used in draw pixel: " << ancillasUsedInFunc << std::endl;
        ancillaLock(ancillasUsedInFunc, "ancillaFunctionDrawPixel");
    }

}

// Draw a pixel based on the values of some registers
void drawPixel(std::string name) {
    callSubroutine("drawPixel" + name);
}

// Draw a pixel based on the values of some registers if another register
void drawPixelIf(std::string name, std::string regToCheck) {
    callSubroutineIf("drawPixel" + name, regToCheck);
}

// Draw a pixel based on the values of some registers if the values are in range
void drawPixelIfValid(std::string name, std::string xCoord, int minX, int maxX, std::string yCoord, int minY, int maxY) {
    std::string allInRange = ancillaLock(4, "ancillaAllInRange");
    std::string xGreaterThanMin = subreg(allInRange, 0, 1);
    std::string xLessThanMax = subreg(allInRange, 1, 2);
    std::string yGreaterThanMin = subreg(allInRange, 2, 3);
    std::string yLessThanMax = subreg(allInRange, 3, 4);
    int rec = startRecording();
    flipIfGreaterOrEqualTC(xGreaterThanMin, xCoord, minX);
    flipIfLessOrEqualTC(xLessThanMax, xCoord, maxX);
    flipIfGreaterOrEqualTC(yGreaterThanMin, yCoord, minY);
    flipIfLessOrEqualTC(yLessThanMax, yCoord, maxY);
    stopRecording(rec);
    callSubroutineIf("drawPixel" + name, allInRange);
    uncomputeRecording(rec);
    ancillaFree(allInRange);
}

// The functions called whenever a line is drawn
void functionDrawLine(std::string startX, std::string startY, std::string endX, std::string endY) {

    // If the function is already defined, don't define it again
    if (subroutineNameToIndex.find("drawLine") != subroutineNameToIndex.end()) {
        std::cout << "Function drawLine already defined, skipping" << std::endl;
        return;
    }

    // How much buffer we need for floating point operations with ints
    std::cout << "Creating draw line functions..." << std::endl;
    int shiftBits = 5;
    int extraBits = 0;
    int totalExtra = extraBits + shiftBits;
    int shiftAmount = std::pow(2, shiftBits);
    int rec = 0;

    // Start recording the peak ancilla usage
    startPeakAncilla();

    // Define all the ancilla we need
    std::string startXLessThanEnd = ancillaLock(1, "ancillaStartXLessThanEnd");
    std::string startYLessThanEnd = ancillaLock(1, "ancillaStartYLessThanEnd");
    std::string diffYGreaterThanX = ancillaLock(1, "ancillaDiffYGreaterThanX");
    std::string subX = ancillaLock(qubitList[endX].amount+totalExtra, "ancillaSubX");
    std::string subY = ancillaLock(qubitList[endY].amount+totalExtra, "ancillaSubY");
    std::string gradient = ancillaLock(qubitList[subY].amount, "ancillaGradient");
    std::string gradientTimesMax = ancillaLock(qubitList[subY].amount, "ancillaGradientTimesMax");
    std::string remainder = ancillaLock(qubitList[subY].amount, "ancillaRemainder");
    std::string movingX = ancillaLock(qubitList[endY].amount+totalExtra, "ancillaMovingX");
    std::string movingY = ancillaLock(qubitList[endY].amount+totalExtra, "ancillaMovingY");
    std::string keepDrawing = ancillaLock(1, "ancillaKeepDrawing");

    // Subregs used for the third level subroutines
    std::string xSubreg = subreg(movingX, extraBits, extraBits+qubitList[endX].amount);
    std::string ySubreg = subreg(movingY, extraBits, extraBits+qubitList[endY].amount);
    int maxDiag = std::sqrt(std::pow(screenWidth, 2) + std::pow(screenHeight, 2));

    // Create a function specifically for drawing pixels from the subregs
    std::string drawPixelName = "DrawLine";
    functionDrawPixel(xSubreg, ySubreg, drawPixelName, false);

    // Third level subroutine 
    beginSubroutine("drawLine,X<X,Y<Y,X<Y");
        rec = startRecording();
        shiftLeft(subX, shiftBits);
        divide(subX, subY, gradient, remainder); 
        multiply(gradient, maxDiag, gradientTimesMax); 
        copy(startX, movingX);
        copy(startY, movingY);
        shiftLeft(movingX, shiftBits); 
        shiftLeft(movingY, shiftBits);
        stopRecording(rec);
        x(keepDrawing);
        for (int i=0; i<maxDiag; i++) {
            add(movingX, gradient);
            add(movingY, shiftAmount);
            flipIfEqual(keepDrawing, ySubreg, endY);
            drawPixelIf(drawPixelName, keepDrawing);
        }
        subtract(movingY, shiftAmount*maxDiag);
        subtract(movingX, gradientTimesMax, false);
        uncomputeRecording(rec);
    endSubroutine();

    // Third level subroutine
    beginSubroutine("drawLine,X<X,Y<Y,X>Y");
        rec = startRecording();
        shiftLeft(subY, shiftBits);
        divide(subY, subX, gradient, remainder);
        multiply(gradient, maxDiag, gradientTimesMax);
        copy(startX, movingX);
        copy(startY, movingY);
        shiftLeft(movingX, shiftBits);
        shiftLeft(movingY, shiftBits);
        stopRecording(rec);
        x(keepDrawing);
        for (int i=0; i<maxDiag; i++) {
            add(movingX, shiftAmount);
            add(movingY, gradient);
            flipIfEqual(keepDrawing, xSubreg, endX);
            drawPixelIf(drawPixelName, keepDrawing);
        }
        subtract(movingX, shiftAmount*maxDiag);
        subtract(movingY, gradientTimesMax, false);
        uncomputeRecording(rec);
    endSubroutine();

    // Third level subroutine 
    beginSubroutine("drawLine,X>X,Y<Y,X<Y");
        rec = startRecording();
        shiftLeft(subX, shiftBits);
        divide(subX, subY, gradient, remainder);
        multiply(gradient, maxDiag, gradientTimesMax);
        copy(startX, movingX);
        copy(startY, movingY);
        shiftLeft(movingX, shiftBits);
        shiftLeft(movingY, shiftBits);
        stopRecording(rec);
        x(keepDrawing);
        for (int i=0; i<maxDiag; i++) {
            subtract(movingX, gradient, false);
            add(movingY, shiftAmount);
            flipIfEqual(keepDrawing, ySubreg, endY);
            drawPixelIf(drawPixelName, keepDrawing);
        }
        add(movingX, gradientTimesMax);
        subtract(movingY, shiftAmount*maxDiag);
        uncomputeRecording(rec);
    endSubroutine();
                
    // Third level subroutine 
    beginSubroutine("drawLine,X>X,Y<Y,X>Y");
        rec = startRecording();
        shiftLeft(subY, shiftBits);
        divide(subY, subX, gradient, remainder);
        multiply(gradient, maxDiag, gradientTimesMax);
        copy(startX, movingX);
        copy(startY, movingY);
        shiftLeft(movingX, shiftBits);
        shiftLeft(movingY, shiftBits);
        stopRecording(rec);
        x(keepDrawing);
        for (int i=0; i<maxDiag; i++) {
            subtract(movingX, shiftAmount);
            add(movingY, gradient);
            flipIfEqual(keepDrawing, xSubreg, endX);
            drawPixelIf(drawPixelName, keepDrawing);
        }
        add(movingX, shiftAmount*maxDiag);
        subtract(movingY, gradientTimesMax, false);
        uncomputeRecording(rec);
    endSubroutine();
                
    // Third level subroutine 
    beginSubroutine("drawLine,X<X,Y>Y,X<Y");
        rec = startRecording();
        shiftLeft(subX, shiftBits);
        divide(subX, subY, gradient, remainder);
        multiply(gradient, maxDiag, gradientTimesMax);
        copy(startX, movingX);
        copy(startY, movingY);
        shiftLeft(movingX, shiftBits);
        shiftLeft(movingY, shiftBits);
        stopRecording(rec);
        x(keepDrawing);
        for (int i=0; i<maxDiag; i++) {
            add(movingX, gradient);
            subtract(movingY, shiftAmount);
            flipIfEqual(keepDrawing, ySubreg, endY);
            drawPixelIf(drawPixelName, keepDrawing);
        }
        subtract(movingX, gradientTimesMax, false);
        add(movingY, shiftAmount*maxDiag);
        uncomputeRecording(rec);
    endSubroutine();
                
    // Third level subroutine 
    beginSubroutine("drawLine,X<X,Y>Y,X>Y");
        rec = startRecording();
        shiftLeft(subY, shiftBits);
        divide(subY, subX, gradient, remainder);
        multiply(gradient, maxDiag, gradientTimesMax);
        copy(startX, movingX);
        copy(startY, movingY);
        shiftLeft(movingX, shiftBits);
        shiftLeft(movingY, shiftBits);
        stopRecording(rec);
        x(keepDrawing);
        for (int i=0; i<maxDiag; i++) {
            add(movingX, shiftAmount);
            subtract(movingY, gradient, false);
            flipIfEqual(keepDrawing, xSubreg, endX);
            drawPixelIf(drawPixelName, keepDrawing);
        }
        subtract(movingX, shiftAmount*maxDiag);
        add(movingY, gradientTimesMax);
        uncomputeRecording(rec);
    endSubroutine();
                
    // Third level subroutine 
    beginSubroutine("drawLine,X>X,Y>Y,X<Y");
        rec = startRecording();
        shiftLeft(subX, shiftBits);
        divide(subX, subY, gradient, remainder);
        multiply(gradient, maxDiag, gradientTimesMax);
        copy(startX, movingX);
        copy(startY, movingY);
        shiftLeft(movingX, shiftBits);
        shiftLeft(movingY, shiftBits);
        stopRecording(rec);
        x(keepDrawing);
        for (int i=0; i<maxDiag; i++) {
            subtract(movingX, gradient, false);
            subtract(movingY, shiftAmount);
            flipIfEqual(keepDrawing, ySubreg, endY);
            drawPixelIf(drawPixelName, keepDrawing);
        }
        add(movingX, gradientTimesMax);
        add(movingY, shiftAmount*maxDiag);
        uncomputeRecording(rec);
    endSubroutine();
                
    // Third level subroutine 
    beginSubroutine("drawLine,X>X,Y>Y,X>Y");
        rec = startRecording();
        shiftLeft(subY, shiftBits);
        divide(subY, subX, gradient, remainder);
        multiply(gradient, maxDiag, gradientTimesMax);
        copy(startX, movingX);
        copy(startY, movingY);
        shiftLeft(movingX, shiftBits);
        shiftLeft(movingY, shiftBits);
        stopRecording(rec);
        x(keepDrawing);
        for (int i=0; i<maxDiag; i++) {
            subtract(movingX, shiftAmount);
            subtract(movingY, gradient, false);
            flipIfEqual(keepDrawing, xSubreg, endX);
            drawPixelIf(drawPixelName, keepDrawing);
        }
        add(movingX, shiftAmount*maxDiag);
        add(movingY, gradientTimesMax);
        uncomputeRecording(rec);
    endSubroutine();
                
    // Second level subroutine
    beginSubroutine("drawLine,X<X,Y<Y");
        rec = startRecording();
        copy(endX, subX);
        subtract(subX, startX, false);
        copy(endY, subY);
        subtract(subY, startY, false);
        flipIfLessOrEqual(diffYGreaterThanX, subX, subY);
        stopRecording(rec);
        callSubroutineIf("drawLine,X<X,Y<Y,X<Y", diffYGreaterThanX);
        x(diffYGreaterThanX);
        callSubroutineIf("drawLine,X<X,Y<Y,X>Y", diffYGreaterThanX);
        x(diffYGreaterThanX);
        uncomputeRecording(rec);
    endSubroutine();

    // Second level subroutine
    beginSubroutine("drawLine,X<X,Y>Y");
        rec = startRecording();
        copy(endX, subX);
        subtract(subX, startX, false);
        copy(startY, subY);
        subtract(subY, endY, false);
        flipIfLessOrEqual(diffYGreaterThanX, subX, subY);
        stopRecording(rec);
        callSubroutineIf("drawLine,X<X,Y>Y,X<Y", diffYGreaterThanX);
        x(diffYGreaterThanX);
        callSubroutineIf("drawLine,X<X,Y>Y,X>Y", diffYGreaterThanX);
        x(diffYGreaterThanX);
        uncomputeRecording(rec);
    endSubroutine();

    // Second level subroutine
    beginSubroutine("drawLine,X>X,Y<Y");
        rec = startRecording();
        copy(startX, subX);
        subtract(subX, endX, false);
        copy(endY, subY);
        subtract(subY, startY, false);
        flipIfLessOrEqual(diffYGreaterThanX, subX, subY);
        stopRecording(rec);
        callSubroutineIf("drawLine,X>X,Y<Y,X<Y", diffYGreaterThanX);
        x(diffYGreaterThanX);
        callSubroutineIf("drawLine,X>X,Y<Y,X>Y", diffYGreaterThanX);
        x(diffYGreaterThanX);
        uncomputeRecording(rec);
    endSubroutine();

    // Second level subroutine
    beginSubroutine("drawLine,X>X,Y>Y");
        rec = startRecording();
        copy(startX, subX);
        subtract(subX, endX, false);
        copy(startY, subY);
        subtract(subY, endY, false);
        flipIfLessOrEqual(diffYGreaterThanX, subX, subY);
        stopRecording(rec);
        callSubroutineIf("drawLine,X>X,Y>Y,X<Y", diffYGreaterThanX);
        x(diffYGreaterThanX);
        callSubroutineIf("drawLine,X>X,Y>Y,X>Y", diffYGreaterThanX);
        x(diffYGreaterThanX);
        uncomputeRecording(rec);
    endSubroutine();

    // First level subroutine
    beginSubroutine("drawLine,X<X");
        flipIfLessOrEqual(startYLessThanEnd, startY, endY);
        callSubroutineIf("drawLine,X<X,Y<Y", startYLessThanEnd);
        x(startYLessThanEnd);
        callSubroutineIf("drawLine,X<X,Y>Y", startYLessThanEnd);
        x(startYLessThanEnd);
        flipIfLessOrEqual(startYLessThanEnd, startY, endY);
    endSubroutine();

    // First level subroutine
    beginSubroutine("drawLine,X>X");
        flipIfLessOrEqual(startYLessThanEnd, startY, endY);
        callSubroutineIf("drawLine,X>X,Y<Y", startYLessThanEnd);
        x(startYLessThanEnd);
        callSubroutineIf("drawLine,X>X,Y>Y", startYLessThanEnd);
        x(startYLessThanEnd);
        flipIfLessOrEqual(startYLessThanEnd, startY, endY);
    endSubroutine();

    // Top level subroutine
    beginSubroutine("drawLine");
        flipIfLessOrEqual("ancillaStartXLessThanEnd", startX, endX);
        callSubroutineIf("drawLine,X<X", "ancillaStartXLessThanEnd");
        x("ancillaStartXLessThanEnd");
        callSubroutineIf("drawLine,X>X", "ancillaStartXLessThanEnd");
        x("ancillaStartXLessThanEnd");
        flipIfLessOrEqual("ancillaStartXLessThanEnd", startX, endX);
    endSubroutine();

    // Free all the ancillas
    ancillaFree("ancillaStartXLessThanEnd");
    ancillaFree("ancillaStartYLessThanEnd");
    ancillaFree("ancillaDiffYGreaterThanX");
    ancillaFree("ancillaSubX");
    ancillaFree("ancillaSubY");
    ancillaFree("ancillaGradient");
    ancillaFree("ancillaGradientTimesMax");
    ancillaFree("ancillaRemainder");
    ancillaFree("ancillaMovingX");
    ancillaFree("ancillaMovingY");
    ancillaFree("ancillaKeepDrawing");

    // See how many peak ancillas we used during the function
    int ancillasUsedInFunc = stopPeakAncilla();
    std::cout << "Peak ancillas used in draw line: " << ancillasUsedInFunc << std::endl;
    ancillaLock(ancillasUsedInFunc, "ancillaFunctionDrawLine");

}

// Draw a line based on the values of some registers
void drawLine() {
    callSubroutine("drawLine");
}

// Draw a line based on the values of some registers
void drawLineIf(std::string reg) {
    callSubroutineIf("drawLine", reg);
}

// Draw a line based on the values of some registers
void drawLineIfValid(std::string reg, int minX, int maxX, int minY, int maxY, std::string x1RegName, std::string y1RegName, std::string x2RegName, std::string y2RegName) {

    // The main ancilla and its subregs
    std::string allInRange = ancillaLock(8, "ancillaAllInRangeLine");
    std::string x1GreaterThanMin = subreg(allInRange, 0, 1);
    std::string x1LessThanMax = subreg(allInRange, 1, 2);
    std::string y1GreaterThanMin = subreg(allInRange, 2, 3);
    std::string y1LessThanMax = subreg(allInRange, 3, 4);
    std::string x2GreaterThanMin = subreg(allInRange, 4, 5);
    std::string x2LessThanMax = subreg(allInRange, 5, 6);
    std::string y2GreaterThanMin = subreg(allInRange, 6, 7);
    std::string y2LessThanMax = subreg(allInRange, 7, 8);
    std::string allGood = ancillaLock(1, "ancillaAllGood");

    // Some extra ancillas that I realised I needed later
    std::string x1NotEqualX2 = ancillaLock(1, "ancillaX1NotEqualX2");
    std::string y1NotEqualY2 = ancillaLock(1, "ancillaY1NotEqualY2");
    std::string notEverythingIsEqual = ancillaLock(1, "ancillaNotEverythingIsEqual");

    // Do each check
    int rec = startRecording();
    flipIfGreaterOrEqualTC(x1GreaterThanMin, x1RegName, minX);
    flipIfLessOrEqualTC(x1LessThanMax, x1RegName, maxX);
    flipIfGreaterOrEqualTC(y1GreaterThanMin, y1RegName, minY);
    flipIfLessOrEqualTC(y1LessThanMax, y1RegName, maxY);
    flipIfGreaterOrEqualTC(x2GreaterThanMin, x2RegName, minX);
    flipIfLessOrEqualTC(x2LessThanMax, x2RegName, maxX);
    flipIfGreaterOrEqualTC(y2GreaterThanMin, y2RegName, minY);
    flipIfLessOrEqualTC(y2LessThanMax, y2RegName, maxY);
    flipIfEqual(x1NotEqualX2, x1RegName, x2RegName);
    flipIfEqual(y1NotEqualY2, y1RegName, y2RegName);
    x(x1NotEqualX2);
    x(y1NotEqualY2);
    flipIfOr(notEverythingIsEqual, {x1NotEqualX2, y1NotEqualY2});

    // Flip the ancilla if all the checks passed
    flipIfAnd(allGood, {allInRange, reg, notEverythingIsEqual});

    // Draw the line if everything is good
    stopRecording(rec);
    callSubroutineIf("drawLine", allGood);
    uncomputeRecording(rec);

    // Free the ancillas
    ancillaFree(allInRange);
    ancillaFree(allGood);
    ancillaFree(x1NotEqualX2);
    ancillaFree(y1NotEqualY2);
    ancillaFree(notEverythingIsEqual);
    clearAllSubregs();

}

// Given a fixed coord, project it onto the screen relative to the player
// https://en.wikipedia.org/wiki/3D_projection
void projectCoord(std::vector<int> coord, std::string xReg, std::string yReg) {

    // Should probably be moved to be global, but eh for now it's fine

    // Define all the ancillas we need
    std::string diffX = ancillaLock(qubitsPer, "ancillaDiffX");
    std::string diffY = ancillaLock(qubitsPer, "ancillaDiffY");
    std::string diffZ = ancillaLock(qubitsPer, "ancillaDiffZ");
    std::string dX = ancillaLock(qubitsPer, "ancillaDX");
    std::string dY = ancillaLock(qubitsPer, "ancillaDY");
    std::string dZ = ancillaLock(qubitsPer, "ancillaDZ");
    std::string diffXCos = ancillaLock(qubitsPer, "ancillaDiffXCos");
    std::string diffXSin = ancillaLock(qubitsPer, "ancillaDiffXSin");
    std::string diffYCos = ancillaLock(qubitsPer, "ancillaDiffYCos");
    std::string diffYSin = ancillaLock(qubitsPer, "ancillaDiffYSin");
    std::string eYdX = ancillaLock(qubitsPer, "ancillaEYdX");
    std::string eYdZ = ancillaLock(qubitsPer, "ancillaEYdZ");
    std::string bX = ancillaLock(qubitsPer, "ancillaBX");
    std::string bY = ancillaLock(qubitsPer, "ancillaBY");
    std::string rem1 = ancillaLock(qubitsPer, "ancillaRem1");
    std::string rem2 = ancillaLock(qubitsPer, "ancillaRem2");
    int rec = startRecording();

    // diff = a - c
    setValTC(diffX, coord[0]);
    setValTC(diffY, coord[1]);
    setValTC(diffZ, coord[2]);
    subtract(diffX, "playerX");
    subtract(diffY, "playerY");
    subtract(diffZ, "playerZ");

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

    // dz = diffZ 
    copy(diffZ, dZ);
    shiftLeft(dZ, leftShift);
    neg(dZ);

    // bX = (eY*dX) / dY + eX
    multiply(dX, eY, eYdX);
    divideTC(eYdX, dY, bX, rem1);
    add(bX, eX);

    // bY = (eY*dZ) / dY + eZ
    multiply(dZ, eY, eYdZ);
    divideTC(eYdZ, dY, bY, rem2);
    add(bY, eZ);

    // If diffY is <= 0, don't draw
    std::string shouldDraw = ancillaLock(3, "ancillaShouldDrawPixel");
    std::string dYPositive = subreg(shouldDraw, 0, 1);
    std::string bXSmall = subreg(shouldDraw, 1, 2);
    std::string bYSmall = subreg(shouldDraw, 2, 3);
    std::string shouldDrawCombined = ancillaLock(1, "ancillaShouldDrawCombined");
    flipIfGreaterOrEqualTC(dYPositive, dY, 1);
    flipIfLessOrEqualTC(bXSmall, bX, screenWidth);
    flipIfLessOrEqualTC(bYSmall, bY, screenHeight);
    flipIf(shouldDrawCombined, shouldDraw);

    // Copy the final coord to the registers
    stopRecording(rec);
    x(shouldDrawCombined);
    setValIf(xReg, 500, shouldDrawCombined);
    setValIf(yReg, 500, shouldDrawCombined);
    x(shouldDrawCombined);
    copyTCIf(bX, xReg, shouldDrawCombined);
    copyTCIf(bY, yReg, shouldDrawCombined);

    // Uncompute everything and free all of our ancillas
    uncomputeRecording(rec);
    ancillaFree(diffX);
    ancillaFree(diffY);
    ancillaFree(diffZ);
    ancillaFree(dX);
    ancillaFree(dY);
    ancillaFree(dZ);
    ancillaFree(diffXCos);
    ancillaFree(diffXSin);
    ancillaFree(diffYCos);
    ancillaFree(diffYSin);
    ancillaFree(eYdX);
    ancillaFree(eYdZ);
    ancillaFree(bX);
    ancillaFree(bY);
    ancillaFree(rem1);
    ancillaFree(rem2);
    ancillaFree(shouldDraw);
    ancillaFree(shouldDrawCombined);

}

// Given a point in world coords, render the pixel relative to the player
void renderPoint(std::vector<int> point) {
    int rec = startRecording();
    projectCoord(point, "pixelToDrawX", "pixelToDrawY");
    stopRecording(rec);
    drawPixelIfValid("Main", "pixelToDrawX", 0, screenWidth, "pixelToDrawY", 0, screenHeight);
    uncomputeRecording(rec);
}

void renderLine(std::vector<int> start, std::vector<int> end) {
    int rec = startRecording();
    setValTC("lineXWorldStart", start[0]);
    setValTC("lineYWorldStart", start[1]);
    setValTC("lineZWorldStart", start[2]);
    setValTC("lineXWorldEnd", end[0]);
    setValTC("lineYWorldEnd", end[1]);
    setValTC("lineZWorldEnd", end[2]);
    stopRecording(rec);
    callSubroutine("renderLine");
    uncomputeRecording(rec);
}

// Given a start and end in world coords, draw a line relative to the player
void functionRenderLine(std::string xStart, std::string yStart, std::string zStart, std::string xEnd, std::string yEnd, std::string zEnd) {

    // Requires draw line function
    functionDrawLine("lineXScreenStart", "lineYScreenStart", "lineXScreenEnd", "lineYScreenEnd");

    // Screen settings based on the FOV
    int clipShift = 5;
    
    // Start recording the peak ancilla usage
    startPeakAncilla();

    // Define all the ancillas we need for the first
    std::string diffX1 = ancillaLock(qubitsPer, "ancillaDiffX1");
    std::string diffY1 = ancillaLock(qubitsPer, "ancillaDiffY1");
    std::string diffZ1 = ancillaLock(qubitsPer, "ancillaDiffZ1");
    std::string dX1 = ancillaLock(qubitsPer, "ancillaDX1");
    std::string dY1 = ancillaLock(qubitsPer, "ancillaDY1");
    std::string dZ1 = ancillaLock(qubitsPer, "ancillaDZ1");
    std::string dZ1Scaled = ancillaLock(qubitsPer, "ancillaDZ1Scaled");
    std::string diffXCos1 = ancillaLock(qubitsPer, "ancillaDiffXCos1");
    std::string diffXSin1 = ancillaLock(qubitsPer, "ancillaDiffXSin1");
    std::string diffYCos1 = ancillaLock(qubitsPer, "ancillaDiffYCos1");
    std::string diffYSin1 = ancillaLock(qubitsPer, "ancillaDiffYSin1");
    std::string eYdX1 = ancillaLock(qubitsPer, "ancillaEYdX1");
    std::string eYdZ1 = ancillaLock(qubitsPer, "ancillaEYdZ1");
    std::string bX1 = ancillaLock(qubitsPer, "ancillaBX1");
    std::string bY1 = ancillaLock(qubitsPer, "ancillaBY1");
    std::string rem11 = ancillaLock(qubitsPer, "ancillaRem11");
    std::string rem21 = ancillaLock(qubitsPer, "ancillaRem21");

    // Define all the ancillas we need for the second
    std::string diffX2 = ancillaLock(qubitsPer, "ancillaDiffX2");
    std::string diffY2 = ancillaLock(qubitsPer, "ancillaDiffY2");
    std::string diffZ2 = ancillaLock(qubitsPer, "ancillaDiffZ2");
    std::string dX2 = ancillaLock(qubitsPer, "ancillaDX2");
    std::string dY2 = ancillaLock(qubitsPer, "ancillaDY2");
    std::string dZ2 = ancillaLock(qubitsPer, "ancillaDZ2");
    std::string dZ2Scaled = ancillaLock(qubitsPer, "ancillaDZ2Scaled");
    std::string diffXCos2 = ancillaLock(qubitsPer, "ancillaDiffXCos2");
    std::string diffXSin2 = ancillaLock(qubitsPer, "ancillaDiffXSin2");
    std::string diffYCos2 = ancillaLock(qubitsPer, "ancillaDiffYCos2");
    std::string diffYSin2 = ancillaLock(qubitsPer, "ancillaDiffYSin2");
    std::string eYdX2 = ancillaLock(qubitsPer, "ancillaEYdX2");
    std::string eYdZ2 = ancillaLock(qubitsPer, "ancillaEYdZ2");
    std::string bX2 = ancillaLock(qubitsPer, "ancillaBX2");
    std::string bY2 = ancillaLock(qubitsPer, "ancillaBY2");
    std::string rem12 = ancillaLock(qubitsPer, "ancillaRem12");
    std::string rem22 = ancillaLock(qubitsPer, "ancillaRem22");

    // Ancillas for the clipping
    std::string outLeft1 = ancillaLock(1, "outLeft1");
    std::string outRight1 = ancillaLock(1, "outRight1");
    std::string outTop1 = ancillaLock(1, "outTop1");
    std::string outBottom1 = ancillaLock(1, "outBottom1");
    std::string outNear1 = ancillaLock(1, "outNear1");
    std::string outFar1 = ancillaLock(1, "outFar1");
    std::string outLeft2 = ancillaLock(1, "outLeft2");
    std::string outRight2 = ancillaLock(1, "outRight2");
    std::string outTop2 = ancillaLock(1, "outTop2");
    std::string outBottom2 = ancillaLock(1, "outBottom2");
    std::string outNear2 = ancillaLock(1, "outNear2");
    std::string outFar2 = ancillaLock(1, "outFar2");
    std::string out2Left1 = ancillaLock(1, "out2Left1");
    std::string out2Right1 = ancillaLock(1, "out2Right1");
    std::string out2Top1 = ancillaLock(1, "out2Top1");
    std::string out2Bottom1 = ancillaLock(1, "out2Bottom1");
    std::string out2Near1 = ancillaLock(1, "out2Near1");
    std::string out2Far1 = ancillaLock(1, "out2Far1");
    std::string out2Left2 = ancillaLock(1, "out2Left2");
    std::string out2Right2 = ancillaLock(1, "out2Right2");
    std::string out2Top2 = ancillaLock(1, "out2Top2");
    std::string out2Bottom2 = ancillaLock(1, "out2Bottom2");
    std::string out2Near2 = ancillaLock(1, "out2Near2");
    std::string out2Far2 = ancillaLock(1, "out2Far2");
    std::string needClipping = ancillaLock(1, "needClipping");
    std::string shouldDraw = ancillaLock(1, "shouldDraw");
    std::string andLeft = ancillaLock(1, "andLeft");
    std::string andRight = ancillaLock(1, "andRight");
    std::string andTop = ancillaLock(1, "andTop");
    std::string andBottom = ancillaLock(1, "andBottom");
    std::string andNear = ancillaLock(1, "andNear");
    std::string andFar = ancillaLock(1, "andFar");
    std::string tRem = ancillaLock(qubitsPer, "tRem");
    std::string tTop = ancillaLock(qubitsPer, "tTop");
    std::string tBottom = ancillaLock(qubitsPer, "tBottom");
    std::string tAdd = ancillaLock(qubitsPer, "tAdd");
    std::string tSub = subreg(tAdd, 0, qubitsPer-clipShift);
    std::string dXDiff = ancillaLock(qubitsPer, "dXDiff");
    std::string dYDiff = ancillaLock(qubitsPer, "dYDiff");
    std::string dZDiff = ancillaLock(qubitsPer, "dZDiff");
    std::string dZDiffScaled = ancillaLock(qubitsPer, "dZDiffScaled");

    std::string tRight1 = ancillaLock(qubitsPer, "tRight1");
    std::string tRight2 = ancillaLock(qubitsPer, "tRight2");
    std::string tLeft1 = ancillaLock(qubitsPer, "tLeft1");
    std::string tLeft2 = ancillaLock(qubitsPer, "tLeft2");
    std::string tTopMain1 = ancillaLock(qubitsPer, "tTopMain1");
    std::string tTopMain2 = ancillaLock(qubitsPer, "tTopMain2");
    std::string tBottomMain1 = ancillaLock(qubitsPer, "tBottomMain1");
    std::string tBottomMain2 = ancillaLock(qubitsPer, "tBottomMain2");
    std::string tNear1 = ancillaLock(qubitsPer, "tNear1");
    std::string tNear2 = ancillaLock(qubitsPer, "tNear2");
    std::string tFar1 = ancillaLock(qubitsPer, "tFar1");
    std::string tFar2 = ancillaLock(qubitsPer, "tFar2");
    std::string tSwapRight1 = ancillaLock(qubitsPer, "tSwapRight1");
    std::string tSwapRight2 = ancillaLock(qubitsPer, "tSwapRight2");
    std::string tSwapLeft1 = ancillaLock(qubitsPer, "tSwapLeft1");
    std::string tSwapLeft2 = ancillaLock(qubitsPer, "tSwapLeft2");
    std::string tSwapTop1 = ancillaLock(qubitsPer, "tSwapTop1");
    std::string tSwapTop2 = ancillaLock(qubitsPer, "tSwapTop2");
    std::string tSwapBottom1 = ancillaLock(qubitsPer, "tSwapBottom1");
    std::string tSwapBottom2 = ancillaLock(qubitsPer, "tSwapBottom2");
    std::string tSwapNear1 = ancillaLock(qubitsPer, "tSwapNear1");
    std::string tSwapNear2 = ancillaLock(qubitsPer, "tSwapNear2");
    std::string tSwapFar1 = ancillaLock(qubitsPer, "tSwapFar1");
    std::string tSwapFar2 = ancillaLock(qubitsPer, "tSwapFar2");

    // The function for when everything has been clipped
    beginSubroutine("renderLineInside");

        // bX1 = (eY*dX1) / dY1 + eX
        multiply(dX1, eY, eYdX1);
        divideTC(eYdX1, dY1, bX1, rem11);
        add(bX1, eX);

        // bX2 = (eY*dX2) / dY2 + eX
        multiply(dX2, eY, eYdX2);
        divideTC(eYdX2, dY2, bX2, rem12);
        add(bX2, eX);

        // bY1 = (eY*dZ1) / dY1 + eZ
        multiply(dZ1, eY, eYdZ1);
        divideTC(eYdZ1, dY1, bY1, rem21);
        add(bY1, eZ);

        // bY2 = (eY*dZ2) / dY2 + eZ
        multiply(dZ2, eY, eYdZ2);
        divideTC(eYdZ2, dY2, bY2, rem22);
        add(bY2, eZ);

        // Copy the final coord to the registers
        copyTC(bX1, "lineXScreenStart");
        copyTC(bY1, "lineYScreenStart");
        copyTC(bX2, "lineXScreenEnd");
        copyTC(bY2, "lineYScreenEnd");

        // Debug text
        if (debugRender) {
            outputReg("dX1 = ", dX1, "denaryTC");
            outputReg("dY1 = ", dY1, "denaryTC");
            outputReg("dZ1 = ", dZ1, "denaryTC");
            outputReg("dX2 = ", dX2, "denaryTC");
            outputReg("dY2 = ", dY2, "denaryTC");
            outputReg("dZ2 = ", dZ2, "denaryTC");
            outputReg("lineXScreenStart = ", "lineXScreenStart", "denaryTC");
            outputReg("lineYScreenStart = ", "lineYScreenStart", "denaryTC");
            outputReg("lineXScreenEnd = ", "lineXScreenEnd", "denaryTC");
            outputReg("lineYScreenEnd = ", "lineYScreenEnd", "denaryTC");
        }

    endSubroutine();

    // Things shared by all clipping subroutines
    beginSubroutine("renderLineShared");

        // Calculate the differences between the two points
        copyTC(dX2, dXDiff);
        subtract(dXDiff, dX1);
        copyTC(dY2, dYDiff);
        subtract(dYDiff, dY1);
        copyTC(dZ2, dZDiff);
        subtract(dZDiff, dZ1);
        copyTC(dZ2Scaled, dZDiffScaled);
        subtract(dZDiffScaled, dZ1Scaled);

        //outputReg("xDiff = ", dXDiff, "denaryTC");
        //outputReg("yDiff = ", dYDiff, "denaryTC");
        //outputReg("zDiff = ", dZDiff, "denaryTC");
        //outputReg("zDiff scaled = ", dZDiffScaled, "denaryTC");

    endSubroutine();

    // If need clipping and second point is out the right plane, clip
    beginSubroutine("renderLineRight2");

        if (debugRender) {
            outputReg("Clipping second point to the right");
            outputReg("x = ", dX2, "denaryTC");
            outputReg("y = ", dY2, "denaryTC");
            outputReg("z = ", dZ2, "denaryTC");
        }

        // t = (dX2 - dY2) / (dXDiff - dYDiff)
        int recTRight2 = startRecording();
        copyTC(dX2, tTop);
        subtract(tTop, dY2);
        copyTC(dXDiff, tBottom);
        subtract(tBottom, dYDiff);
        //outputReg("tTop = ", tTop, "denaryTC");
        //outputReg("tBottom = ", tBottom, "denaryTC");
        shiftLeftTC(tTop, clipShift);
        stopRecording(recTRight2);
        //outputReg("tRight before = ", tRight2, "denaryTC");
        divideTC(tTop, tBottom, tRight2, tRem);
        uncomputeRecording(recTRight2);
        //outputReg("t * 32 = ", tRight2, "denaryTC");

        // dX2 <- dX2 - t*dXDiff
        int recXRight2 = startRecording();
        multiply(dXDiff, tRight2, tAdd);
        stopRecording(recXRight2);
        //outputReg("tAdd X = ", tAdd, "denaryTC");
        //outputReg("tSub X = ", tSub, "denaryTC");
        subtractTC(dX2, tSub);
        uncomputeRecording(recXRight2);
        //outputReg("uncomuted tAdd X = ", tAdd, "denaryTC");

        // dY2 <- dX2
        swap(dY2, tSwapRight2);
        copyTC(dX2, dY2);

        // dZ2 <- dZ2 - t*dZDiff
        int recZRight2 = startRecording();
        multiply(dZDiff, tRight2, tAdd);
        stopRecording(recZRight2);
        //outputReg("tAdd Z = ", tAdd, "denaryTC");
        //outputReg("tSub Z = ", tSub, "denaryTC");
        subtractTC(dZ2, tSub);
        uncomputeRecording(recZRight2);

        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");
        //outputReg("uncomuted tTop = ", tTop, "denaryTC");
        //outputReg("uncomuted tBottom = ", tBottom, "denaryTC");
        //outputReg("new x = ", dX2, "denaryTC");
        //outputReg("new y = ", dY2, "denaryTC");
        //outputReg("new z = ", dZ2, "denaryTC");
        //outputReg();

    endSubroutine();
    
    // If need clipping and first point is out the right plane, clip
    beginSubroutine("renderLineRight1");

        if (debugRender) {
            outputReg("Clipping first point to the right");
            outputReg("x = ", dX1, "denaryTC");
            outputReg("y = ", dY1, "denaryTC");
            outputReg("z = ", dZ1, "denaryTC");
        }

        // t = (dX1 - dY1) / (dYDiff - dXDiff)
        int recTRight1 = startRecording();
        copyTC(dX1, tTop);
        subtract(tTop, dY1);
        copyTC(dYDiff, tBottom);
        subtract(tBottom, dXDiff);
        //outputReg("tTop = ", tTop, "denaryTC");
        //outputReg("tBottom = ", tBottom, "denaryTC");
        shiftLeftTC(tTop, clipShift);
        stopRecording(recTRight1);
        divideTC(tTop, tBottom, tRight1, tRem);
        uncomputeRecording(recTRight1);
        //outputReg("t * 32 = ", tRight1, "denaryTC");

        // dX1 <- dX1 + t*dXDiff
        int recXRight1 = startRecording();
        multiply(dXDiff, tRight1, tAdd);
        stopRecording(recXRight1);
        //outputReg("tAdd X = ", tAdd, "denaryTC");
        //outputReg("tSub X = ", tSub, "denaryTC");
        addTC(dX1, tSub);
        uncomputeRecording(recXRight1);
        //outputReg("uncomuted tAdd X = ", tAdd, "denaryTC");

        // dY1 <- dX1
        swap(dY1, tSwapRight1);
        copyTC(dX1, dY1);

        // dZ1 <- dZ1 + t*dZDiff
        int recZRight1 = startRecording();
        multiply(dZDiff, tRight1, tAdd);
        stopRecording(recZRight1);
        //outputReg("tAdd Z = ", tAdd, "denaryTC");
        //outputReg("tSub Z = ", tSub, "denaryTC");
        addTC(dZ1, tSub);
        uncomputeRecording(recZRight1);

        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");
        //outputReg("uncomuted tTop = ", tTop, "denaryTC");
        //outputReg("uncomuted tBottom = ", tBottom, "denaryTC");
        //outputReg("new x = ", dX1, "denaryTC");
        //outputReg("new y = ", dY1, "denaryTC");
        //outputReg("new z = ", dZ1, "denaryTC");
        //outputReg();

    endSubroutine();

    // If need clipping and second point is out the left plane, clip
    beginSubroutine("renderLineLeft2");

        if (debugRender) {
            outputReg("Clipping second point to the left");
            outputReg("x = ", dX2, "denaryTC");
            outputReg("y = ", dY2, "denaryTC");
            outputReg("z = ", dZ2, "denaryTC");
        }

        // t = (dX2 + dY2) / (dYDiff + dXDiff)
        int recTLeft2 = startRecording();
        copyTC(dX2, tTop);
        add(tTop, dY2);
        copyTC(dYDiff, tBottom);
        add(tBottom, dXDiff);
        //outputReg("tTop = ", tTop, "denaryTC");
        //outputReg("tBottom = ", tBottom, "denaryTC");
        shiftLeftTC(tTop, clipShift);
        stopRecording(recTLeft2);
        divideTC(tTop, tBottom, tLeft2, tRem);
        uncomputeRecording(recTLeft2);
        //outputReg("t * 32 = ", tLeft2, "denaryTC");

        // dX2 <- dX2 - t*dXDiff
        int recXLeft2 = startRecording();
        multiply(dXDiff, tLeft2, tAdd);
        stopRecording(recXLeft2);
        //outputReg("tAdd X = ", tAdd, "denaryTC");
        //outputReg("tSub X = ", tSub, "denaryTC");
        subtractTC(dX2, tSub);
        uncomputeRecording(recXLeft2);
        //outputReg("uncomuted tAdd X = ", tAdd, "denaryTC");

        // dY2 <- -dX2
        swap(dY2, tSwapLeft2);
        copyTC(dX2, dY2);
        neg(dY2);

        // dZ2 <- dZ2 - t*dZDiff
        int recZLeft2 = startRecording();
        multiply(dZDiff, tLeft2, tAdd);
        stopRecording(recZLeft2);
        //outputReg("tAdd Z = ", tAdd, "denaryTC");
        //outputReg("tSub Z = ", tSub, "denaryTC");
        subtractTC(dZ2, tSub);
        uncomputeRecording(recZLeft2);

        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");
        //outputReg("uncomuted tTop = ", tTop, "denaryTC");
        //outputReg("uncomuted tBottom = ", tBottom, "denaryTC");
        //outputReg("new x = ", dX2, "denaryTC");
        //outputReg("new y = ", dY2, "denaryTC");
        //outputReg("new z = ", dZ2, "denaryTC");
        //outputReg();

    endSubroutine();

    // If need clipping and first point is out the left plane, clip
    beginSubroutine("renderLineLeft1");

        if (debugRender) {
            outputReg("Clipping first point to the left");
            outputReg("x = ", dX1, "denaryTC");
            outputReg("y = ", dY1, "denaryTC");
            outputReg("z = ", dZ1, "denaryTC");
        }

        // t = (-dX1 - dY1) / (dYDiff + dXDiff)
        int recTLeft1 = startRecording();
        copyTC(dX1, tTop);
        neg(tTop);
        subtract(tTop, dY1);
        copyTC(dYDiff, tBottom);
        add(tBottom, dXDiff);
        //outputReg("tTop = ", tTop, "denaryTC");
        //outputReg("tBottom = ", tBottom, "denaryTC");
        shiftLeftTC(tTop, clipShift);
        stopRecording(recTLeft1);
        divideTC(tTop, tBottom, tLeft1, tRem);
        uncomputeRecording(recTLeft1);
        //outputReg("t * 32 = ", tLeft1, "denaryTC");

        // dX1 <- dX1 + t*dXDiff
        int recXLeft1 = startRecording();
        multiply(dXDiff, tLeft1, tAdd);
        stopRecording(recXLeft1);
        //outputReg("tAdd X = ", tAdd, "denaryTC");
        //outputReg("tSub X = ", tSub, "denaryTC");
        addTC(dX1, tSub);
        uncomputeRecording(recXLeft1);
        //outputReg("uncomuted tAdd X = ", tAdd, "denaryTC");

        // dY1 <- -dX1
        swap(dY1, tSwapLeft1);
        copyTC(dX1, dY1);
        neg(dY1);

        // dZ1 <- dZ1 + t*dZDiff
        int recZLeft1 = startRecording();
        multiply(dZDiff, tLeft1, tAdd);
        stopRecording(recZLeft1);
        //outputReg("tAdd Z = ", tAdd, "denaryTC");
        //outputReg("tSub Z = ", tSub, "denaryTC");
        addTC(dZ1, tSub);
        uncomputeRecording(recZLeft1);
        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");

        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");
        //outputReg("uncomuted tTop = ", tTop, "denaryTC");
        //outputReg("uncomuted tBottom = ", tBottom, "denaryTC");
        //outputReg("new x = ", dX2, "denaryTC");
        //outputReg("new y = ", dY2, "denaryTC");
        //outputReg("new z = ", dZ2, "denaryTC");
        //outputReg();

    endSubroutine();

    // If need clipping and second point is out the top plane, clip
    beginSubroutine("renderLineTop2");

        if (debugRender) {
            outputReg("Clipping second point to the top");
            outputReg("x = ", dX2, "denaryTC");
            outputReg("y = ", dY2, "denaryTC");
            outputReg("z = ", dZ2, "denaryTC");
            outputReg("z scaled = ", dZ2Scaled, "denaryTC");
        }

        // t = (1.6 dZ2 + dY2) / (1.6 dZDiff + dYDiff)
        int recTTop2 = startRecording();
        copyTC(dZ2Scaled, tTop);
        add(tTop, dY2);
        copyTC(dZDiffScaled, tBottom);
        add(tBottom, dYDiff);
        //outputReg("tTop = ", tTop, "denaryTC");
        //outputReg("tBottom = ", tBottom, "denaryTC");
        shiftLeftTC(tTop, clipShift);
        stopRecording(recTTop2);
        divideTC(tTop, tBottom, tTopMain2, tRem);
        uncomputeRecording(recTTop2);
        //outputReg("t * 32 = ", tTopMain2, "denaryTC");

        // dX2 <- dX2 - t*dXDiff
        int recXTop2 = startRecording();
        multiply(dXDiff, tTopMain2, tAdd);
        stopRecording(recXTop2);
        //outputReg("tAdd X = ", tAdd, "denaryTC");
        //outputReg("tSub X = ", tSub, "denaryTC");
        subtractTC(dX2, tSub);
        uncomputeRecording(recXTop2);
        //outputReg("uncomuted tAdd X = ", tAdd, "denaryTC");

        // 1.6 dZ2 <- 1.6 dZ2 - t * 1,6 dZDiff
        int recZTop2 = startRecording();
        multiply(dZDiffScaled, tTopMain2, tAdd);
        stopRecording(recZTop2);
        //outputReg("tAdd Z scaled = ", tAdd, "denaryTC");
        //outputReg("tSub Z scaled = ", tSub, "denaryTC");
        subtractTC(dZ2Scaled, tSub);
        uncomputeRecording(recZTop2);
        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");

        // dZ2 <- dZ2 - t*dZDiff
        int recZSTop2 = startRecording();
        multiply(dZDiff, tTopMain2, tAdd);
        stopRecording(recZSTop2);
        //outputReg("tAdd Z = ", tAdd, "denaryTC");
        //outputReg("tSub Z = ", tSub, "denaryTC");
        subtractTC(dZ2, tSub);
        uncomputeRecording(recZSTop2);
        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");

        // dY2 <- -1.6 dZ2
        swap(dY2, tSwapTop2);
        copyTC(dZ2Scaled, dY2);
        neg(dY2);

        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");
        //outputReg("uncomuted tTop = ", tTop, "denaryTC");
        //outputReg("uncomuted tBottom = ", tBottom, "denaryTC");
        //outputReg("new x = ", dX2, "denaryTC");
        //outputReg("new y = ", dY2, "denaryTC");
        //outputReg("new z = ", dZ2, "denaryTC");
        //outputReg("new z scaled = ", dZ2Scaled, "denaryTC");
        //outputReg();

    endSubroutine();

    // If need clipping and first point is out the top plane, clip
    beginSubroutine("renderLineTop1");

        if (debugRender) {
            outputReg("Clipping first point to the top");
            outputReg("x = ", dX1, "denaryTC");
            outputReg("y = ", dY1, "denaryTC");
            outputReg("z = ", dZ1, "denaryTC");
            outputReg("z scaled = ", dZ1Scaled, "denaryTC");
        }

        // t = (1.6 dZ1 + dY1) / (-1.6 dZDiff - dYDiff)
        int recTTop1 = startRecording();
        copyTC(dZ1Scaled, tTop);
        add(tTop, dY1);
        copyTC(dZDiffScaled, tBottom);
        add(tBottom, dYDiff);
        neg(tBottom);
        //outputReg("tTop = ", tTop, "denaryTC");
        //outputReg("tBottom = ", tBottom, "denaryTC");
        shiftLeftTC(tTop, clipShift);
        stopRecording(recTTop1);
        divideTC(tTop, tBottom, tTopMain1, tRem);
        uncomputeRecording(recTTop1);
        //outputReg("t * 32 = ", tTopMain1, "denaryTC");

        // dX1 <- dX1 + t*dXDiff
        int recXTop1 = startRecording();
        multiply(dXDiff, tTopMain1, tAdd);
        stopRecording(recXTop1);
        //outputReg("tAdd X = ", tAdd, "denaryTC");
        //outputReg("tSub X = ", tSub, "denaryTC");
        addTC(dX1, tSub);
        uncomputeRecording(recXTop1);
        //outputReg("uncomuted tAdd X = ", tAdd, "denaryTC");

        // 1.6 dZ1 <- 1.6 dZ1 + t * 1,6 dZDiff
        int recZTop1 = startRecording();
        multiply(dZDiffScaled, tTopMain1, tAdd);
        stopRecording(recZTop1);
        //outputReg("tAdd Z scaled = ", tAdd, "denaryTC");
        //outputReg("tSub Z scaled = ", tSub, "denaryTC");
        addTC(dZ1Scaled, tSub);
        uncomputeRecording(recZTop1);
        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");

        // dZ1 <- dZ1 + t*dZDiff
        int recZSTop1 = startRecording();
        multiply(dZDiff, tTopMain1, tAdd);
        stopRecording(recZSTop1);
        //outputReg("tAdd Z = ", tAdd, "denaryTC");
        //outputReg("tSub Z = ", tSub, "denaryTC");
        addTC(dZ1, tSub);
        uncomputeRecording(recZSTop1);
        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");

        // dY1 <- -1.6 dZ1
        swap(dY1, tSwapTop1);
        copyTC(dZ1Scaled, dY1);
        neg(dY1);

        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");
        //outputReg("uncomuted tTop = ", tTop, "denaryTC");
        //outputReg("uncomuted tBottom = ", tBottom, "denaryTC");
        //outputReg("new x = ", dX1, "denaryTC");
        //outputReg("new y = ", dY1, "denaryTC");
        //outputReg("new z = ", dZ1, "denaryTC");
        //outputReg("new z scaled = ", dZ1Scaled, "denaryTC");
        //outputReg();

    endSubroutine();

    // If need clipping and second point is out the bottom plane, clip
    beginSubroutine("renderLineBottom2");

        if (debugRender) {
            outputReg("Clipping second point to the bottom");
            outputReg("x = ", dX2, "denaryTC");
            outputReg("y = ", dY2, "denaryTC");
            outputReg("z = ", dZ2, "denaryTC");
            outputReg("z scaled = ", dZ2Scaled, "denaryTC");
        }

        // t = (dY2 - 1.6 dZ2) / (dYDiff - 1.6 dZDiff)
        int recTBottom2 = startRecording();
        copyTC(dY2, tTop);
        subtract(tTop, dZ2Scaled);
        copyTC(dYDiff, tBottom);
        subtract(tBottom, dZDiffScaled);
        //outputReg("tTop = ", tTop, "denaryTC");
        //outputReg("tBottom = ", tBottom, "denaryTC");
        shiftLeftTC(tTop, clipShift);
        stopRecording(recTBottom2);
        divideTC(tTop, tBottom, tBottomMain2, tRem);
        uncomputeRecording(recTBottom2);
        //outputReg("t * 32 = ", tBottomMain2, "denaryTC");

        // dX2 <- dX2 - t*dXDiff
        int recXBottom2 = startRecording();
        multiply(dXDiff, tBottomMain2, tAdd);
        stopRecording(recXBottom2);
        //outputReg("tAdd X = ", tAdd, "denaryTC");
        //outputReg("tSub X = ", tSub, "denaryTC");
        subtractTC(dX2, tSub);
        uncomputeRecording(recXBottom2);
        //outputReg("uncomuted tAdd X = ", tAdd, "denaryTC");

        // 1.6 dZ2 <- 1.6 dZ2 - t * 1,6 dZDiff
        int recZSBottom2 = startRecording();
        multiply(dZDiffScaled, tBottomMain2, tAdd);
        stopRecording(recZSBottom2);
        //outputReg("tAdd Z scaled = ", tAdd, "denaryTC");
        //outputReg("tSub Z scaled = ", tSub, "denaryTC");
        subtractTC(dZ2Scaled, tSub);
        uncomputeRecording(recZSBottom2);
        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");

        // dZ2 <- dZ2 - t*dZDiff
        int recZBottom2 = startRecording();
        multiply(dZDiff, tBottomMain2, tAdd);
        stopRecording(recZBottom2);
        //outputReg("tAdd Z = ", tAdd, "denaryTC");
        //outputReg("tSub Z = ", tSub, "denaryTC");
        subtractTC(dZ2, tSub);
        uncomputeRecording(recZBottom2);
        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");

        // dY2 <- 1.6 dZ2
        swap(dY2, tSwapBottom2);
        copyTC(dZ2Scaled, dY2);

        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");
        //outputReg("uncomuted tTop = ", tTop, "denaryTC");
        //outputReg("uncomuted tBottom = ", tBottom, "denaryTC");
        //outputReg("new x = ", dX2, "denaryTC");
        //outputReg("new y = ", dY2, "denaryTC");
        //outputReg("new z = ", dZ2, "denaryTC");
        //outputReg("new z scaled = ", dZ2Scaled, "denaryTC");
        //outputReg();

    endSubroutine();

    // If need clipping and first point is out the bottom plane, clip
    beginSubroutine("renderLineBottom1");

        if (debugRender) {
            outputReg("Clipping first point to the bottom");
            outputReg("x = ", dX1, "denaryTC");
            outputReg("y = ", dY1, "denaryTC");
            outputReg("z = ", dZ1, "denaryTC");
            outputReg("z scaled = ", dZ1Scaled, "denaryTC");
        }

        // t = (dY1 - 1.6 dZ1) / (1.6 dZDiff - dYDiff)
        int recTBottom1 = startRecording();
        copyTC(dY1, tTop);
        subtract(tTop, dZ1Scaled);
        copyTC(dZDiffScaled, tBottom);
        subtract(tBottom, dYDiff);
        //outputReg("tTop = ", tTop, "denaryTC");
        //outputReg("tBottom = ", tBottom, "denaryTC");
        shiftLeftTC(tTop, clipShift);
        stopRecording(recTBottom1);
        divideTC(tTop, tBottom, tBottomMain1, tRem);
        uncomputeRecording(recTBottom1);
        //outputReg("t * 32 = ", tBottomMain1, "denaryTC");

        // dX1 <- dX1 + t*dXDiff
        int recXBottom1 = startRecording();
        multiply(dXDiff, tBottomMain1, tAdd);
        stopRecording(recXBottom1);
        //outputReg("tAdd X = ", tAdd, "denaryTC");
        //outputReg("tSub X = ", tSub, "denaryTC");
        addTC(dX1, tSub);
        uncomputeRecording(recXBottom1);
        //outputReg("uncomuted tAdd X = ", tAdd, "denaryTC");

        // 1.6 dZ1 <- 1.6 dZ1 + t * 1,6 dZDiff
        int recZSBottom1 = startRecording();
        multiply(dZDiffScaled, tBottomMain1, tAdd);
        stopRecording(recZSBottom1);
        //outputReg("tAdd Z scaled = ", tAdd, "denaryTC");
        //outputReg("tSub Z scaled = ", tSub, "denaryTC");
        addTC(dZ1Scaled, tSub);
        uncomputeRecording(recZSBottom1);
        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");

        // dZ1 <- dZ1 + t*dZDiff
        int recZBottom1 = startRecording();
        multiply(dZDiff, tBottomMain1, tAdd);
        stopRecording(recZBottom1);
        //outputReg("tAdd Z = ", tAdd, "denaryTC");
        //outputReg("tSub Z = ", tSub, "denaryTC");
        addTC(dZ1, tSub);
        uncomputeRecording(recZBottom1);
        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");

        // dY1 <- 1.6 dZ1
        swap(dY1, tSwapBottom1);
        copyTC(dZ1Scaled, dY1);

        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");
        //outputReg("uncomuted tTop = ", tTop, "denaryTC");
        //outputReg("uncomuted tBottom = ", tBottom, "denaryTC");
        //outputReg("new x = ", dX1, "denaryTC");
        //outputReg("new y = ", dY1, "denaryTC");
        //outputReg("new z = ", dZ1, "denaryTC");
        //outputReg("new z scaled = ", dZ1Scaled, "denaryTC");
        //outputReg();

    endSubroutine();

    // If need clipping and second point is out the near plane, clip
    beginSubroutine("renderLineNear2");

        if (debugRender) {
            outputReg("Clipping second point to the near");
            outputReg("x = ", dX2, "denaryTC");
            outputReg("y = ", dY2, "denaryTC");
            outputReg("z = ", dZ2, "denaryTC");
        }

        // t = (dY2 - nearDist) / dYDiff
        int recTNear2 = startRecording();
        copyTC(dY2, tTop);
        subtract(tTop, nearDist);
        copyTC(dYDiff, tBottom);
        //outputReg("tTop = ", tTop, "denaryTC");
        //outputReg("tBottom = ", tBottom, "denaryTC");
        shiftLeftTC(tTop, clipShift);
        stopRecording(recTNear2);
        divideTC(tTop, tBottom, tNear2, tRem);
        uncomputeRecording(recTNear2);
        //outputReg("t * 32 = ", tNear2, "denaryTC");

        // dX2 <- dX2 - t*dXDiff
        int recXNear2 = startRecording();
        multiply(dXDiff, tNear2, tAdd);
        stopRecording(recXNear2);
        //outputReg("tAdd X = ", tAdd, "denaryTC");
        //outputReg("tSub X = ", tSub, "denaryTC");
        subtractTC(dX2, tSub);
        uncomputeRecording(recXNear2);
        //outputReg("uncomuted tAdd X = ", tAdd, "denaryTC");

        // dZ2 <- dZ2 - t*dZDiff
        int recZNear2 = startRecording();
        multiply(dZDiff, tNear2, tAdd);
        stopRecording(recZNear2);
        //outputReg("tAdd Z = ", tAdd, "denaryTC");
        //outputReg("tSub Z = ", tSub, "denaryTC");
        subtractTC(dZ2, tSub);
        uncomputeRecording(recZNear2);

        // dY2 <- nearDist
        swap(dY2, tSwapNear2);
        setValTC(dY2, nearDist);

        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");
        //outputReg("uncomuted tTop = ", tTop, "denaryTC");
        //outputReg("uncomuted tBottom = ", tBottom, "denaryTC");
        //outputReg("new x = ", dX2, "denaryTC");
        //outputReg("new y = ", dY2, "denaryTC");
        //outputReg("new z = ", dZ2, "denaryTC");
        //outputReg();

    endSubroutine();

    // If need clipping and first point is out the near plane, clip
    beginSubroutine("renderLineNear1");

        if (debugRender) {
            outputReg("Clipping first point to the near");
            outputReg("x = ", dX1, "denaryTC");
            outputReg("y = ", dY1, "denaryTC");
            outputReg("z = ", dZ1, "denaryTC");
        }

        // t = (nearDist - dY1) / dYDiff
        int recTNear1 = startRecording();
        setValTC(tTop, nearDist);
        subtract(tTop, dY1);
        copyTC(dYDiff, tBottom);
        //outputReg("tTop = ", tTop, "denaryTC");
        //outputReg("tBottom = ", tBottom, "denaryTC");
        shiftLeftTC(tTop, clipShift);
        stopRecording(recTNear1);
        divideTC(tTop, tBottom, tNear1, tRem);
        uncomputeRecording(recTNear1);
        //outputReg("t * 32 = ", tNear1, "denaryTC");

        // dX1 <- dX1 + t*dXDiff
        int recXNear1 = startRecording();
        multiply(dXDiff, tNear1, tAdd);
        stopRecording(recXNear1);
        //outputReg("tAdd X = ", tAdd, "denaryTC");
        //outputReg("tSub X = ", tSub, "denaryTC");
        addTC(dX1, tSub);
        uncomputeRecording(recXNear1);
        //outputReg("uncomuted tAdd X = ", tAdd, "denaryTC");

        // dZ1 <- dZ1 + t*dZDiff
        int recZNear1 = startRecording();
        multiply(dZDiff, tNear1, tAdd);
        stopRecording(recZNear1);
        //outputReg("tAdd Z = ", tAdd, "denaryTC");
        //outputReg("tSub Z = ", tSub, "denaryTC");
        addTC(dZ1, tSub);
        uncomputeRecording(recZNear1);
        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");

        // dY1 <- nearDist
        swap(dY1, tSwapNear1);
        setValTC(dY1, nearDist);

        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");
        //outputReg("uncomuted tTop = ", tTop, "denaryTC");
        //outputReg("uncomuted tBottom = ", tBottom, "denaryTC");
        //outputReg("new x = ", dX1, "denaryTC");
        //outputReg("new y = ", dY1, "denaryTC");
        //outputReg("new z = ", dZ1, "denaryTC");
        //outputReg();

    endSubroutine();

    // If need clipping and second point is out the far plane, clip
    beginSubroutine("renderLineFar2");

        if (debugRender) {
            outputReg("Clipping second point to the far");
            outputReg("x = ", dX2, "denaryTC");
            outputReg("y = ", dY2, "denaryTC");
            outputReg("z = ", dZ2, "denaryTC");
        }

        // t = (dY2 - farDist) / dYDiff
        int recTFar2 = startRecording();
        copyTC(dY2, tTop);
        subtract(tTop, farDist);
        copyTC(dYDiff, tBottom);
        //outputReg("tTop = ", tTop, "denaryTC");
        //outputReg("tBottom = ", tBottom, "denaryTC");
        shiftLeftTC(tTop, clipShift);
        stopRecording(recTFar2);
        divideTC(tTop, tBottom, tFar2, tRem);
        uncomputeRecording(recTFar2);
        //outputReg("t * 32 = ", tFar2, "denaryTC");

        // dX2 <- dX2 - t*dXDiff
        int recXFar2 = startRecording();
        multiply(dXDiff, tFar2, tAdd);
        stopRecording(recXFar2);
        //outputReg("tAdd X = ", tAdd, "denaryTC");
        //outputReg("tSub X = ", tSub, "denaryTC");
        subtractTC(dX2, tSub);
        uncomputeRecording(recXFar2);
        //outputReg("uncomuted tAdd X = ", tAdd, "denaryTC");

        // dZ2 <- dZ2 - t*dZDiff
        int recZFar2 = startRecording();
        multiply(dZDiff, tFar2, tAdd);
        stopRecording(recZFar2);
        //outputReg("tAdd Z = ", tAdd, "denaryTC");
        //outputReg("tSub Z = ", tSub, "denaryTC");
        subtractTC(dZ2, tSub);
        uncomputeRecording(recZFar2);

        // dY2 <- farDist
        swap(dY2, tSwapFar2);
        setValTC(dY2, farDist);

        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");
        //outputReg("uncomuted tTop = ", tTop, "denaryTC");
        //outputReg("uncomuted tBottom = ", tBottom, "denaryTC");
        //outputReg("new x = ", dX2, "denaryTC");
        //outputReg("new y = ", dY2, "denaryTC");
        //outputReg("new z = ", dZ2, "denaryTC");
        //outputReg();

    endSubroutine();

    // If need clipping and first point is out the far plane, clip
    beginSubroutine("renderLineFar1");

        if (debugRender) {
            outputReg("Clipping first point to the far");
            outputReg("x = ", dX1, "denaryTC");
            outputReg("y = ", dY1, "denaryTC");
            outputReg("z = ", dZ1, "denaryTC");
        }

        // t = (farDist - dY1) / dYDiff
        int recTFar1 = startRecording();
        setValTC(tTop, farDist);
        subtract(tTop, dY1);
        copyTC(dYDiff, tBottom);
        //outputReg("tTop = ", tTop, "denaryTC");
        //outputReg("tBottom = ", tBottom, "denaryTC");
        shiftLeftTC(tTop, clipShift);
        stopRecording(recTFar1);
        divideTC(tTop, tBottom, tFar1, tRem);
        uncomputeRecording(recTFar1);
        //outputReg("t * 32 = ", tFar1, "denaryTC");

        // dX1 <- dX1 + t*dXDiff
        int recXFar1 = startRecording();
        multiply(dXDiff, tFar1, tAdd);
        stopRecording(recXFar1);
        //outputReg("tAdd X = ", tAdd, "denaryTC");
        //outputReg("tSub X = ", tSub, "denaryTC");
        addTC(dX1, tSub);
        uncomputeRecording(recXFar1);
        //outputReg("uncomuted tAdd X = ", tAdd, "denaryTC");

        // dZ1 <- dZ1 + t*dZDiff
        int recZFar1 = startRecording();
        multiply(dZDiff, tFar1, tAdd);
        stopRecording(recZFar1);
        //outputReg("tAdd Z = ", tAdd, "denaryTC");
        //outputReg("tSub Z = ", tSub, "denaryTC");
        addTC(dZ1, tSub);
        uncomputeRecording(recZFar1);
        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");

        // dY1 <- farDist
        swap(dY1, tSwapFar1);
        setValTC(dY1, farDist);

        //outputReg("uncomuted tAdd Z = ", tAdd, "denaryTC");
        //outputReg("uncomuted tTop = ", tTop, "denaryTC");
        //outputReg("uncomuted tBottom = ", tBottom, "denaryTC");
        //outputReg("new x = ", dX1, "denaryTC");
        //outputReg("new y = ", dY1, "denaryTC");
        //outputReg("new z = ", dZ1, "denaryTC");
        //outputReg();

    endSubroutine();

    // The main outer function
    beginSubroutine("renderLine");
    
        // Start the recording
        int rec = startRecording();

        // diff1 = a1 - c
        copyTC(xStart, diffX1);
        copyTC(yStart, diffY1);
        copyTC(zStart, diffZ1);
        subtract(diffX1, "playerX");
        subtract(diffY1, "playerY");
        subtract(diffZ1, "playerZ");

        // diff2 = a2 - c
        copyTC(xEnd, diffX2);
        copyTC(yEnd, diffY2);
        copyTC(zEnd, diffZ2);
        subtract(diffX2, "playerX");
        subtract(diffY2, "playerY");
        subtract(diffZ2, "playerZ");

        // dX = diffX1 * cos(theta) + diffY1 * sin(theta)
        multiply(diffX1, "cosTheta", diffXCos1);
        multiply(diffY1, "sinTheta", diffYSin1);
        copy(diffXCos1, dX1);
        add(dX1, diffYSin1);

        // dX2 = diffX2 * cos(theta) + diffY2 * sin(theta)
        multiply(diffX2, "cosTheta", diffXCos2);
        multiply(diffY2, "sinTheta", diffYSin2);
        copy(diffXCos2, dX2);
        add(dX2, diffYSin2);
        
        // dY1 = diffY1 * cos(theta) - diffX1 * sin(theta)
        multiply(diffY1, "cosTheta", diffYCos1);
        multiply(diffX1, "sinTheta", diffXSin1);
        copy(diffYCos1, dY1);
        subtract(dY1, diffXSin1);

        // dY2 = diffY2 * cos(theta) - diffX2 * sin(theta)
        multiply(diffY2, "cosTheta", diffYCos2);
        multiply(diffX2, "sinTheta", diffXSin2);
        copy(diffYCos2, dY2);
        subtract(dY2, diffXSin2);

        // dZ1 = diffZ1
        copy(diffZ1, dZ1);
        shiftLeftTC(dZ1, leftShift);
        neg(dZ1);

        // dZ2 = diffZ2
        copy(diffZ2, dZ2);
        shiftLeftTC(dZ2, leftShift);
        neg(dZ2);

        // dZ1 scaled = dZ1 * 1.6 ~= dZ1 * 51/32
        multiply(diffZ1, 51, dZ1Scaled);
        neg(dZ1Scaled);

        // dZ2 scaled = dZ2 * 1.6 ~= dZ2 * 51/32
        multiply(diffZ2, 51, dZ2Scaled);
        neg(dZ2Scaled);

        if (debugRender) {
            outputReg();
            outputReg("xStart = ", xStart, "denaryTC");
            outputReg("yStart = ", yStart, "denaryTC");
            outputReg("zStart = ", zStart, "denaryTC");
            outputReg("xEnd = ", xEnd, "denaryTC");
            outputReg("yEnd = ", yEnd, "denaryTC");
            outputReg("zEnd = ", zEnd, "denaryTC");
            outputReg("dX1 = ", dX1, "denaryTC");
            outputReg("dY1 = ", dY1, "denaryTC");
            outputReg("dZ1 = ", dZ1, "denaryTC");
            outputReg("dZ1 scaled = ", dZ1Scaled, "denaryTC");
            outputReg("dX2 = ", dX2, "denaryTC");
            outputReg("dY2 = ", dY2, "denaryTC");
            outputReg("dZ2 = ", dZ2, "denaryTC");
            outputReg("dZ2 scaled = ", dZ2Scaled, "denaryTC");
        }

        // Determine clipping of the first point in camera space
        flipIfGreaterOrEqualTC(outRight1, dX1, dY1);
        neg(dY1);
        flipIfLessOrEqualTC(outLeft1, dX1, dY1);
        flipIfLessOrEqualTC(outTop1, dZ1Scaled, dY1);
        neg(dY1);
        flipIfGreaterOrEqualTC(outBottom1, dZ1Scaled, dY1);
        flipIfLessOrEqualTC(outNear1, dY1, nearDist);
        flipIfGreaterOrEqualTC(outFar1, dY1, farDist);

        // Determine clipping of the second point in camera space
        flipIfGreaterOrEqualTC(outRight2, dX2, dY2);
        neg(dY2);
        flipIfLessOrEqualTC(outLeft2, dX2, dY2);
        flipIfLessOrEqualTC(outTop2, dZ2Scaled, dY2);
        neg(dY2);
        flipIfGreaterOrEqualTC(outBottom2, dZ2Scaled, dY2);
        flipIfLessOrEqualTC(outNear2, dY2, nearDist);
        flipIfGreaterOrEqualTC(outFar2, dY2, farDist);

        // If AND of both is not all zero, reject
        flipIfAnd(andLeft, {outLeft1, outLeft2});
        flipIfAnd(andRight, {outRight1, outRight2});
        flipIfAnd(andTop, {outTop1, outTop2});
        flipIfAnd(andBottom, {outBottom1, outBottom2});
        flipIfAnd(andNear, {outNear1, outNear2});
        flipIfAnd(andFar, {outFar1, outFar2});
        x({andLeft, andRight, andTop, andBottom, andNear, andFar});
        flipIfAnd(shouldDraw, {andLeft, andRight, andTop, andBottom, andNear, andFar});

        // If all are zero, accept, no clipping
        x({outLeft1, outRight1, outTop1, outBottom1, outNear1, outFar1, outLeft2, outRight2, outTop2, outBottom2, outNear2, outFar2});
        flipIfAnd(needClipping, {outLeft1, outRight1, outTop1, outBottom1, outNear1, outFar1, outLeft2, outRight2, outTop2, outBottom2, outNear2, outFar2});
        x({outLeft1, outRight1, outTop1, outBottom1, outNear1, outFar1, outLeft2, outRight2, outTop2, outBottom2, outNear2, outFar2});
        x(needClipping);

        //outputReg();
        //outputReg("initial checks");
        //outputReg("outLeft1 = ", outLeft1, "binary");
        //outputReg("outRight1 = ", outRight1, "binary");
        //outputReg("outTop1 = ", outTop1, "binary");
        //outputReg("outBottom1 = ", outBottom1, "binary");
        //outputReg("outNear1 = ", outNear1, "binary");
        //outputReg("outFar1 = ", outFar1, "binary");
        //outputReg("outLeft2 = ", outLeft2, "binary");
        //outputReg("outRight2 = ", outRight2, "binary");
        //outputReg("outTop2 = ", outTop2, "binary");
        //outputReg("outBottom2 = ", outBottom2, "binary");
        //outputReg("outNear2 = ", outNear2, "binary");
        //outputReg("outFar2 = ", outFar2, "binary");
        //outputReg();
        //outputReg("needClipping = ", needClipping, "binary");
        //outputReg("shouldDraw = ", shouldDraw, "binary");
        //outputReg();
            
        // If need clipping, calculate the gradient of the line and then clip
        callSubroutineIfAnd("renderLineShared", {shouldDraw, needClipping});

        // Check, clip, check clip etc. 

        neg(dY1);
        neg(dY2);
        flipIfLessOrEqualTC(out2Top1, dZ1Scaled, dY1);
        flipIfLessOrEqualTC(out2Top2, dZ2Scaled, dY2);
        neg(dY1);
        neg(dY2);
        callSubroutineIfAnd("renderLineTop1", {shouldDraw, needClipping, out2Top1});
        callSubroutineIfAnd("renderLineTop2", {shouldDraw, needClipping, out2Top2});

        flipIfGreaterOrEqualTC(out2Bottom1, dZ1Scaled, dY1);
        flipIfGreaterOrEqualTC(out2Bottom2, dZ2Scaled, dY2);
        callSubroutineIfAnd("renderLineBottom1", {shouldDraw, needClipping, out2Bottom1});
        callSubroutineIfAnd("renderLineBottom2", {shouldDraw, needClipping, out2Bottom2});

        flipIfLessOrEqualTC(out2Near1, dY1, nearDist);
        flipIfLessOrEqualTC(out2Near2, dY2, nearDist);
        callSubroutineIfAnd("renderLineNear1", {shouldDraw, needClipping, out2Near1});
        callSubroutineIfAnd("renderLineNear2", {shouldDraw, needClipping, out2Near2});

        flipIfGreaterOrEqualTC(out2Far1, dY1, farDist);
        flipIfGreaterOrEqualTC(out2Far2, dY2, farDist);
        callSubroutineIfAnd("renderLineFar1", {shouldDraw, needClipping, outFar1});
        callSubroutineIfAnd("renderLineFar2", {shouldDraw, needClipping, outFar2});

        flipIfGreaterOrEqualTC(out2Right1, dX1, dY1);
        flipIfGreaterOrEqualTC(out2Right2, dX2, dY2);
        callSubroutineIfAnd("renderLineRight1", {shouldDraw, needClipping, out2Right1});
        callSubroutineIfAnd("renderLineRight2", {shouldDraw, needClipping, out2Right2});

        neg(dY1);
        neg(dY2);
        flipIfLessOrEqualTC(out2Left1, dX1, dY1);
        flipIfLessOrEqualTC(out2Left2, dX2, dY2);
        neg(dY1);
        neg(dY2);
        callSubroutineIfAnd("renderLineLeft1", {shouldDraw, needClipping, out2Left1});
        callSubroutineIfAnd("renderLineLeft2", {shouldDraw, needClipping, out2Left2});

        //outputReg("secondary checks");
        //outputReg("out2Right1 = ", out2Right1, "binary");
        //outputReg("out2Right2 = ", out2Right2, "binary");
        //outputReg("out2Left1 = ", out2Left1, "binary");
        //outputReg("out2Left2 = ", out2Left2, "binary");
        //outputReg("out2Top1 = ", out2Top1, "binary");
        //outputReg("out2Top2 = ", out2Top2, "binary");
        //outputReg("out2Bottom1 = ", out2Bottom1, "binary");
        //outputReg("out2Bottom2 = ", out2Bottom2, "binary");

        // Render the point if we should
        callSubroutineIf("renderLineInside", shouldDraw);

        // Draw the line
        stopRecording(rec);
        drawLineIfValid(shouldDraw, 0, screenWidth+10, 0, screenHeight+10, "lineXScreenStart", "lineYScreenStart", "lineXScreenEnd", "lineYScreenEnd");

        // Uncompute everything, including the called functions
        uncomputeRecording(rec);

    endSubroutine();

    // Free ancillas for clipping
    ancillaFree(outLeft1);
    ancillaFree(outRight1);
    ancillaFree(outTop1);
    ancillaFree(outBottom1);
    ancillaFree(outFar1);
    ancillaFree(outNear1);
    ancillaFree(outLeft2);
    ancillaFree(outRight2);
    ancillaFree(outTop2);
    ancillaFree(outBottom2);
    ancillaFree(outFar2);
    ancillaFree(outNear2);
    ancillaFree(out2Left1);
    ancillaFree(out2Right1);
    ancillaFree(out2Top1);
    ancillaFree(out2Bottom1);
    ancillaFree(out2Far1);
    ancillaFree(out2Near1);
    ancillaFree(out2Left2);
    ancillaFree(out2Right2);
    ancillaFree(out2Top2);
    ancillaFree(out2Bottom2);
    ancillaFree(out2Far2);
    ancillaFree(out2Near2);
    ancillaFree(needClipping);
    ancillaFree(shouldDraw);
    ancillaFree(andLeft);
    ancillaFree(andRight);
    ancillaFree(andTop);
    ancillaFree(andBottom);
    ancillaFree(andNear);
    ancillaFree(andFar);
    ancillaFree(tTop);
    ancillaFree(tBottom);
    ancillaFree(tRem);
    ancillaFree(tAdd);
    ancillaFree(dXDiff);
    ancillaFree(dYDiff);
    ancillaFree(dZDiff);
    ancillaFree(dZDiffScaled);
    ancillaFree(tRight1);
    ancillaFree(tRight2);
    ancillaFree(tLeft1);
    ancillaFree(tLeft2);
    ancillaFree(tTopMain1);
    ancillaFree(tTopMain2);
    ancillaFree(tBottomMain1);
    ancillaFree(tBottomMain2);
    ancillaFree(tNear1);
    ancillaFree(tNear2);
    ancillaFree(tFar1);
    ancillaFree(tFar2);
    ancillaFree(tSwapRight1);
    ancillaFree(tSwapRight2);
    ancillaFree(tSwapLeft1);
    ancillaFree(tSwapLeft2);
    ancillaFree(tSwapTop1);
    ancillaFree(tSwapTop2);
    ancillaFree(tSwapBottom1);
    ancillaFree(tSwapBottom2);
    ancillaFree(tSwapNear1);
    ancillaFree(tSwapNear2);
    ancillaFree(tSwapFar1);
    ancillaFree(tSwapFar2);

    // Free ancillas for the first coord
    ancillaFree(diffX1);
    ancillaFree(diffY1);
    ancillaFree(diffZ1);
    ancillaFree(dX1);
    ancillaFree(dY1);
    ancillaFree(dZ1);
    ancillaFree(dZ1Scaled);
    ancillaFree(diffXCos1);
    ancillaFree(diffXSin1);
    ancillaFree(diffYCos1);
    ancillaFree(diffYSin1);
    ancillaFree(eYdX1);
    ancillaFree(eYdZ1);
    ancillaFree(bX1);
    ancillaFree(bY1);
    ancillaFree(rem11);
    ancillaFree(rem21);

    // Free ancillas for the second coord
    ancillaFree(diffX2);
    ancillaFree(diffY2);
    ancillaFree(diffZ2);
    ancillaFree(dX2);
    ancillaFree(dY2);
    ancillaFree(dZ2);
    ancillaFree(dZ2Scaled);
    ancillaFree(diffXCos2);
    ancillaFree(diffXSin2);
    ancillaFree(diffYCos2);
    ancillaFree(diffYSin2);
    ancillaFree(eYdX2);
    ancillaFree(eYdZ2);
    ancillaFree(bX2);
    ancillaFree(bY2);
    ancillaFree(rem12);
    ancillaFree(rem22);

    // See how many peak ancillas we used during the function
    int ancillasUsedInFunc = stopPeakAncilla();
    std::cout << "Peak ancillas used in render line: " << ancillasUsedInFunc << std::endl;
    ancillaLock(ancillasUsedInFunc, "ancillaFunctionRenderLine");

}

// Function to render a sprite
void functionRenderSprite(std::string spriteName, std::string xCoord, std::string yCoord, std::string zCoord) {

    // Need draw pixel if not already defined
    functionDrawPixel("pixelToDrawX", "pixelToDrawY", "Main");

    // Define all the ancillas we need
    std::string diffX = ancillaLock(qubitsPer, "ancillaDiffX");
    std::string diffY = ancillaLock(qubitsPer, "ancillaDiffY");
    std::string diffZ = ancillaLock(qubitsPer, "ancillaDiffZ");
    std::string dX = ancillaLock(qubitsPer, "ancillaDX");
    std::string dY = ancillaLock(qubitsPer, "ancillaDY");
    std::string dZ = ancillaLock(qubitsPer, "ancillaDZ");
    std::string dZ2 = ancillaLock(qubitsPer, "ancillaDZ2");
    std::string diffXCos = ancillaLock(qubitsPer, "ancillaDiffXCos");
    std::string diffXSin = ancillaLock(qubitsPer, "ancillaDiffXSin");
    std::string diffYCos = ancillaLock(qubitsPer, "ancillaDiffYCos");
    std::string diffYSin = ancillaLock(qubitsPer, "ancillaDiffYSin");
    std::string eYdX = ancillaLock(qubitsPer, "ancillaEYdX");
    std::string eYdZ = ancillaLock(qubitsPer, "ancillaEYdZ");
    std::string eYdZ2 = ancillaLock(qubitsPer, "ancillaEYdZ2");
    std::string bX = ancillaLock(qubitsPer, "ancillaBX");
    std::string bY = ancillaLock(qubitsPer, "ancillaBY");
    std::string bY2 = ancillaLock(qubitsPer, "ancillaBY2");
    std::string rem1 = ancillaLock(qubitsPer, "ancillaRem1");
    std::string rem2 = ancillaLock(qubitsPer, "ancillaRem2");
    std::string rem3 = ancillaLock(qubitsPer, "ancillaRem3");
    std::string spriteSize = ancillaLock(qubitsPer, "ancillaSpriteSize");
    std::string shouldDraw = ancillaLock(6, "ancillaShouldDraw");

    // The functions for drawing each sprite
    for (int i = 1; i <= maxSpriteHeight; i++) {

        // Check that the sprite exists
        std::string spriteNamePlusSize = spriteName + "-" + std::to_string(i);
        if (spriteList.find(spriteNamePlusSize) == spriteList.end()) {
            std::cout << "Sprite " << spriteNamePlusSize << " not found" << std::endl;
        }
        int halfSize = i/2;

        // The inner function for drawing the sprite
        std::cout << "Creating function for sprite " << spriteNamePlusSize << "         \r" << std::flush;
        beginSubroutine("drawSprite" + spriteName + "-" + std::to_string(i));
            copy(bX, "pixelToDrawX");
            copy(bY2, "pixelToDrawY");
            subtract("pixelToDrawX", halfSize);
            int lastX = 0;
            int lastY = 0;
            for (int j=0; j<int(spriteList[spriteNamePlusSize].size()); j++) {
                for (int k=0; k<int(spriteList[spriteNamePlusSize][j].size()); k++) {
                    if (spriteList[spriteNamePlusSize][k][j]) {
                        addTC("pixelToDrawX", k-lastX);
                        addTC("pixelToDrawY", j-lastY);
                        drawPixel("Main");
                        lastX = k;
                        lastY = j;
                    }
                }
            }
            subtract("pixelToDrawY", lastY);
            addTC("pixelToDrawX", halfSize-lastX);
            copy(bX, "pixelToDrawX");
            copy(bY2, "pixelToDrawY");
        endSubroutine();
    }
    std::cout << std::endl;

    // The main outer function
    beginSubroutine("renderSprite" + spriteName + xCoord + yCoord + zCoord);

        // Start recording 
        int rec = startRecording();

        // diff = a - c
        copyTC(xCoord, diffX);
        copyTC(yCoord, diffY);
        copyTC(zCoord, diffZ);
        subtract(diffX, "playerX");
        subtract(diffY, "playerY");
        subtract(diffZ, "playerZ");

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

        // dz = diffZ 
        copy(diffZ, dZ);
        shiftLeft(dZ, leftShift);
        neg(dZ);

        // dz2 = diffZ + something
        copy(dZ, dZ2);
        subtract(dZ2, spriteWorldHeight*std::pow(2, leftShift));

        // bX = (eY*dX) / dY + eX
        multiply(dX, eY, eYdX);
        divideTC(eYdX, dY, bX, rem1);
        add(bX, eX);

        // bY = (eY*dZ) / dY + eZ
        multiply(dZ, eY, eYdZ);
        divideTC(eYdZ, dY, bY, rem2);
        add(bY, eZ);

        // bY2 = (eY*dZ2) / dY + eZ
        multiply(dZ2, eY, eYdZ2);
        divideTC(eYdZ2, dY, bY2, rem3);
        add(bY2, eZ);

        // Check that the coords are valid
        std::string dYPositive = subreg(shouldDraw, 0, 1);
        std::string bXNotTooBig = subreg(shouldDraw, 1, 2);
        std::string bYNotTooBig = subreg(shouldDraw, 2, 3);
        std::string bXNotTooSmall = subreg(shouldDraw, 3, 4);
        std::string bYNotTooSmall = subreg(shouldDraw, 4, 5);
        std::string inYRange = subreg(shouldDraw, 5, 6);
        flipIfGreaterOrEqualTC(dYPositive, dY, 1);
        flipIfLessOrEqualTC(bXNotTooBig, bX, screenWidth-10);
        //flipIfLessOrEqualTC(bYNotTooBig, bY, screenHeight+10);
        x(bYNotTooBig);
        flipIfGreaterOrEqualTC(bXNotTooSmall, bX, 10);
        flipIfGreaterOrEqualTC(bYNotTooSmall, bY, 10);
        copyTC(bY, spriteSize);
        subtract(spriteSize, bY2);
        stopRecording(rec);

        // Draw the correct sprite based on dY
        //outputReg("spriteName = " + spriteName);
        //outputReg("dY = ", dY, "denaryTC");
        //outputReg("bX = ", bX, "denaryTC");
        //outputReg("bY = ", bY, "denaryTC");
        //outputReg("bY2 = ", bY2, "denaryTC");
        //outputReg("spriteSize = ", spriteSize, "denaryTC");
        //outputReg("shouldDraw = ", shouldDraw, "binary");
        for (int i = 1; i <= maxSpriteHeight; i++) {
            flipIfEqual(inYRange, spriteSize, i);
            callSubroutineIf("drawSprite"+spriteName+"-"+std::to_string(i), shouldDraw);
            flipIfEqual(inYRange, spriteSize, i);
        }

        // Uncompute everything
        uncomputeRecording(rec);

    endSubroutine();

    // Free all of our ancillas
    ancillaFree(diffX);
    ancillaFree(diffY);
    ancillaFree(diffZ);
    ancillaFree(dX);
    ancillaFree(dY);
    ancillaFree(dZ);
    ancillaFree(dZ2);
    ancillaFree(diffXCos);
    ancillaFree(diffXSin);
    ancillaFree(diffYCos);
    ancillaFree(diffYSin);
    ancillaFree(eYdX);
    ancillaFree(eYdZ);
    ancillaFree(eYdZ2);
    ancillaFree(spriteSize);
    ancillaFree(bX);
    ancillaFree(bY);
    ancillaFree(bY2);
    ancillaFree(rem1);
    ancillaFree(rem2);
    ancillaFree(rem3);
    ancillaFree(shouldDraw);

}

// Given a sprite name and world location, draw it (just flat with scaling)
void renderSprite(std::string spriteName, std::string xCoord, std::string yCoord, std::string zCoord) {
    callSubroutine("renderSprite"+spriteName+xCoord+yCoord+zCoord);
}

// Given a sprite name and world location, draw it (just flat with scaling)
void renderSpriteIf(std::string spriteName, std::string xCoord, std::string yCoord, std::string zCoord, std::string regToCheck) {
    callSubroutineIf("renderSprite"+spriteName+xCoord+yCoord+zCoord, regToCheck);
}

// Create all the functions to render an enemy
void functionRenderEnemy(std::string spriteName, std::string xCoord, std::string yCoord, std::string zCoord, std::vector<int> limits) {

    // Start recording the peak ancilla usage
    startPeakAncilla();

    // For each non-zero sprite, create a function to draw it
    for (int i=limits[0]; i<=limits[1]; i++) {
        for (int j=limits[2]; j<=limits[3]; j++) {
            std::string fullName = spriteName + "-" + std::to_string(i) + "-" + std::to_string(j);
            std::string fullName60 = fullName + "-60";
            if (spriteList.find(fullName60) != spriteList.end() && spriteList[fullName60].size() > 0) {
                bool hasOnes = false;
                for (int k=0; k<int(spriteList[fullName60].size()); k++) {
                    for (int l=0; l<int(spriteList[fullName60][k].size()); l++) {
                        if (spriteList[fullName60][k][l]) {
                            hasOnes = true;
                            break;
                        }
                    }
                    if (hasOnes) {
                        break;
                    }
                }
                if (hasOnes) {
                    functionRenderSprite(fullName, xCoord, yCoord, zCoord);
                }
            }
        }
    }

    // See how many peak ancillas we used during the function
    int ancillasUsedInFunc = stopPeakAncilla();
    std::cout << "Peak ancillas used in render sprite: " << ancillasUsedInFunc << std::endl;
    ancillaLock(ancillasUsedInFunc, "ancillaFunctionRenderSprite" + spriteName + xCoord + yCoord + zCoord);

}

// Given a enemy name and world location, draw it (just flat with scaling)
void renderEnemy(std::string spriteName, std::string xCoord, std::string yCoord, std::string zCoord, std::string action, std::string frame) {
    std::string ancAction = ancillaLock(1, "ancillaRenderAction");
    std::string ancFrame = ancillaLock(1, "ancillaRenderFrame");
    for (int i=0; i<=maxAction; i++) {
        for (int j=0; j<=maxFrame; j++) {
            std::string fullName = spriteName + "-" + std::to_string(j) + "-" + std::to_string(i);
            std::string subroutineName = "renderSprite"+fullName+xCoord+yCoord+zCoord;
            if (spriteList.find(fullName+"-60") != spriteList.end() && spriteList[fullName+"-60"].size() > 0 && subroutineNameToIndex.find(subroutineName) != subroutineNameToIndex.end()) {
                flipIfEqual(ancAction, action, i);
                flipIfEqual(ancFrame, frame, j);
                callSubroutineIfAnd(subroutineName, {ancAction, ancFrame});
                flipIfEqual(ancAction, action, i);
                flipIfEqual(ancFrame, frame, j);
            }
        }
    }
    ancillaFree(ancAction);
    ancillaFree(ancFrame);
}

// Function that renders a room
void functionRenderRoom(Room room) {

    // Start the function
    startPeakAncilla();
    beginSubroutine("renderRoom" + std::to_string(room.id));

        // For each core, split everything
        beginParallel();
        for (int core=0; core<numCores; core++) {

            // For each wall
            int startWall = core * (room.walls.size() / numCores);
            int endWall = (core+1) * (room.walls.size() / numCores);
            if (core == numCores-1) {
                endWall = room.walls.size();
            }
            for (int i=startWall; i<endWall; i++) {

                // If not visible, skip it
                if (!walls[room.walls[i]].visible) {
                    continue;
                }

                // Debug output
                if (debugRender) {
                    outputReg("Rendering wall " + std::to_string(room.walls[i]));
                }

                // If it's a door
                std::string type = walls[room.walls[i]].type;
                if (type == "door") {

                    // Same base locations
                    std::vector<int> cornerTopLeft = walls[room.walls[i]].startLoc;
                    std::vector<int> cornerBottomRight = walls[room.walls[i]].endLoc;
                    std::vector<int> cornerBottomLeft = {cornerTopLeft[0], cornerTopLeft[1], cornerBottomRight[2]};
                    std::vector<int> cornerTopRight = {cornerBottomRight[0], cornerBottomRight[1], cornerTopLeft[2]};
                    std::string doorRegOpen = "door" + std::to_string(room.walls[i]) + "Open";

                    // Top line is the same
                    renderLine(cornerTopLeft, cornerTopRight);

                    // Diagonal line
                    int recDiag1 = startRecording();
                    setValTC("lineXWorldStart", cornerTopLeft[0]);
                    setValTC("lineYWorldStart", cornerTopLeft[1]);
                    setValTC("lineZWorldStart", cornerTopLeft[2]);
                    setValTC("lineXWorldEnd", cornerBottomRight[0]);
                    setValTC("lineYWorldEnd", cornerBottomRight[1]);
                    setValTC("lineZWorldEnd", cornerBottomRight[2]);
                    add("lineZWorldEnd", doorRegOpen);
                    stopRecording(recDiag1);
                    callSubroutine("renderLine");
                    uncomputeRecording(recDiag1);

                    // Diagonal line
                    int recDiag2 = startRecording();
                    setValTC("lineXWorldStart", cornerTopRight[0]);
                    setValTC("lineYWorldStart", cornerTopRight[1]);
                    setValTC("lineZWorldStart", cornerTopRight[2]);
                    setValTC("lineXWorldEnd", cornerBottomLeft[0]);
                    setValTC("lineYWorldEnd", cornerBottomLeft[1]);
                    setValTC("lineZWorldEnd", cornerBottomLeft[2]);
                    add("lineZWorldEnd", doorRegOpen);
                    stopRecording(recDiag2);
                    callSubroutine("renderLine");
                    uncomputeRecording(recDiag2);

                    // Bottom and right line are adjusted by the open amount
                    int rec = startRecording();
                    setValTC("lineXWorldStart", cornerBottomLeft[0]);
                    setValTC("lineYWorldStart", cornerBottomLeft[1]);
                    setValTC("lineZWorldStart", cornerBottomLeft[2]);
                    setValTC("lineXWorldEnd", cornerBottomRight[0]);
                    setValTC("lineYWorldEnd", cornerBottomRight[1]);
                    setValTC("lineZWorldEnd", cornerBottomRight[2]);
                    add("lineZWorldStart", doorRegOpen);
                    add("lineZWorldEnd", doorRegOpen);
                    stopRecording(rec);
                    callSubroutine("renderLine");
                    uncomputeRecording(rec);

                // If it's a line
                } else if (type == "line") {
                    std::vector<int> corner1 = walls[room.walls[i]].startLoc;
                    std::vector<int> corner2 = walls[room.walls[i]].endLoc;
                    renderLine(corner1, corner2);

                // If it's a floor with three points (rotated floor)
                } else if (type == "floor" && walls[room.walls[i]].startLoc.size() > 3) {
                    std::vector<int> cornerBackRight = walls[room.walls[i]].endLoc;
                    std::vector<int> cornerFrontLeft = {walls[room.walls[i]].startLoc[0], walls[room.walls[i]].startLoc[1], walls[room.walls[i]].startLoc[2]};
                    std::vector<int> cornerFrontRight = {walls[room.walls[i]].startLoc[3], walls[room.walls[i]].startLoc[4], walls[room.walls[i]].startLoc[5]};
                    std::vector<int> cornerBackLeft(3, 0);
                    cornerBackLeft[0] = cornerFrontLeft[0] + (cornerBackRight[0] - cornerFrontRight[0]);
                    cornerBackLeft[1] = cornerBackRight[1] + (cornerFrontLeft[1] - cornerFrontRight[1]);
                    cornerBackLeft[2] = cornerFrontLeft[2];
                    //std::cout << "cornerFrontLeft = " << cornerFrontLeft[0] << " " << cornerFrontLeft[1] << " " << cornerFrontLeft[2] << std::endl;
                    //std::cout << "cornerFrontRight = " << cornerFrontRight[0] << " " << cornerFrontRight[1] << " " << cornerFrontRight[2] << std::endl;
                    //std::cout << "cornerBackRight = " << cornerBackRight[0] << " " << cornerBackRight[1] << " " << cornerBackRight[2] << std::endl;
                    //std::cout << "cornerBackLeft = " << cornerBackLeft[0] << " " << cornerBackLeft[1] << " " << cornerBackLeft[2] << std::endl;
                    renderLine(cornerFrontLeft, cornerFrontRight);
                    renderLine(cornerFrontRight, cornerBackRight);
                    renderLine(cornerBackRight, cornerBackLeft);
                    renderLine(cornerBackLeft, cornerFrontLeft);

                // If it's a floor
                } else if (type == "floor") {
                    std::vector<int> cornerFrontLeft = walls[room.walls[i]].startLoc;
                    std::vector<int> cornerBackRight = walls[room.walls[i]].endLoc;
                    std::vector<int> cornerBackLeft = {cornerFrontLeft[0], cornerBackRight[1], cornerBackRight[2]};
                    std::vector<int> cornerFrontRight = {cornerBackRight[0], cornerFrontLeft[1], cornerFrontLeft[2]};
                    renderLine(cornerFrontLeft, cornerFrontRight);
                    renderLine(cornerFrontRight, cornerBackRight);
                    renderLine(cornerBackRight, cornerBackLeft);
                    renderLine(cornerBackLeft, cornerFrontLeft);

                // If it's a button
                } else if (type == "button") {

                    std::vector<int> cornerTopLeft = walls[room.walls[i]].startLoc;
                    std::vector<int> cornerBottomRight = walls[room.walls[i]].endLoc;
                    std::vector<int> cornerTopRight = {cornerBottomRight[0], cornerBottomRight[1], cornerTopLeft[2]};
                    std::vector<int> cornerBottomLeft = {cornerTopLeft[0], cornerTopLeft[1], cornerBottomRight[2]};

                    double leftRatio = 0.2;
                    double rightRatio = 0.55;
                    double topRatio = 0.1;
                    double middleRatio = 0.43;
                    std::vector<int> cornerInterior1TopLeft = {roundInt((1-leftRatio)*cornerTopLeft[0] + leftRatio*cornerTopRight[0]), roundInt((1-leftRatio)*cornerTopLeft[1] + leftRatio*cornerTopRight[1]), roundInt((1-topRatio)*cornerTopLeft[2] + topRatio*cornerBottomRight[2])};
                    std::vector<int> cornerInterior1BottomRight = {roundInt((1-rightRatio)*cornerTopLeft[0] + rightRatio*cornerTopRight[0]), roundInt((1-rightRatio)*cornerTopLeft[1] + rightRatio*cornerTopRight[1]), roundInt((1-middleRatio)*cornerTopLeft[2] + middleRatio*cornerBottomRight[2])};
                    std::vector<int> cornerInterior1TopRight = {cornerInterior1BottomRight[0], cornerInterior1BottomRight[1], cornerInterior1TopLeft[2]};
                    std::vector<int> cornerInterior1BottomLeft = {cornerInterior1TopLeft[0], cornerInterior1TopLeft[1], cornerInterior1BottomRight[2]};
                    std::vector<int> cornerInterior2BottomRight = {cornerInterior1BottomRight[0], cornerInterior1BottomRight[1], roundInt(topRatio*cornerTopLeft[2] + (1-topRatio)*cornerBottomRight[2])};
                    std::vector<int> cornerInterior2TopLeft = {cornerInterior1TopLeft[0], cornerInterior1TopLeft[1], roundInt(middleRatio*cornerTopLeft[2] + (1-middleRatio)*cornerBottomRight[2])};
                    std::vector<int> cornerInterior2TopRight = {cornerInterior2BottomRight[0], cornerInterior2BottomRight[1], cornerInterior2TopLeft[2]};
                    std::vector<int> cornerInterior2BottomLeft = {cornerInterior2TopLeft[0], cornerInterior2TopLeft[1], cornerInterior2BottomRight[2]};

                    double lineRatio = 0.75;
                    double lineTopRatio = 0.20;
                    std::vector<int> lineTop = {roundInt((1-lineRatio)*cornerTopLeft[0] + lineRatio*cornerTopRight[0]), roundInt((1-lineRatio)*cornerTopLeft[1] + lineRatio*cornerTopRight[1]), roundInt((1-lineTopRatio)*cornerTopLeft[2] + lineTopRatio*cornerBottomRight[2])};
                    std::vector<int> lineBottom = {roundInt((1-lineRatio)*cornerTopLeft[0] + lineRatio*cornerTopRight[0]), roundInt((1-lineRatio)*cornerTopLeft[1] + lineRatio*cornerTopRight[1]), roundInt(lineTopRatio*cornerTopLeft[2] + (1-lineTopRatio)*cornerBottomRight[2])};

                    renderLine(cornerTopLeft, cornerTopRight);
                    renderLine(cornerTopRight, cornerBottomRight);
                    renderLine(cornerBottomRight, cornerBottomLeft);
                    renderLine(cornerBottomLeft, cornerTopLeft);
                    renderLine(cornerInterior1TopLeft, cornerInterior1TopRight);
                    renderLine(cornerInterior1TopRight, cornerInterior1BottomRight);
                    renderLine(cornerInterior1BottomRight, cornerInterior1BottomLeft);
                    renderLine(cornerInterior1BottomLeft, cornerInterior1TopLeft);
                    renderLine(cornerInterior2TopLeft, cornerInterior2TopRight);
                    renderLine(cornerInterior2TopRight, cornerInterior2BottomRight);
                    renderLine(cornerInterior2BottomRight, cornerInterior2BottomLeft);
                    renderLine(cornerInterior2BottomLeft, cornerInterior2TopLeft);
                    renderLine(lineTop, lineBottom);

                // 3-point wall
                } else if (walls[room.walls[i]].startLoc.size() > 3) {
                    std::vector<int> corner1 = walls[room.walls[i]].startLoc;
                    std::vector<int> corner2 = {walls[room.walls[i]].startLoc[3], walls[room.walls[i]].startLoc[4], walls[room.walls[i]].startLoc[5]};
                    std::vector<int> corner3 = walls[room.walls[i]].endLoc;
                    if (type == "both") {
                        renderLine(corner1, corner2);
                    }
                    renderLine(corner2, corner3);
                    renderLine(corner1, corner3);

                // If it's a normal wall
                } else {
                    std::vector<int> cornerTopLeft = walls[room.walls[i]].startLoc;
                    std::vector<int> cornerBottomRight = walls[room.walls[i]].endLoc;
                    std::vector<int> cornerBottomLeft = {cornerTopLeft[0], cornerTopLeft[1], cornerBottomRight[2]};
                    std::vector<int> cornerTopRight = {cornerBottomRight[0], cornerBottomRight[1], cornerTopLeft[2]};
                    renderLine(cornerTopLeft, cornerTopRight);
                    renderLine(cornerBottomRight, cornerBottomLeft);
                    if (type == "left" || type == "both") {
                        renderLine(cornerBottomLeft, cornerTopLeft);
                    }
                    if (type == "right" || type == "both") {
                        renderLine(cornerTopRight, cornerBottomRight);
                    }
                }
            }

            // For each enemy
            if (!simpleRooms) {
                int startEnemy = core * (room.enemies.size() / numCores);
                int endEnemy = (core+1) * (room.enemies.size() / numCores);
                if (core == numCores-1) {
                    endEnemy = room.enemies.size();
                }
                std::string actionCheck = ancillaLock(1, "ancillaActionCheck");
                for (int i=startEnemy; i<endEnemy; i++) {
                    std::string id = "enemy" + std::to_string(room.enemies[i]);

                    // Copy the location and action
                    int rec = startRecording();
                    copyTC(id+"X", "enemyDrawX");
                    copyTC(id+"Y", "enemyDrawY");
                    copyTC(id+"Z", "enemyDrawZ");
                    copy(id+"Action", "enemyDrawAction");

                    // Choice of frame counter depends how many frames for this action
                    std::string enemyName = enemies[room.enemies[i]].name;
                    for (size_t j=0; j<framesPerAction[enemyName].size(); j++) {
                        int frameCountNum = std::log2(framesPerAction[enemyName][j]);
                        std::string regName = "frameCounter"+std::to_string(frameCountNum);
                        if (j == 6 || j == 5) {
                            regName = "enemy" + std::to_string(room.enemies[i]) + "DeathAnim";
                        }
                        std::string frameCountSubreg = subreg(regName, 0, qubitList[regName].amount-frameDuplication);
                        flipIfEqual(actionCheck, "enemyDrawAction", j);
                        copyIf(frameCountSubreg, "enemyDrawFrame", actionCheck);
                        flipIfEqual(actionCheck, "enemyDrawAction", j);
                    }

                    // Render the enemy, uncomputing afterwards
                    stopRecording(rec);
                    //outputReg("Draw action ", "enemyDrawAction");
                    //outputReg("Draw frame ", "enemyDrawFrame");
                    renderEnemy(enemies[room.enemies[i]].name, "enemyDrawX", "enemyDrawY", "enemyDrawZ", "enemyDrawAction", "enemyDrawFrame");
                    uncomputeRecording(rec);

                }
                ancillaFree(actionCheck);
            }

            // For each pickup
            if (!simpleRooms) {
                int startPickup = core * (room.pickups.size() / numCores);
                int endPickup = (core+1) * (room.pickups.size() / numCores);
                if (core == numCores-1) {
                    endPickup = room.pickups.size();
                }
                for (int i=startPickup; i<endPickup; i++) {
                    std::string id = "pickup" + std::to_string(room.pickups[i]);
                    int xLoc = pickups[room.pickups[i]].loc[0];
                    int yLoc = pickups[room.pickups[i]].loc[1];
                    int zLoc = pickups[room.pickups[i]].loc[2];
                    setValTC("enemyDrawX", xLoc);
                    setValTC("enemyDrawY", yLoc);
                    setValTC("enemyDrawZ", zLoc);
                    std::string pickupCollected = "pickup" + std::to_string(room.pickups[i]) + "Collected";
                    x(pickupCollected);
                    renderSpriteIf(pickupNames[pickups[room.pickups[i]].name], "enemyDrawX", "enemyDrawY", "enemyDrawZ", pickupCollected);
                    x(pickupCollected);
                    setValTC("enemyDrawX", xLoc);
                    setValTC("enemyDrawY", yLoc);
                    setValTC("enemyDrawZ", zLoc);
                }
            }

            // Finish this core's work
            sectionParallel();

        }

        // Finish the parallel block
        endParallel();

    endSubroutine();

    // Reserve ancillas
    int ancillasUsedInFunc = stopPeakAncilla();
    std::cout << "Peak ancillas used in render room: " << ancillasUsedInFunc << std::endl;
    if (ancillasUsedInFunc > 0) {
        ancillaLock(ancillasUsedInFunc, "ancillaFunctionRenderRoom" + std::to_string(room.id));
    }

}



