/**
 * @file business.h
 * @brief Warehouse business logic: merchandise, storage locations and carts. Contains no user interaction
 */
#pragma once

#include <stddef.h>
#include <stdbool.h>
typedef struct shelf shelf_t;
typedef struct warehouse warehouse_t;

/** 
* @brief Creates empty warehouse.
* @return A pointer to the newly created warehouse.
*/
warehouse_t *create_warehouse(void);

/**
* @brief Frees the warehouse and all its associated data.
* @param wh The warehouse to destroy.
*/
void destroy_warehouse(warehouse_t *wh);

/**
* @brief Creates a new shopping cart and assigns it an ID.
* @param wh The warehouse in which the cart is created.
*/
void create_cart(warehouse_t *wh);

/**
* @brief Retrieves the ID of the most recently created cart.
* @param wh The warehouse containing the carts.
* @return The ID of the most recently created cart.
*/
size_t get_added_cart_id(warehouse_t *wh);

/**
* @brief Adds new merchandise to the warehouse.
* @param wh The warehouse.
* @param name The name of the merchandise.
* @param desc A description of the merchandise.
* @param price The price of the merchandise.
* @return True if the merchandise was added, false if it already exists.
*/
bool add_item(warehouse_t *wh, char *name, char *desc, int price);

/**
* @brief Checks whether merchandise exists in the warehouse.
* @param wh The warehouse.
* @param merch The name of the merchandise.
* @return True if the merchandise exists, false otherwise.
*/
bool lookup_merch(warehouse_t *wh, char *merch);

/**
* @brief Retrieves the price of a merchandise item.
* @param wh The warehouse.
* @param merch The name of the merchandise.
* @return The price of the merchandise.
*/
int get_price(warehouse_t *wh, char *merch);

/**
* @brief Retrieves the description of a merchandise item.
* @param wh The warehouse.
* @param merch The name of the merchandise.
* @return A pointer to the merchandise description.
*/
char *get_desc(warehouse_t *wh, char *merch);

char **get_merch_names(warehouse_t *wh, size_t *count);


/**
* @brief Removes merchandise and its associated storage locations.
* @param wh The warehouse.
* @param remove The name of the merchandise to remove.
* @return True if the merchandise was removed, false if it was not found.
*/
bool remove_item(warehouse_t *wh, char *remove);

/**
* @brief Changes the name, description and price of existing merchandise.
* @param wh The warehouse.
* @param old_name The current name of the merchandise.
* @param new_name The new name of the merchandise.
* @param new_desc The new description.
* @param new_price The new price.
* @return True if the merchandise was succesfully edited, false otherwise.
*/
bool edit_merch(warehouse_t *wh, char *old_name, char *new_name, char *new_desc, int new_price);

/**
 * @brief Gets the storage locations of a merch, sorted by name.
 * @param wh The warehouse.
 * @param merch_name The name of the merchandise.
 * @param count Set to the number of storage locations.
 * @return An array of shelves, or NULL if the merch does not exist.
 */
shelf_t **get_sorted_shelves(warehouse_t *wh, char *merch_name, size_t *count);

/// @brief The name of a storage location (borrowed, do not free).
char *shelf_get_place(shelf_t *shelf);

/// @brief The number of items stored on a storage location.
int shelf_get_stock(shelf_t *shelf);

/**
* @brief Adds stock to merchandise at a specified storage location.
* @param wh The warehouse.
* @param merch_name The name of the merchandise.
* @param storage_id The storage location identifier.
* @param stock_increase The amount of stock to add.
* @return True if the stock was successfully replenished, false otherwise.
*/
bool replenish_stock(warehouse_t *wh, char *merch_name, char* storage_id, int stock_increase);

/**
* @brief Removes a shopping cart from the warehouse.
* @param wh The warehouse.
* @param id The ID of the cart to remove.
* @return True if the cart was removed, false if it was not found.
*/
bool remove_cart(warehouse_t *wh, size_t id);

/**
* @brief Adds merchandise to a shopping cart.
* @param wh The warehouse.
* @param key The name of the merchandise to add.
* @param quantity The quantity to add.
* @param id The ID of the shopping cart.
* @return True if the merchandise was added, false otherwise.
*/
bool add_to_cart(warehouse_t *wh, char *key, int quantity, size_t id);

/**
* @brief Calculates the total cost of the merchandise in a cart.
* @param wh The warehouse.
* @param cart_id The ID of the shopping cart.
* @return The total cost, or -1 if the cart does not exist.
*/
int calculate_cost(warehouse_t *wh, size_t cart_id);

/**
* @brief Removes a specified quantity of merchandise from a shopping cart.
* @param wh The warehouse.
* @param cart_id The ID of the shopping cart.
* @param merch_name The name of the merchandise to remove.
* @param amount The quantity to remove.
* @return True if the merchandise was removed, false otherwise.
*/
bool remove_from_cart(warehouse_t *wh, size_t cart_id, char *merch_name, int amount);

/**
* @brief Completes a purchase by deducting stock and removing the cart.
* @param wh The warehouse.
* @param cart_id The ID of the shopping cart to check out.
* @return True if checkout was successful, false if the cart was not found.
*/
bool checkout(warehouse_t *wh, size_t cart_id);