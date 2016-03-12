BASE=hello_world
OB=$(BASE).o
C=$(BASE).c

all: $(BASE)

clean:
	rm -f $(BASE) $(OB)

%: %.c
	gcc $< -o $(OB) -c
	gcc -s $(OB) -o $@
