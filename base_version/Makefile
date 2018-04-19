all:
	g++ -std=gnu++0x main.cpp acorn.h hash_functions.h hash_table.h -o hash
	valgrind --tool=callgrind ./hash
	rm -rf *.o *.h.gch hash
