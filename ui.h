#pragma once
#include "business.h"
#include <stdbool.h>


void print_menu();
void input_item(warehouse_t *wh);
char ask_question_menu();
bool confirm_edit(char *confirm);
void list_items(warehouse_t *wh);
bool confirm_removal(char* confirm);
bool confirm_cart_removal(size_t cart_id);
void event_loop(void);



/**
* @brief Lists the merchandise stored in the warehouse.
* @param wh The warehouse.
*/
void list_items(warehouse_t *wh);