#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "utils.h"
extern char *strdup(const char *);



void clear_input_buffer(void){
    int c;
    do
    {
        c = getchar();
    } 
    while (c != '\n' && c != EOF);
}

int read_string(char *buf, int buf_size){
  int counter = 0;
  int c;

  while(counter < buf_size - 1){
    c = getchar();
    if(c != '\n' ){
      buf[counter] = c;
      counter += 1;
    }
    if(c == '\n' && EOF){
      break;
    }
    if(counter == buf_size - 1){
      clear_input_buffer();
      break;
    }
    
  }
    buf[counter] = '\0';
    return counter;
}

typedef bool check_func(char*);
typedef answer_t convert_func(char*);

bool is_number(char *str){
    int length = strlen(str);
    if(str[0] == '-' || isdigit(str[0])){
        if(str[0] == '-' && length == 1){
            return false;
        }
        for(int i = 1; i < length; i++){
            char symbol = str[i];
            if(!isdigit(symbol)){
                return false;
            }
        }
        return true;
    }
    return false;
}

bool not_empty(char *str) {
  return strlen(str) > 0;
}

answer_t ask_question(char *question, check_func *check, convert_func *convert)
{
  int buf_size = 250;
  char buf[buf_size];
  do {
    printf("%s\n", question);
    read_string(buf, buf_size);
  } while(!check(buf));
  return convert(buf);
}

char *ask_question_string(char *question) {
  return ask_question(question, not_empty, (convert_func *) strdup).string_value;
}

int ask_question_int(char *question){
  answer_t answer = ask_question(question, is_number, (convert_func *) atoi);
  return answer.int_value;
}




