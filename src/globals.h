#pragma once
#include <vector>
#include <string>
#include <math.h>
#include <unordered_map>
#include <map>
#include <set>

// Whether to make it QASM compliant
extern bool qasmCompliant;

// Whether to optimize gates
extern bool optimizeGates;

// Whether to check that all ancillas are zero when freed
extern bool ancillaCheck;

// Whether to output render information (lot of output)
extern bool debugRender;

// Whether to just render the rooms without enemies and pickups
extern bool simpleRooms;

// Immortal mode
extern bool playerImmortal;

// Whether to check for unused functions and qubits
extern bool unusedChecks;

// Whether to force to not output anything
extern bool forceNoOutput;

// Qubit structure
struct qubitDescription {
	bool saved;
	int start;
	int end;
	int amount;
	int buffer;
	int bufferStart;
	int bufferEnd;
	int offset;
	int min;
	int max;
    std::vector<bool> used;
	bool isTC;
};

// List of qubits
extern std::unordered_map<std::string, qubitDescription> qubitList;
	
// The structure of each enemy
struct Enemy {
    int id;
    std::string name;
    std::vector<int> startLoc;
    int startAngle;
    std::string behaviour;
};

// The structure of each wall
struct Wall {
    int id;
    std::string type;
    std::vector<int> startLoc;
    std::vector<int> endLoc;
    bool checkCollision;
    bool visible;
    bool blockLineOfSight;
};

// The structure of each pickup
struct Pickup {
    int id;
    std::string name;
    std::vector<int> loc;
};

// Something interactable
struct Interactable {
    int id;
    std::string type;
    int idToAffect;
    std::vector<int> loc;
    int range;
};

// The structure of each room
struct Room {
    int id;
    int minX;
    int maxX;
    int minY;
    int maxY;
    std::vector<int> walls;
    std::vector<int> enemies;
    std::vector<int> pickups;
    std::vector<int> interactables;
    std::vector<std::pair<int, int>> zonesX;
    std::vector<std::pair<int, int>> zonesY;
    std::vector<std::vector<int>> noVisionZones;
};

// The global containing all the things
extern std::vector<Room> level;
extern std::vector<Enemy> enemies;
extern std::vector<Pickup> pickups;
extern std::vector<Wall> walls;
extern std::vector<Interactable> interactables;

// Various globals
extern int qubitsNeeded;
extern int qubitsToSave;
extern int nonScreenQubits;
extern int currentGateCount;
extern int peakAncillaUsed;
extern int peakAncillaUsedTemp;
extern int peakAncillaUsedPrev;
extern bool recordingPeakAncilla;

// Various settings
extern int screenWidth;
extern int screenHeight;
extern int leftShift;
extern int qubitsPer;
extern int fov;
extern int maxSpriteHeight;
extern int spriteWorldHeight;
extern int pickupDistance;
extern int enemyCollisionDistance;
extern int playerStartX;
extern int playerStartY;
extern int playerStartZ;
extern int playerStartAngle;
extern int doorQubits;
extern int renderDistance;
extern int turnSpeed;
extern int moveSpeed;
extern int doorSpeed;
extern int numCores;
extern int maxAction;
extern int maxFrame;
extern std::set<std::string> staticPickups;
extern int enemyMoveShift;
extern int frameDuplication;
extern int rayCastSections;
extern int meleeDistance;
extern int dispersion;
extern int barHeight;

// Define the names of the pickups
extern std::map<std::string, std::string> pickupNames;

// Define the frames per action
extern std::map<std::string, std::vector<int>> framesPerAction;

// Globals for storing the instructions
extern std::vector<std::string> output;
extern std::vector<std::pair<int, int>> recordings;

// Globals for subroutine handling
extern int currentSubroutineStart;
extern std::unordered_map<std::string, int> subroutineNameToIndex;
extern std::vector<std::vector<std::string>> subroutineLines;
extern std::vector<std::string> subroutineCallString;
extern std::vector<std::vector<int>> subroutineQubits;

// Globals for parallel handling
extern int parallelStart;
extern int parallelEnd;
extern std::vector<int> sectionStarts;

// List of all saves sprites
extern std::unordered_map<std::string, std::vector<std::vector<bool>>> spriteList;

