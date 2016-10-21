main:  MemoryManager.o MemoryManagerTest.o
	g++ -std=c++0x MemoryManager.o MemoryManagerTest.o -o main

MemoryManagerTest.o:  MemoryManagerTest.cpp
	g++ -std=c++0x -c MemoryManagerTest.cpp

MemoryManager.o: MemoryManager.cpp MemoryManager.h
	g++ -std=c++0x -c MemoryManager.cpp

clean:
	rm *.o
