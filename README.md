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

- Select options 1 - 3 from the menus to explore features, or 0 to exit.
- For file management, run in a directory with files to see their attributes,

```markdown
## Sample Memory Management Output
With blocks `{15, 10, 20, 35, 80}` and processes `{10, 20, 5, 30, 65}`:

### First Fit
| Process No. | Process Size | Block No. | Remaining Space |
|-------------|--------------|-----------|-----------------|
| 1           | 10           | 1         | 5               |
| 2           | 20           | 3         | 0               |
| 3           | 5            | 1         | 0               |
| 4           | 30           | 4         | 5               |
| 5           | 65           | 5         | 15              |


## Directory Structure
- `OSManagement.c`: Main source code
- `.gitignore`: Ignores compiled files
- `README.md`: This documentation

## Author
- **Stephen Miller**
- PID: 4078565
- Original: November 2021
- Enhanced: February 26, 2025

## License
This project is for educational purposes and not licensed for commercial use.

## Acknowledgments
- Enhanced with assistance from Grok (xAI) in 2025.
- Originally submitted for CGS 3763 Assignment 5 & 6.

