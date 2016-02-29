BASE=cat
OB=$(BASE).o
C=$(BASE).c
SHELL=bash -O extglob

all: $(BASE)

clean:
	rm !(cat.c|Makefile)

%: %.c
	gcc $< -o $(OB) -c
	gcc -s $(OB) -o $@