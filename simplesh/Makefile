BASE=simplesh
OB=$(BASE).o
C=$(BASE).cpp
SHELL=bash -O extglob

all: $(BASE)

clean:
	rm -f !($(C)|Makefile)

%: %.cpp
	g++ $< -o $(OB) -c
	g++ -s $(OB) -o $@
