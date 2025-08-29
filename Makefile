all: GAME_P Flappy

GAME_P: Main.c game.c
	gcc Main.c game.c -o GAME_P

Flappy: Flappy.c game.c
	gcc Flappy.c game.c -o Flappy

clean: rm -f GAME_P Flappy
