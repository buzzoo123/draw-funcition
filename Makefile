# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++20

# SDL and FFTW library flags
SDL_LIBS = -lSDL2
FFTW_LIBS = -lfftw3

# Source files
SRC = sdl_test.cpp thinning.cpp fourier.cpp
OBJ = $(SRC:.cpp=.o)

# Output executable
TARGET = sdl_test

# Compile target
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(SDL_LIBS) $(FFTW_LIBS)

# Compile individual source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up object files and executable
clean:
	rm -f $(OBJ) $(TARGET)
