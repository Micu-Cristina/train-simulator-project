#include "functions.h"

// Create and return an empty queue.
TQueue *InitQueue() {
    TQueue *queue;
    queue = (TQueue *)malloc(sizeof(TQueue));   // allocate memory for the queue
    if (!queue)     // check if allocation succeeded
        return NULL;
    queue->head = NULL;  // initialize head pointer
    queue->tail = NULL;  // initialize tail pointer
    return queue;        // return the queue
}

// Add an instruction to the end of the queue.
int Enqueue(TQueue *queue, char *instruction) {
    if (!queue || !instruction)
        return 0;

    TList2 temp = (TList2)malloc(sizeof(TCell2));
    if (!temp)
        return 0;

    temp->info = strdup(instruction);
    if (!temp->info) {
        free(temp);
        return 0;
    }
    temp->next = NULL;
    temp->prev = NULL;

    if (queue->tail == NULL && queue->head == NULL) {      // inserting into empty queue
        queue->tail = temp;    // head becomes new element
        queue->head = temp;    // tail becomes new element
    } else  {    // inserting into non-empty queue
        temp->prev = queue->tail;   // link new element to old tail
        queue->tail->next = temp;   // old tail points to new element
        queue->tail = temp;         // new element becomes tail
    }
    return 1;
}

// Remove the first element from the queue and return its string via x.
int Dequeue(TQueue *queue, char **x) {
    if (!queue || !x)
        return 0;
    if (queue->tail == NULL && queue->head == NULL)
        return 0;
    if (queue->head == queue->tail) {    // single element in the queue
        TList2 temp = queue->head;
        (*x) = temp->info;
        queue->head = NULL;
        queue->tail = NULL;
        free(temp);
        return 1;
    }

    (*x) = queue->head->info;
    TList2 temp = queue->head;
    queue->head = temp->next;
    if (temp->next)
        temp->next->prev = NULL;
    free(temp);   /* caller owns info string via *x */
    return 1;
}

// Destroy the queue and free all stored instruction strings.
void DestroyQueue(TQueue **queue) {
    if (!queue || !*queue)
        return;
    char *extracted;
    while ((*queue)->tail != NULL) {
        Dequeue(*queue, &extracted);
        free(extracted);
    }
    free(*queue);
    *queue = NULL;
}   

// Print the contents of the queue to the output file.
void DisplayQueue(TQueue *queue, FILE *output_file) {
    if (!queue || !output_file)
        return;
    fprintf(output_file, "Queue is: ");
    TList2 temp = queue->head;
    while (temp != NULL) {
        fprintf(output_file, "%s ", temp->info);
        temp = temp->next;
    }
}

// Initialize the train with a locomotive and one wagon.
TTrain *Init() {
    TTrain *train = (TTrain *)malloc(sizeof(TTrain));
    if (!train)
        return NULL;

    TList locomotive = (TList)malloc(sizeof(TCell));
    if (!locomotive) {
        free(train);
        return NULL;
    }
    locomotive->info = '!';
    locomotive->next = locomotive->prev = locomotive;

    TList wagon1 = (TList)malloc(sizeof(TCell));
    if (!wagon1) {
        free(locomotive);
        free(train);
        return NULL;
    }
    wagon1->info = '#';     // default wagon symbol
    wagon1->next = locomotive;   // link wagon after the locomotive
    wagon1->prev = locomotive;
    locomotive->next = locomotive->prev = wagon1;

    train->locomotive = locomotive;    // store locomotive pointer
    train->curr = wagon1;              // place mechanic in first wagon

    return train;
}

// Move the mechanic one wagon to the left (wrap to last wagon if needed).
void MOVE_LEFT(TTrain *train) {
    if (!train || !train->locomotive || !train->curr)
        return;
    if (train->curr == train->locomotive->next)   // mechanic is in first wagon
        train->curr = train->locomotive->prev;   // wrap to last wagon
    else
        train->curr = train->curr->prev;   // move to the wagon on the left
}

// Move the mechanic one wagon to the right (add wagon if at end).
void MOVE_RIGHT(TTrain *train) {
    if (!train || !train->locomotive || !train->curr)
        return;
    if (train->curr == train->locomotive->prev) {    // if in last wagon, add a new wagon
        TList temp = (TList)malloc(sizeof(TCell));   // allocate memory for a new cell
        if (!temp)   // check if allocation succeeded
            return;
        // link the new wagon at the end, right before the locomotive
        temp->prev = train->locomotive->prev;
        train->locomotive->prev->next = temp;
        temp->next = train->locomotive;
        train->locomotive->prev = temp;
        temp->info = '#';
        train->curr = temp;
    } else {
        train->curr = train->curr->next;    // move to the wagon on the right
    }
}

// Overwrite the character in the current wagon.
void WRITE(TTrain *train, char character) {
    if (!train || !train->curr)
        return;
    train->curr->info = character;    // update wagon content
}

void CLEAR_CELL(TTrain *train) {
    if (!train || !train->locomotive || !train->curr)
        return;
    if (train->curr->next == train->locomotive && train->curr->prev == train->locomotive) {   // if the train has a single wagon
        // set the train to its initial form
        train->locomotive->next = train->curr;
        train->locomotive->prev = train->curr;
        train->curr->info = '#';
        train->curr = train->locomotive->prev;
        train->curr = train->locomotive->next;
    } else {
        TList mechanic; // auxiliary pointer for the new mechanic position
        mechanic = train->curr;
        if (train->curr->prev == train->locomotive) {    // if it is the first wagon
            mechanic = train->locomotive->prev;  // move to last wagon
        } else {
            mechanic = train->curr->prev;  // otherwise, move to wagon on the left
        }
        train->curr->prev->next = train->curr->next;     // unlink current wagon
        train->curr->next->prev = train->curr->prev;
        free(train->curr);   // free removed wagon
        train->curr = mechanic;   // move mechanic to new position
    }
}

void CLEAR_ALL(TTrain *train) {
    if (!train || !train->locomotive || !train->curr)
        return;
    TList temp, mechanic, locomotive;   // auxiliary pointers
    mechanic = train->curr;
    locomotive = train->locomotive;
    TList current = train->locomotive->next;    
    while (current != locomotive) {     // traverse wagons until we reach the locomotive
        temp = current;
        current = current->next;   // move to next wagon
        if (temp != mechanic) {
            free(temp);   // free each wagon except the mechanic's
        }
    }
    // restore initial train state and links
    mechanic->info = '#';
    mechanic->next = locomotive;
    mechanic->prev = locomotive;
    locomotive->next = mechanic;
    locomotive->prev = mechanic;
}

void Insert_Left(TTrain *train, char character, FILE *output_file) {
    if (!train || !train->locomotive || !train->curr || !output_file)
        return;
    if (train->curr == train->locomotive->next) {    // check if the mechanic is in the first wagon
        fprintf(output_file, "ERROR\n");
        return;
    }

    TList temp = (TList)malloc(sizeof(TCell));    // allocate memory for the new wagon
    if (!temp)   // check if allocation succeeded
        return;
    // insert the new wagon to the left of the mechanic's wagon
    temp->info = character;
    temp->next = train->curr;
    temp->prev = train->curr->prev;
    train->curr->prev->next = temp;
    train->curr->prev = temp;
    train->curr = temp;    // move the mechanic to the newly inserted wagon
}

void Insert_Right(TTrain *train, char character) {
    if (!train || !train->locomotive || !train->curr)
        return;
    TList temp = (TList)malloc(sizeof(TCell));    // allocate memory for the new wagon
    if (!temp)   // check if allocation succeeded
        return;
    if (train->curr == train->locomotive->prev) {     // if mechanic is in last wagon, add on the right
        // link the new wagon to the right of the last wagon
        temp->info = character;
        train->curr->next = temp;
        temp->prev = train->curr;
        temp->next = train->locomotive;      // new wagon is linked to the locomotive
        train->locomotive->prev = temp;
    } else {
        // link a new wagon to the right of the mechanic
        temp->info = character;
        temp->next = train->curr->next;
        train->curr->next->prev = temp;
        train->curr->next = temp;
        temp->prev = train->curr;
    }
    train->curr = temp;   // move the mechanic to the newly inserted wagon
}

void SHOW_CURRENT(TTrain *train, FILE *output_file) {
    if (!train || !train->curr || !output_file)
        return;
    fprintf(output_file, "%c\n", train->curr->info);  // print the current wagon character
}

void SHOW(TTrain *train, FILE *output_file) {
    if (!train || !train->locomotive || !output_file)
        return;
    TList temp = train->locomotive->next;   // start with the first wagon
    while (temp != train->locomotive) {     // traverse the train
        if (temp == train->curr)  // if this is the mechanic's wagon
            fprintf(output_file, "|%c|", train->curr->info);      // print with markers
        else
            fprintf(output_file, "%c", temp->info);   // print the wagon character
        temp = temp->next;     // move to next wagon
    }
    fprintf(output_file, "\n");
}

void SWITCH(TQueue *queue) {
    if (!queue)
        return;
    TList2 current, temp1 = NULL, temp2 = NULL;    // auxiliary pointers
    // if the queue is empty or has a single element, it need not be reversed
    if ((queue->head == NULL && queue->tail == NULL) || (queue->head == queue->tail))
        return;

    current = queue->head;     // start at the head
    while (current) {     // traverse the queue
        temp1 = current->next;  // save address of next element
        current->next = temp2;   // reverse the next link
        current->prev = temp1;
        temp2 = current;   // advance the previous pointer
        current = temp1;
    }
    // update pointers to the head and tail of the reversed queue
    queue->tail = queue->head;
    queue->head = temp2;
}

void SEARCH(TTrain *train, char *string, FILE *output_file) {
    if (!train || !string || !output_file)
        return;
    int length = strlen(string);  // length of the string to search for
    TList start = train->curr;    // start from mechanic's position
    do {
        int i;
        TList beginning = start;     // remember the candidate starting position
        for (i = 0; i < length; i++) {
            if (start->info != string[i]) {    // compare wagon with string character
                break;
            }
            if (start->next == train->locomotive)    // if we encounter the locomotive
                start = train->locomotive->next;    // wrap to first wagon
            else
                start = start->next;     // move to next wagon
        }
        if (i == length) {       // if the string was found
            train->curr = beginning;   // move mechanic to the match start
            return;
        }
        start = beginning->next;   // move to next wagon
    } while (start != train->curr);  // repeat until we meet the mechanic
    fprintf(output_file, "ERROR\n");     // printed if string is not found
}

void SEARCH_RIGHT(TTrain *train, char *string, FILE *output_file) {
    if (!train || !string || !output_file)
        return;
    int length = strlen(string);
    TList start = train->curr;
    do {
        int i;
        TList beginning = start;
        for (i = 0; i < length; i++) {
            if (start == NULL) {
                fprintf(output_file, "ERROR\n");
                return;
            }
            if (start->info != string[i]) {    // compare wagon with string character
                break;
            }
            start = start->next;     // move to next wagon
        }
        if (i == length) {   // if the string was found
            while (start != NULL && start->next != train->locomotive)
                start = start->next;
            train->curr = start;     // move mechanic to last position in the match
            return;
        }
        start = beginning->next;   // move to next wagon
    } while (start != train->locomotive);      // repeat until we meet the locomotive
    fprintf(output_file, "ERROR\n");      // printed if string is not found
}


void SEARCH_LEFT(TTrain *train, char *string, FILE *output_file) {
    if (!train || !string || !output_file)
        return;
    int length = strlen(string);      // length of the string to search for
    TList start = train->curr;  // initialize start with the mechanic's position
    do {
        int i;
        TList beginning = start;  // remember the candidate starting position
        for (i = 0; i < length; i++) {
            if (start->info != string[i]) {        // compare wagon with string character
                break;
            }
            start = start->prev;     // move to the previous wagon
        }
        if (i == length) {     // if the string was found
            train->curr = start->next;    // move mechanic to last position in the match
            return;
        }
        start = beginning->prev;   // move to the previous wagon
    } while ((start != train->locomotive->next) && (start != NULL));    // repeat until we meet the locomotive
    fprintf(output_file, "ERROR\n");     // printed if string is not found
}
