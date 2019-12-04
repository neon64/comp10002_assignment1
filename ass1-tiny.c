#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define MAX_CHARS 999

/* a challenge to implement stage 1 in as few lines of code as possible */
int main(int argc, char *argv[]) {
    char line[MAX_CHARS + 1];
    int len, col = 0;
    printf("    ");
    while(fgets(line, MAX_CHARS + 1, stdin)) {
        if(line[0] == '.') { continue; }
        char *tok = line;
        while((tok = strtok(tok, " \r\n\t"))) {
            len = strlen(tok);
            if(col + len >= 50) {
                printf("\n    ");
                col = 0;
            }
            if(len > 0 && col > 0) {
                printf(" ");
                col += 1;
            }
            printf("%s", tok);
            col += len;
            tok = NULL;
        }
    }
    printf("\n");
    return 0;
}