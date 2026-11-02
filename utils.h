/**
 * @file utils.h
 * @brief general functions needed to get input from the user
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdbool.h>
extern char *strdup(const char *);

typedef union { 
  int   int_value;
  float float_value;
  char *string_value;
  char symbol;
} answer_t;

/**
* @brief Function type for validating user input.
* @return  true if the input is valid.
*/
typedef bool check_func(char *);

/**
* Function type for converting user input
* into an answer_t value.
*/
typedef answer_t convert_func(char *);

/**
* Clears the remaining characters in the input buffer
* until a newline or EOF is encountered.
*/
void clear_input_buffer(void);

/**
* Reads a string from standard input into a buffer.
* Returns the number of characters read. 
*/
int read_string(char *buf, int buf_size);

/**
* Checks whether a string contains a valid integer.
*/
bool is_number(char *str);

/**
* @brief Checks whether a string is non-empty.
* @param str input string to see if its empty or not
* @return true if the string wasnt empty
*/
bool not_empty(char *str);


/**
* Asks the user a question, validates the input
* and converts it into an answer_t.
*/
answer_t ask_question(char *question, check_func *check, convert_func *convert);

/**
* @brief Asks the user for a non-empty string.
* @param question The question to print
* @return A newly allocated string. The caller must free it.
*/
char *ask_question_string(char *question);

/**
* @brief Asks the user for a valid integer.
* @param question The question to print
* @return An integer
*/
int ask_question_int(char *question);

#endif
