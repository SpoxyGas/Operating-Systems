#ifndef COMBAT_QUEUE_H
#define COMBAT_QUEUE_H

#include <pthread.h>
#include <semaphore.h>

#define MAX_ACTIONS 1000
#define MAX_LINE 256

#define INITIAL_MONSTER_HP 300
#define INITIAL_FORTRESS_HP 100

#define MAX_QUEUE_SIZE 1000
#define MAX_HEROES 1000
#define MAX_WORKERS 1000

typedef enum {
    EVENT_ARROW,
    EVENT_FIREBALL,
    EVENT_TRAP,
    EVENT_HEAL,
    EVENT_MONSTER_ATTACK
} EventType;

typedef struct {
    int source_id;
    EventType type;
    int power;
    int delay_ms;
} CombatEvent;

void check_pthread_error(int err, const char *operation);
void check_system_error(const char *operation);

int parse_positive_int(const char *text, const char *name, int max_value);
int parse_event_type(const char *text, EventType *type);

const char *event_name(EventType type);

void load_actions(const char *filename);

void enqueue_event(CombatEvent event);
CombatEvent dequeue_event(void);

void process_event(CombatEvent event, long worker_id);

void *hero_thread(void *arg);
void *monster_thread(void *arg);
void *worker_thread(void *arg);

void print_event_queued(CombatEvent event, int slot);
void print_event_dequeued(CombatEvent event, int slot);
void print_event_processed(
    CombatEvent event,
    long worker_id,
    int monster_hp,
    int fortress_hp
);
void print_final_result(int processed_events, int monster_hp, int fortress_hp);

#endif