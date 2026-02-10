#include "lib/types.h"
#include "lib/string.h"

#define MARKOV_SIZE 4096
#define CHAIN_LEN 3

struct markov_node {
    char prefix[CHAIN_LEN + 1];
    char next_char;
    int count;
};

static struct markov_node markov_table[MARKOV_SIZE];
static int markov_entries = 0;

void markov_init() {
    markov_entries = 0;
    memset(markov_table, 0, sizeof(markov_table));
}

void markov_learn(const char* text) {
    int len = strlen(text);
    if (len < CHAIN_LEN + 1) {
        return;
    }
    
    for (int i = 0; i < len - CHAIN_LEN && markov_entries < MARKOV_SIZE; i++) {
        char prefix[CHAIN_LEN + 1];
        for (int j = 0; j < CHAIN_LEN; j++) {
            prefix[j] = text[i + j];
        }
        prefix[CHAIN_LEN] = 0;
        
        char next = text[i + CHAIN_LEN];
        
        int found = 0;
        for (int j = 0; j < markov_entries; j++) {
            if (strcmp(markov_table[j].prefix, prefix) == 0 &&
                markov_table[j].next_char == next) {
                markov_table[j].count++;
                found = 1;
                break;
            }
        }
        
        if (!found && markov_entries < MARKOV_SIZE) {
            strcpy(markov_table[markov_entries].prefix, prefix);
            markov_table[markov_entries].next_char = next;
            markov_table[markov_entries].count = 1;
            markov_entries++;
        }
    }
}

char markov_predict(const char* text) {
    int len = strlen(text);
    if (len < CHAIN_LEN) {
        return 0;
    }
    
    char prefix[CHAIN_LEN + 1];
    for (int i = 0; i < CHAIN_LEN; i++) {
        prefix[i] = text[len - CHAIN_LEN + i];
    }
    prefix[CHAIN_LEN] = 0;
    
    int best_idx = -1;
    int best_count = 0;
    
    for (int i = 0; i < markov_entries; i++) {
        if (strcmp(markov_table[i].prefix, prefix) == 0) {
            if (markov_table[i].count > best_count) {
                best_count = markov_table[i].count;
                best_idx = i;
            }
        }
    }
    
    if (best_idx != -1) {
        return markov_table[best_idx].next_char;
    }
    
    return 0;
}
