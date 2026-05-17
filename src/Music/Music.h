#ifndef MUSIC_HH
#define MUSIC_HH

//Pre: The given path is a valid path to an MP3 file.
//Post: The music is played in loop until Stop_Music is called. The function is non-blocking, so the program can continue running while the music is playing.
void Start_Music(const char* path);

void Stop_Music();

#endif