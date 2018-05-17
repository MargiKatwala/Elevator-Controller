/*
 Margi Katwala
 UIN :676885503
 Net id : mkatwa3
 NOTE: This project is not complete project so the new changes are commented out
 Resources: Lab 10, 11 and main.c ( for this current project)
 
 */
#include "hw6.h"
#include <stdio.h>
#include<pthread.h>

pthread_mutex_t lock;

int current_floor;
int direction;
int occupancy;
enum {ELEVATOR_ARRIVED=1, ELEVATOR_OPEN=2, ELEVATOR_CLOSED=3} state;
// Instead of having global variables we would construct two struct one for Elevtor and another one for Passanger
/*static struct Elevator {
 int current_floor;
 int direction;
 int occupancy;
 int passengerID;  // passenger id on board
 pthread_mutex_t lock;
 enum {ELEVATOR_ARRIVED=1, ELEVATOR_OPEN=2, ELEVATOR_CLOSED=3} state;
 } elevators[ELEVATORS];
 //new
 //create a struct with passenger to identify each
 static struct Passenger {
 int id;
 int in_elevator;
 int to_floor;
 int from_floor;
 enum {WAITING, ENTERED, EXITED} state;
 } passengers[PASSENGERS];
 */

void scheduler_init() {
    current_floor=0;
    direction=-1;
    occupancy=0;
    state=ELEVATOR_ARRIVED;
    pthread_mutex_init(&lock,0);
}
// The function scheduler init will change according to this
//In this function each elevator is put it into an array

/*
 
 void scheduler_init()
 {
 current_floor=0;
 direction=-1;
 occupancy=0;
 state=ELEVATOR_ARRIVED;
 pthread_mutex_init(&lock,0);
 int i;
 do
 {
 elevators[i].current_floor=0;
 elevators[i].direction=-1;
 elevators[i].occupancy=0;
 elevators[i].state=ELEVATOR_ARRIVED;
 pthread_mutex_init(&elevators[i].lock,0);
 i++;
 }while(i<ELEVATORS);
 int j;
 //assign each passanger to
 do{
 passengers[j].in_elevator = j % ELEVATORS;
 j++;
 } while(j<PASSENGERS);
 } // main loop
 
 */


 //  Now each pasanger needs to run accordinly to the elevator list
// In this case we will use the code proivded in the main.c and run it with the multiple evelator
//void* start_passenger(void *arg) {
//    int passenger=(int)arg;
//    struct Passenger *p = &passengers[passenger];
//    log(6,"Starting passenger %d\n", passenger);
//    p->from_floor = random()%FLOORS;
//    p->in_elevator = -1;
//    p->id = passenger;
//    int trips = TRIPS_PER_PASSENGER;
//    while(!stop && trips-- > 0) {
//        p->to_floor = random() % FLOORS;
//        log(6,"Passenger %d requesting %d->%d\n",
//            passenger,p->from_floor,p->to_floor);
//
//        struct timeval before;
//        gettimeofday(&before,0);
//        passengers[passenger].state=WAITING;
//        passenger_request(passenger, p->from_floor, p->to_floor, passenger_enter, passenger_exit);
//        struct timeval after;
//        gettimeofday(&after,0);
//        int ms = (after.tv_sec - before.tv_sec)*1000 + (after.tv_usec - before.tv_usec)/1000;
//        log(1,"Passenger %d trip duration %d ms, %d slots\n",passenger,ms,ms*1000/DELAY);
//
//        p->from_floor = p->to_floor;
//        usleep(100);
//            //   usleep(DELAY); // sleep for some time
//    }
//}
void passenger_request(int passenger, int from_floor, int to_floor,
                       void (*enter)(int, int),
                       void(*exit)(int, int))
{
    // wait for the elevator to arrive at our origin floor, then get in
    int waiting = 1;
    while(waiting) {
        pthread_mutex_lock(&lock);
        
        if(current_floor == from_floor && state == ELEVATOR_OPEN && occupancy==0) {
            enter(passenger, 0);
            occupancy++;
            waiting=0;
        }
        
        pthread_mutex_unlock(&lock);
    }
    
    // wait for the elevator at our destination floor, then get out
    int riding=1;
    while(riding) {
        pthread_mutex_lock(&lock);
        
        if(current_floor == to_floor && state == ELEVATOR_OPEN) {
            exit(passenger, 0);
            occupancy--;
            riding=0;
        }
        
        pthread_mutex_unlock(&lock);
    }
}
//void elevator_ready(int elevator, int at_floor,
//                    void(*move_direction)(int, int),
//                    void(*door_open)(int), void(*door_close)(int)) {
//    if(elevator!=0) return;
//
//    pthread_mutex_lock(&lock);
//   if(elevators[elevator].state == ELEVATOR_ARRIVED)
//       {
//      door_open(elevator);
//      elevators[elevator].state=ELEVATOR_OPEN;
//      }
//  else if(elevators[elevator].state == ELEVATOR_OPEN) {
//    door_close(elevator);
//    elevators[elevator].state=ELEVATOR_CLOSED;
//}
//  else {
//    if(at_floor==0 || at_floor==FLOORS-1)
//        elevators[elevator].direction*=-1;
//        move_direction(elevator,elevators[elevator].direction);
//        elevators[elevator].current_floor=at_floor+elevators[elevator].direction;
//        elevators[elevator].state=ELEVATOR_ARRIVED;
//        }
//    pthread_mutex_unlock(&elevators[elevator].lock);
//}

void elevator_ready(int elevator, int at_floor,
                    void(*move_direction)(int, int),
                    void(*door_open)(int), void(*door_close)(int)) {
    if(elevator!=0) return;
    
    pthread_mutex_lock(&lock);
    if(state == ELEVATOR_ARRIVED) {
        door_open(elevator);
        state=ELEVATOR_OPEN;
    }
    else if(state == ELEVATOR_OPEN) {
        door_close(elevator);
        state=ELEVATOR_CLOSED;
    }
    else {
        if(at_floor==0 || at_floor==FLOORS-1)
            direction*=-1;
        move_direction(elevator,direction);
        current_floor=at_floor+direction;
        state=ELEVATOR_ARRIVED;
    }
    pthread_mutex_unlock(&lock);
}
