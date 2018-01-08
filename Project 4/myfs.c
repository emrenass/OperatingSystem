#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "myfs.h"

// Global Variables
char disk_name[128];   // name of virtual disk file
int  disk_size;        // size in bytes - a power of 2
int  disk_fd;          // disk file handle
int  disk_blockcount;  // block count on disk
char* buf[BLOCKSIZE];
struct openfiletable* oft;
struct mounted* moun;


/*
   Reads block blocknum into buffer buf.
   You will not modify the getblock() function.
   Returns -1 if error. Should not happen.
*/
int getblock (int blocknum, void *buf)
{
    int offset, n;

    if (blocknum >= disk_blockcount)
        return (-1); //error

    offset = lseek (disk_fd, blocknum * BLOCKSIZE, SEEK_SET);
    if (offset == -1) {
        printf ("lseek error\n");
        exit(0);

    }

    n = read (disk_fd, buf, BLOCKSIZE);
    if (n != BLOCKSIZE)
        return (-1);

    return (0);
}


/*
    Puts buffer buf into block blocknum.
    You will not modify the putblock() function
    Returns -1 if error. Should not happen.
*/
int putblock (int blocknum, void *buf)
{
    int offset, n;

    if (blocknum >= disk_blockcount)
        return (-1); //error

    offset = lseek (disk_fd, blocknum * BLOCKSIZE, SEEK_SET);
    if (offset == -1) {
        printf ("lseek error\n");
        exit (1);
    }

    n = write (disk_fd, buf, BLOCKSIZE);
    if (n != BLOCKSIZE)
        return (-1);

    return (0);
}

int getfreeblock() {
    int offset, n;

    int16_t * fat = moun->fattable;

    for(int i = BLOCKCOUNT/4; i < BLOCKCOUNT; i++){
        if(fat[i]==-1) {
            return i;
        }
    }
    return -1;

    //getblock(moun.)
}


/*
   IMPLEMENT THE FUNCTIONS BELOW - You can implement additional
   internal functions.
 */


int myfs_diskcreate (char *vdisk)
{
    int n, size ,ret, i;
    int fd;
    int numblocks = 0;

    size = DISKSIZE;
    numblocks = DISKSIZE / BLOCKSIZE;

    //printf ("diskname=%s size=%d blocks=%d\n", vdisk, size, numblocks);

    ret = open (vdisk,  O_CREAT | O_RDWR, 0666);
    if (ret == -1) {
        //printf ("could not create disk\n");
        exit(1);
    }

    bzero ((void *)buf, BLOCKSIZE);
    fd = open (vdisk, O_RDWR);
    for (i=0; i < (size / BLOCKSIZE); ++i) {
        //printf ("block=%d\n", i);
        n = write (fd, buf, BLOCKSIZE);
        if (n != BLOCKSIZE) {
            printf ("write error\n");
            exit (1);
        }
    }
    //close (fd);

    //printf ("created a virtual disk=%s of size=%d\n", vdisk, size);
    return (0);
}


/* format disk of size dsize */
int myfs_makefs(char *vdisk)
{
    strcpy (disk_name, vdisk);
    disk_size = DISKSIZE;
    disk_blockcount = disk_size / BLOCKSIZE;

    disk_fd = open (disk_name, O_RDWR);
    if (disk_fd == -1) {
        printf ("disk open error %s\n", vdisk);
        exit(1);
    }

    // perform your format operations here.
    //printf ("formatting disk=%s, size=%d\n", vdisk, disk_size);
    struct superblockFixed* superblockFixed = malloc(4096);

    superblockFixed->firstData = BLOCKCOUNT/4;
    superblockFixed->directoryLocation=1;
    superblockFixed->fileLocation=2;
    superblockFixed->fatlocation=4;
    int sizeofsb = sizeof(superblockFixed);
    int firstFreeBlock = 0;

    if(sizeofsb <= BLOCKSIZE) {
        putblock(0, superblockFixed);
    }
    else {
        float requiredBlockCount = sizeofsb / BLOCKSIZE;
        for (int i = 0; i < requiredBlockCount; i++) {
            putblock(i, superblockFixed + i*BLOCKSIZE);
        }
    }

    void* den = malloc(4096);
    getblock(0, den);
    struct fileFixed files[128];
    struct directoryFixed* dfp = malloc(4096);
    strcpy(dfp->name, "root");
    dfp->fileBlock=BLOCKCOUNT/4;
    dfp->permission=0x777;
    dfp->filesize = 0;
    //free(cast);
    putblock(1, dfp);
    getblock(1, den);

    struct fileFixed ff[128];
    for (int i = 0; i < 128; i++) {
        strcpy(ff[i].name, "");
    }

    for (int i = 0; i < 2; i++) {
        struct fileFixed *castff = malloc(93 * sizeof(struct fileFixed));
        //free(cast);
        memcpy(castff, &ff[i * 93], 4096);
        putblock(i+2, castff);
    }
    getblock(2, den);
    getblock(3, den);

    int16_t fatTable[32768];
    for (int i = 0; i < 32768; i++) {
        fatTable[i] = -1;
    }

    int sizeoffat = sizeof(fatTable);

    if(sizeoffat <= BLOCKSIZE) {
        putblock(4, fatTable);
    }
    else {
        float requiredBlockCount = sizeoffat / BLOCKSIZE;
        int16_t* cast = malloc(2048*sizeof(int16_t));
        for (int i = 0; i < requiredBlockCount; i++) {

            //free(cast);
            memcpy(cast, &fatTable[i*2048], 4096);
            putblock(i+4, cast);
        }
        free(cast);
    }

    //int* revert = malloc(BLOCKSIZE);
    //getblock(1, (void*)revert);



    fsync (disk_fd);
    close (disk_fd);

    return (0);
}

/*
   Mount disk and its file system. This is not the same mount
   operation we use for real file systems:  in that the new filesystem
   is attached to a mount point in the default file system. Here we do
   not do that. We just prepare the file system in the disk to be used
   by the application. For example, we get FAT into memory, initialize
   an open file table, get superblockFixed into into memory, etc.
*/

int myfs_mount (char *vdisk)
{
    struct stat finfo;

    strcpy (disk_name, vdisk);
    disk_fd = open (disk_name, O_RDWR);
    if (disk_fd == -1) {
        printf ("myfs_mount: disk open error %s\n", disk_name);
        exit(1);
    }

    fstat (disk_fd, &finfo);

    //printf ("myfs_mount: mounting %s, size=%d\n", disk_name, (int) finfo.st_size);
    disk_size = (int) finfo.st_size;
    disk_blockcount = disk_size / BLOCKSIZE;
    void* myfs = malloc(BLOCKSIZE);
    getblock(0, myfs);


    //int a = getblock(0, myfs);
    //struct superblockFixed* sb =  (struct superblockFixed*)myfs;
    oft = malloc(64*sizeof(struct openfiletable));
    for (int i = 0; i < 64; ++i) {
        oft[i].accessmode = 0x777;
        oft[i].disklocationoffile = BLOCKCOUNT/4;
        oft[i].fileopencount = 0;
        oft[i].filepositionpointer = 0;
        oft[i].name = "";
    }

    moun = malloc(sizeof(struct mounted));
    moun->sb = malloc(sizeof(struct superblockFixed));
    memcpy(moun->sb, (struct superblockFixed*)myfs, sizeof(struct superblockFixed));
    int16_t *buffer = malloc(4096);

    moun->fattable = malloc (BLOCKSIZE*16);
    for (int i = 0; i < 16; i++) {
        getblock(moun->sb->fatlocation+i, buffer);
        memcpy(moun->fattable +(2048*i), buffer, 4096);
        /*for (int j = 0; j < 2048; ++j) {
            moun->fattable[j+(2048*i)] = buffer[j];
        }*/
    }

    moun->root = malloc(sizeof(struct directoryFixed));
    struct directoryFixed *dir = malloc(4096);
    getblock(1, dir);
    memcpy(moun->root, dir, sizeof(struct directoryFixed));

    struct fileFixed *df = malloc(4096);
    moun->root->files = malloc(128*sizeof(struct fileFixed));
    moun->files = malloc(128*sizeof(struct fileFixed));
    for (int i = 0; i < 2; i++) {
        getblock(moun->sb->fileLocation+i, df);
        for (int j = 0; j < 93 && i==0; j++) {
            moun->root->files[j] = df[j];
        }
        for (int j = 93; j < 128 && i==1; j++) {
            moun->root->files[j] = df[j];
        }
        //memcpy(df, moun->files + (93*i), 4096);

    }


    //memcpy(moun->root->files, df, 128*sizeof(struct fileFixed));
    moun->oft = oft;
    // perform your mount operations here

    // write your code

    /* you can place these returns wherever you want. Below
       we put them at the end of functions so that compiler will not
       complain.
        */

    return (0);
}


int myfs_umount()
{
    // perform your unmount operations here

    // write your code
    // TODO Free everything I guess
    /*struct superblockFixed asd;
    struct directoryFixed root;
    asd = *moun->sb;
    root = *moun->root.name;
    asd.root = root;*/
    //struct superblockFixed *tobuf = &asd;
    //*tobuf->root.files = *moun->root.name.files;
    //memcpy(tobuf->root, &moun->root.name, sizeof(moun->root.name));
    //memcpy(tobuf->root.name, &moun->root.name, sizeof(moun->root.name));
    struct superblockFixed* ref = moun->sb;

    //free(moun->root.name);
    struct directoryFixed *df = malloc(sizeof(df));
    strcpy(df->name, moun->root->name);
    df->filesize = moun->root->filesize;
    df->fileBlock = moun->root->fileBlock;
    df->permission = moun->root->permission;

    //moun->sb->fatlocation = 2;

    //struct superblockFixed* buf1 = malloc(4096);
    //memcpy(buf1, moun->sb, sizeof(moun->sb));
    putblock(0, ref);
    //getblock(0, buf1);
    putblock(1, df);

    struct fileFixed *ff = malloc(4096);
    for (int i = 0; i < 2; i++) {
        getblock(moun->sb->fileLocation+i, df);
        for (int j = 0; j < 93 && i==0; j++) {
            strcpy(ff[j].name, moun->root->files[j].name);
            ff[j].permission = moun->root->files[j].permission;
            ff[j].fileBlock = moun->root->files[j].fileBlock;
            ff[j].size = moun->root->files[j].size;
        }
        if (i == 0) {
            putblock(moun->sb->fileLocation+i, ff);
        }
        for (int j = 93; j < 128 && i==1; j++) {
            strcpy(ff[j].name, moun->root->files[j].name);
            ff[j].permission = moun->root->files[j].permission;
            ff[j].fileBlock = moun->root->files[j].fileBlock;
            ff[j].size = moun->root->files[j].size;
        }
        if (i == 1) {
            putblock(moun->sb->fileLocation+i, ff);
        }
        //memcpy(df, moun->files + (93*i), 4096);

    }

    /*for (int i = 0; i < 2; i++) {
        memcpy(ff, moun->root->files + (93*i), 4096);

        getblock(moun->sb->fileLocation+i, ff);

    }*/

    for (int i = 0; i < 16; i++) {
        putblock(moun->sb->fatlocation + i, moun->fattable + (2048*i));
    }


    free(moun->root);
    free(moun);
    fsync (disk_fd);
    close (disk_fd);
    return (0);
}



/* create a file with name filename */
int myfs_create(char *filename)
{
    for (int i = 0; i < moun->root->filesize; i++) {
        if (strcmp(moun->root->files[i].name, filename) == 0) {
            return myfs_open(filename);
        }
    }
    int fileBlock = getfreeblock();
    moun->root->filesize++;
    //moun->root.name.files = realloc(moun->root.name.files, moun->root.name.filesize * sizeof(struct file));
    moun->fattable[fileBlock] = 0;
    moun->root->files[moun->root->filesize-1].fileBlock = fileBlock;
    moun->root->files[moun->root->filesize-1].permission = 0x777;
    memcpy(moun->root->files[moun->root->filesize-1].name, filename, 32);

    /*moun->root.name.filesize++;
    moun->root.name.files = realloc(moun->root.name.files, moun->root.name.filesize * sizeof(struct file) + moun->root.name.filesize);
    moun->fattable[file->fileBlock] = 0;
    //moun->root.name.files[moun->root.name.filesize-1] = file;
    moun->root.name.files[moun->root.name.filesize-1].fileBlock = file->fileBlock;
    moun->root.name.files[moun->root.name.filesize-1].permission = file->permission;
    strcpy(moun->root.name.files[moun->root.name.filesize-1].name, filename);*/


    //return (moun->root.name.filesize-1);
    return (moun->root->filesize-1);
}




/* open file filename */
int myfs_open(char *filename)
{
    int index = -1;
    int isvalid = -1;
    int i = 0;
    for (; i < moun->root->filesize; i++) {
        if (strcmp(moun->root->files[i].name, filename) == 0) {
            isvalid = 1;
            break;
        }
    }

    if (isvalid == -1) {
        return -1;
    }


    for (int j = 0; j < 64; ++j) {
        if(strcmp(moun->oft[j].name, "") == 0) {
            moun->oft[j].name = filename;
            moun->oft[j].filepositionpointer=0;
            moun->oft[j].fileopencount=1;
            moun->oft[j].disklocationoffile=moun->root->files[i].fileBlock;
            moun->oft[j].accessmode=0x777;

            index=j;
            break;
        }
    }


    /*int index = -1;

    int i = 0;
    for (; i < moun->root.name.filesize; i++) {
        if (strcmp(moun->root.name.files[i].name, filename) == 0) {
            break;
        }
    }

    for (int j = 0; j < 64; ++j) {
        if(strcmp(moun->oft[j].name, "") == 0) {
            strcpy(moun->oft[j].name, filename);
            moun->oft[j].filepositionpointer=0;
            moun->oft[j].fileopencount=1;
            moun->oft[j].disklocationoffile=moun->root.name.files[i].fileBlock;
            moun->oft[j].accessmode=0x777;

            index=j;
            break;
        }
    }*/


    return (index);
}

/* close file filename */
int myfs_close(int fd)
{
    moun->oft[fd].name = "";
    moun->oft[fd].filepositionpointer = 0;
    //moun->root->files[fd].fileBlock = moun->oft[fd].disklocationoffile;
    moun->oft[fd].disklocationoffile = 0;
    moun->oft[fd].accessmode = 0;
    moun->oft[fd].fileopencount = 0;

    return (0);
}

int myfs_delete(char *filename)
{

    // write your code
    int i = 0;
    int isvalid = -1;
    int currentBlock;
    int nextBlock;
    for (i = 0; i<moun->root->filesize; i++) {
        if (strcmp(filename, moun->root->files[i].name) == 0) {
            currentBlock = moun->root->files[i].fileBlock;
            nextBlock = currentBlock;
            strcpy(moun->root->files[i].name, "");
            moun->root->files[i].fileBlock = 0;
            moun->root->files[i].permission = 0;
            moun->root->files[i].size = 0;
            isvalid = 1;
            memmove(&moun->root->files[i], &moun->root->files[i+1], (moun->root->filesize-i-1)*sizeof(struct fileFixed));
            //moun->root->files[i] = value;
            moun->root->filesize--;
            break;
        }
    }

    if (isvalid == -1) {
        return -1;
    }

    currentBlock = moun->root->files[i].fileBlock;
    nextBlock = currentBlock;
    //printf("%s:", moun->root->files[i].name);
    while (currentBlock != -0) {
        nextBlock = moun->fattable[currentBlock];
        moun->fattable[currentBlock] = -1;
        currentBlock = nextBlock;
    }


    return (0);
}

int myfs_read(int fd, void *buf, int n)
{
    int bytes_read = 0;
    int loc = moun->oft[fd].disklocationoffile;
    if (moun->fattable[loc] != 0) {
        long* buffer = malloc(BLOCKSIZE);
        getblock(8193, buffer);
        int a = 5;
    }
    //buf = malloc(n+1);

    int currentPosition = moun->oft[fd].filepositionpointer;
    int size = moun->root->files[fd].size;

    int checkforlimit = 0;
    if (currentPosition + n >= size) {
        checkforlimit = size - currentPosition;
        int offset = lseek(disk_fd, loc * BLOCKSIZE + currentPosition, SEEK_SET);
        bytes_read = read(disk_fd, buf, checkforlimit);
        if (bytes_read == -1) {
            return -1;
        }
        moun->oft[fd].filepositionpointer += bytes_read;
        return bytes_read;
    }

    int gotoblock = 0;
    int16_t currentBlock= moun->oft[fd].disklocationoffile;
    int tempPosition = currentPosition;
    /*while(tempPosition >= 4096) {
        tempPosition = tempPosition / 4096;
        gotoblock++;
    }*/
    if (currentPosition >= BLOCKSIZE) {
        gotoblock = currentPosition / BLOCKSIZE;
    }
    while (gotoblock > 0) {
        //currentBlock = moun->fattable[currentBlock];
        if (moun->fattable[currentBlock] != -1) {
            currentBlock = moun->fattable[currentBlock];
            loc = currentBlock;
        }
        gotoblock--;
    }

    if (currentPosition % BLOCKSIZE + n <= 4096) {
        int offset = lseek(disk_fd, loc * BLOCKSIZE + currentPosition%BLOCKSIZE, SEEK_SET);
        bytes_read = read(disk_fd, buf, n);
        if (bytes_read == -1) {
            return (-1);
        }
        long* buffer = malloc(BLOCKSIZE);
        getblock(loc, buffer);
        moun->oft[fd].filepositionpointer += bytes_read;
        return bytes_read;
    } else {
        int forfirstblock = 4096 - currentPosition % BLOCKSIZE;
        int offset = lseek(disk_fd, loc * BLOCKSIZE + currentPosition, SEEK_SET);
        bytes_read = read(disk_fd, buf, forfirstblock);
        if (bytes_read == -1) {
            return -1;
        }
        //moun->oft[fd].filepositionpointer += bytes_read;
        offset = lseek(disk_fd, loc * BLOCKSIZE + currentPosition+bytes_read, SEEK_SET);
        bytes_read += read(disk_fd, buf+forfirstblock, n-forfirstblock);
        moun->oft[fd].filepositionpointer += bytes_read;
        return bytes_read;
    }
}

int myfs_write(int fd, void *buf, int n)
{

    int bytes_written = 0;

    int loc = moun->oft[fd].disklocationoffile;
    if (loc < 0) {
        loc = getfreeblock();
        moun->fattable[moun->oft[fd].disklocationoffile] = loc;
        moun->fattable[loc]=0;
    }
    int currentPosition = moun->oft[fd].filepositionpointer;
    //int sizeOnLastBlock = moun->root->files[fd].size % BLOCKSIZE;
    int checkForOverlap = (currentPosition%DISKSIZE) + n;
    /*while (moun->fattable[loc] !=0) {
        loc = moun->fattable[loc];
    }*/
    if(checkForOverlap <= 4096) {
        int offset = lseek (disk_fd, (loc * BLOCKSIZE)+currentPosition, SEEK_SET);
        bytes_written = write (disk_fd, buf, n);
        if (bytes_written == -1) {
            return -1;
        }
        moun->root->files[fd].size += bytes_written;
        moun->oft[fd].filepositionpointer += bytes_written;
        return (bytes_written);

    } else {
        int gotoblock = 0;
        int16_t currentBlock= moun->oft[fd].disklocationoffile;

        /*while(currentPosition >= 4096) {
            currentPosition = currentPosition / 4096;
            gotoblock++;
        }*/
        if (currentPosition >= BLOCKSIZE) {
            gotoblock = currentPosition / BLOCKSIZE;
        }
        while (gotoblock > 0) {
            //currentBlock = moun->fattable[currentBlock];
            if (moun->fattable[currentBlock] == 0) {
                int newblock = getfreeblock();
                moun->fattable[currentBlock] = newblock;
                moun->fattable[newblock] = 0;
                //printf(" New block should be %d and it must be 0 while %d and old one %d\n", newblock, moun->fattable[newblock], moun->fattable[currentBlock]);
                currentBlock = newblock;
                break;
            }
            currentBlock = moun->fattable[currentBlock];
            gotoblock--;
        }
        if(moun->oft[fd].filepositionpointer % BLOCKSIZE != 0) {
            int offset = lseek(disk_fd, (currentBlock * BLOCKSIZE) + moun->oft[fd].filepositionpointer % BLOCKSIZE, SEEK_SET);
            int writeN = BLOCKSIZE - moun->oft[fd].filepositionpointer % BLOCKSIZE;
            if (BLOCKSIZE - moun->root->files[fd].size % BLOCKSIZE == 0) {
                writeN = 0;
            }
            if (n >= BLOCKSIZE-writeN) {
                offset = lseek(disk_fd, (currentBlock * BLOCKSIZE) + moun->oft[fd].filepositionpointer % BLOCKSIZE, SEEK_SET);
                bytes_written = write(disk_fd, buf, BLOCKSIZE-writeN);
                moun->oft[fd].filepositionpointer += bytes_written;
                moun->root->files[fd].size += bytes_written;
                return bytes_written;
            } else {
                offset = lseek(disk_fd, (currentBlock * BLOCKSIZE) + moun->oft[fd].filepositionpointer % BLOCKSIZE, SEEK_SET);
                bytes_written = write(disk_fd, buf, n);
                long* buffer = malloc(BLOCKSIZE);
                getblock(currentBlock, buffer);
                moun->root->files[fd].size += bytes_written;
                moun->oft[fd].filepositionpointer += bytes_written;
                return (bytes_written);
            }

            if (bytes_written == -1) {
                return -1;
            }
            moun->oft[fd].filepositionpointer += bytes_written;
            int newLoc = getfreeblock();
            moun->fattable[currentBlock] = newLoc;
            moun->fattable[newLoc] = 0;
            currentBlock = newLoc;
        }

        int offset = lseek(disk_fd, (currentBlock * BLOCKSIZE), SEEK_SET);
        bytes_written += write(disk_fd, buf+bytes_written, n - bytes_written);
        moun->oft[fd].filepositionpointer += bytes_written;
        long* buffer = malloc(BLOCKSIZE);
        getblock(currentBlock, buffer);
        int a = 5;
    }
    if (bytes_written > 0) {
        moun->root->files[fd].size += bytes_written;
    }
    //moun->oft[fd].filepositionpointer += bytes_written;
    return (bytes_written);
}



int myfs_truncate(int fd, int size)
{

    // write your code
    int byte_deleted;
    int16_t block = moun->root->files[fd].fileBlock;
    int until = moun->root->files[fd].size - size;



    int times = 0;
    char* zero = malloc(size);
    if (until <= 4096) {
        int offset = lseek (disk_fd, (block * BLOCKSIZE)+size, SEEK_SET);
        byte_deleted = write (disk_fd, zero, until);
        int currentBlock = moun->root->files[fd].fileBlock;
        int nextBlock = moun->fattable[currentBlock];
        moun->fattable[currentBlock] = 0;
        //printf("%s:", moun->root->files[fd].name);
        while (currentBlock != 0) {
            nextBlock = moun->fattable[currentBlock];
            moun->fattable[nextBlock] = -1;
            currentBlock = nextBlock;
        }
    } else {
        int tempSize = size;
        while (tempSize > 4096) {
            times++;
            tempSize = tempSize - BLOCKSIZE;
        }
        int currentBlock = moun->root->files[fd].fileBlock;
        while (times > 0) {
            //printf(" %d", currentBlock);
            currentBlock = moun->fattable[currentBlock];
            times--;
            if (currentBlock == -1) {
                return 0;
            }
        }
        int offset = lseek (disk_fd, (currentBlock * BLOCKSIZE)+size, SEEK_SET);
        byte_deleted = write (disk_fd, zero, BLOCKSIZE);
        moun->root->files[fd].size = size;
    }



    return (byte_deleted);
}


int myfs_seek(int fd, int offset)
{
    int position = -1;

    // write your code

    int size = moun->root->files[fd].size;
    if (size <= offset) {
        position = offset;
        moun->oft[fd].filepositionpointer = size;
        return position;
    } else {
        moun->oft[fd].filepositionpointer = offset;
        position = offset;
        return position;
        /*if (offset >= 4096) {
            int newoffset = offset;
            int gotoblock = 0;
            while(newoffset < 4096) {
                gotoblock++;
                newoffset = newoffset/4096;
            }
            int loc = moun->oft[fd].disklocationoffile;
            for (int i = 0; i < gotoblock; i++) {
                loc = moun->fattable[loc];
            }

        }*/
    }

    return (position);
}

int myfs_filesize (int fd)
{
    int size = -1;

    size = moun->root->files[fd].size;

    return (size);
}


void myfs_print_dir ()
{
    for (int i = 0; i<moun->root->filesize; i++) {
        printf("%s\n", moun->root->files[i].name);
    }

}


void myfs_print_blocks (char *  filename)
{
    // write your code
    int i = 0;
    for (i = 0; i<moun->root->filesize; i++) {
        if (strcmp(filename, moun->root->files[i].name) == 0) {
            break;
        }
    }
    int currentBlock = moun->root->files[i].fileBlock;
    printf("%s:", moun->root->files[i].name);
    while (currentBlock != 0) {
        printf(" %d", currentBlock);
        currentBlock = moun->fattable[currentBlock];
    }
    printf("\n");
}


