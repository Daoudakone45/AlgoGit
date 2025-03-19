#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "event.h"
#include "customer.h"
#include "queue.h"
#include "prioqueue.h"

#define N_VENDORS 3

#define CLOSING_TIME 360   // L'heure de fermeture de la boulangerie
#define ARRIVAL_RATE (1.0/60)
#define MEAN_SERVICE_TIME 150

/* La file de priorité d'événements */
prioqueue*  event_queue;

/* La file d'attente des clients */
queue*      customer_queue;

/**
 * Les vendeurs 
 * vendor[i] point sur le client actuellement servi par le vendeur i
 * vendor[i] == NULL si le vendeur i est libre
 */
customer*   vendor[N_VENDORS];

//L'heure courante de la stimulation
int current_time = 0;

// Pour les calculs de statistiques
int customers_served= 0;
int total_time_in_system =0;


double normal_delay(double mean) {
    return -mean*log(1-((double)rand()/RAND_MAX));
    }

void add_customer(customer *c) {
    for (int i = 0; i < N_VENDORS; i++) {
        if (vendor[i] == NULL) { // Vendeur libre trouvé
            vendor[i] = c;
            //printf("Client affecté au vendeur %d\n", i);
            event *departure_event = create_departure(current_time + normal_delay(MEAN_SERVICE_TIME), c);
            insert_pq(event_queue, departure_event);
            return;
        }
    }
    enqueue_q(customer_queue, c);
    //printf("Client ajouté à la file d'attente\n");
}

 
void display_system_state(){
    printf("%d | ", current_time);
    
    // Affichage des vendeurs
    for (int i = 0; i < N_VENDORS; i++) {
        if (vendor[i] == NULL) {
            printf("_");
        } else {
            printf("X");
        }
    }
    
    printf(" | ");
    // Affichage de la file d'attente
    for (int i = 0; i < size_q(customer_queue); i++) {
        printf("X");
    }
    printf("\n");
}

// Recherche de client associé au vendeur et association du vendeur a un autre client 
void remove_customer(customer *c) {
    int vid = -1;
    
    // Trouver le vendeur qui s'occupe du client c
    for (int i = 0; i < N_VENDORS; i++) {
        if (vendor[i] == c) {
            vid = i;
            break;
        }
    }
    
    if (vid == -1) return; // Le client n'a pas été trouvé chez un vendeur
    
    total_time_in_system += current_time - c->atime;
    customers_served++;

    // Libérer le client actuel
    free_customer(c);
    vendor[vid] = NULL;
    
    // Vérifier s'il y a un client en attente
    if (size_q(customer_queue) > 0) {
        customer *c2 = dequeue_q(customer_queue);
        vendor[vid] = c2;
        
        // Créer un événement de départ pour le nouveau client
        event *departure_event = create_departure(current_time + normal_delay(MEAN_SERVICE_TIME), c2);
        insert_pq(event_queue, departure_event);
    }
}

void process_arrival(event *e) {
    
    add_customer(e->c);
    customer *new_customer = create_customer(current_time + normal_delay(1.0/ARRIVAL_RATE));
    event *new_event = create_arrival(current_time + normal_delay(1.0/ARRIVAL_RATE), new_customer);
    insert_pq(event_queue, new_event);
    

}

void process_departure(event *e) {
    remove_customer(e->c);

}

void event_loop(){

    while( size_pq(event_queue) > 0 && current_time + 60 <= CLOSING_TIME ){

        
        event* e = remove_min_pq(event_queue);
        current_time = e->etime;

        //printf("L'heure de l'événement avant son traitement %d \n", e->etime);
        //printf("Le type d'événement %c", e->type); // J'ai fait pour me permettre de voir le type avant son traiteùent(Arrivée ou depart)

        if(e->type == EVENT_ARRIVAL)
            process_arrival(e);

        else if(e->type == EVENT_DEPARTURE)
            process_departure(e);
        
        free_event(e);
        display_system_state();

    }
}
    
// Liberation des clients restant après la fermeture du restaurant
void free_unprocessed_events(prioqueue *pq) {
    while (size_pq(pq) > 0) {
        event *e = remove_min_pq(pq);

        // Si c'est un événement d'arrivée, libérer aussi le client
        if (e->type == EVENT_ARRIVAL) {
            free_customer(e->c);
        }

        free_event(e);
    }
}

// Calcul des statistique 
void statistique(){
    if(customers_served > 0){
        double average_response_time = (double) total_time_in_system / customers_served;
        printf("Nombre de clients servis : %d\n", customers_served);
        printf("Temps moyen de réponse : %.2f\n", average_response_time);
    } else {
        printf("Aucun client n'a été servi.\n");
    }
}

//Liberation de la mémoire
void free_vendors(){
    for (int i = 0; i < N_VENDORS; i++) {
        if (vendor[i] != NULL) {
            free_customer(vendor[i]);
            vendor[i] = NULL;
        }
    }
}


int main() {

    // intialisation
    event_queue = create_pq();
    customer_queue = create_q();
    for (int i = 0; i < N_VENDORS; i++) {
        vendor[i] = NULL;
    }

    // Création d'un client avec une heure d'arrivée de 42 secondes
    customer *c = create_customer(60);
    event *arrival_event = create_arrival(60, c);
    insert_pq(event_queue, arrival_event); // Ajouter l'événement à la file de priorité

    // Lancer la boucle d'événements
    event_loop();
    display_system_state();

    // Affichage des statistiques
    statistique();

    // Libération de la mémoire
    free_unprocessed_events(event_queue);
    //Liberation de le memoire
    free_vendors();
    free_q(customer_queue);
    free_pq(event_queue);
    

    return 0;
}
