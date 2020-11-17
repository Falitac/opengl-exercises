
LIBS = -lsfml-graphics
LIBS += -lsfml-audio
LIBS += -lsfml-window
LIBS += -lsfml-system
LIBS += -lsfml-network
LIBS += -lglew32
LIBS += -lopengl32

run: compile
	a.exe

compile:
	g++ src/*.cpp -std=c++17 $(LIBS)
