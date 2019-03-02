CXX = clang++
CXXFLAGS = -std=c++17 -Wall

matrix: matrix.cpp main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

matrix.cpp: matrix.hpp

main.cpp: matrix.hpp


clean:
	rm -rf *.o *.gch
	rm -rf matrix
