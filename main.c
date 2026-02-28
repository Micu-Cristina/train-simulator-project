#include "functions.h"

int main(int argc, char *argv[]) {
    TTrain *train = Init();  // create initial train with locomotive and one wagon
    if (!train) {
        return EXIT_FAILURE;
    }

    char *number = (char *)calloc(20, sizeof(char));  // buffer for number of commands
    if (!number) {
        free(train->curr);
        free(train->locomotive);
        free(train);
        return EXIT_FAILURE;
    }

    FILE *file = NULL;
    if (argc > 1) {
        file = fopen(argv[1], "r");  // input file with commands
        if (!file) {
            fprintf(stderr, "Could not open input file\n");
            free(number);
            free(train->curr);
            free(train->locomotive);
            free(train);
            return EXIT_FAILURE;
        }
    } else {
        file = stdin;  // fall back to standard input
    }

    FILE *output_file = stdout;  // write results to standard output
    if (!output_file) {
        fprintf(stderr, "Could not open output stream\n");
        if (file && file != stdin)
            fclose(file);
        free(number);
        free(train->curr);
        free(train->locomotive);
        free(train);
        return EXIT_FAILURE;
    }

    if (!fgets(number, 20, file)) {
        if (file && file != stdin)
            fclose(file);
        free(number);
        free(train->curr);
        free(train->locomotive);
        free(train);
        return EXIT_FAILURE;
    }
    int number_length = strlen(number);  // trim newline from the number line
    if (number_length > 0)
        number[number_length - 1] = '\0';
    int q = atoi(number);
    fprintf(stderr, "%d\n", q);  

    TQueue *queue = InitQueue();  // queue to store deferred instructions
    if (!queue) {
        if (file && file != stdin)
            fclose(file);
        free(number);
        free(train->curr);
        free(train->locomotive);
        free(train);
        return EXIT_FAILURE;
    }

    char *s = (char *)calloc(30, sizeof(char));  // buffer for each command line
    if (!s) {
        DestroyQueue(&queue);
        if (file && file != stdin)
            fclose(file);
        free(number);
        free(train->curr);
        free(train->locomotive);
        free(train);
        return EXIT_FAILURE;
    }

    char *x = NULL;               // buffer for dequeued instruction
    char *instruction = NULL;     // pointer to current instruction string
    char character;
    while (fgets(s, 30, file)) {
        fprintf(stderr, "%s", s);  
        int l = strlen(s);
        if (l > 0)
            s[l] = '\0';

        if (strstr(s, "SHOW_CURRENT")) {
            SHOW_CURRENT(train, output_file);
        } else if (strstr(s, "SHOW") && (strstr(s, "SHOW_CURRENT") == 0)) {
            SHOW(train, output_file);
        } else if (strstr(s, "SWITCH")) {
            SWITCH(queue);
        } else if (strstr(s, "EXECUTE")) {
            if (queue->head == NULL) {
                /* empty queue, nothing to execute */
            } else {
                instruction = queue->head->info;  // peek at the first queued instruction
                if (strstr(instruction, "MOVE_RIGHT")) {
                    MOVE_RIGHT(train);
                }
                if (strstr(instruction, "MOVE_LEFT")) {
                    MOVE_LEFT(train);
                }
                if (strstr(instruction, "WRITE")) {
                    char *word = strtok(instruction, " \n");
                    word = strtok(NULL, " \n");
                    if (word && word[0] != '\0')
                        WRITE(train, word[0]);
                }
                if (strstr(instruction, "INSERT_RIGHT")) {
                    character = instruction[strlen(instruction) - 2];
                    Insert_Right(train, character);
                }
                if (strstr(instruction, "INSERT_LEFT")) {
                    character = instruction[strlen(instruction) - 2];
                    Insert_Left(train, character, output_file);
                }
                if (strstr(instruction, "SEARCH_RIGHT")) {
                    char *word = strtok(instruction, " \n");
                    word = strtok(NULL, " \n");
                    if (word)
                        SEARCH_RIGHT(train, word, output_file);
                }
                if (strstr(instruction, "SEARCH_LEFT")) {
                    char *word = strtok(instruction, " \n");
                    word = strtok(NULL, " \n");
                    if (word)
                        SEARCH_LEFT(train, word, output_file);
                }
                if (strstr(instruction, "SEARCH") && (strstr(instruction, "SEARCH_RIGHT") == 0) && (strstr(instruction, "SEARCH_LEFT") == 0)) {
                    char *word = strtok(instruction, " \n");
                    word = strtok(NULL, " \n");
                    if (word)
                        SEARCH(train, word, output_file);
                }
                if (strstr(instruction, "CLEAR_CELL")) {
                    CLEAR_CELL(train);
                }
                if (strstr(instruction, "CLEAR_ALL")) {
                    CLEAR_ALL(train);
                }
                Dequeue(queue, &x);  // remove executed instruction
                free(x);
                x = NULL;
            }
        } else {
            Enqueue(queue, s);  // store non-executed instructions
        }
    }
    char *extracted;
    free(s);
    s = NULL;
    while (queue->tail != NULL) {
        Dequeue(queue, &extracted);
        free(extracted);
    }
    free(queue);  // queue struct itself
    queue = NULL;
    CLEAR_ALL(train);  // reset train to initial state
    free(train->curr);
    free(train->locomotive);
    free(train);
    train = NULL;
    if (file && file != stdin)
        fclose(file);        // close input file if opened

    return 0;
}
