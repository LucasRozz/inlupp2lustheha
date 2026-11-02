#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "business.h"
#include "utils.h"

void print_menu()
{
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

    fprintf(stdout, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", add, list, remove, edit, show, replenish, create_cart, rm_cart, add_to_cart, rm_from_cart, calc_cost, checkout, quit);
}

void input_item(warehouse_t *wh)
{
    char *name = ask_question_string("Varans namn:");
    char *desc = ask_question_string("Varubeskrivning:");
    int price = ask_question_int("Varans kostnad: ");

    if (!add_item(wh, name, desc, price))
    { // Frees the data even if the item exists
        printf("Varan finns redan\n");
    }
    free(name);
    free(desc);
    return;
}

char ask_question_menu()
{
    int buf_size = 50;
    char buf[buf_size];
    do
    {
        print_menu();
        read_string(buf, buf_size);
        putchar('\n');
    } while (!accepted_menu(buf));
    return toupper(buf[0]);
}

bool confirm_edit(char *confirm)
{
    fprintf(stdout, "Are you sure you want to edit: %s\n\n", confirm);
    char *input = ask_question_string("To confirm enter a sentence beginning with [Y] or [N] to regret");
    if (input[0] == 'Y' || input[0] == 'y')
    {
        fprintf(stdout, "\n%s has now been changed\n\n", confirm);
        free(input);
        return true;
    }
    free(input);
    return false;
}

void list_items(warehouse_t *wh) {
    size_t count;
    char **names = get_merch_names(wh, &count);

    for(size_t i = 0; i < count; i ++){
        printf("%ld: %s\n", i + 1, names[i]);

        if((i + 1) % 20 == 0){
            char *input = ask_question_string("Type N or n to return to main menu, otherwise enter");
            if(input[0] == 'N' || input[0] == 'n'){
                free(input);
                break;
            }
            free(input);
        }
    }
}
  

bool confirm_removal(char *confirm)
{
    fprintf(stdout, "Are you sure you want to remove: %s\n", confirm);

    char *input = ask_question_string("Enter a string beginning with y or Y to confirm");
    if (input[0] == 'Y' || input[0] == 'y')
    {
        free(input);
        return true;
    }
    free(input);
    return false;
}

bool confirm_cart_removal(size_t cart_id)
{
    fprintf(stdout, "Are you sure you want to remove cart with id: %ld\n", cart_id);

    char *input = ask_question_string("Enter a string beginning with y or Y to confirm");
    if (input[0] == 'Y' || input[0] == 'y')
    {
        free(input);
        return true;
    }
    free(input);
    return false;
}



void event_loop(void)
{
    warehouse_t *wh = create_warehouse();
    add_item(wh, "shurda", "shurda", 1000);
    replenish_stock(wh, "shurda", "A25", 5);
    replenish_stock(wh, "shurda", "A26", 5);

    char input;
    while (true)
    {
        input = ask_question_menu();
        if (input == 'A')
        {
            input_item(wh);
        }
        else if (input == 'L')
        {
            list_items(wh);
        }
        else if (input == 'D')
        {
            char *to_remove = ask_question_string("Vilken vara vill du ta bort?\n");
            if (!lookup_merch(wh, to_remove))
            {
                fprintf(stdout, "%s does not exist\n", to_remove);
                continue;
            }
            if (!confirm_removal(to_remove))
            {
                fprintf(stdout, "%s was not removed\n", to_remove);
                continue;
            }
            remove_item(wh, to_remove);
            fprintf(stdout, "%s was removed\n", to_remove);
            free(to_remove);
        }
        else if (input == 'E')
        {
            char *edit;
            edit = ask_question_string("What merch would you like to change?");
            if (!lookup_merch(wh, edit))
            {
                fprintf(stdout, "Merch: %s does not exist\n\n", edit);
                continue;
            }

            char *new_name = ask_question_string("New name: ");
            char *new_desc = ask_question_string("New description: ");
            int new_price = ask_question_int("New price: ");
            if (!confirm_edit(edit))
            {
                fprintf(stdout, "\n%s was not changed\n\n", edit);
                continue;
            }
            if (!edit_merch(wh, edit, new_name, new_desc, new_price))
            {
                fprintf(stdout, "Merch: %s already exists\n", new_name);
            }
            free(new_name);
            free(new_desc);
            free(edit);
        }
        else if (input == 'S')
        {
            char *merch_to_show = ask_question_string("Which merchandise do you want to see the stock for: ");
            if (!lookup_merch(wh, merch_to_show))
            {
                fprintf(stdout, "%s does not exist", merch_to_show);
                continue;
            }
            show_stock(wh, merch_to_show);
            fprintf(stdout, "\n");
            free(merch_to_show);
        }
        else if (input == 'P')
        {
            char *storage_location = ask_question_string("Where should the merchandise be replenished");
            char *merch_to_replenish = ask_question_string("Which merchandise do you want to replenish: ");

            if (!lookup_merch(wh, merch_to_replenish))
            {
                fprintf(stdout, "\n%s does not exist\n\n", merch_to_replenish);
                continue;
            }

            if (!correct_shelf(storage_location))
            {
                fprintf(stdout, "\n%s is not an accepted storage location\n\n", storage_location);
                continue;
            }
            int amount = ask_question_int("Amount to be replenished: ");
            if (!replenish_stock(wh, merch_to_replenish, storage_location, amount))
            {
                fprintf(stdout, "\n stock couldnt be replenished");
                continue;
            }
            fprintf(stdout, "%d %s was placed in %s\n\n", amount, merch_to_replenish, storage_location);
            free(merch_to_replenish);
            free(storage_location);
        }
        else if (input == 'C')
        {
            create_cart(wh);
            fprintf(stdout, "Cart with id: %ld was created\n\n", get_added_cart_id(wh));
        }
        else if (input == 'R')
        {
            size_t cart_to_remove = ask_question_int("Which cart do you want to remove?");
            size_t carts = get_added_cart_id(wh);
            if (cart_to_remove > carts || cart_to_remove <= 0)
            {
                fprintf(stdout, "Cart with ID: %ld does not exist\n", cart_to_remove);
                continue;
            }
            if (!confirm_cart_removal(cart_to_remove))
            {
                fprintf(stdout, "Cart with ID: %ld was not removed\n", cart_to_remove);
                continue;
            }

            remove_cart(wh, cart_to_remove);
            fprintf(stdout, "\nCart with ID: %ld was removed\n\n", cart_to_remove);
        }
        else if (input == '+')
        {
            size_t cart_to_add = ask_question_int("Which cart do you want to add to?");
            char *merch = ask_question_string("Name of the merch you want to add to the cart?");
            int amount = ask_question_int("Amount of items to add to cart:");

            if (!add_to_cart(wh, merch, amount, cart_to_add))
            {
                fprintf(stdout, "\n%d %s could not be added to cart: %ld\n\n", amount, merch, cart_to_add);
                continue;
            }
            fprintf(stdout, "\n%d of: %s was added to cart: %ld\n\n", amount, merch, cart_to_add);
            free(merch);
        }
        else if (input == '-')
        {
            size_t cart = ask_question_int("Which cart do you want to remove from?");
            char *merch = ask_question_string("Name of the merch you want to remove from the cart?");
            int amount = ask_question_int("Amount of items to remove from cart:");

            if (!remove_from_cart(wh, cart, merch, amount))
            {
                fprintf(stdout, "\n%d %s could not be removed from cart: %ld\n\n", amount, merch, cart);
                continue;
            }
            fprintf(stdout, "\n%d %s was removed from cart: %ld\n\n", amount, merch, cart);
            free(merch);
        }
        else if (input == '=')
        {
            size_t cart = ask_question_int("Which cart do you want to calculate the cost for?");
            int cost = calculate_cost(wh, cart);
            if (cost == -1)
            {
                fprintf(stdout, "\n\nCart: %ld does not exist\n\n", cart);
                continue;
            }
            fprintf(stdout, "\n\nCurrent cost for cart: %ld is: %dSEK\n\n", cart, cost);
        }
        else if (input == 'O')
        {
            size_t cart_to_checkout = ask_question_int("Id of the cart you want to checkout?");
            checkout(wh, cart_to_checkout);
        }
        else if (input == 'Q')
        {
            char *quit = ask_question_string("If you want to quit confirm with [y] or deny with [n]");
            if (quit[0] == 'Y' || quit[0] == 'y')
            {
                destroy_warehouse(wh);
                return;
            }
        }
    }
}

int main(void)
{
    event_loop();
}