#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define BUFFER_SIZE 50
#define HISTORY 10

static char buffer[BUFFER_SIZE];

int execute_command = 1;
int matched = 0;
char *searched_command;

struct QNode {
    char *key;
    struct QNode* next;
};

struct Queue {
    struct QNode *front, *rear;
};

struct QNode* newNode(char *buf)
{
    struct QNode* temp
            = (struct QNode*)malloc(sizeof(struct QNode));
    temp->key = buf;
    temp->next = NULL;
    return temp;
}

struct Queue* createQueue()
{
    struct Queue* q
            = (struct Queue*)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}

struct Queue* q;


int queue_len(struct Queue* q) {
    int len = 0;
    struct QNode* temp_ptr = q->front;
    if (temp_ptr == NULL)
        return 0;
    while (temp_ptr != NULL) {
        len++;
        temp_ptr = temp_ptr->next;
    }
    return len;
}

void deQueue(struct Queue* q)
{
    if (q->front == NULL)
        return;

    struct QNode* temp = q->front;
    q->front = q->front->next;

    if (q->front == NULL)
        q->rear = NULL;

    free(temp);
}

void enQueue(struct Queue* q, char *buf)
{
    struct QNode* temp = newNode(buf);

    if (q->rear == NULL) {
        q->front = q->rear = temp;
        return;
    }

    q->rear->next = temp;
    q->rear = temp;

    if (queue_len(q) > HISTORY) {
        deQueue(q);
    }
}

void print_queue(struct Queue* q) {
    struct QNode* temp_ptr = q->front;

    if (temp_ptr == NULL)
    {
        printf("\nNo Command to show.\n");
        return;
    }

    printf("The 10 most recent commands are :\n");
    while (temp_ptr != NULL) {
        printf("%s", temp_ptr->key);
        temp_ptr = temp_ptr->next;
    }
    printf("\n");
}

void search_command(char search_letter) {
    struct QNode* temp_ptr = q->front;
    if (temp_ptr == NULL)
    {
        return;
    }

    while (temp_ptr != NULL) {
        char* temp_command = temp_ptr->key;
        if (temp_command[0] == search_letter) {
            searched_command = temp_command;
            enQueue(q, searched_command);
            matched = 1;
            return;
        }
        temp_ptr = temp_ptr->next;
    }
}

/* the signal handler function */
void handle_SIGINT() {
    write(STDOUT_FILENO, buffer, strlen(buffer));

    print_queue(q);
    execute_command = 0;
//    exit(0); // This is only for illustration purposes. You may want to
//    // delete the exit(0) statement for this lab assignment.
}

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */

/**
 * setup() reads in the next command line, separating it into distinct tokens
 * using whitespace as delimiters. setup() sets the args parameter as a 
 * null-terminated string.
 */
void setup(char inputBuffer[], char *args[], int *background) {
    int length, /* # of characters in the command line */
    i,      /* loop index for accessing inputBuffer array */
    start,  /* index where beginning of next command parameter is */
    ct;     /* index of where to place the next parameter into args[] */

    ct = 0;

    /* read what the user enters on the command line */
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);

    if (length > 0) {
        char command_search[] = "r ";
        if (inputBuffer[0] == command_search[0] && inputBuffer[1] == command_search[1]) {
            char search_letter = inputBuffer[2];
            search_command(search_letter);
            if (matched == 1) {
                printf("Command found in history! Executing searched command : %s", searched_command);
                strcpy(inputBuffer, searched_command);
                length = strlen(inputBuffer);
                matched = 0;
            }
            else {
                printf("Could not find command starting with %c in history.\n", search_letter);
                return;
            }
        }
        else if (execute_command == 1) {
            char *buf = (char *) malloc(sizeof(inputBuffer));
            strncpy(buf, inputBuffer, length);
            enQueue(q, buf);
        }
    }

    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */
//    if (length < 0) {
//        perror("error reading the command");
//        exit(-1);           /* terminate with error code of -1 */
//    }

    /* examine every character in the inputBuffer */
    for (i = 0; i < length; i++) {
        switch (inputBuffer[i]) {
            case ' ':
            case '\t' :               /* argument separators */
                if (start != -1) {
                    args[ct] = &inputBuffer[start];    /* set up pointer */
                    ct++;
                }
                inputBuffer[i] = '\0'; /* add a null char; make a C string */
                start = -1;
                break;

            case '\n':                 /* should be the final char examined */
                if (start != -1) {
                    args[ct] = &inputBuffer[start];
                    ct++;
                }
                inputBuffer[i] = '\0';
                args[ct] = NULL; /* no more arguments to this command */
                break;

            case '&':
                *background = 1;
                inputBuffer[i] = '\0';
                break;

            default :             /* some other character */
                if (start == -1)
                    start = i;
        }
    }
    args[ct] = NULL; /* just in case the input line was > 80 */
}

int main(void) {
    char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
    int background;             /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE / 2 + 1];/* command line (of 80) has max of 40 arguments */

    /* set up the signal handler */
    struct sigaction handler;
    handler.sa_handler = handle_SIGINT;
    sigaction(SIGINT, &handler, NULL);
    strcpy(buffer, "Caught <ctrl><c>\n");

    q = createQueue();

    while (1) {            /* Program terminates normally inside setup */
        background = 0;
        printf("COMMAND->");
        fflush(0);

        setup(inputBuffer, args, &background);

        int rc = fork();
        if (rc < 0) {
            fprintf(stderr, "%s\n", "fork unsuccessful");
            exit(1);
        } else if (rc == 0) {
            execvp(args[0], args);
        } else {
            if (background != 1) {
                int rc_wait = wait(NULL);
            }
        }
        memset(inputBuffer, 0, MAX_LINE*sizeof(char));
        execute_command = 1;
    }
    return 0;
}
