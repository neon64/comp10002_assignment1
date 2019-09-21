using namespace std;

#include <iostream>
#include <sstream>

typedef struct {
    unsigned int column;
    unsigned int max_width;
} state_t;

void process_line(std::string *line, state_t *state) {
    if((*line)[0] == '.') { return; }

    string word;
    istringstream stream(*line);
    while(stream >> word) {
        if(state->column + word.length() >= state->max_width) {
            cout << endl << "    ";
            state->column = 0;
        } else if(state->column > 0) {
            cout << " ";
            state -> column += 1;
        }
        cout << word;
        state->column += word.length();
    }
}

int main() {
    state_t state = {
        .column = 0,
        .max_width = 50
    };
    cout << "    ";
    for(std::string line; getline(cin, line); ) {
        process_line(&line, &state);
    }
}