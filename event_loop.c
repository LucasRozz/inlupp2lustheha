#include "utils.h"
#include "business.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int main(void){
    warehouse_t *wh = create_warehouse();
    add_item(wh, "shurda", "shurda", 1000);
    replenish_stock(wh, "shurda", "A25", 100);
    char input;
    do{
    input = ask_question_menu();
    if(input == 'A'){
        input_item(wh);
    }
    else if(input == 'L'){
        list_items(wh);
    }
    else if(input == 'D'){
        char *to_remove = ask_question_string("Vilken vara vill du ta bort?\n");
        if(!lookup_merch(wh, to_remove)){
            fprintf(stdout, "%s does not exist\n", to_remove);
            continue;
        }
        if(!confirm_removal(to_remove)){
            fprintf(stdout, "%s was not removed\n", to_remove);
            continue; 
        }
    remove_item(wh, to_remove);
    fprintf(stdout, "%s was removed\n", to_remove);
    free(to_remove);
    
    }
    else if(input == 'E'){
        char *edit;
        edit = ask_question_string("What merch would you like to change?");
        if(!lookup_merch(wh, edit)){
            fprintf(stdout, "Merch: %s does not exist\n\n", edit);
            continue;
        }

        char *new_name = ask_question_string("New name: ");
        char *new_desc = ask_question_string("New description: ");
        int new_price = ask_question_int("New price: ");

        if(!edit_merch(wh, edit, new_name, new_desc, new_price)){
            fprintf(stdout, "Merch: %s already exists\n", new_name);
        }
        fprintf(stdout, "\nMerch has been changed\n");
        free(new_name);
        free(new_desc);
        free(edit);
    }
    else if(input == 'S'){
        char *merch_to_show = ask_question_string("Which merchandise do you want to see the stock for: ");
        if(!lookup_merch(wh, merch_to_show)){
            fprintf(stdout, "%s does not exist", merch_to_show);
            continue;
        }
        show_stock(wh, merch_to_show);
        free(merch_to_show);
    }
    else if(input == 'P'){
        char *merch_to_replenish = ask_question_string("Which merchandise do you want to replenish: ");
        
        if(!lookup_merch(wh, merch_to_replenish)){
            fprintf(stdout, "\n%s does not exist\n\n", merch_to_replenish);
            continue;
        }

        char *storage_location = ask_question_string("Where should the merchandise be replenished");
        if(!correct_shelf(storage_location)){
            fprintf(stdout, "\n%s is not an accepted storage location\n\n", storage_location);
            continue;
        }
        int amount = ask_question_int("Amount to be replenished: ");
        if(!replenish_stock(wh, merch_to_replenish, storage_location, amount)){
            fprintf(stdout,"\n stock couldnt be replenished");
            continue;
        }
        fprintf(stdout, "%d %s was placed in %s\n\n", amount, merch_to_replenish, storage_location);
        free(merch_to_replenish);
        free(storage_location);
    }
    else if(input == 'C'){
        create_cart(wh);
        fprintf(stdout, "Cart with id: %ld was created\n\n", get_added_cart_id(wh));
    }
    else if(input == 'R'){
        size_t cart_to_remove = ask_question_int("Which cart do you want to remove?");
        size_t carts = get_added_cart_id(wh);
        if(cart_to_remove > carts || cart_to_remove <= 0){
            fprintf(stdout,"Cart with ID: %ld does not exist\n", cart_to_remove);
            continue;
        }
        remove_cart(wh, cart_to_remove);
        fprintf(stdout, "\nCart with ID: %ld was removed\n\n", cart_to_remove);
    }
    else if(input == '+'){
        char *merch = ask_question_string("Name of the merch you want to add to the cart?");
        int amount = ask_question_int("Amount of items to add to cart:");
        size_t cart_to_add = ask_question_int("Which cart do you want to add to?");

        if(!add_to_cart(wh, merch, amount, cart_to_add)){
            fprintf(stdout,"\n%d %s could not be added to cart: %ld\n\n", amount, merch, cart_to_add);
            continue;
        }
        fprintf(stdout,"\n%d of: %s was added to cart: %ld\n\n", amount, merch, cart_to_add);
        free(merch);
    }
    else if(input == '-'){
        char *merch = ask_question_string("Name of the merch you want to remove from the cart?");
        int amount = ask_question_int("Amount of items to remove from cart:");
        size_t cart = ask_question_int("Which cart do you want to remove from?");

        if(!remove_from_cart(wh, cart, merch, amount)){
            fprintf(stdout,"\n%d %s could not be removed from cart: %ld\n\n", amount, merch, cart);
            continue;
        }   
        fprintf(stdout,"\n%d %s was removed from cart: %ld\n\n", amount, merch, cart);
        free(merch);
    }
    else if(input == '='){
        size_t cart = ask_question_int("Which cart do you want to calculate the cost for?");
        int cost = calculate_cost(wh, cart);
        if(cost == -1){
            fprintf(stdout, "\n\nCart: %ld does not exist\n\n", cart);
            continue;
        }
        cost = get_price(cost);
        fprintf(stdout, "\n\nCurrent cost for cart: %ld is: %d\n\n", cart, cost);
    }
    else if(input == 'O'){
        
    }

    } while(input != 'Q');
    destroy_warehouse(wh);

    return 0;
}