#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

typedef struct _link {
    customer*       c;
    struct _link*   next;
} link;

struct _queue {
    link*   first;
    link*   last;
    int     size;
};

queue *create_q() {
    queue *q = (queue*)malloc(sizeof(queue));
    q->first = NULL;
    q->last = NULL;
    q->size = 0;
    return q;
}

void free_q(queue *q) {
    link *current = q->first;
    while (current != NULL) {
        link *temp = current;
        current = current->next;
        free_customer(temp->c); // Libérer le client avant la cellule
        free(temp);
    }
    free(q);
}

int size_q(queue *q) {
    return q->size;
}

void enqueue_q(queue *q, customer *c){
    link* new_link =(link*)malloc(sizeof(link));
    if(new_link == NULL){
        exit(1);
    }
    new_link->c = c;
    new_link->next = NULL;

    if(q->last != NULL){
        q->last->next = new_link;
    }else{
        q->first = new_link;
    }
    q->last = new_link;
    q->size++ ;
}

// Affichage de la file d'attente dans le terminal
void display_q(queue *q){
    /*
    for(int i = 0; i< size; i++){
        printf("%d \n", q[i]);
    }
    */
    if(q->size == 0){
        printf("La file est vide\n");
        return;
    }

    link *ptr = q->first;
    printf("Les clients dans la file: ");
    while(ptr != NULL){
        printf("%d", ptr->c->atime);
        ptr = ptr->next;
    }printf("\n");
}


//customer *dequeue_q(queue *q){}
customer *dequeue_q(queue *q){
    if(q->first == NULL){
        return NULL;
    }

    link* temp = q->first;
    customer *c = temp->c;
    q->first = q->first->next;

    if(q->first == NULL){
        q->last = NULL;
    }

    free(temp);
    q->size--;
    return c;
}