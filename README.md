Customized Virtual File System (CVFS)


Project Overview
This project implements a Unix-inspired custom virtual file system in C. It supports core file operations such as create, open, read, write, close, ls, stat, and fstat, along with permission handling and error checking — all from a command-line shell.

The system uses an inode structure, file descriptor tables, and memory buffers to simulate how an operating system handles files internally.



👩‍💻 Author
Vaishnavi Jadhav
MS in Embedded Systems, University of Colorado Boulder


⚙️ Features
Create and delete files

Open files with various modes (Read / Write / Read+Write)

Write data to virtual files

Read data from virtual files

List all created files (ls)

View file metadata (stat, fstat)

Close individual files or all open files

Permission-based access (Read / Write / Both)

Error handling and memory leak prevention



📂 Internal Architecture
SUPERBLOCK: Tracks total and free inodes.

INODE Table: Stores file metadata, permissions, and file content buffer.

UFDT (User File Descriptor Table): Maps open file descriptors to inodes.

FILETABLE: Maintains read/write offsets, mode, and reference count.



Sample Commands (Inside the Shell)
> create file1.txt 3           # Creates file with read+write permission
> open file1.txt 2             # Opens file in write-only mode
> write 0 HelloWorld           # Writes to fd 0
> read file1.txt 50            # Reads 50 bytes from file
> ls                           # Lists all files
> stat file1.txt               # Displays metadata of the file
> fstat 0                      # Displays metadata using file descriptor
> close file1.txt              # Closes file by name
> closeall                     # Closes all open files
> rm file1.txt                 # Removes the file
> exit                         # Exits the shell

🛠️ Compilation
gcc vfs.c -o vfs
./vfs


