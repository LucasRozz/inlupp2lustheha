#pragma once
typedef struct ioopm_list_element ioopm_list_element_t;
typedef struct link link_t;
typedef struct shelf shelf_t;
typedef struct merch merch_t;
typedef struct warehouse warehouse_t;
typedef struct cart cart_t;


void destroy_warehouse(warehouse_t *wh);

void show_stock(warehouse_t *wt, char* merch_name);
bool add_item(warehouse_t *wh, char *name, char *desc, int price);
void list_items(warehouse_t *wt);
bool remove_item(warehouse_t *wt, char *remove);
bool edit_merch(warehouse_t *wh, char *old_name, char *new_name, char *new_desc, int new_price);
void show_stock(warehouse_t *wt, char* merch_name);
warehouse_t *create_warehouse();
bool replenish_stock(warehouse_t *wh, char *merch_name, char* storage_id, int stock_increase);

