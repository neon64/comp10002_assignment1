/* Solution to comp10002 Assignment 1, 2019 semester 2.

   Authorship Declaration:

   (1) I certify that the program contained in this submission is completely
   my own individual work, except where explicitly noted by comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students,
   or by non-students as a result of request, solicitation, or payment,
   may not be submitted for assessment in this subject.  I understand that
   submitting for assessment work developed by or in collaboration with
   other students or non-students constitutes Academic Misconduct, and
   may be penalized by mark deductions, or by other penalties determined
   via the University of Melbourne Academic Honesty Policy, as described
   at https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will
   I do so until after the marks are released. I understand that providing
   my work to other students, regardless of my intention or any undertakings
   made to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring
   service, or drawing the attention of others to such services and code
   that may have been made available via such a service, may be regarded
   as Student General Misconduct (interfering with the teaching activities
   of the University and/or inciting others to commit Academic Misconduct).
   I understand that an allegation of Student General Misconduct may arise
   regardless of whether or not I personally make use of such solutions
   or sought benefit from such actions.

   Signed by: Christopher Chamberlain 1082551
   Dated:     11/9/2019

*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#define MAX_LINE_LENGTH 999
#define LEFT_MARGIN 4
#define MAX_WIDTH 50
#define END_OF_TOKEN -2
#define END_OF_LINE -1
#define PARAGRAPH_BREAK 1
#define LINE_BREAK 2
#define LOWEST_HEADING_LEVEL 1
#define HEADING_LEVELS 5
#define WORD_WRAP_ENABLED true
#define WORD_WRAP_DISABLED false

typedef char line_t[MAX_LINE_LENGTH + 1];

/**
 * Attribution: Alistair Moffat
 * https://people.eng.unimelb.edu.au/ammoffat/teaching/10002/ass1/ass1.pdf
 *
 * Changes: function decl on one line as per my own preferred style, and renamed it
 */
int wingetchar() {
    int c;
    while ((c=getchar())=='\r') {
    }
    return c;
}

int get_line(line_t line) {
    int i;
    char c = wingetchar();
    for(i = 0; i < MAX_LINE_LENGTH; i++) {
        if(c == EOF || c == '\n') {
            line[i] = '\0';
            return c;
        }
        line[i] = c;
        c = wingetchar();
    }

    /* print to standard error instead */
    fprintf(stderr, "Line exceeded maximum line length of %d chars, aborting...\n", MAX_LINE_LENGTH);
    exit(EXIT_FAILURE);
}

typedef struct {
    bool suppressing_whitespace;
    int needs_break;
    int left_margin;
    int max_width;
    int current_width;
    int headings[HEADING_LEVELS];
} state_t;

void begin_line(state_t *state) {
    int j;
    for(j = 0; j < state->left_margin; j++) {
        printf(" ");
    }
    state->current_width = 0;
    /* we want the next word to be flush with the beginning of the line */
    state->suppressing_whitespace = true;
}

void line_break(state_t *state) {
    printf("\n");
    begin_line(state);
    state->needs_break = false;
}

int consume_space(int *i, char *line, state_t *state) {
    while(line[*i] != '\0') {
        if(!isspace(line[*i])) {
            return END_OF_TOKEN;
        }
        *i += 1;
    }
    return END_OF_LINE;
}

int consume_word(int *i, char *line, state_t *state, bool wrap) {
    /* lookahead to find the length of the next word */
    int word_start = *i;
    while(!isspace(line[*i]) && line[*i] != '\0') {
        (*i)++;
    }
    int word_len = *i - word_start;

    /* check for word wrapping */
    if(wrap && state->current_width + word_len >= state->max_width) {
        line_break(state);
    } else if(!state->suppressing_whitespace) {
        printf(" ");
        state->current_width += 1;
    }

    /* print the word */
    int k;
    for(k = word_start; k < *i; k++) {
        printf("%c", line[k]);
    }
    state->current_width += word_len;

    /* we probably want to emit a space after this word (unless it is at the end of a line) */
    state->suppressing_whitespace = false;

    /* how to avoid this second check? */
    if(line[*i] == '\0') {
        return END_OF_LINE;
    } else {
        return END_OF_TOKEN;
    }
}

void new_paragraph(state_t *state) {
    printf("\n\n");
    begin_line(state);
    state->needs_break = false;
}

int parse_int(char *text) {
    int num = atoi(text);
    if(num == 0) {
        fprintf(stderr, "Failed to parse integer '%s'\n", text);
        exit(EXIT_FAILURE);
    }
    return num;
}

void process_line_body(char *text, state_t *state, bool wrap) {
    int i = 0;
    while(true) {
        if(consume_space(&i, text, state) == END_OF_LINE) {
            return;
        }

        if(consume_word(&i, text, state, wrap) == END_OF_LINE) {
            return;
        }
    }
}

void emit_heading_numbering(state_t *state, int level) {
    /* reset lower heading levels */
    int i;
    for(i = level; i < HEADING_LEVELS; i++) {
        state->headings[i] = LOWEST_HEADING_LEVEL - 1;
    }

    state->headings[level - 1] += 1;
    /* print out the heading */
    for(i = 0; i < level; i++) {
        printf("%d", state->headings[i]);
        if(i + 1 == level) {
            printf(" ");
        } else {
            printf(".");
        }
    }
}

void maybe_break(state_t *state) {
    if(state->needs_break == PARAGRAPH_BREAK) {
        new_paragraph(state);
    } else if(state->needs_break == LINE_BREAK) {
        line_break(state);
    }
}

void process_command(char command[], state_t *state) {
    if(command[0] == 'b') {
        /* this isn't exactly specified, however I've decided that
         * a paragraph break should take the place of a line break.
         */
        if(state->needs_break != PARAGRAPH_BREAK) {
            state->needs_break = LINE_BREAK;
        }
    } else if(command[0] == 'p') {
        state->needs_break = PARAGRAPH_BREAK;
    } else if(command[0] == 'l') {
        int margin = parse_int(command + 1);
        state->left_margin = margin;
        state->needs_break = PARAGRAPH_BREAK;
    } else if(command[0] == 'w') {
        int width = parse_int(command + 1);
        state->max_width = width;
        state->needs_break = PARAGRAPH_BREAK;
    } else if(command[0] == 'c' && command[1] == ' ') {
        maybe_break(state);
        char *text = command + 2;
        int len = strlen(text);
        if(len < state->max_width) {
            int offset = (state->max_width - len) / 2;
            int i;
            for(i = 0; i < offset; i++) {
                printf(" ");
            }
        }
        printf("%s", text);
        state->needs_break = LINE_BREAK;
    } else if(command[0] == 'h' && command[1] == ' ') {
        /* since the heading level is only 1-5, we could assume that bytes is 1,
         * however this way is more resillient with whitespace etc... */
        int level, bytes;
        int elements = sscanf(command + 1, "%d%n", &level, &bytes);
        if(elements == 0) {
            fprintf(stderr, "Failed to parse heading level, aborting...\n");
        }
        assert(1 <= level && level <= HEADING_LEVELS);
        new_paragraph(state);
        if(level == 1) {
            int i;
            for(i = 0; i < state->max_width; i++) {
                printf("-");
            }
            line_break(state);
        }
        emit_heading_numbering(state, level);
        process_line_body(command + 1 + bytes, state, WORD_WRAP_DISABLED);
        state->needs_break = PARAGRAPH_BREAK;
    }
}

void process_line(line_t line, state_t *state) {
    /* handle commands */
    if(line[0] == '.') {
        process_command(line + 1, state);
        return;
    }

    maybe_break(state);

    process_line_body(line, state, WORD_WRAP_ENABLED);
}

int main(int argc, char *argv[]) {
    line_t line;
    state_t state = {
        .suppressing_whitespace = false,
        .needs_break = false,
        .left_margin = LEFT_MARGIN,
        .max_width = MAX_WIDTH,
        .current_width = 0
    };

    int i;
    for(i = 0; i < HEADING_LEVELS; i++) {
        state.headings[i] = LOWEST_HEADING_LEVEL - 1;
    }

    begin_line(&state);

    while(get_line(line) != EOF) {
        process_line(line, &state);
    }

    printf("\n");

	return 0;
}

/* algorithms are fun! */