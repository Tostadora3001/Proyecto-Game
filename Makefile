all: GAME_P bin/Flappy

GAME_P: Main.c src/game/game.c
	gcc Main.c src/game/game.c -o GAME_P -Wall -Werror

bin/Flappy: src/Flappy/Flappy.c src/game/game.c src/Music/Music.c
	gcc src/Flappy/Flappy.c src/game/game.c src/Music/Music.c -o bin/Flappy -Wall -Werror

clean:
	rm -f GAME_P bin/*

ultraclean: 
	rm -f GAME_P bin/*
