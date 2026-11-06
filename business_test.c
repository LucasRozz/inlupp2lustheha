#include <CUnit/Basic.h>
#include <stdbool.h>
#include <stddef.h>
#include "business.h"

/*
 * Tests for the warehouse business layer.
 *
 * All functions in business.c copy the strings they keep (strdup), so the
 * tests can pass string literals directly.
 *
 * There is no public function for reading a merch's total stock, so stock is
 * checked indirectly: a cart may reserve at most the total stock, so
 * "add_to_cart(N) succeeds but add_to_cart(N + 1) fails" means the stock is N.
 *
 * Run under valgrind as well: several tests (remove, edit, checkout) mainly
 * exist to catch double frees, use after free and leaks.
 * 
 * Most tests in this files is from ai.
 */

int init_suite(void)  { return 0; }
int clean_suite(void) { return 0; }

/* ---------- helpers ---------- */

static size_t new_cart(warehouse_t *wh)
{
  create_cart(wh);
  return get_added_cart_id(wh);
}

/* True if exactly `amount` units of `name` are available to reserve.
   Uses a temporary cart, which is removed again afterwards. */
static bool stock_is(warehouse_t *wh, char *name, int amount)
{
  size_t id = new_cart(wh);
  bool exact;
  if (amount == 0)
    {
      exact = !add_to_cart(wh, name, 1, id);
    }
  else
    {
      exact = add_to_cart(wh, name, amount, id) && !add_to_cart(wh, name, 1, id);
    }
  remove_cart(wh, id);
  return exact;
}

/* ---------- warehouse ---------- */

/* An empty warehouse can be created and destroyed without leaks. */
void test_create_destroy(void)
{
  warehouse_t *wh = create_warehouse();
  CU_ASSERT_PTR_NOT_NULL(wh);
  destroy_warehouse(wh);
}

/* Destroying a warehouse that still holds merch, shelves and carts frees
   everything (check with valgrind: 0 bytes lost). */
void test_destroy_full_warehouse(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  add_item(wh, "Hat", "Blue hat", 50);
  replenish_stock(wh, "Shoe", "A01", 5);
  replenish_stock(wh, "Hat", "B02", 2);
  size_t id = new_cart(wh);
  add_to_cart(wh, "Shoe", 2, id);
  destroy_warehouse(wh);
}


void test_add_item(void)
{
  warehouse_t *wh = create_warehouse();
  CU_ASSERT_TRUE(add_item(wh, "Shoe", "Red shoe", 100));
  CU_ASSERT_TRUE(lookup_merch(wh, "Shoe"));
  CU_ASSERT_EQUAL(get_price(wh, "Shoe"), 100);
  CU_ASSERT_STRING_EQUAL(get_desc(wh, "Shoe"), "Red shoe");
  CU_ASSERT_FALSE(lookup_merch(wh, "Hat"));
  destroy_warehouse(wh);
}

void test_add_duplicate(void)
{
  warehouse_t *wh = create_warehouse();
  CU_ASSERT_TRUE(add_item(wh, "Shoe", "Red shoe", 100));
  CU_ASSERT_FALSE(add_item(wh, "Shoe", "Other shoe", 999));
  CU_ASSERT_EQUAL(get_price(wh, "Shoe"), 100);
  CU_ASSERT_STRING_EQUAL(get_desc(wh, "Shoe"), "Red shoe");
  destroy_warehouse(wh);
}

void test_new_item_not_in_stock(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  CU_ASSERT_TRUE(stock_is(wh, "Shoe", 0));
  destroy_warehouse(wh);
}

void test_remove_item(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  add_item(wh, "Hat", "Blue hat", 50);

  CU_ASSERT_FALSE(remove_item(wh, "Sock"));
  CU_ASSERT_TRUE(remove_item(wh, "Shoe"));
  CU_ASSERT_FALSE(lookup_merch(wh, "Shoe"));
  CU_ASSERT_FALSE(remove_item(wh, "Shoe"));
  CU_ASSERT_TRUE(lookup_merch(wh, "Hat"));
  destroy_warehouse(wh);
}

void test_remove_item_with_stock(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  add_item(wh, "Hat", "Blue hat", 50);
  replenish_stock(wh, "Shoe", "A01", 5);
  replenish_stock(wh, "Shoe", "B02", 3);

  CU_ASSERT_FALSE(replenish_stock(wh, "Hat", "A01", 1)); 
  CU_ASSERT_TRUE(remove_item(wh, "Shoe"));
  CU_ASSERT_TRUE(replenish_stock(wh, "Hat", "A01", 1));   
  CU_ASSERT_TRUE(replenish_stock(wh, "Hat", "B02", 1));
  destroy_warehouse(wh);
}


void test_edit_same_name(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  CU_ASSERT_TRUE(edit_merch(wh, "Shoe", "Shoe", "Green shoe", 120));
  CU_ASSERT_EQUAL(get_price(wh, "Shoe"), 120);
  CU_ASSERT_STRING_EQUAL(get_desc(wh, "Shoe"), "Green shoe");
  destroy_warehouse(wh);
}


void test_edit_rename(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  add_item(wh, "Hat", "Blue hat", 50);
  replenish_stock(wh, "Shoe", "A01", 5);

  CU_ASSERT_TRUE(edit_merch(wh, "Shoe", "Boot", "Black boot", 200));
  CU_ASSERT_FALSE(lookup_merch(wh, "Shoe"));
  CU_ASSERT_TRUE(lookup_merch(wh, "Boot"));
  CU_ASSERT_EQUAL(get_price(wh, "Boot"), 200);

  CU_ASSERT_TRUE(stock_is(wh, "Boot", 5));                /* stock kept */
  CU_ASSERT_TRUE(replenish_stock(wh, "Boot", "A01", 1));  /* shelf follows */
  CU_ASSERT_FALSE(replenish_stock(wh, "Hat", "A01", 1));  /* still taken */
  destroy_warehouse(wh);
}

/* Spec 2.1.4: renaming to the name of another existing merch fails and
   changes nothing. */
void test_edit_to_existing_name(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  add_item(wh, "Hat", "Blue hat", 50);

  CU_ASSERT_FALSE(edit_merch(wh, "Shoe", "Hat", "x", 1));
  CU_ASSERT_EQUAL(get_price(wh, "Shoe"), 100);
  CU_ASSERT_EQUAL(get_price(wh, "Hat"), 50);
  CU_ASSERT_STRING_EQUAL(get_desc(wh, "Hat"), "Blue hat");
  destroy_warehouse(wh);
}

/* Editing a merch that does not exist fails. */
void test_edit_nonexisting(void)
{
  warehouse_t *wh = create_warehouse();
  CU_ASSERT_FALSE(edit_merch(wh, "Sock", "Sock2", "x", 1));
  destroy_warehouse(wh);
}

/* ---------- replenish ---------- */

/* Invalid replenishes: unknown merch, and amounts below 1 (spec 2.1.6:
   "increases the stock by at least one"). */
void test_replenish_invalid(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);

  CU_ASSERT_FALSE(replenish_stock(wh, "Sock", "A01", 5));
  CU_ASSERT_FALSE(replenish_stock(wh, "Shoe", "A01", 0));
  CU_ASSERT_FALSE(replenish_stock(wh, "Shoe", "A01", -3));
  CU_ASSERT_TRUE(stock_is(wh, "Shoe", 0));
  destroy_warehouse(wh);
}

/* Replenishing the same shelf twice adds to it; a second shelf adds to the
   total. Total stock = sum over all shelves. */
void test_replenish_adds_up(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);

  CU_ASSERT_TRUE(replenish_stock(wh, "Shoe", "A01", 3));
  CU_ASSERT_TRUE(replenish_stock(wh, "Shoe", "A01", 2));
  CU_ASSERT_TRUE(stock_is(wh, "Shoe", 5));

  CU_ASSERT_TRUE(replenish_stock(wh, "Shoe", "B02", 4));
  CU_ASSERT_TRUE(stock_is(wh, "Shoe", 9));
  destroy_warehouse(wh);
}

/* Spec: a storage location holds only one kind of merch. */
void test_replenish_shelf_taken(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  add_item(wh, "Hat", "Blue hat", 50);
  replenish_stock(wh, "Shoe", "A01", 5);

  CU_ASSERT_FALSE(replenish_stock(wh, "Hat", "A01", 1));
  CU_ASSERT_TRUE(stock_is(wh, "Shoe", 5));   /* Shoe unchanged */
  CU_ASSERT_TRUE(stock_is(wh, "Hat", 0));
  destroy_warehouse(wh);
}

/* ---------- carts ---------- */

/* Spec 2.1.7: cart ids are 1, 2, 3, ... and are never reused, even after a
   cart has been removed. */
void test_cart_ids(void)
{
  warehouse_t *wh = create_warehouse();
  CU_ASSERT_EQUAL(new_cart(wh), 1);
  CU_ASSERT_EQUAL(new_cart(wh), 2);
  CU_ASSERT_EQUAL(new_cart(wh), 3);
  CU_ASSERT_TRUE(remove_cart(wh, 2));
  CU_ASSERT_EQUAL(new_cart(wh), 4);
  destroy_warehouse(wh);
}

/* Removing a cart works once; non-existing ids fail. */
void test_remove_cart(void)
{
  warehouse_t *wh = create_warehouse();
  size_t id = new_cart(wh);
  CU_ASSERT_FALSE(remove_cart(wh, id + 1));
  CU_ASSERT_TRUE(remove_cart(wh, id));
  CU_ASSERT_FALSE(remove_cart(wh, id));
  destroy_warehouse(wh);
}

/* Removing a cart releases its reservation, so another cart can take the
   same stock. */
void test_remove_cart_releases_items(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  replenish_stock(wh, "Shoe", "A01", 5);
  size_t c1 = new_cart(wh);
  size_t c2 = new_cart(wh);

  CU_ASSERT_TRUE(add_to_cart(wh, "Shoe", 5, c1));
  CU_ASSERT_FALSE(add_to_cart(wh, "Shoe", 1, c2));
  CU_ASSERT_TRUE(remove_cart(wh, c1));
  CU_ASSERT_TRUE(add_to_cart(wh, "Shoe", 5, c2));
  destroy_warehouse(wh);
}

/* ---------- add to cart ---------- */

/* Invalid adds: unknown merch, unknown cart, quantity < 1, more than stock. */
void test_add_to_cart_invalid(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  replenish_stock(wh, "Shoe", "A01", 5);
  size_t id = new_cart(wh);

  CU_ASSERT_FALSE(add_to_cart(wh, "Sock", 1, id));
  CU_ASSERT_FALSE(add_to_cart(wh, "Shoe", 1, id + 1));
  CU_ASSERT_FALSE(add_to_cart(wh, "Shoe", 0, id));
  CU_ASSERT_FALSE(add_to_cart(wh, "Shoe", -1, id));
  CU_ASSERT_FALSE(add_to_cart(wh, "Shoe", 6, id));
  CU_ASSERT_TRUE(add_to_cart(wh, "Shoe", 5, id));
  destroy_warehouse(wh);
}

/* Merch without stock cannot be added at all. */
void test_add_to_cart_no_stock(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  size_t id = new_cart(wh);
  CU_ASSERT_FALSE(add_to_cart(wh, "Shoe", 1, id));
  destroy_warehouse(wh);
}

/* Spec 2.1.9: the total over ALL carts may never exceed the stock, so that
   every cart can be checked out. A failed add must not change the cart. */
void test_add_to_cart_all_carts(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  replenish_stock(wh, "Shoe", "A01", 5);
  size_t c1 = new_cart(wh);
  size_t c2 = new_cart(wh);

  CU_ASSERT_TRUE(add_to_cart(wh, "Shoe", 3, c1));
  CU_ASSERT_FALSE(add_to_cart(wh, "Shoe", 3, c2));   /* 3 + 3 > 5 */
  CU_ASSERT_TRUE(add_to_cart(wh, "Shoe", 2, c2));    /* 3 + 2 = 5 */
  CU_ASSERT_FALSE(add_to_cart(wh, "Shoe", 1, c1));   /* nothing left */
  CU_ASSERT_EQUAL(calculate_cost(wh, c1), 300);      /* c1 still has 3 */
  CU_ASSERT_EQUAL(calculate_cost(wh, c2), 200);
  destroy_warehouse(wh);
}

/* Adding the same merch twice to one cart adds up the quantities. */
void test_add_to_cart_accumulates(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  replenish_stock(wh, "Shoe", "A01", 5);
  size_t id = new_cart(wh);

  CU_ASSERT_TRUE(add_to_cart(wh, "Shoe", 2, id));
  CU_ASSERT_TRUE(add_to_cart(wh, "Shoe", 3, id));
  CU_ASSERT_EQUAL(calculate_cost(wh, id), 500);
  destroy_warehouse(wh);
}

/* ---------- calculate cost ---------- */

/* The example from spec 2.1.11: 2 x 50 + 8 x 3 = 124. An empty cart costs
   0, and a cart that does not exist gives -1. */
void test_calculate_cost(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "M1", "first", 50);
  add_item(wh, "M2", "second", 3);
  replenish_stock(wh, "M1", "A01", 10);
  replenish_stock(wh, "M2", "B02", 10);
  size_t id = new_cart(wh);
  size_t empty = new_cart(wh);

  add_to_cart(wh, "M1", 2, id);
  add_to_cart(wh, "M2", 8, id);
  CU_ASSERT_EQUAL(calculate_cost(wh, id), 124);
  CU_ASSERT_EQUAL(calculate_cost(wh, empty), 0);
  CU_ASSERT_EQUAL(calculate_cost(wh, 999), -1);
  destroy_warehouse(wh);
}

/* ---------- remove from cart ---------- */

/* Removing part, then the rest, then one too many. Removing 0 is allowed
   (spec 2.1.10: "zero or more"). */
void test_remove_from_cart(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  replenish_stock(wh, "Shoe", "A01", 5);
  size_t id = new_cart(wh);
  add_to_cart(wh, "Shoe", 5, id);

  CU_ASSERT_TRUE(remove_from_cart(wh, id, "Shoe", 0));
  CU_ASSERT_EQUAL(calculate_cost(wh, id), 500);
  CU_ASSERT_TRUE(remove_from_cart(wh, id, "Shoe", 2));
  CU_ASSERT_EQUAL(calculate_cost(wh, id), 300);
  CU_ASSERT_TRUE(remove_from_cart(wh, id, "Shoe", 3));
  CU_ASSERT_EQUAL(calculate_cost(wh, id), 0);
  CU_ASSERT_FALSE(remove_from_cart(wh, id, "Shoe", 1));
  destroy_warehouse(wh);
}

/* Invalid removes: too many, unknown cart, merch not in the cart, and a
   negative amount (which would otherwise ADD items). */
void test_remove_from_cart_invalid(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  add_item(wh, "Hat", "Blue hat", 50);
  replenish_stock(wh, "Shoe", "A01", 5);
  size_t id = new_cart(wh);
  add_to_cart(wh, "Shoe", 2, id);

  CU_ASSERT_FALSE(remove_from_cart(wh, id, "Shoe", 3));
  CU_ASSERT_FALSE(remove_from_cart(wh, id + 1, "Shoe", 1));
  CU_ASSERT_FALSE(remove_from_cart(wh, id, "Hat", 1));
  CU_ASSERT_FALSE(remove_from_cart(wh, id, "Shoe", -1));
  CU_ASSERT_EQUAL(calculate_cost(wh, id), 200);   /* nothing changed */
  destroy_warehouse(wh);
}

/* Items removed from a cart become available to other carts again. */
void test_remove_from_cart_releases(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  replenish_stock(wh, "Shoe", "A01", 5);
  size_t c1 = new_cart(wh);
  size_t c2 = new_cart(wh);

  add_to_cart(wh, "Shoe", 5, c1);
  CU_ASSERT_FALSE(add_to_cart(wh, "Shoe", 2, c2));
  remove_from_cart(wh, c1, "Shoe", 2);
  CU_ASSERT_TRUE(add_to_cart(wh, "Shoe", 2, c2));
  destroy_warehouse(wh);
}

/* ---------- checkout ---------- */

/* Checkout taking part of one shelf: stock decreases and the cart is gone. */
void test_checkout_partial_shelf(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  replenish_stock(wh, "Shoe", "A01", 5);
  size_t id = new_cart(wh);
  add_to_cart(wh, "Shoe", 3, id);

  CU_ASSERT_TRUE(checkout(wh, id));
  CU_ASSERT_TRUE(stock_is(wh, "Shoe", 2));
  CU_ASSERT_EQUAL(calculate_cost(wh, id), -1);   /* cart removed */
  CU_ASSERT_FALSE(checkout(wh, id));             /* can't check out twice */
  destroy_warehouse(wh);
}

/* Checkout emptying a shelf exactly: the shelf is removed (a location always
   holds at least one item) and becomes free for other merch. */
void test_checkout_exact_shelf(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  add_item(wh, "Hat", "Blue hat", 50);
  replenish_stock(wh, "Shoe", "A01", 5);
  size_t id = new_cart(wh);
  add_to_cart(wh, "Shoe", 5, id);

  CU_ASSERT_TRUE(checkout(wh, id));
  CU_ASSERT_TRUE(stock_is(wh, "Shoe", 0));
  CU_ASSERT_TRUE(replenish_stock(wh, "Hat", "A01", 1));
  destroy_warehouse(wh);
}

/* Checkout that needs several shelves: the first shelf is emptied and
   removed, the second is reduced. */
void test_checkout_multiple_shelves(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  add_item(wh, "Hat", "Blue hat", 50);
  replenish_stock(wh, "Shoe", "A01", 2);
  replenish_stock(wh, "Shoe", "B02", 3);
  size_t id = new_cart(wh);
  add_to_cart(wh, "Shoe", 4, id);

  CU_ASSERT_TRUE(checkout(wh, id));
  CU_ASSERT_TRUE(stock_is(wh, "Shoe", 1));
  CU_ASSERT_TRUE(replenish_stock(wh, "Hat", "A01", 1));   /* A01 freed */
  CU_ASSERT_FALSE(replenish_stock(wh, "Hat", "B02", 1));  /* B02 kept */
  destroy_warehouse(wh);
}

/* Checkout of a cart with several kinds of merch, and checkout of an
   empty cart. */
void test_checkout_several_merch(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  add_item(wh, "Hat", "Blue hat", 50);
  replenish_stock(wh, "Shoe", "A01", 5);
  replenish_stock(wh, "Hat", "B02", 5);
  size_t id = new_cart(wh);
  size_t empty = new_cart(wh);
  add_to_cart(wh, "Shoe", 1, id);
  add_to_cart(wh, "Hat", 4, id);

  CU_ASSERT_TRUE(checkout(wh, id));
  CU_ASSERT_TRUE(stock_is(wh, "Shoe", 4));
  CU_ASSERT_TRUE(stock_is(wh, "Hat", 1));
  CU_ASSERT_TRUE(checkout(wh, empty));
  destroy_warehouse(wh);
}

/* Checkout of a cart that does not exist fails. */
void test_checkout_nonexisting(void)
{
  warehouse_t *wh = create_warehouse();
  CU_ASSERT_FALSE(checkout(wh, 1));
  destroy_warehouse(wh);
}

/* ---------- consistency between merch and carts ---------- */

/* Removing a merch that is in a cart must not leave the cart pointing at it.
   (Your chosen rule: the merch is removed from all carts.) */
void test_remove_merch_in_cart(void)
{
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  replenish_stock(wh, "Shoe", "A01", 5);
  size_t id = new_cart(wh);
  add_to_cart(wh, "Shoe", 2, id);

  CU_ASSERT_TRUE(remove_item(wh, "Shoe"));
  CU_ASSERT_EQUAL(calculate_cost(wh, id), 0);
  destroy_warehouse(wh);
}

void test_edit_merch_in_cart(void){
  warehouse_t *wh = create_warehouse();
  add_item(wh, "Shoe", "Red shoe", 100);
  add_item(wh, "Crab", "sushi", 50);
  
  replenish_stock(wh, "Shoe", "A01", 15);
  replenish_stock(wh, "Crab", "A02", 10);
  new_cart(wh); //1
  new_cart(wh); //2
  new_cart(wh); //3

  add_to_cart(wh, "Shoe", 3, 1); //300
  add_to_cart(wh, "Crab", 1, 1); //350
  add_to_cart(wh, "Shoe", 10, 2); // 1000
  add_to_cart(wh, "Crab", 2, 3);
  edit_merch(wh, "Shoe", "BBall", "lebron", 40);
  CU_ASSERT_EQUAL(calculate_cost(wh, 1), 170);
  CU_ASSERT_EQUAL(calculate_cost(wh, 2), 400);
  CU_ASSERT_EQUAL(calculate_cost(wh, 3), 100);

  destroy_warehouse(wh);


}


/* ---------- main ---------- */

int main(void)
{
  if (CU_initialize_registry() != CUE_SUCCESS)
    return CU_get_error();

  CU_pSuite suite = CU_add_suite("Warehouse business logic", init_suite, clean_suite);
  if (suite == NULL)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (
    (CU_add_test(suite, "create and destroy", test_create_destroy) == NULL) ||
    (CU_add_test(suite, "destroy full warehouse", test_destroy_full_warehouse) == NULL) ||
    (CU_add_test(suite, "add item", test_add_item) == NULL) ||
    (CU_add_test(suite, "add duplicate", test_add_duplicate) == NULL) ||
    (CU_add_test(suite, "new item not in stock", test_new_item_not_in_stock) == NULL) ||
    (CU_add_test(suite, "remove item", test_remove_item) == NULL) ||
    (CU_add_test(suite, "remove item with stock", test_remove_item_with_stock) == NULL) ||
    (CU_add_test(suite, "edit same name", test_edit_same_name) == NULL) ||
    (CU_add_test(suite, "edit rename", test_edit_rename) == NULL) ||
    (CU_add_test(suite, "edit to existing name", test_edit_to_existing_name) == NULL) ||
    (CU_add_test(suite, "edit nonexisting", test_edit_nonexisting) == NULL) ||
    (CU_add_test(suite, "replenish invalid", test_replenish_invalid) == NULL) ||
    (CU_add_test(suite, "replenish adds up", test_replenish_adds_up) == NULL) ||
    (CU_add_test(suite, "replenish shelf taken", test_replenish_shelf_taken) == NULL) ||
    (CU_add_test(suite, "cart ids", test_cart_ids) == NULL) ||
    (CU_add_test(suite, "remove cart", test_remove_cart) == NULL) ||
    (CU_add_test(suite, "remove cart releases items", test_remove_cart_releases_items) == NULL) ||
    (CU_add_test(suite, "add to cart invalid", test_add_to_cart_invalid) == NULL) ||
    (CU_add_test(suite, "add to cart no stock", test_add_to_cart_no_stock) == NULL) ||
    (CU_add_test(suite, "add to cart all carts", test_add_to_cart_all_carts) == NULL) ||
    (CU_add_test(suite, "add to cart accumulates", test_add_to_cart_accumulates) == NULL) ||
    (CU_add_test(suite, "calculate cost", test_calculate_cost) == NULL) ||
    (CU_add_test(suite, "remove from cart", test_remove_from_cart) == NULL) ||
    (CU_add_test(suite, "remove from cart invalid", test_remove_from_cart_invalid) == NULL) ||
    (CU_add_test(suite, "remove from cart releases", test_remove_from_cart_releases) == NULL) ||
    (CU_add_test(suite, "checkout partial shelf", test_checkout_partial_shelf) == NULL) ||
    (CU_add_test(suite, "checkout exact shelf", test_checkout_exact_shelf) == NULL) ||
    (CU_add_test(suite, "checkout multiple shelves", test_checkout_multiple_shelves) == NULL) ||
    (CU_add_test(suite, "checkout several merch", test_checkout_several_merch) == NULL) ||
    (CU_add_test(suite, "checkout nonexisting", test_checkout_nonexisting) == NULL) ||
    (CU_add_test(suite, "remove merch in cart", test_remove_merch_in_cart) == NULL) ||
    (CU_add_test(suite, "edit merch in cart", test_edit_merch_in_cart) == NULL) ||
    0
  )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
