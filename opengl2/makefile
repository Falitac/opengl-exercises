
SFML_LIB = -lsfml-graphics
SFML_LIB += -lsfml-audio
SFML_LIB += -lsfml-window
SFML_LIB += -lsfml-system
SFML_LIB += -lsfml-network
SFML_LIB += -lglew32
SFML_LIB += -lopengl32

run: compile
	a.exe

compile:
	g++ src/*.cpp -std=c++17 $(SFML_LIB)
