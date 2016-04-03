BASE=sigusr
OB=$(BASE).o
C=$(BASE).c
SHELL=bash -O extglob

all: $(BASE)

clean:
	rm -f !($(C)|Makefile)

%: %.c
	gcc $< -o $(OB) -c
	gcc -s $(OB) -o $@
