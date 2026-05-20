#define _DEFAULT_SOURCE
#include "combat_queue.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* =========================================================
   Global Variables
   ========================================================= */

CombatEvent *queue = NULL;
CombatEvent actions[MAX_ACTIONS];

int queue_size = 0;
int hero_count = 0;
int worker_count = 0;
int action_count = 0;

int in = 0;
int out = 0;

int processed_events = 0;

int monster_hp = INITIAL_MONSTER_HP;
int fortress_hp = INITIAL_FORTRESS_HP;

sem_t empty_slots;
sem_t full_slots;

pthread_mutex_t queue_mutex;
pthread_mutex_t state_mutex;

/* =========================================================
   Error Handling
   ========================================================= */

void check_pthread_error(int err, const char *operation) {
    if (err != 0) {
        fprintf(stderr, "%s failed: %s\n", operation, strerror(err));
        exit(EXIT_FAILURE);
    }
}

void check_system_error(const char *operation) {
    perror(operation);
    exit(EXIT_FAILURE);
}

/* =========================================================
   Parsing Helpers
   ========================================================= */

int parse_positive_int(const char *text, const char *name, int max_value) {
    char *endptr = NULL;

    errno = 0;
    long value = strtol(text, &endptr, 10);

    if (errno != 0) {
        perror(name);
        exit(EXIT_FAILURE);
    }

    if (endptr == text || *endptr != '\0') {
        fprintf(stderr, "Invalid %s: '%s' is not an integer.\n", name, text);
        exit(EXIT_FAILURE);
    }

    if (value <= 0) {
        fprintf(stderr, "Invalid %s: value must be positive.\n", name);
        exit(EXIT_FAILURE);
    }

    if (value > max_value) {
        fprintf(stderr,
                "Invalid %s: value must not exceed %d.\n",
                name,
                max_value);
        exit(EXIT_FAILURE);
    }

    return (int)value;
}

const char *event_name(EventType type) {
    switch (type) {
        case EVENT_ARROW:
            return "ARROW";
        case EVENT_FIREBALL:
            return "FIREBALL";
        case EVENT_TRAP:
            return "TRAP";
        case EVENT_HEAL:
            return "HEAL";
        case EVENT_MONSTER_ATTACK:
            return "MONSTER_ATTACK";
        default:
            return "UNKNOWN";
    }
}

int parse_event_type(const char *text, EventType *type) {
    if (strcmp(text, "ARROW") == 0) {
        *type = EVENT_ARROW;
        return 1;
    }

    if (strcmp(text, "FIREBALL") == 0) {
        *type = EVENT_FIREBALL;
        return 1;
    }

    if (strcmp(text, "TRAP") == 0) {
        *type = EVENT_TRAP;
        return 1;
    }

    if (strcmp(text, "HEAL") == 0) {
        *type = EVENT_HEAL;
        return 1;
    }

    if (strcmp(text, "MONSTER_ATTACK") == 0) {
        *type = EVENT_MONSTER_ATTACK;
        return 1;
    }

    return 0;
}

/* =========================================================
   Input File Loading
   ========================================================= */

void load_actions(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL)
        check_system_error("fopen");

    char line[MAX_LINE];

    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == '\n' || line[0] == '#')
            continue;

        if (action_count >= MAX_ACTIONS) {
            fprintf(stderr, "Too many actions. Maximum is %d.\n", MAX_ACTIONS);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        int source_id;
        char action_name[32];
        int power;
        int delay_ms;

        int matched = sscanf(
            line,
            "%d %31s %d %d",
            &source_id,
            action_name,
            &power,
            &delay_ms
        );

        if (matched != 4) {
            fprintf(stderr, "Invalid input line: %s", line);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        EventType type;

        if (!parse_event_type(action_name, &type)) {
            fprintf(stderr, "Invalid action type: %s\n", action_name);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        if (type == EVENT_MONSTER_ATTACK) {
            if (source_id != 0) {
                fprintf(stderr, "MONSTER_ATTACK must use source_id 0.\n");
                fclose(file);
                exit(EXIT_FAILURE);
            }
        } else {
            if (source_id < 1 || source_id > hero_count) {
                fprintf(stderr,
                        "Invalid hero_id %d. Must be between 1 and %d.\n",
                        source_id,
                        hero_count);
                fclose(file);
                exit(EXIT_FAILURE);
            }
        }

        if (power <= 0) {
            fprintf(stderr, "Invalid power value: %d\n", power);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        if (delay_ms < 0) {
            fprintf(stderr, "Invalid delay_ms value: %d\n", delay_ms);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        actions[action_count].source_id = source_id;
        actions[action_count].type = type;
        actions[action_count].power = power;
        actions[action_count].delay_ms = delay_ms;

        action_count++;
    }

    fclose(file);

    if (action_count == 0) {
        fprintf(stderr, "Input file contains no actions.\n");
        exit(EXIT_FAILURE);
    }
}

/* =========================================================
   Printing Helpers
   ========================================================= */

void print_event_queued(CombatEvent event, int slot) {
    if (event.type == EVENT_MONSTER_ATTACK) {
        printf("Monster queued attack power=%d at slot %d\n",
               event.power,
               slot);
    } else {
        printf("Hero %d queued %s power=%d at slot %d\n",
               event.source_id,
               event_name(event.type),
               event.power,
               slot);
    }
}

void print_event_dequeued(CombatEvent event, int slot) {
    if (event.source_id == -1 && event.type == EVENT_HEAL) {
        printf("Worker received STOP event from slot %d\n", slot);
    } else if (event.type == EVENT_MONSTER_ATTACK) {
        printf("Worker removed MONSTER_ATTACK from slot %d\n", slot);
    } else {
        printf("Worker removed %s from Hero %d at slot %d\n",
               event_name(event.type),
               event.source_id,
               slot);
    }
}

void print_event_processed(
    CombatEvent event,
    long worker_id,
    int current_monster_hp,
    int current_fortress_hp
) {
    switch (event.type) {
        case EVENT_ARROW:
        case EVENT_FIREBALL:
        case EVENT_TRAP:
            printf("Worker %ld processed %s from Hero %d. Monster HP = %d\n",
                   worker_id,
                   event_name(event.type),
                   event.source_id,
                   current_monster_hp);
            break;

        case EVENT_HEAL:
            printf("Worker %ld processed HEAL from Hero %d. Fortress HP = %d\n",
                   worker_id,
                   event.source_id,
                   current_fortress_hp);
            break;

        case EVENT_MONSTER_ATTACK:
            printf("Worker %ld processed MONSTER_ATTACK. Fortress HP = %d\n",
                   worker_id,
                   current_fortress_hp);
            break;
    }
}

void print_final_result(
    int total_processed,
    int final_monster_hp,
    int final_fortress_hp
) {
    printf("\nFinal result:\n");
    printf("Processed events: %d\n", total_processed);
    printf("Monster HP: %d\n", final_monster_hp);
    printf("Fortress HP: %d\n", final_fortress_hp);
}

/* =========================================================
   Queue Operations
   ========================================================= */

void enqueue_event(CombatEvent event) {
    int err;

    if (sem_wait(&empty_slots) < 0) {
        check_system_error("sem_wait(empty_slots)");
    }

    err = pthread_mutex_lock(&queue_mutex);
    if (err != 0) {
        check_pthread_error(err, "pthread_mutex_lock(queue_mutex)");
    }
    
    queue[in] = event;
    print_event_queued(event, in);
    in = (in + 1) % queue_size;
    
    err = pthread_mutex_unlock(&queue_mutex);
    if (err != 0) {
        check_pthread_error(err, "pthread_mutex_unlock(queue_mutex)");
    }

    if (sem_post(&full_slots) < 0) {
        check_system_error("sem_post(full_slots)");
    }
}

CombatEvent dequeue_event(void) {
    CombatEvent event = {0};
    int err;

    if(sem_wait(&full_slots) < 0) {
        check_system_error("sem_wait(full_slots)");
    }

    err = pthread_mutex_lock(&queue_mutex);
    if (err != 0) {
        check_pthread_error(err, "pthread_mutex_lock(queue_mutex)");
    }

    event = queue[out];
    print_event_dequeued(event, out);
    out = (out + 1) % queue_size;
    
    err = pthread_mutex_unlock(&queue_mutex);
    if (err != 0) {
        check_pthread_error(err, "pthread_mutex_unlock(queue_mutex)");
    }

    if (sem_post(&empty_slots) < 0) {
        check_system_error("sem_post(empty_slots)");
    }

    return event;
}

/* =========================================================
   Event Processing
   ========================================================= */

void process_event(CombatEvent event, long worker_id) {
    int err;
    
    err = pthread_mutex_lock(&state_mutex);
    if (err != 0){
        check_pthread_error(err,"pthread_mutex_lock(state_mutex)");
    }

    if(event.source_id == 0){
        fortress_hp -= event.power;
    } else {
        if(event.type == EVENT_HEAL){
            fortress_hp += event.power;
        } else {
        monster_hp -= event.power;
        }
    }

    processed_events++;
    print_event_processed(event, worker_id, monster_hp, fortress_hp);

    err = pthread_mutex_unlock(&state_mutex);
    if (err != 0) {
        check_pthread_error(err, "pthread_mutex_unlock(state_mutex)");
    }
}

/* =========================================================
   Hero Thread
   ========================================================= */

void *hero_thread(void *arg) {
    long hero_id = (long)arg;

    for (int i = 0; i < action_count; i++){
        if(actions[i].source_id == hero_id){
            usleep(actions[i].delay_ms * 1000);
            enqueue_event(actions[i]);
        }
    }
    return NULL;
}

/* =========================================================
   Monster Thread
   ========================================================= */

void *monster_thread(void *arg) {
    (void)arg;
    
    for (int i = 0; i < action_count; i++){
        if(actions[i].type == EVENT_MONSTER_ATTACK){
            usleep(actions[i].delay_ms * 1000);
            enqueue_event(actions[i]);
        }
    }
    return NULL;
}

/* =========================================================
   Worker Thread
   ========================================================= */

void *worker_thread(void *arg) {
    long worker_id = (long)arg;
    
    while(1){
        CombatEvent event = dequeue_event();
        if(event.source_id == -1){
            break;
        } else {
            process_event(event, worker_id);
            usleep(150000);
        }
    }
    return NULL;
}

/* =========================================================
   Main
   ========================================================= */

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr,
                "Usage: %s actions.txt queue_size hero_threads worker_threads\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];

    queue_size = parse_positive_int(argv[2], "queue_size", MAX_QUEUE_SIZE);
    hero_count = parse_positive_int(argv[3], "hero_threads", MAX_HEROES);
    worker_count = parse_positive_int(argv[4], "worker_threads", MAX_WORKERS);

    queue = malloc((size_t)queue_size * sizeof(CombatEvent));

    if (queue == NULL)
        check_system_error("malloc(queue)");

    load_actions(filename);

    pthread_t *heroes = malloc((size_t)hero_count * sizeof(pthread_t));
    pthread_t *workers = malloc((size_t)worker_count * sizeof(pthread_t));
    pthread_t monster;

    if (heroes == NULL || workers == NULL){
        check_system_error("malloc(threads)");
    }


    //initialize synchronization objects
    if(sem_init(&empty_slots, 0, queue_size) != 0){
        check_system_error("sem_init(empty_slots)");
    }
    if(sem_init(&full_slots, 0, 0) != 0){
        check_system_error("sem_init(full_slots)");
    }

    int err;
    err = pthread_mutex_init(&queue_mutex, NULL);
    if (err != 0) {
        check_pthread_error(err, "pthread_mutex_init(queue_mutex)");
    }
    err = pthread_mutex_init(&state_mutex, NULL);
    if (err != 0) {
        check_pthread_error(err, "pthread_mutex_init(state_mutex)");
    }


    //create hero, monster, and worker threads
    for (int i = 0; i < hero_count; i++){
        err = pthread_create(&heroes[i], NULL, hero_thread, (void *)(long)(i + 1));
        if(err != 0){
            check_pthread_error(err, "pthread_create(hero)");
        }
    }

    err = pthread_create(&monster, NULL, monster_thread, NULL);
    if(err != 0){
        check_pthread_error(err, "pthread_create(monster)");
    }

    for (int i = 0; i < worker_count; i++){
        err = pthread_create(&workers[i], NULL, worker_thread, (void *)(long)(i + 1));
        if(err != 0){
            check_pthread_error(err, "pthread_create(worker)");
        }
    }


    //wait for thread completion
    for(int i = 0; i < hero_count; i++){
        err = pthread_join(heroes[i], NULL);
        if(err != 0){
            check_pthread_error(err, "pthread_join(hero)");
        }
    }

    err = pthread_join(monster, NULL);
    if(err != 0){
        check_pthread_error(err, "pthread_join(monster)");
    }

    //insert STOP events
    for(int i = 0; i < worker_count; i++){ 
    CombatEvent stop;
    stop.source_id = -1;
    stop.type = EVENT_HEAL;
    stop.power = 1;
    stop.delay_ms = 1;
    enqueue_event(stop);
    }

    for(int i = 0; i < worker_count; i++){
        err = pthread_join(workers[i], NULL);
        if(err != 0){
            check_pthread_error(err, "pthread_join(worker)");
        }
    }
    

    /*
       TODO
       TODO
       TODO
       TODO
       TODO
       TODO
    */



    print_final_result(processed_events, monster_hp, fortress_hp);

    free(queue);
    free(heroes);
    free(workers);


    //destroy synchronization objects
    if(sem_destroy(&empty_slots) != 0){
        check_system_error("sem_destroy(empty_slots)");
    }
    if(sem_destroy(&full_slots) != 0){
        check_system_error("sem_destroy(full_slots)");
    }

    err = pthread_mutex_destroy(&queue_mutex);
    if (err != 0) {
        check_pthread_error(err, "pthread_mutex_destroy(queue_mutex)");
    }
    err = pthread_mutex_destroy(&state_mutex);
    if (err != 0) {
        check_pthread_error(err, "pthread_mutex_destroy(state_mutex)");
    }
    
    return 0;
}