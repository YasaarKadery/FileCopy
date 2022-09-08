#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

extern int errno;
void display_message(int s);

int main(int argc, char *argv[])
{
    // variable to hold number of bytes written
    int numBytes = 0;
    // variables to hold return values for read() and write() functions
    int readSize, writeNumBytes;
    // file descriptors
    int fdSource, fdTarget;

    // check to make sure the correct number of arguments are being passed
    if (argc != 3)
    {
        printf("copypit: Insufficient num of args\n");
        printf("usage: ./copyit <sourceFile <targetFile>\n");
        return -1;
    }

    // Open the source file and check for errors

    fdSource = open(argv[1], O_RDONLY, 0);
    if (fdSource < 0)
    {
        printf("Unable to open %s: %s\n", argv[1], strerror(errno));
        return -1;
    }
    // Open target file and check for errors

    fdTarget = open(argv[2], O_RDWR | O_CREAT, 00700);
    if (fdTarget < 0)
    {
        printf("Unable to open %s: %s\n", argv[2], strerror(errno));
        return -1;
    }

    // create the buffer of memory to read to and write from
    char *c = (char *)malloc(1024 * sizeof(char));

    // register the signal and set the alarm
    signal(SIGALRM, display_message);
    alarm(1);

    // read and write file
    while (1)
    {
        // number of bytes that are read into the buffer (up to 1024)
        readSize = read(fdSource, c, 1024);

        // if the readsize is 0 that means we reached the end of file and we break from the loop
        if (readSize == 0)
        {
            break;
        }
        // if we return < 0, there was an error
        if (readSize < 0)
        {
            printf("ERROR while reading: %s\n", strerror(errno));
            break;
        }

        // write() returns number of bytes written into buffer
        writeNumBytes = write(fdTarget, c, readSize);
        // check for errors
        if (writeNumBytes < 0)
        {
            printf("ERROR while writing: %s\n", strerror(errno));
            return -1;
        }
        // add the total number of bytes written
        numBytes = numBytes + writeNumBytes;
    }

    printf("copyit: Copied %d bytes from file %s to %s\n", numBytes, argv[1], argv[2]);

    // close the files and free allocated memory
    close(fdSource);
    close(fdTarget);
    free(c);
}

// handler function that prints messages every second
void display_message(int s)
{
    printf("copyit: still working...\n");
    alarm(1);
}
