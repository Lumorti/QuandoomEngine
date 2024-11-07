#include "globals.h"

// ------------------------------------------
//                 SETTINGS
// ------------------------------------------

// Whether to make it QASM compliant
bool qasmCompliant = true;

// Whether to optimize gates
bool optimizeGates = true;

// Whether to check that all ancillas are zero when freed
bool ancillaCheck = false;

// Whether to output render information (lot of output)
bool debugRender = false;

// Whether to just render the rooms without enemies and pickups
bool simpleRooms = false;

// Whether to check for unused functions and qubits
bool unusedChecks = false;

// Whether to make the player immortal
bool playerImmortal = false;

// Whether to force to not output anything
bool forceNoOutput = true;

// Number of cores to use
int numCores = 4;

// Various parameters
int screenWidth = 320;
int screenHeight = 200;
int leftShift = 5;
int qubitsPer = 23;
int fov = 90;
int maxSpriteHeight = 200;
int spriteWorldHeight = 40;
int pickupDistance = 35;
int enemyCollisionDistance = 35;
int doorQubits = 16;
int renderDistance = 700;
int turnSpeed = 4;
int moveSpeed = 3;
int doorSpeed = 3;
int maxAction = 6;
int maxFrame = 3;
int enemyMoveShift = 1;
int frameDuplication = 1;
int rayCastSections = 10;
int meleeDistance = 80;
int dispersion = 30;
int barHeight = 31;

// Main start location
int playerStartX = 1516;
int playerStartY = 1059;
int playerStartZ = 32;
int playerStartAngle = 1;

// Room K
//int playerStartX = 1009;
//int playerStartY = 1347;
//int playerStartZ = 32;
//int playerStartAngle = 77; 

// A to B 
//int playerStartX = 1678;
//int playerStartY = 1654;
//int playerStartZ = 32;
//int playerStartAngle = 352; 

// B
//int playerStartX = 1995;
//int playerStartY = 1907;
//int playerStartZ = 32;
//int playerStartAngle = 270; 

// B to C
//int playerStartX = 2598;
//int playerStartY = 1801;
//int playerStartZ = 32;
//int playerStartAngle = 270; 

// C
//int playerStartX = 2938;
//int playerStartY = 1527;
//int playerStartZ = 32;
//int playerStartAngle = 1; 

// C to pre-D
//int playerStartX = 2926;
//int playerStartY = 1049;
//int playerStartZ = 32;
//int playerStartAngle = 1; 

// Pre-D
//int playerStartX = 2926;
//int playerStartY = 746;
//int playerStartZ = 32;
//int playerStartAngle = 1; 

// D (elevator facing button)
//int playerStartX = 2983;
//int playerStartY = 291;
//int playerStartZ = 32;
//int playerStartAngle = 85; 

// Define the names of the pickups
std::map<std::string, std::string> pickupNames = {
    {"armor", "pickups-0-0"},
    {"armor2", "pickups-1-0"},
    {"armor3", "pickups-2-0"},
    {"health", "pickups-3-0"},
    {"health2", "pickups-4-0"},
    {"health3", "pickups-5-0"},
    {"clip", "pickups-6-0"},
    {"shells", "pickups-7-0"},
    {"corpse", "pickups-0-1"},
    {"corpse2", "pickups-1-1"},
    {"corpse3", "pickups-2-1"},
    {"barrel", "pickups-0-2"},
    {"light", "pickups-1-2"},
    {"light2", "pickups-2-2"},
    {"pillar", "pickups-3-2"},
    {"pillar2", "pickups-4-2"}
};

// Which pickups are static
std::set<std::string> staticPickups = {
    "corpse", "corpse2", "corpse3", "barrel", "light", "light2", "pillar", "pillar2"
};

// How many frames per action in the enemy spritesheet (multiples of 2)
std::map<std::string, std::vector<int>> framesPerAction = {
    {"zombieman", {2, 4, 4, 4, 2, 2, 4}},
    {"imp", {2, 4, 4, 4, 2, 2, 4}},
    {"shotgunguy", {2, 4, 4, 4, 2, 2, 4}}
};

// ------------------------------------------

// Various globals
int qubitsNeeded = 0;
int qubitsToSave = 0;
int nonScreenQubits = 0;
int currentGateCount = 0;
int peakAncillaUsed = 0;
int peakAncillaUsedTemp = 0;
int peakAncillaUsedPrev = 0;
bool recordingPeakAncilla = false;

// List of qubits
std::unordered_map<std::string, qubitDescription> qubitList;
	
// The global containing all the things
std::vector<Room> level;
std::vector<Enemy> enemies;
std::vector<Pickup> pickups;
std::vector<Wall> walls;
std::vector<Interactable> interactables;

// Globals for storing the instructions
std::vector<std::string> output;
std::vector<std::pair<int, int>> recordings;

// Globals for subroutine handling
int currentSubroutineStart = -1;
std::unordered_map<std::string, int> subroutineNameToIndex;
std::vector<std::vector<std::string>> subroutineLines = {{}};
std::vector<std::string> subroutineCallString = {""};
std::vector<std::vector<int>> subroutineQubits = {{}};

// Globals for parallel handling
int parallelStart = 0;
int parallelEnd = 0;
std::vector<int> sectionStarts;

// List of all saves sprites
std::unordered_map<std::string, std::vector<std::vector<bool>>> spriteList;

