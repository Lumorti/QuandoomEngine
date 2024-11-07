# QuandoomEngine

This is the code used to generate the quantum circuit (QASM) file for running DOOM on a quantum computer. The main repository is found [here](https://github.com/Lumorti/Quandoom).

This contains the stuff to make two executables: **run** (the simulator) and **gen** (the code generating the circuit). For the simulator you need SDL, see the main repo for details. For the generator there aren't any non-standard dependencies, it's pure C++, baby.

To summarize the structure (because it's still a bit spaghetti-y):
    - gen.cpp contains the overall game logic and loop
    - draw.cpp/h contains subroutines specific to drawing to the quantum screen
    - binary.cpp/h contains subroutines allowing operators on the quantum registers (e.g. adding, comparison, etc.)
    - registers.cpp/h contains subroutines handling the setup and manipulation of the quantum registers, as well as the ancillas, subreg, recording and garbage systems
    - gates.cpp/h contains the lowest level routines going from something like cx(control, qubit) to an actual QASM instruction.
    - globals.cpp/h contains settings, you should be safe to change these values within reason, but I make no promises
    - levels.cpp/h contains the description of the level and all of the rooms within it, as well as subroutines for checking the level validity. Realistically this should have been automated from the WAD file, but there were a lot of reversible-specific things that I needed to change, the levels are far from one-to-one
    - debug.cpp/h contains subroutines related to printing, debugging and optimising the resulting QASM file, as well as parallelisation
    - logic.cpp/h contains the game logic, such as the movement, shooting, etc.

If you wanna use this code to try to make your own 3D quantum game, be my guest, I would love to see it. Or maybe better would be actually turn this into a more portable/versatile game engine to make it even easier for people to do stuff like this as a quantum circuit. For now I don't plan on working on anything like this, I have my PhD to finish, but I would love to see some active forks of this.

Some things that have been pointed out to me, since the release:
 - there are some MCMT instructions that technically aren't valid QASM, since I use a lot of "x qubit1 qubit2" to mean that we apply an x gate to qubit 1 and also simultaneously to qubit 2. But apparently that isn't in the OPENQASM 3 standard, despite it being physically valid. I cannot be bothered to change the code to make it specifically compliant, because conforming specifically to the QASM standard is not the aim of the project. The circuit is a valid quantum circuit, and that's what matters. Also, the file size would be huge if we have to split x q1 q2 q3 ... q10000 into 10000 different instructions. Also, simulation speed will be reduced, I deliberately compress things like static sprite draws into big x gates acting in parallel on many qubits.
