sim: sim.cpp
	g++ -std=c++11  -o sim.exe sim.cpp

clean:
	rm sim.exe