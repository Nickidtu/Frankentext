#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAX_WORD_COUNT 15000
#define MAX_SUCCESSOR_COUNT (MAX_WORD_COUNT / 2)

char book[1000000];

char *tokens[MAX_WORD_COUNT];
size_t tokens_size = 0;

char *succs[MAX_WORD_COUNT][MAX_SUCCESSOR_COUNT];
size_t succs_sizes[MAX_WORD_COUNT] = {0};

/// Replace all non-printable characters in book with space
void replace_non_printable_chars_with_space() {
    for (size_t i = 0; book[i] != '\0'; i++) {
        if (!isprint((unsigned char)book[i])) {
            book[i] = ' ';
        }
    }
}

/// Returns index of token in tokens array, creates it if not found
size_t token_id(char *token) {
    for (size_t i = 0; i < tokens_size; i++) {
        if (strcmp(tokens[i], token) == 0)
            return i;
    }
    tokens[tokens_size] = token;
    return tokens_size++;
}

/// Append successor token to token's successor list
void append_to_succs(char *token, char *succ) {
    size_t id = token_id(token);
    size_t *size_ptr = &succs_sizes[id];

    if (*size_ptr >= MAX_SUCCESSOR_COUNT) {
        printf("Successor array full.");
        exit(EXIT_FAILURE);
    }

    succs[id][(*size_ptr)++] = succ;
}

/// Tokenize string and fill tokens & successors
void tokenize_and_fill_succs(char *delimiters, char *str) {
    char *current = strtok(str, delimiters);
    if (!current) return;

    char *prev = NULL;
    while (current) {
        if (prev != NULL) {
            append_to_succs(prev, current);
        }
        prev = current;
        current = strtok(NULL, delimiters);
    }
}

/// Return last character of string
char last_char(char *str) {
    size_t len = strlen(str);
    if (len == 0) return '\0';
    return str[len - 1];
}

/// Return true if token ends with . ? !
bool token_ends_a_sentence(char *token) {
    char c = last_char(token);
    return (c == '.' || c == '?' || c == '!');
}

/// Return a random token index that starts with a capital letter
size_t random_token_id_that_starts_a_sentence() {
    size_t id;
    do {
        id = rand() % tokens_size;
    } while (!isupper((unsigned char)tokens[id][0]));
    return id;
}

/// Generate a random sentence
char *generate_sentence(char *sentence, size_t sentence_size) {
    size_t current_id = random_token_id_that_starts_a_sentence();
    sentence[0] = '\0';

    strcat(sentence, tokens[current_id]);

    if (token_ends_a_sentence(tokens[current_id]))
        return sentence;

    while (1) {
        size_t succ_count = succs_sizes[current_id];
        if (succ_count == 0) break;

        char *next = succs[current_id][rand() % succ_count];
        size_t next_len = strlen(sentence) + strlen(next) + 2;
        if (next_len >= sentence_size) break;

        strcat(sentence, " ");
        strcat(sentence, next);

        if (token_ends_a_sentence(next)) break;

        current_id = token_id(next);
    }

    return sentence;
}

int main() {
    // Add file reading at the beginning
    FILE *file = fopen("pg84.txt", "r");
    if (!file) {
        printf("Error: Could not open pg84.txt\n");
        return 1;
    }
    
    size_t bytes_read = fread(book, 1, sizeof(book) - 1, file);
    book[bytes_read] = '\0';
    fclose(file);

    replace_non_printable_chars_with_space();

    tokenize_and_fill_succs(" \n\r\t", book);

    srand(time(NULL));

    char sentence[1000];

    // Generate a random question
    do {
        generate_sentence(sentence, sizeof sentence);
    } while (last_char(sentence) != '?');
    puts(sentence);

    // Generate a random exclamation
    do {
        generate_sentence(sentence, sizeof sentence);
    } while (last_char(sentence) != '!');
    puts(sentence);
}
