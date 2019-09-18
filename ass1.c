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
#define TOP_HEADING_LEVEL 1
#define BOTTOM_HEADING_LEVEL 5
#define HEADING_LEVELS (BOTTOM_HEADING_LEVEL - TOP_HEADING_LEVEL + 1)

#define END_OF_TOKEN -2
#define END_OF_LINE -1
#define PARAGRAPH_BREAK 1
#define LINE_BREAK 2
#define DEBUG 0

typedef char line_t[MAX_LINE_LENGTH + 1];

typedef struct {
    bool at_beginning;
    int needs_break;
    int left_margin;
    int max_width;
    int current_width;
    int headings[HEADING_LEVELS];
} state_t;

/**
 * Attribution: Alistair Moffat
 * https://people.eng.unimelb.edu.au/ammoffat/teaching/10002/ass1/ass1.pdf
 *
 * Changes: function decl on one line as per my own preferred style, and
 * renamed it
 */
int wingetchar() {
    int c;
    while ((c=getchar())=='\r') {
    }
    return c;
}

/**
 * Retrieves one line of input, trimming whitespace.
 * Whitespace at the start of the line is kept, however, as we must
 * be able to tell if `.` commands were at the beginning of the line.
 */
int get_line(line_t line) {
    int i = 0, j = 0;
    bool suppressing_whitespace = false;
    while(j < MAX_LINE_LENGTH) {
        j++;
        char c = wingetchar();
        if(c == EOF || c == '\n') {
            /* work backwards to trim trailing whitespace */
            do {
                i--;
            } while(isspace(line[i]));

            line[i + 1] = '\0';
            return c;
        } else if(isspace(c)) {
            /* only output one space in a row */
            if(!suppressing_whitespace) {
                line[i++] = ' ';
                suppressing_whitespace = true;
            }
        } else {
            suppressing_whitespace = false;
            line[i++] = c;
        }
    }

    fprintf(stderr, "Line exceeded maximum line length of %d chars, aborting...\n", MAX_LINE_LENGTH);
    exit(EXIT_FAILURE);
}

/* begins a line with a left margin */
void begin_line(state_t *state) {
    int j;
    for(j = 0; j < state->left_margin; j++) {
        printf(" ");
    }
    state->at_beginning = false;
    state->current_width = 0;
}

/* emits line/paragraph breaks if they are needed and sets up margins etc... */
void maybe_break(state_t *state) {
    if(state->needs_break == false) {
        /* don't even add margins */
        return;
    } else if(state->at_beginning) {
        /* don't emit any line breaks, but still add margins etc... */
    } else if(state->needs_break == PARAGRAPH_BREAK) {
        printf("\n\n");
    } else if(state->needs_break == LINE_BREAK) {
        printf("\n");
    }

    begin_line(state);
    state->needs_break = false;
}

void request_line_break(state_t *state) {
    /* don't 'downgrade' a paragraph break to a mere line break */
    if(state->needs_break != PARAGRAPH_BREAK) {
        state->needs_break = LINE_BREAK;
    }
}

void request_paragraph_break(state_t *state) {
    /* line breaks are 'upgraded' to paragraph breaks */
    state->needs_break = PARAGRAPH_BREAK;
}

/* increments text until a non whitespace char is reached */
char *consume_whitespace(char *text) {
    while(isspace(*text) && *text != '\0') {
        text++;
    }
    return text;
}

char *process_word(char *text, state_t *state) {
    /* finally emit a line break from a previous command,
       now that we are just about to output more text */
    maybe_break(state);

    /* lookahead to find the length of the next word */
    char *word_start = text;
    while(!isspace(*text) && *text != '\0') {
        text++;
    }
    char *word_end = text;
    int word_len = text - word_start;

    /* wrap if needed, otherwise emit a space */
    if(state->current_width + word_len >= state->max_width) {
        request_line_break(state);
        maybe_break(state);
    } else if(state->current_width > 0) {
        printf(" ");
        state->current_width += 1;
    }

    /* print the word */
    for(text = word_start; text < word_end; text++) {
        printf("%c", *text);
    }
    state->current_width += word_len;

    /* now points to one char past the word */
    return text;
}

int parse_int(char *text) {
    int num = atoi(text);
    if(num == 0) {
        fprintf(stderr, "Failed to parse integer '%s'\n", text);
        exit(EXIT_FAILURE);
    }
    return num;
}

void emit_heading_numbering(state_t *state, int level) {
    if(level == TOP_HEADING_LEVEL) {
        int i;
        for(i = 0; i < state->max_width; i++) {
            printf("-");
        }
        request_line_break(state);
        maybe_break(state);
    }

    /* reset lower heading levels */
    int i;
    for(i = level; i < HEADING_LEVELS; i++) {
        state->headings[i] = TOP_HEADING_LEVEL - 1;
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

void process_command(char *command, state_t *state) {
    /* strips the single letter command name, to point to command arguments */
    char *command_args = command + 1;

    if(command[0] == 'b') {
        request_line_break(state);
    } else if(command[0] == 'p') {
        request_paragraph_break(state);
    } else if(command[0] == 'l') {
        int margin = parse_int(command_args);
        state->left_margin = margin;
        request_paragraph_break(state);
    } else if(command[0] == 'w') {
        int width = parse_int(command_args);
        state->max_width = width;
        request_paragraph_break(state);
    } else if(command[0] == 'c') {
        request_line_break(state);
        maybe_break(state);
        char *text = consume_whitespace(command_args);
        int len = strlen(text);
        if(len < state->max_width) {
            int offset = (state->max_width - len) / 2;
            int i;
            for(i = 0; i < offset; i++) {
                printf(" ");
            }
        }
        #if(DEBUG)
            fprintf(stderr, "outputting centered text: '%s'\n", text);
        #endif
        printf("%s", text);
        request_line_break(state);
    } else if(command[0] == 'h') {
        /* since the heading level is only 1-5, we could assume that bytes is 1,
           however this way is more resillient at handling whitespace etc... */
        int level, skip_arg;
        int elements = sscanf(command_args, "%d%n", &level, &skip_arg);
        if(elements == 0) {
            fprintf(stderr, "Failed to parse heading level, aborting...\n");
        }
        assert(TOP_HEADING_LEVEL <= level && level <= BOTTOM_HEADING_LEVEL);
        request_paragraph_break(state);
        maybe_break(state);
        emit_heading_numbering(state, level);
        char *body = consume_whitespace(command_args + skip_arg);
        #if(DEBUG)
            fprintf(stderr, "outputting heading %d: '%s'\n", level, body);
        #endif
        printf("%s", body);
        request_paragraph_break(state);
    }
}

/* processes one line of the input text which will become 0 or more lines
   of output text */
void process_line(line_t line, state_t *state) {
    #if(DEBUG)
        fprintf(stderr, "processing: '%s'\n", line);
    #endif

    if(line[0] == '.') {
        /* handle commands, stripping the dot off the command */
        process_command(line + 1, state);
    } else {
        char *cursor = line;
        /* alternate between whitespace and words */
        while(*cursor != '\0') {
            cursor = consume_whitespace(cursor);
            cursor = process_word(cursor, state);
        }
    }
}

int main(int argc, char *argv[]) {
    line_t line;
    state_t state = {
        /* the combination of `at_beginning=true` and `needs_break=LINE_BREAK`
           ensures that the first line of text has proper indentation, but also
           that the there are no extraneous newlines at the top of the file. */
        .at_beginning = true,
        .needs_break = LINE_BREAK,
        .left_margin = LEFT_MARGIN,
        .max_width = MAX_WIDTH,
        .current_width = 0,
    };
    int i;
    for(i = 0; i < HEADING_LEVELS; i++) {
        state.headings[i] = TOP_HEADING_LEVEL - 1;
    }

    while(get_line(line) != EOF) {
        process_line(line, &state);
    }
    /* don't forget to process the last line as well */
    process_line(line, &state);

    /* newline at end of file */
    printf("\n");

	return 0;
}

/*
    _____________________
    < Algorithms are fun! >
    ---------------------
            \   ^__^
            \  (oo)\_______
                (__)\       )\/\
                    ||----w |
                    ||     ||
*/
