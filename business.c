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
#include "utils.h"


struct shelf {
    char* place;
    int stock; 
};

struct link
{
    shelf_t *value;
    link_t *next;
};

struct merch {
    char* name;
    char* desc;
    int price;
    ioopm_list_t *locations;
};

struct warehouse {
    ioopm_hash_table_t *ht_merch;
    ioopm_hash_table_t *ht_identifier;
    ioopm_list_t *cart;
};

static bool eq_function(elem_t a, elem_t b)
{
  return strcmp(a.s, b.s) == 0;
}

static size_t hash_function(elem_t key)
{
  const char *str = key.s;
  size_t result = 0;
  while (*str != '\0')
  {
    result = result * 31 + ((unsigned char)*str);
    str++;
  }
  return result;
}



static void free_merch(merch_t *merch){
    ioopm_list_destroy(merch->locations);
    free(merch->name);
    free(merch->desc);
    free(merch);
    return;
}

static merch_t *make_item(char *name, char *desc, int price){
    merch_t *new_merch = calloc(1, sizeof(merch_t));
    new_merch->name = name;
    new_merch->desc = desc;
    new_merch->price = price;
    new_merch->locations = ioopm_list_create();
    return new_merch;
}

//HAS REPSONSIBILITY FOR ALMOST ALL MERCH AND HASHTABLES AND STUFF
warehouse_t *create_warehouse(){
    warehouse_t *new = calloc(sizeof(warehouse_t), 1);
    new->ht_identifier = ioopm_hash_table_create(hash_function, eq_function);
    new->ht_merch = ioopm_hash_table_create(hash_function, eq_function);
    return new;
}

void destroy_warehouse(warehouse_t *wh){
    ioopm_hash_table_destroy(wh->ht_identifier);
    ioopm_hash_table_destroy(wh->ht_merch);

    //Förstöra carts här

    free(wh);
}

bool add_item(ioopm_hash_table_t *ht, char *name, char *desc, int price) {;
    elem_t result;
    if (ioopm_hash_table_lookup(ht, string_elem(name), &result)) {
        return false;
    }
    else {
        merch_t *new_merch = make_item(name, desc, price);
        ioopm_hash_table_insert(ht, string_elem(new_merch->name), ptr_elem(new_merch));
        return true;
    }
}

void input_item(warehouse_t *wt){
    char *name = ask_question_string("Varans namn:");
    char *desc = ask_question_string("Varubeskrivning:");
    int price = ask_question_int("Varans kostnad (uttryckt i ören):");

    if(!add_item(wt->ht_merch, name, desc, price)){ // Giive the data to add_item if successfull otherwise free
        printf("Varan finns redan\n");
        free(name);
        free(desc);
    }
    return;
}



void list_items(warehouse_t *wt) {
    ioopm_hash_table_iterator_t *iter = ioopm_list_iterator_create(wt->ht_merch); 
    bool stop = false;
    
    do
    {
    for (int i = 1; i <= 20; i++) {
        if (ioopm_hash_table_iterator_at_end(iter)) {
            printf("Last item reached\n");
            return;
        }
        else {
        printf("%d. %s\n", i, ioopm_hash_table_iterator_current_key(iter).s);
        ioopm_hash_table_iterator_advance(iter);
        }
    }
    char *input = ask_question_string("Type N or n to return to main menu, otherwise enter");
    stop = (input[0] == 'N' || input[0] == 'n');
    free(input);
    
    } while (!stop);
    ioopm_hash_table_iterator_destroy(iter);
    return;
}

bool remove_item(warehouse_t *wt, char *remove) {
    elem_t result;
    if (ioopm_hash_table_lookup(wt->ht_merch, string_elem(remove), &result)) {
        merch_t *merch = result.p; 
        ioopm_list_iterator_t *list_iter = ioopm_list_iterator_create(merch->locations);

        while(!ioopm_list_iterator_at_end(list_iter)){
            shelf_t *current = ioopm_list_iterator_current(list_iter).p;
            elem_t ignore;
            ioopm_hash_table_remove(wt->ht_identifier, string_elem(current->place), &ignore);
            free(current->place);
            free(current);
            ioopm_list_iterator_advance(list_iter);
        }
        ioopm_list_iterator_destroy(list_iter);
        ioopm_hash_table_remove(wt->ht_merch, string_elem(remove), &result);
        free_merch(merch);
        return true;
    }
    else {
        return false;
    }
}

static void change_location_name(warehouse_t *wt, merch_t *merch) {
    ioopm_list_iterator_t *list_iter = ioopm_list_iterator_create(merch->locations);
    while(!ioopm_list_iterator_at_end(list_iter)){
        shelf_t *current = ioopm_list_iterator_current(list_iter).p;
        ioopm_hash_table_insert(wt->ht_identifier, string_elem(current->place), string_elem(merch->name));
        ioopm_list_iterator_advance(list_iter);
    }
    ioopm_list_iterator_destroy(list_iter);
}

void edit_merch(warehouse_t *wt) {
    elem_t result;
    elem_t ignore;
    char *old_name = ask_question_string("Name of item you want to edit\n");
    if (ioopm_hash_table_lookup(wt->ht_merch, string_elem(old_name), &result)) {
        merch_t *merch = result.p; 
        char *new_name = ask_question_string("Item's new name:");
        char *new_desc = ask_question_string("Description of item:");
        int new_price = ask_question_int("Price of new item:");
        if (!ioopm_hash_table_lookup(wt->ht_merch, string_elem(new_name), &result) || strcmp(old_name, new_name) == 0) {
            ioopm_hash_table_remove(wt->ht_merch, string_elem(old_name), &ignore);
            char *previous_name = merch->name;
            char *previous_desc = merch->desc;
            merch->desc = new_desc;
            merch->price = new_price;
            change_location_name(wt, merch);
            ioopm_hash_table_insert(wt->ht_merch, string_elem(new_name), ptr_elem(merch));
            free(previous_name);
            free(previous_desc);
        }
        else {
            printf("Item already exists");
        }
    }
}


//Gives ownership of the sorted list to the caller, only responsible for freeing copied
//The caller should not free the shelves since they still belong to the warehouse, only the list
static ioopm_list_t *sort_list(ioopm_list_t *locations){
    ioopm_list_t *copy = ioopm_list_create();

    for(size_t i = 0; i < ioopm_list_size(locations); i ++){
        elem_t element;
        ioopm_list_get(locations, i, &element);
        ioopm_list_append(copy, element);
    }

    ioopm_list_t *sorted_locations = ioopm_list_create();
    size_t first_list_size = ioopm_list_size(copy); 
    for(size_t i = 0; i < first_list_size; i++){
        size_t shelf_to_remove = 0;
        elem_t smallest;
        ioopm_list_get(copy, shelf_to_remove, &smallest);
        shelf_t *smallest_ascii = smallest.p;
        for(size_t j = 1; j < ioopm_list_size(copy); j ++){
            elem_t compare;
            ioopm_list_get(copy, j, &compare);
            shelf_t *compare_ascii = compare.p;

            if(strcmp(compare_ascii->place, smallest_ascii->place) < 0){
                smallest_ascii = compare_ascii;
                shelf_to_remove = j;
            }
        }
        elem_t ignore;
        ioopm_list_append(sorted_locations, ptr_elem(smallest_ascii));
        ioopm_list_remove(copy, shelf_to_remove, &ignore);
    }
    ioopm_list_destroy(copy);
    return sorted_locations;
}




void show_stock(warehouse_t *wt, char* merch_name) {
    elem_t result; 
    if (ioopm_hash_table_lookup(wt->ht_merch, string_elem(merch_name), &result)) {
        merch_t *merch = result.p; 
        ioopm_list_t *sorted_list = sort_list(merch->locations);
        ioopm_list_iterator_t *iter = ioopm_list_iterator_create(sorted_list); 
        while (!ioopm_list_iterator_at_end(iter)) {
            shelf_t *shelf = ioopm_list_iterator_current(iter).p;
            printf("Location: %s, stock: %d\n", shelf->place, shelf->stock);
            ioopm_list_iterator_advance(iter);
        }
        return;
    }
    else {
        printf("Stock doesn't exist");
    }
}





