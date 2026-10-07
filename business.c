#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "generic_data/common.h"
#include "generic_data/hash_table.h"
#include "generic_data/hash_table_iterator.h"
#include "generic_data/linked_list.h"
#include "generic_data/list_iterator.h"
#include "business.h"


struct shelf {
    char* name;
    int stock;
};

struct link
{
    shelf_t value;
    link_t *next;
};


struct list
{
    link_t *first;
    link_t *last;
    size_t size;
};


struct merch {
    char* name;
    char* desc;
    int price;
    ioopm_linked_list_t *locations;
};

struct warehouse {
    ioopm_hash_table_t *ht_main;
    ioopm_hash_table_t *ht_identifier;
    ioopm_linked_list_t *cart;
};

merch_t make_item(char *name, char *desc, int price){
    merch_t new_item = {.name = name, .desc = desc, .price = price};
    return new_item;
}

merch_t input_item(ioopm_hash_table_t *ht){
    char *name = ask_question_string("Varans namn:");
    char *desc = ask_question_string("Varubeskrivning:");
    int price = ask_question_int("Varans kosstnad (uttryckt i ören):");
    return make_item(name, desc, price);
}

void add_item(ioopm_hash_table_t *ht) {
    merch_t new_item = input_item(ht);
    elem_t result;
    if (ioopm_hash_table_lookup(ht, string_elem(new_item.name), &result)) {
        printf("Item already exists"); 
    }
    else {
        ioopm_hash_table_insert(ht, string_elem(new_item.name), );
    }
}

void list_db(ioopm_hash_table_t *ht, int no_items) {
  for ()
  if (ioopm_hash_table_lookup(ht, elem_t key, elem_t *result)
    printf("%d. %s\n", i + 1, product.name);
  }
}






