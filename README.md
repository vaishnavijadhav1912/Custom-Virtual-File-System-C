# Customized Virtual File System (CVFS)

A lightweight virtual file system implemented in C. This project simulates basic file system functionality such as creating, reading, writing, and deleting files within a command-line interface, without interacting with the actual OS file system.

---

## 📌 Project Description

This project implements a simplified virtual file system (VFS) in C, designed to replicate core functionalities of real operating system file systems. It supports basic commands like `create`, `open`, `read`, `write`, `close`, `ls`, `stat`, and `fstat`.

### 🧠 Core Data Structures:
- **Superblock**: Tracks total and free inodes in the system.
- **Inode Structure**: Stores file metadata including name, size, permissions, link/reference count, and a pointer to the file’s data buffer.
- **File Table**: Maintains runtime information of each opened file including read/write offsets and access modes.
- **UFDT (User File Descriptor Table)**: Array that holds references to open file tables, simulating file descriptors.
- **Linked List of Inodes**: Used to manage and traverse the list of available or active inodes.

### 💡 C Programming Concepts Used:
- **Structures (`struct`)**: For defining Superblock, Inode, File Table, and UFDT entries.
- **Pointers and Pointer to Structures**: For dynamic data access and linking.
- **Dynamic Memory Allocation**: Using `malloc()` and `free()` for file buffers and file tables.
- **String Handling**: Functions like `strcpy`, `strcmp`, and `strlen` are used for managing file names and data.
- **Function Modularization**: Each command (create, open, read, etc.) is implemented as a separate function.
- **Command Parsing**: Using `sscanf()` and tokenized input to interpret user commands in a shell-like interface.
- **File Permissions and Modes**: Implemented via integer flags (1 = Read, 2 = Write, 3 = Read & Write).
- **Basic Error Handling**: Return codes and condition checks for invalid operations (like reading unopened files or writing without permission).

This virtual file system mimics an OS shell environment, making it ideal for learning low-level file handling, memory management, and command-driven design in C.

---

## 🔧 Features

- 📂 Create and manage files using a command-line interface
- 🛡️ Support for file permissions: Read (1), Write (2), Read & Write (3)
- 📋 Simulated inodes and UFDT (User File Descriptor Table)
- 📑 Metadata retrieval via `stat` and `fstat`
- 🚫 File truncation and removal
- 📄 List all files using `ls`
- 🧠 Internal file buffer management (max 2048 bytes per file)
- 📌 Supports up to 50 files (MAXINODE = 50)

---

## 🧪 Sample Commands

Here are the available commands you can test in the shell prompt:

```bash
> create demo.txt 3          # Create file with Read & Write permission
> open demo.txt 2            # Open file in Write mode
> write demo.txt             # Then enter your content
> read demo.txt 20           # Read 20 bytes from file
> lseek demo.txt 10 0        # Move read/write offset (START=0, CURRENT=1, END=2)
> truncate demo.txt          # Clear contents of the file
> close demo.txt             # Close file
> rm demo.txt                # Delete file
> ls                         # List all files
> stat demo.txt              # Show file info by name
> fstat 0                    # Show file info by file descriptor
> closeall                   # Close all open files
> help                       # Show all command usage
> exit                       # Exit CVFS
