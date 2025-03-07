# OS Management Simulator

## Overview
This is a C-based Operating System simulator developed as a student project for **CGS 3763** at the University of Central Florida. It demonstrates fundamental OS concepts including memory allocation algorithms, file system operations, and basic multithreading. 

### Project Timeline:
- **Initial Version:** November 1, 2021 (Assignment 5)
- **Assignment 6 Completion:** November 19, 2021
- **Enhanced Version:** February 26, 2025

## Features
- **Memory Management**
  - Implements four allocation algorithms:
    - First Fit
    - Best Fit
    - Worst Fit
    - Next Fit
- **File System Operations**
  - Displays file attributes and metadata from the current directory.
- **Multithreading Demonstration**
  - Implements basic thread execution using POSIX threads (`pthreads`).
- **Enhanced Usability**
  - Improved input validation and screen-clearing mechanics.

## Requirements
- **C Compiler** (e.g., `gcc`)
- **POSIX-compliant System** (Linux, macOS; Windows requires adaptations like Cygwin/MinGW)
- **POSIX Threads Library** (`-pthread` flag)

## Compilation
To compile the program, use:
gcc -o os_management OSManagement.c -pthread

## Usage
Run the compile executable:
./os_management