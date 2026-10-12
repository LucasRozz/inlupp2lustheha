#pragma once
typedef struct ioopm_list_element ioopm_list_element_t;
typedef struct link link_t;
typedef struct shelf shelf_t;
typedef struct merch merch_t;
typedef struct warehouse warehouse_t;




void show_stock(warehouse_t *wt, char* merch_name);
bool add_item(ioopm_hash_table_t *ht, char *name, char *desc, int price);
void list_items(warehouse_t *wt);
bool remove_item(warehouse_t *wt, char *remove);
void edit_merch(warehouse_t *wt);
void show_stock(warehouse_t *wt, char* merch_name);
warehouse_t *create_warehouse();


