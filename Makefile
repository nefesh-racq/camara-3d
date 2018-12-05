# comilacion del programa
OBJ = -lGL -lGLU -lSDL2
BINARY = a.out

$(BINARY): $(OBJ)
	g++ $(OBJ) camera_fps.cpp -o $(BINARY)

clean:
	rm -r $(BINARY)
