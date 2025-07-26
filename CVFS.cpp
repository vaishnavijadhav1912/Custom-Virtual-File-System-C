/*
    Author Name: Vaishnavi Jadhav

    Project Name: Unix based Customized Virtual File System

    Description:
    This project implements a simplified virtual file system in C,
    supporting essential file operations such as create, open, read,
    write, close, ls, stat, and fstat. It uses a custom inode structure,
    file descriptor table, and memory buffers to simulate a real-world
    file system within a user-defined shell interface.
    The system handles permissions, memory allocation, and robust error
    detection to provide a realistic CLI-based file management experience.


*/


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<iostream>
#include<io.h>

#define MAXINODE 50

#define READ 1
#define WRITE 2

#define MAXFILESIZE 2048

#define REGULAR 1
#define SPECIAL 2

#define START 0
#define CURRENT 1
#define END 2


/*
 * Structure: superblock
 * ---------------------
 * This structure holds basic information about the file system's inode usage.
 *
 * Fields:
 *  - TotalInodes : Total number of inodes available in the file system.
 *  - FreeInodes  : Number of inodes currently free (not allocated).
 *
 * Typedefs:
 *  - SUPERBLOCK     : Alias for the struct superblock.
 *  - PSUPERBLOCK    : Pointer to a SUPERBLOCK structure.
 */
typedef struct superblock
{
    int TotalInodes;
    int FreeInodes;
}SUPERBLOCK, *PSUPERBLOCK;




/*
 * Structure: inode
 * ----------------
 * This structure represents a file in the virtual file system.
 * It stores metadata and a pointer to the file's data buffer.
 *
 * Fields:
 *  - FileName       : Name of the file (max 50 characters).
 *  - InodeNumber    : Unique number assigned to each inode.
 *  - FileSize       : Maximum allowed size of the file.
 *  - FileActualSize : Actual size of the data written in the file.
 *  - FileType       : Type of file (e.g., REGULAR or SPECIAL).
 *  - Buffer         : Pointer to the memory where file data is stored.
 *  - LinkCount      : Number of references (links) to this inode.
 *  - ReferenceCount : Number of file descriptors currently using this inode.
 *  - Permission     : Permissions assigned to the file (read, write, etc.).
 *  - next           : Pointer to the next inode in the linked list.
 *
 * Typedefs:
 *  - INODE   : Alias for the struct inode.
 *  - PINODE  : Pointer to an INODE structure.
 *  - PPINODE : Pointer to a pointer to an INODE (used in some functions).
 */
typedef struct inode
{
    char FileName[50];
    int InodeNumber;
    int FileSize;
    int FileActualSize;
    int FileType;
    char *Buffer;
    int LinkCount;
    int ReferenceCount;
    int Permission;                     
    struct inode *next;
}INODE,*PINODE,**PPINODE;


/*
 * Structure: filetable
 * --------------------
 * This structure holds information about an open file.
 * It acts like a file descriptor entry and tracks how the file is being accessed.
 *
 * Fields:
 *  - readoffset   : Current position for reading from the file.
 *  - writeoffset  : Current position for writing into the file.
 *  - count        : Number of references to this file table (used for tracking open instances).
 *  - mode         : Access mode (READ, WRITE, or READ + WRITE).
 *  - ptrinode     : Pointer to the inode representing the actual file.
 *
 * Typedefs:
 *  - FILETABLE   : Alias for the struct filetable.
 *  - PFILETABLE  : Pointer to a FILETABLE structure.
 */
typedef struct filetable
{
    int readoffset;
    int writeoffset;
    int count;
    int mode;                            
    PINODE ptrinode;
}FILETABLE, *PFILETABLE;


/*
 * Structure: ufdt (User File Descriptor Table)
 * -------------------------------------------
 * This structure represents a single entry in the user file descriptor table (UFDT).
 * It links a file descriptor to its corresponding file table entry.
 *
 * Fields:
 *  - ptrfiletable : Pointer to the FILETABLE structure associated with this file descriptor.
 *
 * Typedef:
 *  - UFDT : Alias for the struct ufdt.
 */

typedef struct ufdt
{
    PFILETABLE ptrfiletable;
}UFDT;



/*
 * Global Variables:
 * -----------------
 * UFDTArr         : Array of 50 User File Descriptor Table entries. Each entry stores 
 *                   a pointer to the filetable of an open file, simulating file descriptors.
 *
 * SUPERBLOCKobj   : An instance of the SUPERBLOCK structure that maintains metadata about 
 *                   total and available inodes in the virtual file system.
 *
 * head            : Pointer to the head of the linked list of inodes (Disk Inode List Block),
 *                   used to manage metadata for all files in the system.
 */
UFDT UFDTArr[50];
SUPERBLOCK SUPERBLOCKobj;
PINODE head = NULL;





/*
 * Function: man
 * -------------
 * Displays a short description and usage instructions for supported commands
 * in the virtual file system.
 *
 * @param name - The name of the command for which help is requested.
 *
 * If the command is recognized, its purpose and syntax are printed.
 * Otherwise, an error message is shown.
 */

void man(char *name)
{
    if(name == NULL) return;

    if(strcmp(name, "create") == 0)
    {
        printf("Description : Used to create a new file\n");
        printf("Usage : create File_name No_of_Bytes_To_Create\n");
    }
    else if (strcmp(name, "read") == 0)
    {
        printf("Description : Used to read data from regular file\n");
        printf("Usage : read File_name No_Of_Bytes_To_Read\n");
    }
    else if(strcmp(name, "write") == 0)
    {
        printf("Description : Used to write data into a regular file\n");
        printf("Usage : write File_name\n After this enter the data that we want to write\n");
    }
    else if(strcmp(name, "ls") == 0) 
    {
        printf("Description : Used to list all information of file\n");
        printf("Usage : ls\n"); 
    }
    else if(strcmp(name, "stat") == 0)
    {
        printf("Description : Used to display information of file\n");
        printf("Usage : stat File_Name\n");
    }
    else if(strcmp(name, "fstat") == 0)
    {
        printf("Description : Used to display information of file from file descriptor\n");
        printf("Usage : fstat File_Description\n");
    }
    else if(strcmp(name, "truncate") == 0)
    {
        printf("Description : Used to remove data from file\n");
        printf("Usage : truncate File_name\n");
    }
    else if(strcmp(name, "open") == 0)
    {
        printf("Description : Used to open an existing file\n");
        printf("Usage : open File_name\n");
    }
    else if(strcmp(name, "close") == 0)
    {
        printf("Description : Used to close an opened file\n");
        printf("Usage : close File_name\n");
    }
    else if(strcmp(name, "closeall") == 0)
    {
        printf("Description : Used to close all opened files\n");
        printf("Usage : closeall\n");
    }
    else if(strcmp(name, "lseek") == 0) 
    {
        printf("Description : Used to change file offset\n");
        printf("Usage : lseek File_Name ChangeInOffset StartPoint\n");
    }
    else if(strcmp(name, "rm") == 0)
    {
        printf("Description : Used to delete the file\n");
        printf("Usage : rm FileName\n");
    }
    else
    {
        printf("ERROR : No manual entry available.\n");
    }
}



/*
 * Function: DisplayHelp
 * ---------------------
 * Prints a list of all available commands in the virtual file system along with
 * a brief description of what each command does.
 *
 * This function acts as a built-in help guide for users.
 */

void DisplayHelp()
{
    printf("ls : To List out all files\n");
    printf("clear : To clear console\n");
    printf("open : To open the file\n");
    printf("close : To close the file\n");
    printf("closeall : To close all opened files\n");
    printf("read : To read the contents from file\n");
    printf("write : To write contents into file\n");
    printf("exit : To terminate file system\n");
    printf("stat : To display information of file using name\n");
    printf("fstat : To display information of file using file descriptor\n");
    printf("truncate : To remove all data from file\n");
    printf("rm : To delete the file\n");
}


/*
 * Function: GetFDFromName
 * -----------------------
 * Searches the UFDT (User File Descriptor Table) to find the file descriptor
 * associated with the given file name.
 *
 * @param name - The name of the file to search for.
 *
 * @return - The file descriptor (index in UFDTArr) if the file is found,
 *           or -1 if the file does not exist or is not currently opened.
 */
int GetFDFromName(char *name)
{
    int i = 0;

    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable != NULL)
            if(strcmp((UFDTArr[i].ptrfiletable->ptrinode->FileName), name) == 0)
                break;
        i++;
    }

    if(i == 50)         return -1;  // File descriptor not found
    else                return i;   // Return file descriptor
}



/*
 * Function: Get_Inode
 * -------------------
 * Searches the linked list of inodes to find a file with the given name.
 *
 * @param name - The name of the file to look for.
 *
 * @return - Pointer to the inode (PINODE) if the file is found,
 *           or NULL if the file does not exist or if the input name is NULL.
 */
PINODE Get_Inode(char *name)
{
    PINODE temp = head;
    int i = 0;

    if (name == NULL)
        return NULL;  // Return NULL if the name is NULL

    while(temp != NULL)
    {
        if(strcmp(name, temp->FileName) == 0)  // Fix incorrect arrow usage
            break;
        temp = temp->next;
    }

    return temp;  // Return the found inode or NULL if not found
}

/*
 * Function: CreateDILB
 * --------------------
 * Creates the Disk Inode List Block (DILB), which is a linked list of inodes.
 * Initializes all inodes with default values and assigns unique inode numbers.
 *
 * This function sets up the file system’s basic structure for managing files.
 *
 * Prints a success message once all inodes are created.
 */
void CreateDILB()
{
    int i = 1;
    PINODE newn = NULL;
    PINODE temp = head;

    while(i <= MAXINODE)
    {
        newn = (PINODE)malloc(sizeof(INODE));
        
        // Check if memory allocation was successful
        if (newn == NULL)
        {
            printf("Memory allocation failed for inode %d\n", i);
            return;
        }

        // Initialize inode fields
        newn->LinkCount = 0;
        newn->ReferenceCount = 0;
        newn->FileType = 0;
        newn->FileSize = 0;
        newn->Buffer = NULL;
        newn->InodeNumber = i;
        newn->next = NULL;  // No need to initialize this, malloc does it by default.

        if(temp == NULL)
        {
            head = newn;
            temp = head;
        }
        else
        {
            temp->next = newn;
            temp = temp->next;
        }
        i++;
    }
    printf("DILB created successfully\n");
}

/*
 * Function: InitialiseSuperBlock
 * ------------------------------
 * Initializes the SuperBlock and User File Descriptor Table (UFDT).
 *
 * - Sets all entries in the UFDT array to NULL (no files are open initially).
 * - Sets the total and free inode count in the superblock to MAXINODE.
 *
 * This function prepares the file system for use by resetting its metadata.
 */
void InitialiseSuperBlock()
{
    int i = 0;
    while(i < MAXINODE)
    {
        UFDTArr[i].ptrfiletable = NULL;
        i++;
    }

    SUPERBLOCKobj.TotalInodes = MAXINODE;
    SUPERBLOCKobj.FreeInodes = MAXINODE;
}



/*
 * Function: CreateFile
 * --------------------
 * Creates a new regular file in the virtual file system with the given name and permission.
 * Allocates an inode, initializes metadata, and assigns a file descriptor entry.
 *
 * @param name       - The name of the file to be created.
 * @param permission - Access permission (1 = Read, 2 = Write, 3 = Read + Write).
 *
 * @return 
 *  >= 0  : File descriptor index if the file is successfully created.
 *   -1   : Invalid parameters (null name or incorrect permission).
 *   -2   : No free inodes available.
 *   -3   : File with the same name already exists.
 *   -4   : No available inode slot found.
 *   -5   : No available file descriptor slot in UFDT.
 *   -6   : Memory allocation failed for file table.
 *   -7   : Memory allocation failed for file data buffer.
 */

int CreateFile(char *name, int permission)
{
    int i = 0;
    PINODE temp = head;

    // Check if name is valid, permission is in the range 1 to 3
    if ((name == NULL) || (permission == 0) || (permission > 3))
        return -1;  // Invalid input

    // Check if there are free inodes
    if (SUPERBLOCKobj.FreeInodes == 0)
        return -2;  // No free inodes available

    // Decrement free inodes
    SUPERBLOCKobj.FreeInodes--;

    // Check if file with the same name already exists
    if (Get_Inode(name) != NULL)
        return -3;  // File already exists

    // Find an available inode with FileType 0 (empty slot)
    while (temp != NULL)
    {
        if (temp->FileType == 0)  // Found an empty inode slot
            break;
        temp = temp->next;
    }

    // If no empty inode slot was found
    if (temp == NULL)
        return -4;  // No available inode for new file

    // Find an available slot in the UFDT array
    while (i < 50)
    {
        if (UFDTArr[i].ptrfiletable == NULL)  // Found an empty slot in UFDT
            break;
        i++;
    }

    // If no available file descriptor slot was found
    if (i == 50)
        return -5;  // No available file descriptor slot

    // Allocate memory for the file table entry
    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
    if (UFDTArr[i].ptrfiletable == NULL)  // Check malloc success
    {
        printf("Memory allocation failed for file table entry.\n");
        return -6;  // Memory allocation failed
    }

    // Initialize the file table entry
    UFDTArr[i].ptrfiletable->count = 1;
    UFDTArr[i].ptrfiletable->mode = permission;
    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;

    UFDTArr[i].ptrfiletable->ptrinode = temp;

    // Assign the file name and initialize inode attributes
    strcpy(UFDTArr[i].ptrfiletable->ptrinode->FileName, name);
    UFDTArr[i].ptrfiletable->ptrinode->FileType = REGULAR;
    UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->LinkCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->FileSize = MAXFILESIZE;
    UFDTArr[i].ptrfiletable->ptrinode->FileActualSize = 0;
    UFDTArr[i].ptrfiletable->ptrinode->Permission = permission;

    // Allocate memory for the file data buffer
    UFDTArr[i].ptrfiletable->ptrinode->Buffer = (char *)malloc(MAXFILESIZE);
    if (UFDTArr[i].ptrfiletable->ptrinode->Buffer == NULL)  // Check malloc success
    {
        printf("Memory allocation failed for file buffer.\n");
        free(UFDTArr[i].ptrfiletable);  // Clean up already allocated memory
        return -7;  // Memory allocation failed for file buffer
    }

    return i;  // Return the file descriptor index
}

/*
 * Function: rm_File
 * -----------------
 * Deletes the specified file from the virtual file system.
 * Decreases its link count, and if it reaches zero, frees its resources
 * (buffer, file table, and marks inode as free).
 *
 * @param name - Name of the file to be deleted.
 *
 * @return 
 *   0  : File successfully deleted or unlinked.
 *  -1  : File not found (invalid name or not open).
 */
int rm_File(char *name)
{
    int fd = 0;

    fd = GetFDFromName(name);
    if(fd == -1)
        return -1;  // File descriptor not found

    // Decrease link count of the inode
    (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;

    // If no more links, delete the file
    if(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount == 0)
    {
        UFDTArr[fd].ptrfiletable->ptrinode->FileType = 0;  // Mark inode as unused
        free(UFDTArr[fd].ptrfiletable->ptrinode->Buffer);  // Free file data buffer
        free(UFDTArr[fd].ptrfiletable);  // Free the file table entry
    }

    // Set the file descriptor slot to NULL
    UFDTArr[fd].ptrfiletable = NULL;

    // Increment free inodes in superblock
    (SUPERBLOCKobj.FreeInodes)++;

    return 0;  // Success
}


/*
 * Function: ReadFile
 * ------------------
 * Reads data from a file into the provided buffer, starting from the current read offset.
 *
 * @param fd    - File descriptor from which to read.
 * @param arr   - Buffer where the read data will be stored.
 * @param isize - Number of bytes to read.
 *
 * @return 
 *  > 0  : Number of bytes actually read.
 *  -1   : Invalid file descriptor or file not opened in readable mode.
 *  -2   : Read permission denied.
 *  -3   : End of file reached.
 *  -4   : File is not a regular file.
 */
int ReadFile(int fd, char *arr, int isize)
{
    int read_size = 0;

    // Check if file descriptor is valid
    if(UFDTArr[fd].ptrfiletable == NULL)
        return -1;  // Invalid file descriptor

    // Check if file is in read or read-write mode
    if(UFDTArr[fd].ptrfiletable->mode != READ && UFDTArr[fd].ptrfiletable->mode != (READ + WRITE))
        return -1;  // Invalid file mode

    // Check if user has permission to read
    if(UFDTArr[fd].ptrfiletable->ptrinode->Permission != READ && UFDTArr[fd].ptrfiletable->ptrinode->Permission != (READ + WRITE))
        return -2;  // Permission denied

    // Check if read offset is at the end of the file
    if(UFDTArr[fd].ptrfiletable->readoffset == UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
        return -3;  // End of file reached

    // Check if the file is of regular type
    if(UFDTArr[fd].ptrfiletable->ptrinode->FileType != REGULAR)
        return -4;  // Invalid file type

    // Calculate the actual number of bytes to read
    read_size = UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize - UFDTArr[fd].ptrfiletable->readoffset;
    
    // Ensure we don't read beyond the available file data
    if(read_size < isize)
    {
        // Copy the data into the provided buffer
        strncpy(arr, UFDTArr[fd].ptrfiletable->ptrinode->Buffer + UFDTArr[fd].ptrfiletable->readoffset, read_size);

        // Update the read offset
        UFDTArr[fd].ptrfiletable->readoffset += read_size;
    }
    else
    {
        // Copy the exact amount requested
        strncpy(arr, UFDTArr[fd].ptrfiletable->ptrinode->Buffer + UFDTArr[fd].ptrfiletable->readoffset, isize);

        // Update the read offset
        UFDTArr[fd].ptrfiletable->readoffset += isize;
    }

    // Return the actual number of bytes read
    return read_size < isize ? read_size : isize;
}


/*
 * Function: WriteFile
 * -------------------
 * Writes data from the provided buffer into the file starting at the current write offset.
 *
 * @param fd    - File descriptor of the file to write to.
 * @param arr   - Data buffer to be written into the file.
 * @param isize - Number of bytes to write.
 *
 * @return 
 *  > 0  : Number of bytes successfully written.
 *  -1   : Invalid file descriptor or write permission denied.
 *  -2   : File has reached its maximum size.
 *  -3   : File is not a regular file.
 */
int WriteFile(int fd, char *arr, int isize)
{
    // Check if file is in correct mode for writing
    if ((UFDTArr[fd].ptrfiletable->mode != WRITE) && (UFDTArr[fd].ptrfiletable->mode != (READ + WRITE)))
        return -1;  // Invalid mode for writing

    // Check if user has write permission
    if ((UFDTArr[fd].ptrfiletable->ptrinode->Permission != WRITE) && (UFDTArr[fd].ptrfiletable->ptrinode->Permission != (READ + WRITE)))
        return -1;  // Permission denied

    // Check if the file size exceeds the limit
    if (UFDTArr[fd].ptrfiletable->writeoffset == MAXFILESIZE)
        return -2;  // File is full

    // Check if the file is of regular type
    if (UFDTArr[fd].ptrfiletable->ptrinode->FileType != REGULAR)
        return -3;  // Invalid file type

    // Ensure we don't write past the maximum file size
    if (UFDTArr[fd].ptrfiletable->writeoffset + isize > MAXFILESIZE)
    {
        isize = MAXFILESIZE - UFDTArr[fd].ptrfiletable->writeoffset;  // Adjust write size
    }

    // Write data into the buffer
    strncpy(UFDTArr[fd].ptrfiletable->ptrinode->Buffer + UFDTArr[fd].ptrfiletable->writeoffset, arr, isize);

    // Update the write offset
    UFDTArr[fd].ptrfiletable->writeoffset += isize;

    // Update the actual file size
    UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize += isize;

    return isize;  // Return the number of bytes written
}



/*
 * Function: OpenFile
 * ------------------
 * Opens an existing file with the specified access mode and creates
 * an entry in the User File Descriptor Table (UFDT).
 *
 * @param name - Name of the file to open.
 * @param mode - Mode to open the file in (1 = Read, 2 = Write, 3 = Read + Write).
 *
 * @return 
 *  >= 0 : File descriptor index if the file is successfully opened.
 *   -1  : Invalid parameters (null name or invalid mode).
 *   -2  : File does not exist.
 *   -3  : Permission denied.
 *   -4  : No free file descriptor available.
 *   -5  : Memory allocation failure.
 */
int OpenFile(char *name, int mode)
{
    int i = 0;
    PINODE temp = NULL;

    // Check if the name is valid and mode is positive
    if (name == NULL || mode <= 0)
        return -1;  // Invalid input

    // Get inode based on file name
    temp = Get_Inode(name);
    if (temp == NULL)
        return -2;  // File not found

    // Check if the file has the required permission
    if ((temp->Permission & mode) != mode)
        return -3;  // Permission denied

    // Find an empty slot in UFDT array
    while (i < 50)
    {
        if (UFDTArr[i].ptrfiletable == NULL)
            break;
        i++;
    }

    // If no free slots available in UFDT array
    if (i == 50)
        return -4;  // No free file descriptor

    // Allocate memory for the file table entry
    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
    if (UFDTArr[i].ptrfiletable == NULL)
        return -5;  // Memory allocation failure

    // Initialize the file table entry
    UFDTArr[i].ptrfiletable->count = 1;
    UFDTArr[i].ptrfiletable->mode = mode;

    // Initialize read and write offsets based on the mode
    if (mode == (READ + WRITE))
    {
        UFDTArr[i].ptrfiletable->readoffset = 0;
        UFDTArr[i].ptrfiletable->writeoffset = 0;
    }
    else if (mode == READ)
    {
        UFDTArr[i].ptrfiletable->readoffset = 0;
    }
    else if (mode == WRITE)
    {
        UFDTArr[i].ptrfiletable->writeoffset = 0;
    }

    // Link the file table entry to the inode
    UFDTArr[i].ptrfiletable->ptrinode = temp;

    // Increment the reference count of the inode
    UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount++;

    return i;  // Return the file descriptor index
}


/*
 * Function: CloseFileByName
 * -------------------------
 * Closes the file associated with the given file descriptor.
 * Resets read/write offsets and decreases the reference count of the inode.
 * If no references remain, frees the file table entry.
 *
 * @param fd - File descriptor of the file to close.
 */
void CloseFileByName(int fd)
{
    if (UFDTArr[fd].ptrfiletable == NULL)
        return;

    // Reset the file offsets
    UFDTArr[fd].ptrfiletable->readoffset = 0;
    UFDTArr[fd].ptrfiletable->writeoffset = 0;

    // Decrease reference count
    (UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount)--;

    // If no references are left, free the file table entry
    if (UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount == 0)
    {
        free(UFDTArr[fd].ptrfiletable);
        UFDTArr[fd].ptrfiletable = NULL;
    }
}



/*
 * Function: CloseFileByName
 * -------------------------
 * Closes the file with the given name by locating its file descriptor
 * and delegating to the descriptor-based close function.
 *
 * @param name - Name of the file to be closed.
 *
 * @return 
 *   0  : File closed successfully.
 *  -1  : File not found (invalid or unopened).
 */

int CloseFileByName(char *name)
{
    int i = 0;
    i = GetFDFromName(name);

    if (i == -1)
        return -1;  // File not found

    // Close the file by its index
    CloseFileByName(i);

    return 0;
}



/*
 * Function: CloseAllFile
 * ----------------------
 * Closes all currently opened files in the system by iterating through
 * the User File Descriptor Table (UFDT) and calling the file-close function
 * for each active entry.
 *
 * This helps to release resources and reset the system state.
 */
void CloseAllFile()
{
    int i = 0;
    while (i < 50)
    {
        if (UFDTArr[i].ptrfiletable != NULL)
        {
            CloseFileByName(i);  // Close file at index i
        }
        i++;
    }
}



/*
 * Function: LseekFile
 * -------------------
 * Changes the current read or write offset in an open file, similar to the lseek() system call.
 * The new position is calculated relative to the start, current position, or end of the file.
 *
 * @param fd    - File descriptor of the file.
 * @param size  - Number of bytes to move the offset by.
 * @param from  - Reference point for the offset (0 = START, 1 = CURRENT, 2 = END).
 *
 * @return 
 *   0  : Offset successfully updated.
 *  -1  : Invalid parameters, invalid file descriptor, or operation out of bounds.
 *
 * Notes:
 * - For read mode, it updates the read offset.
 * - For write mode, it updates the write offset.
 * - Does not allow seeking beyond file size or below zero.
 */
int LseekFile(int fd, int size, int from)
{
    if (fd < 0 || from > 2) 
        return -1;

    if (UFDTArr[fd].ptrfiletable == NULL) 
        return -1;  // Invalid file descriptor

    // Reading
    if ((UFDTArr[fd].ptrfiletable->mode == READ) || (UFDTArr[fd].ptrfiletable->mode == (READ + WRITE)))
    {
        if (from == CURRENT)
        {
            if (((UFDTArr[fd].ptrfiletable->readoffset) + size) > UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) 
                return -1;  // Beyond actual file size

            if (((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0) 
                return -1;  // Negative offset is invalid

            UFDTArr[fd].ptrfiletable->readoffset += size;  // Update read offset
        }
        else if (from == START)
        {
            if (size > UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
                return -1;  // Beyond actual file size
            if (size < 0)
                return -1;  // Invalid size

            UFDTArr[fd].ptrfiletable->readoffset = size;  // Set the read offset to start from `size`
        }
        else if (from == END)
        {
            if ((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE) 
                return -1;  // Exceeds max file size

            if (((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0) 
                return -1;  // Negative offset is invalid

            UFDTArr[fd].ptrfiletable->readoffset = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;  // Set the read offset from the end
        }
    }
    // Writing
    else if (UFDTArr[fd].ptrfiletable->mode == WRITE)
    {
        if (from == CURRENT)
        {
            if (((UFDTArr[fd].ptrfiletable->writeoffset) + size) > MAXFILESIZE)
                return -1;  // Exceeds max file size

            if (((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0)
                return -1;  // Negative offset is invalid

            if (((UFDTArr[fd].ptrfiletable->writeoffset) + size) > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)) 
                UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize = (UFDTArr[fd].ptrfiletable->writeoffset) + size;

            UFDTArr[fd].ptrfiletable->writeoffset += size;  // Update write offset
        }
        else if (from == START)
        {
            if (size > MAXFILESIZE) 
                return -1;  // Invalid size

            if (size < 0) 
                return -1;  // Invalid size

            if (size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)) 
                UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize = size;  // Adjust file size if necessary

            UFDTArr[fd].ptrfiletable->writeoffset = size;  // Set the write offset to `size`
        }
        else if (from == END)
        {
            if ((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE) 
                return -1;  // Exceeds max file size

            if (((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0) 
                return -1;  // Negative offset is invalid

            UFDTArr[fd].ptrfiletable->writeoffset = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;  // Set the write offset from the end
        }
    }

    return 0;
}



/*
 * Function: ls_file
 * -----------------
 * Lists all files currently present in the virtual file system.
 * It prints each file's name, inode number, actual size, and link count.
 *
 * If no files are present, it displays an appropriate message.
 */

void ls_file()
{
    PINODE temp = head;

    if (SUPERBLOCKobj.FreeInodes == MAXINODE)
    {
        printf("Error: There are no files\n");
        return;
    }

    printf("\nFile Name\tInode number\tFile size\tLink count\n");
    printf("--------------------------------------------------------\n");

    while (temp != NULL)
    {
        if (temp->FileType != 0)  
        {
            printf("%s\t\t%d\t\t%d\t\t%d\n", temp->FileName, temp->InodeNumber, temp->FileActualSize, temp->LinkCount);
        }
        temp = temp->next;
    }
    printf("-------------------------------------\n");
}



/*
 * Function: fstat_file
 * --------------------
 * Displays metadata about an open file using its file descriptor.
 * Prints details such as file name, inode number, file size, link count,
 * reference count, and permissions.
 *
 * @param fd - File descriptor of the file to be inspected.
 *
 * @return 
 *   0  : Success.
 *  -1  : Invalid file descriptor (negative value).
 *  -2  : File not currently open (null filetable entry).
 */
int fstat_file(int fd)
{
    PINODE temp = head;

    if (fd < 0) return -1;
    if (UFDTArr[fd].ptrfiletable == NULL) return -2;

    temp = UFDTArr[fd].ptrfiletable->ptrinode;

    printf("\n---------------------- Statistical Information about file------------------\n");
    printf("File name: %s\n", temp->FileName);
    printf("Inode Number: %d\n", temp->InodeNumber);
    printf("File size: %d\n", temp->FileActualSize);  
    printf("Actual File size: %d\n", temp->FileActualSize);
    printf("Link count: %d\n", temp->LinkCount);
    printf("Reference count: %d\n", temp->ReferenceCount);

   
    if (temp->Permission == 1)
        printf("File Permission: Read only\n");
    else if (temp->Permission == 2)
        printf("File Permission: Write\n");
    else if (temp->Permission == 3)
        printf("File Permission: Read & Write\n");

    printf("------------------\n\n");

    return 0;
}


/*
 * Function: stat_file
 * -------------------
 * Displays metadata about a file using its name.
 * Searches the inode list to find the file and prints information such as
 * name, inode number, size, link count, reference count, and permissions.
 *
 * @param name - Name of the file to inspect.
 *
 * @return 
 *   0  : Success.
 *  -1  : Invalid input (null file name).
 *  -2  : File not found in the inode list.
 */
int stat_file(char *name)
{
    PINODE temp = head;

    if (name == NULL) return -1;

    while (temp != NULL)
    {
        if (strcmp(name, temp->FileName) == 0)
            break;
        temp = temp->next;
    }

    if (temp == NULL) return -2;

    printf("\nStatistical Information about file-------\n");
    printf("File name: %s\n", temp->FileName);
    printf("Inode Number: %d\n", temp->InodeNumber);
    printf("File size: %d\n", temp->FileActualSize);
    printf("Actual File size: %d\n", temp->FileActualSize);
    printf("Link count: %d\n", temp->LinkCount);
    printf("Reference count: %d\n", temp->ReferenceCount);

    
    if (temp->Permission == 1)
        printf("File Permission: Read only\n");
    else if (temp->Permission == 2)
        printf("File Permission: Write\n");
    else if (temp->Permission == 3)
        printf("File Permission: Read & Write\n");

    printf("-------------\n\n");

    return 0;
}



/*
 * Function: truncate_File
 * -----------------------
 * Removes all data from the specified file without deleting the file itself.
 * Resets the file's buffer, read/write offsets, and actual size to zero.
 *
 * @param name - Name of the file to truncate.
 *
 * @return 
 *   0  : File successfully truncated.
 *  -1  : File not found or not open.
 */
int truncate_File(char *name)
{
    int fd = GetFDFromName(name);
    if (fd == -1)
        return -1;

    // Clear the file buffer and reset offsets
    memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer, 0, MAXFILESIZE);
    UFDTArr[fd].ptrfiletable->readoffset = 0;
    UFDTArr[fd].ptrfiletable->writeoffset = 0;
    UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize = 0;

    return 0;  
}



/*
 * Function: main
 * --------------
 * Entry point of the Customized Virtual File System (VFS).
 * Initializes system components like the SuperBlock and Inode list, 
 * then continuously accepts user commands to perform file system operations.
 *
 * The main function supports a command-line interface where users can:
 *  - create, open, read, write, truncate, and delete files
 *  - view file metadata using stat/fstat
 *  - close single or all files
 *  - seek within files using lseek
 *  - display help, manual pages, and list files
 *  - exit the virtual file system
 *
 * Command parsing is done using sscanf() with support for up to 4 arguments.
 *
 * @return 0 on successful program termination.
 */
int main()
{
    char *ptr = NULL;
    int ret = 0, fd = 0, count = 0;
    char command[4][80], str[80], arr[1024];

    InitialiseSuperBlock();
    CreateDILB();

    while(1)
    {
        fflush(stdin);
        strcpy(str, "");

        printf("\n Customized Virtual File System: >");

        fgets(str, 80, stdin);  // Use fgets instead of scanf

        count = sscanf(str, "%s %s %s %s", command[0], command[1], command[2], command[3]);

        if(count == 1)
        {
            if(strcmp(command[0], "ls") == 0)
            {
                ls_file();
            } 
            else if(strcmp(command[0], "closeall") == 0)
            {
                CloseAllFile();
                printf("All files closed successfully\n");
                continue;
            }
            else if(strcmp(command[0], "clear") == 0)
            {
                system("cls");
                continue;
            }
            else if(strcmp(command[0], "help") == 0)
            {
                DisplayHelp();  // Implement this function
                continue;
            }
            else if(strcmp(command[0], "exit") == 0)
            {
                printf("Terminating the Customized Virtual File System\n");
                break;
            }
            else
            {
                printf("\nERROR: Command not found !!!\n"); 
                continue;
            }
        }
        else if(count == 2)
        {
           if(strcmp(command[0], "stat") == 0)
            {
                ret = stat_file(command[1]);  // ✅ FIXED
                if(ret == -1) 
                    printf("ERROR: Incorrect parameters\n");
                if(ret == -2) 
                    printf("ERROR: There is no such file\n"); 
                continue;
            }

            else if(strcmp(command[0], "fstat") == 0)
            {
                ret = fstat_file(atoi(command[1])); 
                if(ret == -1) 
                    printf("ERROR: Incorrect parameters\n");
                if(ret == -2) 
                    printf("ERROR: There is no such file\n");
                continue;
            }


            else if(strcmp(command[0], "close") == 0) 
            {
                ret = CloseFileByName(command[1]);  // Corrected function name
                if(ret == -1) 
                    printf("ERROR: There is no such file\n"); 
                continue;
            }
            else if(strcmp(command[0], "rm") == 0)
            {
                ret = rm_File(command[1]);
                if(ret == -1)
                    printf("ERROR: There is no such file\n");
                continue;
            }
            else if(strcmp(command[0], "man") == 0)
            {
                man(command[1]);  // Ensure 'man' function is implemented
            }
            else if(strcmp(command[0], "write") == 0)
            {
                fd = GetFDFromName(command[1]);
                if(fd == -1)
                { 
                    printf("ERROR: Incorrect parameter\n"); 
                    continue;
                }
                printf("Enter the data: \n"); 
                scanf("%[^\n]", arr);

                ret = strlen(arr);
                if(ret == 0) 
                {
                    printf("ERROR: Incorrect parameter\n");
                    continue;
                }
                ret = WriteFile(fd, arr, ret);
                if(ret == -1)
                    printf("ERROR: Permission denied\n");
                if(ret == -2)
                    printf("ERROR: There is no sufficient memory to write\n");
                if(ret == -3)
                    printf("ERROR: It is not a regular file\n");
            }
            else if(strcmp(command[0], "truncate") == 0) 
            {
                ret = truncate_File(command[1]);
                if(ret == -1)
                    printf("ERROR: Incorrect parameter\n");
            }
            else
            {
                printf("\nERROR: Command not found !!!\n");
                continue;
            }
        }
        else if(count == 3)
        {
            if(strcmp(command[0], "create") == 0) 
            {
                ret = CreateFile(command[1], atoi(command[2]));
                if(ret >= 0)
                    printf("File is successfully created with file descriptor: %d\n", ret);
                if(ret == -1)
                    printf("ERROR: Incorrect parameters\n");
                if(ret == -2)
                    printf("ERROR: There are no inodes\n");
                if(ret == -3)
                    printf("ERROR: File already exists\n");
                if(ret == -4)
                    printf("ERROR: Memory allocation failure\n");
                continue;
            }
            else if(strcmp(command[0], "open") == 0) 
            {
                ret = OpenFile(command[1], atoi(command[2])); 
                if(ret >= 0)
                    printf("File is successfully opened with file descriptor: %d\n", ret);
                if(ret == -1)
                    printf("ERROR: Incorrect parameters\n");
                if(ret == -2)
                    printf("ERROR: File not present\n"); 
                if(ret == -3)
                    printf("ERROR: Permission denied\n");
                continue;
            }
            else if(strcmp(command[0], "read") == 0)
{
    if(count != 3)  // Expecting: read <fd> <size>
    {
        printf("ERROR: Incorrect parameter count\n");
        continue;
    }

    int fd = atoi(command[1]);  // Convert string to file descriptor
    if(fd < 0 || fd >= 50 || UFDTArr[fd].ptrfiletable == NULL)
    {
        printf("ERROR: Incorrect parameter\n");
        continue;
    }

    int size = atoi(command[2]);
    if(size <= 0)
    {
        printf("ERROR: Invalid size\n");
        continue;
    }

    char *ptr = (char *)malloc(size + 1);
    if(ptr == NULL)
    {
        printf("ERROR: Memory allocation failure\n");
        continue;
    }

    int ret = ReadFile(fd, ptr, size);
    if(ret == -1)
        printf("ERROR: File not existing\n");
    else if(ret == -2)
        printf("ERROR: Permission denied\n");
    else if(ret == -3)
        printf("ERROR: Reached end of file\n");
    else if(ret == -4)
        printf("ERROR: It is not a regular file\n");
    else if(ret == 0)
        printf("ERROR: File is empty\n");
    else if(ret > 0)
    {
        ptr[ret] = '\0';  // Null-terminate the string
        printf("Data Read: %s\n", ptr);
    }

    free(ptr);  // Free allocated memory
    continue;
}

            else
            {
                printf("\nERROR: Command not found !!!\n");
                continue;
            }
        }
        else if(count == 4)
        {
            if(strcmp(command[0], "Iseek") == 0) 
            {
                fd = GetFDFromName(command[1]);
                if(fd == -1)
                {
                    printf("ERROR: Incorrect parameter\n");
                    continue;
                }
                ret = LseekFile(fd, atoi(command[2]), atoi(command[3]));
                if(ret == -1)
                {
                    printf("ERROR: Unable to perform Iseek\n");
                }
                continue;
            }
            else
            {
                printf("\nERROR: Command not found !!!\n");
                continue;
            }
        }
    }
    return 0;
}
