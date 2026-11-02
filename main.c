#include "ui.h"
#include "business.h"

int main(void){
    warehouse_t *wh = create_warehouse();
    event_loop(wh);
    destroy_warehouse(wh);
}