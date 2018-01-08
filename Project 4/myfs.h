#ifndef MYFS_H
#define MYFS_H

#define BLOCKSIZE          4096     // bytes
#define MAXFILECOUNT       128      // files
#define DISKSIZE         (1<<27)  // 256 MB
#define MAXFILENAMESIZE    32  // characters - max that FS can support
#define BLOCKCOUNT      (DISKSIZE / BLOCKSIZE)
#define MAXOPENFILES       64      // files
#define MAXREADWRITE      1024     // bytes; max read/write amount




struct openfiletable{
    int filepositionpointer;
    int fileopencount;
    int disklocationoffile;
    int accessmode;
    char* name;
};

struct directory{
    char* name;
    int permission;
    int fileBlock;
    int filesize;
    struct file* files;
};


struct file{
    char* name;
    int permission;
    int fileBlock;
    int size;
};

struct fileFixed{
    char name[32];
    int permission;
    int fileBlock;
    int size;
};

struct directoryFixed{
    char name[32];
    int permission;
    int fileBlock;
    int filesize;
    struct fileFixed *files;
};

struct superblockFixed{
    float versionNumber;
    int size;
    int directoryLocation;
    int fileLocation;
    int fatlocation;
    int firstData;

};

struct superblock{
    float versionNumber;
    int size;
    struct directory* root;
    int firstData;
    int fatlocation;
};

struct mounted {
    struct superblockFixed *sb;
    struct fileFixed* files;
    struct directoryFixed *root;
    int16_t *fattable;
    struct openfiletable* oft;
};



int getblock (int blocknum, void *buf);

// The following will be use to create and format a disk
int myfs_diskcreate(char *diskname);
int myfs_makefs (char *diskname);

// The following will be used by a program to work with files
int myfs_mount (char *vdisk);
int myfs_umount();

int myfs_create(char *filename);
int myfs_open(char *filename);
int myfs_close(int fd);
int myfs_delete(char *filename);
int myfs_read(int fd, void *buf, int n);
int myfs_write(int fd, void *buf, int n);
int myfs_truncate(int fd, int size);
int myfs_seek(int fd, int offset);
int myfs_filesize(int fd);
void myfs_print_dir();
void myfs_print_blocks(char *filename);

int getfreeblock();

#endif