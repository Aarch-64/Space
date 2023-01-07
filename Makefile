default: 
	gcc space.c -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -o space
	
run:
	@./space
