COMPILER = clang++
SOURCE_LIBS = -Ilib/
OSX_OPT = -std=c++17 -Llib/ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL lib/libraylib.a
OSX_OUT = -o "bin/build_osx"
CPPFILES = src/*.cpp

build_osx_debug: 
	$(COMPILER) -g $(CPPFILES) $(SOURCE_LIBS) $(OSX_OUT) $(OSX_OPT)
	
build_playground_osx:
	$(COMPILER) -g src/tetris.cpp src/solver.cpp src/solver-playground/main.cpp $(SOURCE_LIBS) -o bin/playground_osx $(OSX_OPT)
