# Customized Virtual File System (CVFS)

A lightweight virtual file system implemented in C. This project simulates basic file system functionality such as creating, reading, writing, and deleting files within a command-line interface, without interacting with the actual OS file system.

---

## 📌 Project Description

This project implements a simplified virtual file system in C, supporting essential file operations such as `create`, `open`, `read`, `write`, `close`, `ls`, `stat`, and `fstat`. It uses a custom inode structure, file descriptor table, and memory buffers to simulate the behavior of a real-world file system within a user-defined shell environment. The system includes permission handling, memory management, and error detection for a realistic CLI-based file management experience.

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
