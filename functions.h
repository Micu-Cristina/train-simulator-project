#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct list_node {
    char info;
    struct list_node *next, *prev;
} TCell, *TList;

typedef struct train {
    TList locomotive;
    TList curr;
} TTrain;

typedef struct list_node_2 {
    char *info;
    struct list_node_2 *next, *prev;
} TCell2, *TList2;

typedef struct queue {
    TList2 head, tail;
} TQueue;

TQueue *InitQueue();
int Enqueue(TQueue *queue, char *instruction);
int Dequeue(TQueue *queue, char **x);
void DestroyQueue(TQueue **queue);
void DisplayQueue(TQueue *queue, FILE *output_file);

TTrain *Init();
void MOVE_LEFT(TTrain *train);
void MOVE_RIGHT(TTrain *train);
void WRITE(TTrain *train, char character);
void CLEAR_CELL(TTrain *train);
void CLEAR_ALL(TTrain *train);
void Insert_Left(TTrain *train, char character, FILE *output_file);
void Insert_Right(TTrain *train, char character);
void SHOW_CURRENT(TTrain *train, FILE *output_file);
void SHOW(TTrain *train, FILE *output_file);
void SWITCH(TQueue *queue);
void SEARCH(TTrain *train, char *string, FILE *output_file);
void SEARCH_RIGHT(TTrain *train, char *string, FILE *output_file);
void SEARCH_LEFT(TTrain *train, char *string, FILE *output_file);
