// Do magic ╰( ͡° ͜ʖ ͡° )つ──☆*:・ﾟ and implement your "Hello world" programm here.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


struct ancestor_node {
    dev_t dev;
    ino_t ino;
    struct ancestor_node *next;
};

void print_indent(int level) {
    for (int i = 0; i < level; i++) {
        printf("|   ");
    }
}

int is_cycle(struct ancestor_node *head, dev_t dev, ino_t ino) {
    struct ancestor_node *current = head;
    while (current != NULL) {
        if (current->dev == dev && current->ino == ino) {
            return 1; 
        }
        current = current->next;
    }
    return 0;
}

void traverse_directory(const char *path, int level, struct ancestor_node *ancestors) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        return;
    }

    struct dirent *entry;
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char full_path[4096];
        if ((long unsigned)snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name) >= sizeof(full_path)) {
            continue; 
        }

        struct stat st;
        if (stat(full_path, &st) == -1) {
            continue; 
        }

        if (S_ISDIR(st.st_mode)) {
            print_indent(level);
            
            if (is_cycle(ancestors, st.st_dev, st.st_ino)) {
                printf("|- [%s] (Cycle!)\n", entry->d_name);
            } else {
                printf("|- [%s]\n", entry->d_name);
                
                struct ancestor_node new_node;
                new_node.dev = st.st_dev;
                new_node.ino = st.st_ino;
                new_node.next = ancestors; 
                
                traverse_directory(full_path, level + 1, &new_node);
            }
        } else {
            print_indent(level);
            printf("|- %s\n", entry->d_name);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {

    const char *start_dir = ".";
    if (argc > 1) {
        start_dir = argv[1];
    }

    struct stat st;
    if (stat(start_dir, &st) == -1) {
        perror("stat error");
        return EXIT_FAILURE;
    }

    if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "Provided path is not a directory.\n");
        return EXIT_FAILURE;
    }

    printf("%s\n", start_dir);

    struct ancestor_node root_node;
    root_node.dev = st.st_dev;
    root_node.ino = st.st_ino;
    root_node.next = NULL;

    traverse_directory(start_dir, 0, &root_node);

    return EXIT_SUCCESS;
}