#pragma once
#include "business.h"

void input_item(warehouse_t *wh);
void print_menu();
char ask_question_menu();
bool confirm_removal(char* confirm);
bool confirm_name_edit(warehouse_t *wh, char *old_name, char *new_name);
bool confirm_desc_edit(warehouse_t *wh, char *old_name, char *new_desc);
bool confirm_price_edit(warehouse_t *wh, char *old_name, int new_price);