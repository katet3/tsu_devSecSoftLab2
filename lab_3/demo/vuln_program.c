#include <stdio.h>
#include <string.h>

void vulnerable_function(char *input) {
    char buffer[64];
    strcpy(buffer, input);
    printf("Buffer contents: %s\n", buffer);
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("Usage: %s <input_string>\n", argv[0]);
        return 1;
    }

    char *input = argv[1];
    
    vulnerable_function(input);
    printf("Program executed normally.\n");
    return 0;
}