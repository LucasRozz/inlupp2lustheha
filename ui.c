#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "business.h"
#include "utils.h"



void print_menu(){
  char *add = "[A]dd merchandise";
  char *list = "[L]ist merchandise";
  char *remove = "Remove merchandise with [D]";
  char *edit = "[E]dit merchandise";
  char *show = "[S]how stock";
  char *replenish = "Re[P]lenish stock";
  char *create_cart = "[C]reate cart";
  char *rm_cart = "[R]emove cart";
  char *add_to_cart = "Add a new cart with [+]";
  char *rm_from_cart = "Remove from cart with [-]";
  char *calc_cost = "Calculate the cost with [=]";
  char *checkout = "Check[O]ut";
  char *quit = "[Q]uit";

  fprintf(stdout,"%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",add, list, remove, edit, show, replenish, create_cart, rm_cart, add_to_cart, rm_from_cart, calc_cost, checkout, quit);
}

void input_item(warehouse_t *wh){
    char *name = ask_question_string("Varans namn:");
    char *desc = ask_question_string("Varubeskrivning:");
    int price = ask_question_int("Varans kostnad (uttryckt i ören):");

    if(!add_item(wh, name, desc, price)){ // Frees the data even if the item exists
        printf("Varan finns redan\n");
    }
    free(name);
    free(desc);
    return;
}

char ask_question_menu(){
  int buf_size = 50;
  char buf[buf_size];
  do {
    print_menu();
    read_string(buf, buf_size);
    putchar('\n');
  }while(!accepted_menu(buf));
  return toupper(buf[0]);
}

bool confirm_removal(char* confirm){
  fprintf(stdout, "Are you sure you want to remove: %s\n", confirm);

  char *input = ask_question_string("Enter a string beginning with y or Y to confirm");
  if(input[0] == 'y' ||  input[0] == 'Y'){
    free(input);
    return true;
  }
  free(input);
  return false;
}

int get_price(int product_price){
    int kr = product_price / 100;
    int small = product_price % 100;
    return kr+small;
}