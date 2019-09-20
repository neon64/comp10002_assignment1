#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define MAX_CHARS 999

int main(int argc, char *argv[]) {
    char line[MAX_CHARS + 1];
    int col = 0;
    int i, j, len;
    printf("    ");
    while(fgets(line, MAX_CHARS + 1, stdin)) {
        if(line[0] == '.') { continue; }
        i = 0;
        char *word = strtok(line, " \r\n\t");
        while(line[i] != '\0') {
            while(isspace(line[i])) { i++; }
            len = 0;
            while(line[i + len] != '\0' && !isspace(line[i+len])) {
                len += 1;
            }
            if(col + len >= 50) {
                printf("\n    ");
                col = 0;
            }
            if(len > 0 && col > 0) {
                printf(" ");
                col += 1;
            }
            for(j = 0; j < len; j++) {
                printf("%c", line[i+j]);
            }
            col += len;
            i += len;
        }
    }
    printf("\n");
    return 0;
}