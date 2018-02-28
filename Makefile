all: assgn1

assgn1: assgn1.cpp glad.c
	g++ -o assgn1 assgn1.cpp glad.c -lGL -lglfw -ldl

clean:
	rm assgn1
