#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "Music.h"

int Current_Music_PID = 0;

void Start_Music(const char* path) {
    atexit(Stop_Music); // Ensure music stops when the program exits
    if (Current_Music_PID != 0){
        fprintf(stderr, "Error(Music) Music is already playing\n");
        exit(EXIT_FAILURE);
    }

    int p = fork();
    if (p < 0) {
        fprintf(stderr, "Error: Failed to fork process for music playback.\n");
        exit(EXIT_FAILURE);
    } else if (p == 0) {
        // Child process: Execute the music playback command
        execlp("ffplay", "ffplay", "-nodisp", "-loop", "0", "-loglevel", "quiet", path, NULL);
        fprintf(stderr, "Error: Failed to execl ffplay.\n");
        exit(EXIT_FAILURE);
    }

    Current_Music_PID = p;
}

void Stop_Music() {
    if(Current_Music_PID == 0){
        fprintf(stderr, "Error(Music) No music is currently playing\n");
        exit(EXIT_FAILURE);
    }

    kill(Current_Music_PID, SIGTERM);
    Current_Music_PID = 0;
}