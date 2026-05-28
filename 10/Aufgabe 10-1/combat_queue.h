#ifndef COMBAT_QUEUE_H
#define COMBAT_QUEUE_H

#include <pthread.h>
#include <semaphore.h>

/* =========================================================
   Constants
   ========================================================= */

#define MAX_ACTIONS 1000
#define MAX_LINE 256

#define INITIAL_MONSTER_HP 300
#define INITIAL_FORTRESS_HP 100

#define INITIAL_MANA 100
#define INITIAL_POTIONS 10
#define INITIAL_TRAPS 10
#define INITIAL_ARROWS 50

#define FIREBALL_MANA_COST 10
#define HEAL_MANA_COST 5
#define HEAL_POTION_COST 1
#define TRAP_ITEM_COST 1
#define ARROW_ITEM_COST 1

#define MAX_QUEUE_SIZE 1000
#define MAX_HEROES 1000
#define MAX_WORKERS 1000

/* =========================================================
   Event Types and Structures
   ========================================================= */

typedef enum {
    EVENT_ARROW,
    EVENT_FIREBALL,
    EVENT_TRAP,
    EVENT_HEAL,
    EVENT_MONSTER_ATTACK,
    EVENT_SHIELD_BASH
} EventType;

typedef struct {
    int source_id;
    EventType type;
    int power;
    int delay_ms;
} CombatEvent;

/* =========================================================
   Function Declarations
   ========================================================= */

void check_pthread_error(int err, const char *operation);
void check_system_error(const char *operation);

int parse_positive_int(const char *text, const char *name, int max_value);
int parse_event_type(const char *text, EventType *type);
const char *event_name(EventType type);

void load_actions(const char *filename);

void print_event_queued(CombatEvent event, int slot);
void print_event_dequeued(CombatEvent event, int slot);
void print_event_processed(
    CombatEvent event,
    long worker_id,
    int current_monster_hp,
    int current_fortress_hp
);

void print_final_result(
    int total_processed,
    int final_monster_hp,
    int final_fortress_hp,
    int final_mana,
    int final_potions,
    int final_traps,
    int final_arrows
);

int is_stop_event(CombatEvent event);

void enqueue_event(CombatEvent event);
CombatEvent dequeue_event(void);

void process_event(CombatEvent event, long worker_id);

void *hero_thread(void *arg);
void *monster_thread(void *arg);
void *worker_thread(void *arg);

#endif