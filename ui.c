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

bool confirm_name_edit(warehouse_t *wh, char *old_name, char *new_name){
  char *desc = get_desc(wh, old_name);
  int price = get_price(wh, old_name);
  fprintf(stdout, "\nCurrent merch:\nName: %s\nDescription: %s\nPrice: %d\n\n", old_name, desc, price);
  fprintf(stdout, "Merch after change: \nName: %s\nDescription: %s\nPrice: %d\n\n", new_name, desc, price);
  char *input = ask_question_string("Enter a string beginning with y or Y to confirm");

  if(input[0] == 'y' ||  input[0] == 'Y'){
    free(input);
    return true;
  }
  free(input);
  return false;
}

bool confirm_desc_edit(warehouse_t *wh, char *old_name, char *new_desc){
  char *desc = get_desc(wh, old_name);
  int price = get_price(wh, old_name);
  fprintf(stdout, "Current merch:\nName: %s\nDescription: %s\nPrice: %d\n\n", old_name, desc, price);
  fprintf(stdout, "Merch after change: \nName: %s\nDescription: %s\nPrice: %d\n", old_name, new_desc, price);
  char *input = ask_question_string("Enter a string beginning with y or Y to confirm");

  if(input[0] == 'y' ||  input[0] == 'Y'){
    free(input);
    return true;
  }
  free(input);
  return false;
}

bool confirm_price_edit(warehouse_t *wh, char *old_name, int new_price){
  char *desc = get_desc(wh, old_name);
  int price = get_price(wh, old_name);
  fprintf(stdout, "Current merch:\nName: %s\nDescription: %s\nPrice: %d\n\n", old_name, desc, price);
  fprintf(stdout, "Merch after change: \nName: %s\nDescription: %s\nPrice: %d\n", old_name, desc, new_price);
  char *input = ask_question_string("Enter a string beginning with y or Y to confirm");

  if(input[0] == 'y' ||  input[0] == 'Y'){
    free(input);
    return true;
  }
  free(input);
  return false;
}