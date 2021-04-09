#include <fcntl.h> 
#include <stdio.h> 
#include <string.h>
#include <unistd.h> 
#include "read_files.h" 

void read_file(int fd) {
    unsigned char buffer[1];
    int bytes_read;
    do {
        bytes_read = read(fd, buffer, 1); 
        printf("%c", buffer[0]);
    }
    while (bytes_read != 0); 
}
