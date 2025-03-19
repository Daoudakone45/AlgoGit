/*customer.c*/
#include "customer.h"
#include <stdlib.h>
//#include <queue.h>


customer *create_customer(int atime) {
    customer *c = (customer*)malloc(sizeof(customer));
    c->atime = atime;
    return c;
}

void free_customer(customer *c) {
    free(c);
}
