#include <iostream> // TODO REMOVE
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include "SDL.h" // sudo apt-get install libsdl2-dev
#include <omp.h> // sudo apt-get install libomp-dev

// Entry point
int main(int argc, char** argv) {
    if (argc < 2) {std::cout << "Usage: ./simulator <circuit_file>" << std::endl; return 1;}

    // Load the circuit file (vecs are gates, ints are qubits, first int is num controls)
    // #q total[75111] saved[11096] nonScreen[11111] screen[320][200] cores[4] -> info
    // #w -> -1 (wait)
    // #j 4 12 -> -4 12 (jump core 4 12 lines ahead)
    // x 2 3 -> 0 2 3 (x on qubits 2 and 3)
    // h 1 2 -> 0 10000 1 2 (h on qubits 1 and 2)
    // ctrl @ x 2 3 -> 1 2 3 (x on qubit 3 with 1 control (qubit 2))
    // ctrl @ G3 2 -> 1 2 -3 (call G3 with 1 control (qubit 2)
    // #o 1 ... -> 10000 1 2 99 61 67 68 (print "c=" then {67 68} as denary)
    // #o 2 ... -> 10000 2 2 99 61 67 68 (print "c=" then {67 68} as binary)
    // #o 3 ... -> 10000 3 2 99 61 67 68 (print "c=" then {67 68} as TC)
    // #o 4 ... -> 10000 4 2 99 61 67 68 (print "c=" then {67 68} as binary if)
    std::vector<std::vector<std::vector<int>>> circuits(1);
    std::ifstream file(argv[1]);
    std::string line = "";
    std::vector<int> info = {};
    int currentFunction = 0;
    while (std::getline(file, line)) {
        if (line[0] == '}') {currentFunction = 0;}
        else if (line[0] == 'x' || line[0] == 'h' || line[0] == 'g' || line[0] == 'G' || line[0] == 'c' || line[0] == '#') {
            std::vector<int> qubits = {0};
            std::string currentNum = "";
            bool isFuncCall = false;
            for (size_t i = 0; i < line.size(); i++) {
                if (line[i] >= '0' && line[i] <= '9') {currentNum += line[i];}
                else if (line[0] != 'g' && line[i] == 'G') {isFuncCall = true;}
                else if (line[i] == 'c') {qubits[0]++;}
                else if ((line[i] == ']' || line[i] == ' ' || line[i] == ';') && currentNum != "") {
                    qubits.push_back(std::stoi(currentNum));
                    currentNum = "";
                }
            }
            if (isFuncCall) {
                std::vector<int> newQubits = {qubits[0]};
                for (int i = 2; i < 2+qubits[0]; i++) {
                    newQubits.push_back(qubits[i]);
                }
                newQubits.push_back(-qubits[1]);
                qubits = newQubits;
            }
            if (line[0] == '#' && line[1] == 'w') {qubits = {-1};}
            else if (line[0] == 'h') {qubits.insert(qubits.begin()+1, info[0]);}
            else if (line[0] == '#' && line[1] == 'o') {qubits[0] = info[0];}
            else if (line[0] == '#' && line[1] == 'j') {qubits[0] = -qubits[1]; qubits.erase(qubits.begin()+1);}
            if (line[0] == '#' && line[1] == 'q') {info = qubits; info.erase(info.begin());}
            else if (line[0] == 'g') {currentFunction = qubits[1]; circuits.resize(std::max(circuits.size(), size_t(qubits[1]+1)));}
            else {circuits[currentFunction].push_back(qubits);}
        }
    }
    file.close();

    // Output everything
    //for (size_t i = 0; i < circuits.size(); i++) {
        //std::cout << "Function " << i << std::endl;
        //for (size_t j = 0; j < circuits[i].size(); j++) {
            //for (size_t k = 0; k < circuits[i][j].size(); k++) {
                //std::cout << circuits[i][j][k] << " ";
            //}
            //std::cout << std::endl;
        //}
    //}
    //std::cout << "--------------------" << std::endl;

    // Create a window and start the main game loop
    omp_set_num_threads(info[5]);
    SDL_Window *screen = SDL_CreateWindow("Quandoom", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
    SDL_Event e;
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    bool quit = false, changed = true;
    std::vector<bool> stateInit(info[0], 0);
    #pragma omp parallel default(shared)
    {
        int thread = omp_get_thread_num() + 1;
        while (!quit) {
            auto t1 = std::chrono::high_resolution_clock::now();

            // Only update when we have new state TODO REMOVE
            if (changed || argc > 2) {

                // Run the circuit
                int totalGates = 0; // TODO REMOVE
                for (int i = info[1]; i < info[0]; i++) {stateInit[i] = false;}
                auto t1Local = std::chrono::high_resolution_clock::now(); // TODO REMOVE
                int currentCircuit = 0;
                std::vector<std::pair<int, int>> prevPos;
                std::vector<bool> state(stateInit);
                for (int i = 0; i < int(circuits[currentCircuit].size()); i++) {
                    if (circuits[currentCircuit][i].size() == 1 && circuits[currentCircuit][i][0] == -1) {
                        //#pragma omp critical
                        //std::cout << "Thread " << thread << " waiting at gate " << i << std::endl; // TODO REMOVE
                        #pragma omp barrier
                    } else if (circuits[currentCircuit][i].size() >= 2 && circuits[currentCircuit][i][0] < 0) {
                        if (thread == -circuits[currentCircuit][i][0]) {
                            i += circuits[currentCircuit][i][1];
                            //#pragma omp critical
                            //std::cout << "Thread " << thread << " jumped to gate " << i << std::endl; // TODO REMOVE
                        }
                    } else if (circuits[currentCircuit][i].size() >= 1 && circuits[currentCircuit][i][0] == info[0]) {
                        if (thread == 1) {
                            if (argc <= 2) {
                                std::string asString = "";
                                for (int j = 3; j < 3+circuits[currentCircuit][i][2]; j++) {
                                    asString += char(circuits[currentCircuit][i][j]);
                                }
                                if (3+circuits[currentCircuit][i][2] < int(circuits[currentCircuit][i].size())) {
                                    if (circuits[currentCircuit][i][1] == 1) {
                                        int asInt = 0;
                                        for (size_t j = 3+circuits[currentCircuit][i][2]; j < circuits[currentCircuit][i].size(); j++) {
                                            asInt += state[circuits[currentCircuit][i][j]] * pow(2, circuits[currentCircuit][i].size()-j-1);
                                        }
                                        std::cout << asString << asInt << std::endl;
                                    } else if (circuits[currentCircuit][i][1] == 2) {
                                        std::cout << asString;
                                        for (size_t j = 3+circuits[currentCircuit][i][2]; j < circuits[currentCircuit][i].size(); j++) {
                                            std::cout << state[circuits[currentCircuit][i][j]];
                                        }
                                        std::cout << std::endl;
                                    } else if (circuits[currentCircuit][i][1] == 3) {
                                        int asInt = 0;
                                        for (size_t j = 3+circuits[currentCircuit][i][2]; j < circuits[currentCircuit][i].size(); j++) {
                                            if (int(j) == 3+circuits[currentCircuit][i][2]) {
                                                asInt -= state[circuits[currentCircuit][i][j]] * pow(2, circuits[currentCircuit][i].size()-j-1);
                                            } else {
                                                asInt += state[circuits[currentCircuit][i][j]] * pow(2, circuits[currentCircuit][i].size()-j-1);
                                            }
                                        }
                                        std::cout << asString << asInt << std::endl;
                                    } else if (circuits[currentCircuit][i][1] == 4) {
                                        for (size_t j = 3+circuits[currentCircuit][i][2]; j < circuits[currentCircuit][i].size(); j++) {
                                            if (state[circuits[currentCircuit][i][j]]) {
                                                std::cout << asString;
                                                for (size_t k = 3+circuits[currentCircuit][i][2]; k < circuits[currentCircuit][i].size(); k++) {
                                                    std::cout << state[circuits[currentCircuit][i][k]];
                                                }
                                                std::cout << std::endl;
                                            }
                                        }
                                    }
                                } else {
                                    std::cout << asString << std::endl;
                                }
                            }
                        }
                    } else { // TODO REMOVE
                        //#pragma omp critical
                        //std::cout << "Thread " << thread << " executed gate " << i  << " in circuit " << currentCircuit << std::endl;
                        bool allOn = true;
                        for (int j = 1; j < 1+circuits[currentCircuit][i][0]; j++) {
                            if (!state[circuits[currentCircuit][i][j]]) {allOn = false; break;}
                        }
                        totalGates++; // TODO REMOVE
                        if (allOn) {
                            if (circuits[currentCircuit][i][1+circuits[currentCircuit][i][0]] == info[0]) {
                                for (size_t j = 1; j < circuits[currentCircuit][i].size(); j++) {
                                    state[circuits[currentCircuit][i][j]] = rand() % 2;
                                }
                            } else {
                                for (size_t j = 1+circuits[currentCircuit][i][0]; j < circuits[currentCircuit][i].size(); j++) {
                                    if (circuits[currentCircuit][i][j] < 0) {
                                        prevPos.push_back({currentCircuit, i});
                                        currentCircuit = -circuits[currentCircuit][i][j];
                                        i = -1; break;
                                    } else {state[circuits[currentCircuit][i][j]] = !state[circuits[currentCircuit][i][j]];}
                                }
                            }
                        }
                    } // TODO REMOVE
                    if (currentCircuit > 0 && i >= int(circuits[currentCircuit].size())-1) {
                        i = prevPos[prevPos.size()-1].second;
                        currentCircuit = prevPos[prevPos.size()-1].first;
                        prevPos.pop_back();
                    }
                }

                // Combine all of the local states
                if (thread == 1) {
                    auto t2 = std::chrono::high_resolution_clock::now(); 
                    int nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1Local).count();
                    int micros = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1Local).count();            
                    std::cout << micros << " us to run circuit = " << 1.0e6/float(micros) << " fps \n";
                    std::cout << double(nanos) / double(totalGates) << " ns per gate\n"; 
                    std::cout << "total gates performed = " << totalGates << std::endl; // TODO REMOVE
                    for (int i = 0; i < info[1]; i++) {stateInit[i] = state[i];}
                }

                #pragma omp critical
                for (int i = info[1]; i < info[0]; i++) {stateInit[i] = stateInit[i] != state[i];}

                #pragma omp barrier

                // Output the state to the screen
                if (thread == 1) {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                    SDL_RenderClear(renderer);
                    SDL_RenderSetLogicalSize(renderer, info[3], info[4]);
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    for (int j = 0; j < info[4]; j++) {
                        for (int i = 0; i < info[3]; i++) {
                            if (stateInit[info[2] + info[3]*j + i]) {
                                SDL_RenderDrawPoint(renderer, i, j);
                            }
                        }
                    }
                    SDL_RenderPresent(renderer);
                }

            }

            // Framerate limiter: 20 fps = 1 frame / 50 milliseconds
            auto t3 = std::chrono::high_resolution_clock::now();
            int millis = std::chrono::duration_cast<std::chrono::milliseconds>(t3-t1).count();
            if (millis < 50) {SDL_Delay(50-millis);}

            // Adjust the state based on the keyboard inputs
            if (thread == 1) {
                changed = false; // TODO REMOVE
                while (SDL_PollEvent(&e)) {if (e.type == SDL_QUIT) {quit = true;}}
                std::vector<int> stateList = {SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_UP, 
                                  SDL_SCANCODE_DOWN, SDL_SCANCODE_SPACE, SDL_SCANCODE_LALT, 
                                  SDL_SCANCODE_LCTRL, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3};
                for (size_t i = 0; i < stateList.size(); i++) {
                    if (keystates[stateList[i]]) {stateInit[i] = true; changed = true;} 
                    else {stateInit[i] = false;}
                }
            }
            #pragma omp barrier // can maybe remove

        }

    }

    // Quit the game
    SDL_DestroyWindow(screen);
    SDL_Quit();
    return 0;

}
