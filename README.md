README - COSC1114 Project 1
===========================================================

Student Information
-------------------
Name
Teammate 1: Kannan Arora(s4001021)
Teammate 2: Ariadna Garcia Salinas(s4190914)
Course: COSC1114 Operating Systems
Project: Project 1

-----------------------------------------------------------
How to Compile
-----------------------------------------------------------
Use the provided Makefile:

    make all

This will build two executables:
    - mmcopier  -> Task 1 (multi-file copier)
    - mscopier  -> Task 2 (multi-threaded copier)

To remove compiled executables:

    make clean

-----------------------------------------------------------
How to Run
-----------------------------------------------------------

Task 1: mmcopier
----------------
Usage:
    ./mmcopier <n> <source_dir> <destination_dir>

Example:
    ./mmcopier 3 source_dir dest_dir

Notes:
- <n> must be between 2 and 10.
- Program will copy source1.txt ... sourceN.txt from the source_dir
  into the destination_dir.
- If the destination directory does not exist, the program will exit
  with an error message (no hard-coding of file paths).

Task 2: mscopier
----------------
Usage:
    ./mscopier <n_threads> <source_file> <destination_file>

Example:
    ./mscopier 5 input.txt output.txt

Notes:
- <n_threads> must be between 1 and 10.
- Program reads lines from the source_file and writes them to the
  destination_file using multiple threads and a shared queue.
- Output file contents will exactly match the input file.

-----------------------------------------------------------
Error Handling
-----------------------------------------------------------
- All file open, memory allocation, and thread creation errors are
  checked.
- If an error occurs, the program prints a clear message and exits.

-----------------------------------------------------------
Memory Correctness
-----------------------------------------------------------
- All dynamically allocated memory is freed.
- Verified with valgrind on the RMIT servers:
  valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./program args
- No memory leaks or invalid memory accesses reported.

End of README
===========================================================
