# Train Editor — Data Structures Project

**Author:** Micu Alexandra Cristina


This project implements a **train editor** in C. The train is modeled as a circular doubly-linked list of wagons, each holding a single character. A "mechanic" (cursor) moves between wagons and performs editing operations. Commands can be queued and executed later, or run immediately. The project consists of three source files: **functions.h**, **functions.c**, and **main.c**.

---

## Project Files

| File | Purpose |
|------|---------|
| **functions.h** | Header file with type definitions, structures, and function declarations |
| **functions.c** | Implementation of the queue, train, and all operations |
| **main.c** | Program entry point; reads input, parses commands, and coordinates execution |

---



# functions.h

The header file serves as the interface between `main.c` and `functions.c`. It includes the standard libraries (`stdio.h`, `stdlib.h`, `string.h`, `ctype.h`) and defines all data structures and function prototypes.

---

## Data Structures

### The List Node (Train Wagon)

```c
typedef struct list_node {
    char info;
    struct list_node *next, *prev;
} TCell, *TList;
```

**`TCell`** is the structure for a single wagon. The **`info`** field stores one character (the content of that wagon). The **`next`** and **`prev`** pointers form a doubly-linked list: each node points to the next wagon and the previous wagon. **`TList`** is a pointer type to `TCell`, used for list traversal and manipulation.

### The Train

```c
typedef struct train {
    TList locomotive;
    TList curr;
} TTrain;
```

**`TTrain`** represents the train. The **`locomotive`** field points to a special sentinel node that marks the boundary of the wagon list. The **`curr`** field points to the wagon where the mechanic (cursor) is currently positioned. The train is implemented as a **circular** list: the last wagon links back to the locomotive, and the locomotive links forward to the first wagon.

### The Queue Node

```c
typedef struct list_node_2 {
    char *info;
    struct list_node_2 *next, *prev;
} TCell2, *TList2;
```

**`TCell2`** and **`TList2`** form a second doubly-linked list used for the command queue. Unlike the train, each node stores a **`char *`** (a string) in **`info`**, representing a full command such as `"MOVE_RIGHT"` or `"WRITE X"`. The **`next`** and **`prev`** pointers connect the queue elements.

### The Queue

```c
typedef struct queue {
    TList2 head, tail;
} TQueue;
```

**`TQueue`** is a queue structure with **`head`** (first element) and **`tail`** (last element). Elements are inserted at the tail and extracted from the head (FIFO order).

---

# functions.c

This file implements all operations on the queue and the train.

---

## Queue Functions

### `TQueue *InitQueue()`

**Purpose:** Creates and returns an empty queue.

**What it does:** The function allocates memory for a `TQueue` structure using `malloc`. It checks whether the allocation succeeded; if not, it returns `NULL`. The `head` and `tail` pointers are set to `NULL`, indicating an empty queue. The queue is then returned so it can be used by the rest of the program.

**Structure used:** `TQueue`

---

### `int Enqueue(TQueue *queue, char *instruction)`

**Purpose:** Inserts a new element (a command string) at the end of the queue.

**What it does:** The function allocates memory for a new `TCell2` node. If allocation fails, it returns `0`. The instruction string is copied into the node's `info` field using `strdup`, which allocates memory for the copy. The `next` and `prev` pointers of the new node are set to `NULL` initially. If the queue is empty (`head` and `tail` are both `NULL`), the new node becomes both the head and the tail. Otherwise, the new node is linked after the current tail: the tail's `next` points to the new node, the new node's `prev` points to the tail, and the new node becomes the new tail. The function returns `1` on success.

**Structures used:** `TQueue`, `TCell2`, `TList2`

---

### `int Dequeue(TQueue *queue, char **x)`

**Purpose:** Removes the first element from the queue and returns its string to the caller.

**What it does:** If the queue is empty, the function returns `0`. If there is only one element, the string is stored in `*x`, and both `head` and `tail` are set to `NULL`. For a queue with multiple elements, the function saves the first node's `info` in `*x`, advances `head` to the second node, and frees the memory of the removed node (both its `info` string and the node itself). Returns `1` on success.

**Structures used:** `TQueue`, `TCell2`, `TList2`

---

### `void DestroyQueue(TQueue **queue)`

**Purpose:** Destroys the queue and frees all associated memory.

**What it does:** The function loops while the queue still has elements (i.e., `tail` is not `NULL`). In each iteration, it calls `Dequeue` to remove the first element. Once the queue is empty, it frees the `TQueue` structure itself and sets the pointer to `NULL` so the queue cannot be used again.

**Structures used:** `TQueue`, `TCell2`, `TList2`

---

### `void DisplayQueue(TQueue *queue, FILE *output_file)`

**Purpose:** Prints all elements of the queue to the given output file.

**What it does:** A temporary pointer starts at `head` and traverses the queue by following `next` until it reaches `NULL`. For each node, the function prints the `info` string. The output is prefixed with `"Queue is: "`. This function is useful for debugging or inspection.

**Structures used:** `TQueue`, `TList2`

---

## Train Initialization

### `TTrain *Init()`

**Purpose:** Initializes the train with a locomotive (sentinel) and one wagon.

**What it does:** The function allocates memory for a `TTrain` structure. It then allocates the locomotive node: a `TCell` with `info = '!'` and both `next` and `prev` pointing to itself, forming a trivial circular list. Next, it allocates the first wagon with `info = '#'` and inserts it into the circle: the wagon is placed between the locomotive and itself, so the order becomes locomotive → wagon → locomotive. The train's `locomotive` field is set to the sentinel, and `curr` is set to the first wagon (where the mechanic starts). Returns `NULL` if any allocation fails.

**Structures used:** `TTrain`, `TCell`, `TList`

---

## Train Navigation

### `void MOVE_LEFT(TTrain *train)`

**Purpose:** Moves the mechanic one wagon to the left.

**What it does:** If the mechanic is on the first wagon (i.e., `curr == locomotive->next`), moving left means wrapping to the last wagon, so `curr` is set to `locomotive->prev`. Otherwise, `curr` is simply set to `curr->prev`, moving one step left in the doubly-linked list. The circular structure ensures we never leave the train.

**Structures used:** `TTrain`, `TList`

---

### `void MOVE_RIGHT(TTrain *train)`

**Purpose:** Moves the mechanic one wagon to the right. If at the last wagon, adds a new wagon.

**What it does:** If the mechanic is on the last wagon (`curr == locomotive->prev`), the function allocates a new `TCell`, sets its `info` to `'#'`, and inserts it between the last wagon and the locomotive. The links are updated so the new wagon becomes the new last wagon, and the mechanic moves there. If the mechanic is not on the last wagon, `curr` is set to `curr->next`. This way, moving right from the end dynamically extends the train.

**Structures used:** `TTrain`, `TCell`, `TList`

---

## Train Editing

### `void WRITE(TTrain *train, char character)`

**Purpose:** Overwrites the character in the wagon where the mechanic is located.

**What it does:** The function sets `curr->info` to the given character. No structural changes are made; only the content of the current wagon is modified.

**Structures used:** `TTrain`, `TList`

---

### `void CLEAR_CELL(TTrain *train)`

**Purpose:** Removes the current wagon and moves the mechanic to the wagon on the left.

**What it does:** If the train has only one wagon (both `curr->next` and `curr->prev` point to the locomotive), the wagon is not freed; instead, the train is reset to its initial state: the single wagon keeps `info = '#'`, links are restored, and the mechanic stays on it. If there are multiple wagons, the function determines where the mechanic should go (left wagon, or last wagon if currently on the first). It then unlinks the current wagon by updating the `next` of the previous wagon and the `prev` of the next wagon, frees the current wagon, and moves `curr` to the new position.

**Structures used:** `TTrain`, `TList`

---

### `void CLEAR_ALL(TTrain *train)`

**Purpose:** Removes all wagons except the one under the mechanic, resetting the train to a single wagon.

**What it does:** The function saves the current mechanic position and the locomotive. It traverses the train from the first wagon (`locomotive->next`) to the last (before reaching the locomotive again). For each wagon, if it is not the mechanic's wagon, it is freed. The wagon under the mechanic is kept; its `info` is set to `'#'`, and the circular links are restored so that the locomotive and this single wagon form the ring again. The mechanic remains on that wagon.

**Structures used:** `TTrain`, `TList`

---

### `void Insert_Left(TTrain *train, char character, FILE *output_file)`

**Purpose:** Inserts a new wagon with the given character to the left of the mechanic.

**What it does:** If the mechanic is on the first wagon (`curr == locomotive->next`), there is no wagon to the left (the left neighbor is the locomotive), so the function writes `"ERROR\n"` to the output file and returns. Otherwise, it allocates a new `TCell`, sets its `info` to the character, and inserts it between `curr->prev` and `curr`. The links are updated: the new wagon's `next` points to `curr`, its `prev` points to `curr->prev`, and the adjacent nodes are updated accordingly. The mechanic then moves to the newly inserted wagon (`curr = temp`).

**Structures used:** `TTrain`, `TCell`, `TList`

---

### `void Insert_Right(TTrain *train, char character)`

**Purpose:** Inserts a new wagon with the given character to the right of the mechanic.

**What it does:** The function allocates a new `TCell` and sets its `info`. If the mechanic is on the last wagon (`curr == locomotive->prev`), the new wagon is inserted between the last wagon and the locomotive: `curr->next` points to the new wagon, and the new wagon's `next` points to the locomotive. Otherwise, the new wagon is inserted between `curr` and `curr->next`. In both cases, the links are updated and the mechanic moves to the new wagon.

**Structures used:** `TTrain`, `TCell`, `TList`

---

## Train Display

### `void SHOW_CURRENT(TTrain *train, FILE *output_file)`

**Purpose:** Prints the character in the mechanic's wagon.

**What it does:** The function prints `curr->info` followed by a newline to the output file. No structural changes are made.

**Structures used:** `TTrain`, `TList`

---

### `void SHOW(TTrain *train, FILE *output_file)`

**Purpose:** Prints the entire train, with the mechanic's wagon highlighted.

**What it does:** A temporary pointer starts at the first wagon (`locomotive->next`) and traverses right until it reaches the locomotive again. For each wagon, if it is the mechanic's wagon (`temp == curr`), the character is printed between vertical bars (e.g., `|X|`). Otherwise, the character is printed without bars. A newline is printed at the end. This gives a visual representation of the train and the current position.

**Structures used:** `TTrain`, `TList`

---

## Queue Manipulation

### `void SWITCH(TQueue *queue)`

**Purpose:** Reverses the order of elements in the queue.

**What it does:** If the queue is empty or has a single element, the function returns immediately. Otherwise, it traverses the queue from head to tail. For each node, it saves the next node, sets the current node's `next` to the previously processed node, and sets `prev` to the saved next (effectively reversing the direction of the list). After the loop, the old head becomes the new tail, and the old tail becomes the new head. The queue's `head` and `tail` pointers are updated accordingly. This is an in-place reversal requiring no extra allocation.

**Structures used:** `TQueue`, `TList2`

---

## Search Functions

### `void SEARCH(TTrain *train, char *string, FILE *output_file)`

**Purpose:** Searches for a string in the train, moving right from the mechanic, and at most one full circle. On success, moves the mechanic to the start of the match.

**What it does:** The function computes the length of the search string. It starts from the mechanic's position and uses a do-while loop to try matching at each possible starting position. For each candidate position, it compares wagon characters with the string characters, moving right and wrapping to the first wagon when the locomotive is reached. If a full match is found, the mechanic is moved to the starting position of the match and the function returns. If no match is found after traversing at most one full circle (back to the mechanic), it prints `"ERROR\n"`.

**Structures used:** `TTrain`, `TList`

---

### `void SEARCH_RIGHT(TTrain *train, char *string, FILE *output_file)`

**Purpose:** Searches for a string from the mechanic's position toward the right until the locomotive. On success, moves the mechanic to the wagon at the end of the match.

**What it does:** Similar to `SEARCH`, but the search is constrained to the rightward direction until the locomotive. The loop continues while the current position is not the locomotive. When a match is found, the mechanic is moved to the wagon corresponding to the last character of the match (i.e., the end of the matched string). If the string is not found in that range, `"ERROR\n"` is printed.

**Structures used:** `TTrain`, `TList`

---

### `void SEARCH_LEFT(TTrain *train, char *string, FILE *output_file)`

**Purpose:** Searches for a string from the mechanic's position toward the left. On success, moves the mechanic to the wagon after the match (in leftward order).

**What it does:** The search proceeds leftward using `prev` instead of `next`. For each candidate position, characters are compared while moving left. The loop stops when reaching the first wagon (or `NULL`). When a match is found, the mechanic is moved to `start->next`, which in leftward order is the wagon "after" the matched region. If no match is found, `"ERROR\n"` is printed.

**Structures used:** `TTrain`, `TList`

---

# main.c

The main program ties everything together. It reads commands from an input file (or stdin), decides whether to execute them immediately or queue them, and writes results to stdout.

---

## Initialization

The program calls `Init()` to create the train (locomotive plus one wagon) and `InitQueue()` to create the command queue. It allocates buffers for reading lines (`number`, `s`, `instruction`) and reads from a file path passed as `argv[1]` (or stdin if no file is provided). Output is written to stdout. The first line of the input is read as a number (used for compatibility with the assignment format).

---

## Command Processing Loop

For each subsequent line read from the input file:

1. **Immediate execution:** If the line contains `SHOW_CURRENT`, `SHOW`, or `SWITCH`, the corresponding function is called right away. `SHOW` and `SHOW_CURRENT` write to the output file; `SWITCH` reverses the queue.

2. **EXECUTE:** If the line contains `EXECUTE`, the program takes the first element from the queue (`queue->head->info`), which is a command string. It then uses `strstr` to identify which command it is (e.g., `MOVE_RIGHT`, `WRITE`, `INSERT_LEFT`) and calls the appropriate function. For commands with parameters (e.g., `WRITE X`, `INSERT_RIGHT Y`), the parameter is extracted using `strtok` or by indexing the string. After execution, `Dequeue` is called to remove the executed command from the queue.

3. **Queue insertion:** If the line is not one of the immediate commands and not `EXECUTE`, it is treated as a command to be queued. The line is added to the queue with `Enqueue`.

---

## Cleanup

After all input has been processed, the program empties the queue by repeatedly calling `Dequeue` until the queue is empty, then frees the queue structure. It calls `CLEAR_ALL` on the train to leave it with a single wagon, then frees the train's locomotive and remaining wagon. Finally, it closes the input file if one was opened and returns.

---

## Structures and Functions Used in main.c

The main program uses:
- **`TTrain`** and **`TQueue`** for the train and queue
- **`Init`**, **`InitQueue`** for initialization
- **`Enqueue`**, **`Dequeue`** for queue operations
- **`SHOW`**, **`SHOW_CURRENT`**, **`SWITCH`** for immediate commands
- **`MOVE_LEFT`**, **`MOVE_RIGHT`**, **`WRITE`**, **`Insert_Left`**, **`Insert_Right`**, **`CLEAR_CELL`**, **`CLEAR_ALL`**, **`SEARCH`**, **`SEARCH_RIGHT`**, **`SEARCH_LEFT`** for queued commands

Input is read with `fgets` and `strstr`; string parsing uses `strtok` and `strlen` to extract command names and parameters.

---

## How to Run and Test

- Build the project: `make`
- Run with a test input file: `./train tests/Input/test1.in`
- Test folders: `tests/Input` contains input files, `tests/Output` contains expected outputs
- Checker: `checker.sh` runs all tests and compares output; run `make test`
