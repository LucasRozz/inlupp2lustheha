/**
 * @file ui.h
 * @brief All user interface, almost every function is static since its not needed in any other modules
 */

#pragma once
#include "business.h"

/**
 * Runs the program until the user quits, the event loop does not destroy and create the warehouse
 */
void event_loop(warehouse_t *wh);
