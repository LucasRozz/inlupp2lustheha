#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "business.h"
#include "utils.h"

static bool correct_shelf(char *shelf){
  int length = strlen(shelf);
  if(length == 3){
    if(!isupper(shelf[0])){
      return false;
    }
    for(int i = 1; i < length; i++){
      if(!isdigit(shelf[i])){
        return false;
      }
    }
    return true;
  }
  return false;
}

static bool accepted_menu(char *choice){
  char *accepted = "AaLlDdEeSsPpCcRr+-=OoQq";
  for(size_t i = 0; i < strlen(accepted); i ++){
    if(choice[0] == accepted[i]){

      return true;
    }
  }
  return false;
}

static char *ask_question_shelf(char *question){
  return ask_question(question, correct_shelf, (convert_func *) strdup).string_value;
}

static void print_menu()
{
    char *add = "[A]dd merchandise";
    char *list = "[L]ist merchandise";
    char *remove = "Remove merchandise with [D]";
    char *edit = "[E]dit merchandise";
    char *show = "[S]how stock";
    char *replenish = "Re[P]lenish stock";
    char *create_cart = "[C]reate cart";
    char *rm_cart = "[R]emove cart";
    char *add_to_cart = "Add to cart with [+]";
    char *rm_from_cart = "Remove from cart with [-]";
    char *calc_cost = "Calculate the cost with [=]";
    char *checkout = "Check[O]ut";
    char *quit = "[Q]uit";

    fprintf(stdout, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", add, list, remove, edit, show, replenish, create_cart, rm_cart, add_to_cart, rm_from_cart, calc_cost, checkout, quit);
}

static void input_item(warehouse_t *wh)
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

static char ask_question_menu()
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

static void show_stock(warehouse_t *wh, char *merch_name)
{
    size_t count;
    shelf_t **shelves = get_sorted_shelves(wh, merch_name, &count);

    if (shelves == NULL)
    {
        printf("%s does not exist\n", merch_name);
        return;
    }
    if (count == 0)
    {
        printf("No stock\n");
    }
    for (size_t i = 0; i < count; i++)
    {
        printf("Location: %s, stock: %d\n",
               shelf_get_place(shelves[i]), shelf_get_stock(shelves[i]));
    }
    free(shelves); 
}

static bool confirm_edit(char *confirm)
{
    fprintf(stdout, "Are you sure you want to edit: %s\n\n", confirm);
    char *input = ask_question_string("To confirm enter a sentence beginning with [Y] or [N] to regret");
    if (input[0] == 'Y' || input[0] == 'y')
    {
        free(input);
        return true;
    }
    free(input);
    return false;
}

static void list_items(warehouse_t *wh) {
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
    free(names);
}
  

static bool confirm_removal(char *confirm)
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

static bool confirm_cart_removal(size_t cart_id)
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



void event_loop(warehouse_t *wh)
{
    char input;
    while (true)
    {
        input = ask_question_menu();   // a char, not allocated memory: never free it
        if (input == 'A')
        {
            input_item(wh);            // input_item frees its own strings
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
                free(to_remove);
                continue;
            }
            if (!confirm_removal(to_remove))
            {
                fprintf(stdout, "%s was not removed\n", to_remove);
                free(to_remove);
                continue;
            }
            remove_item(wh, to_remove);
            fprintf(stdout, "%s was removed\n", to_remove);
            free(to_remove);
        }
        else if (input == 'E')
        {
            char *edit = ask_question_string("What merch would you like to change?");
            if (!lookup_merch(wh, edit))
            {
                fprintf(stdout, "Merch: %s does not exist\n\n", edit);
                free(edit);
                continue;
            }

            char *new_name = ask_question_string("New name: ");
            char *new_desc = ask_question_string("New description: ");
            int new_price = ask_question_int("New price: ");
            if (!confirm_edit(edit))
            {
                fprintf(stdout, "\n%s was not changed\n\n", edit);
                free(new_name);
                free(new_desc);
                free(edit);
                continue;
            }
            if (!edit_merch(wh, edit, new_name, new_desc, new_price))
            {
                fprintf(stdout, "Merch: %s already exists\n", new_name);
            }
            fprintf(stdout, "\n%s has now been changed\n\n", edit);
            free(new_name);
            free(new_desc);
            free(edit);
        }
        else if (input == 'S')
        {
            char *merch_to_show = ask_question_string("Which merchandise do you want to see the stock for: ");
            if (!lookup_merch(wh, merch_to_show))
            {
                fprintf(stdout, "%s does not exist\n", merch_to_show);
                free(merch_to_show);
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
                free(storage_location);
                free(merch_to_replenish);
                continue;
            }

            if (!correct_shelf(storage_location))
            {
                fprintf(stdout, "\n%s is not an accepted storage location\n\n", storage_location);
                free(storage_location);
                free(merch_to_replenish);
                continue;
            }
            int amount = ask_question_int("Amount to be replenished: ");
            if (!replenish_stock(wh, merch_to_replenish, storage_location, amount))
            {
                fprintf(stdout, "\nStock couldn't be replenished\n\n");
                free(storage_location);
                free(merch_to_replenish);
                continue;
            }
            fprintf(stdout, "%d %s was placed in %s\n\n", amount, merch_to_replenish, storage_location);
            free(merch_to_replenish);
            free(storage_location);
        }
        else if (input == 'C')
        {
            create_cart(wh);
            fprintf(stdout, "Cart with id: %zu was created\n\n", get_added_cart_id(wh));
        }
        else if (input == 'R')
        {
            size_t cart_to_remove = ask_question_int("Which cart do you want to remove?");
            size_t carts = get_added_cart_id(wh);
            if (cart_to_remove > carts || cart_to_remove <= 0)
            {
                fprintf(stdout, "Cart with ID: %zu does not exist\n", cart_to_remove);
                continue;
            }
            if (!confirm_cart_removal(cart_to_remove))
            {
                fprintf(stdout, "Cart with ID: %zu was not removed\n", cart_to_remove);
                continue;
            }

            if (remove_cart(wh, cart_to_remove))
            {
                fprintf(stdout, "\nCart with ID: %zu was removed\n\n", cart_to_remove);
            }
            else
            {
                fprintf(stdout, "\nCart with ID: %zu does not exist\n\n", cart_to_remove);
            }
        }
        else if (input == '+')
        {
            size_t cart_to_add = ask_question_int("Which cart do you want to add to?");
            char *merch = ask_question_string("Name of the merch you want to add to the cart?");
            int amount = ask_question_int("Amount of items to add to cart:");

            if (!add_to_cart(wh, merch, amount, cart_to_add))
            {
                fprintf(stdout, "\n%d %s could not be added to cart: %zu\n\n", amount, merch, cart_to_add);
                free(merch);
                continue;
            }
            fprintf(stdout, "\n%d of: %s was added to cart: %zu\n\n", amount, merch, cart_to_add);
            free(merch);
        }
        else if (input == '-')
        {
            size_t cart = ask_question_int("Which cart do you want to remove from?");
            char *merch = ask_question_string("Name of the merch you want to remove from the cart?");
            int amount = ask_question_int("Amount of items to remove from cart:");

            if (!remove_from_cart(wh, cart, merch, amount))
            {
                fprintf(stdout, "\n%d %s could not be removed from cart: %zu\n\n", amount, merch, cart);
                free(merch);
                continue;
            }
            fprintf(stdout, "\n%d %s was removed from cart: %zu\n\n", amount, merch, cart);
            free(merch);
        }
        else if (input == '=')
        {
            size_t cart = ask_question_int("Which cart do you want to calculate the cost for?");
            int cost = calculate_cost(wh, cart);
            if (cost == -1)
            {
                fprintf(stdout, "\n\nCart: %zu does not exist\n\n", cart);
                continue;
            }
            fprintf(stdout, "\n\nCurrent cost for cart: %zu is: %dSEK\n\n", cart, cost);
        }
        else if (input == 'O')
        {
            size_t cart_to_checkout = ask_question_int("Id of the cart you want to checkout?");
            if (checkout(wh, cart_to_checkout))
            {
                fprintf(stdout, "\nCart: %zu was checked out\n\n", cart_to_checkout);
            }
            else
            {
                fprintf(stdout, "\nCart: %zu does not exist\n\n", cart_to_checkout);
            }
        }
        else if (input == 'Q')
        {
            char *quit = ask_question_string("If you want to quit confirm with [y] or deny with [n]");
            bool confirmed = (quit[0] == 'Y' || quit[0] == 'y');
            free(quit);                 // free before both returning and continuing
            if (confirmed)
            {
                return;
            }
        }
    }
}

