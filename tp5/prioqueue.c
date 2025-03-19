#include <stdlib.h>
#include <stdio.h>
#include "prioqueue.h"
#include "event.h"


typedef struct _link {
    event*          e;
    struct _link* next;
} link;

struct _prioqueue {
    link*      first;
    int size;
};

prioqueue *create_pq(){
    prioqueue *q = (prioqueue*)malloc(sizeof(prioqueue));
    q->first = NULL;
    q->size = 0;
    return q;
}

void free_pq(prioqueue *q) {
    link* ptr = q->first;
    while(ptr != NULL) {
        link* temp = ptr;
        ptr = ptr->next;
        free_event(temp->e);  // Libérer l'événement
        free(temp);           // Libérer le maillon
    }
    free(q);  // Libérer la file de priorité
}

int size_pq(prioqueue *q){
    return q->size;
}

void insert_pq(prioqueue *q, event *e){
    link* new_link =(link*)malloc(sizeof(link));
    if(new_link == NULL){
        exit(1);
    }

    new_link->e = e;
    new_link->next = NULL;

    if(q->first == NULL || e->etime < q->first->e->etime){
        new_link->next = q->first;
        q->first = new_link;
    }
    else{
        link* ptr = q->first;
        while (ptr->next != NULL && ptr->next->e->etime <= e->etime){
            ptr = ptr->next;
        }
        
        new_link->next = ptr->next;
        ptr->next = new_link;
        
        
    }
    q->size++;

}

void display_pq(prioqueue *pq){
    if(pq->size == 0){
        printf("La file de priorité est vide\n");
    }
    printf("File de priorité : ");
    link * ptr = pq->first;
    while (ptr != NULL){
        printf("%d", ptr->e->etime);
        ptr = ptr->next;
    }
    
}

event *remove_min_pq(prioqueue *q) {
    if (q->first == NULL) {
        return NULL; 
    }
    
    link *temp = q->first;
    event *e = temp->e;
    q->first = q->first->next;
    free(temp);
    q->size--;
    return e;
}