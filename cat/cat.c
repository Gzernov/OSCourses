#include <stdio.h>

int main() {
    int const BUFF_SIZE = 1024;
    char buf[BUFF_SIZE];
    int red_size = 1;
    while (red_size > 0) {
        red_size = read(0, buf, BUFF_SIZE);
        int write_size = 0;
        while (write_size >= 0) {
            write_size += write(1, buf + write_size, red_size);
            if (write_size != red_size) { red_size -= write_size; } else { break; }
        }
    }
    return 0;
};
