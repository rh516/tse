#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "queue.h"

typedef struct node
{
    void *data;
    struct node *next;
} node_t;

typedef struct queue
{
    node_t *front;
    node_t *back;
} myQueueType;

// makes a new node to be placed in the queue
static node_t *makeNode(void *element)
{
    node_t *newNode;

    if (!(newNode = (node_t *)malloc(sizeof(node_t))))
    {
        printf("Error: malloc failed allocating element\n");
        return NULL;
    }

    newNode->data = element;
    newNode->next = NULL;

    return newNode;
}

// creates an empty queue
queue_t *qopen(void)
{
    myQueueType *qp;

    if (!(qp = (myQueueType *)malloc(sizeof(myQueueType))))
    {
        printf("Error: malloc failed allocating queue\n");
        return NULL;
    }

    qp->front = NULL;
    qp->back = NULL;

    return (queue_t *)qp;
}

void *qsearch(queue_t *qp,
              bool (*searchfn)(void *elementp, const void *keyp),
              const void *skeyp)
{
    myQueueType *mqp = (myQueueType *)qp;
    void *refdata = NULL;

    if (searchfn == NULL || mqp == NULL || skeyp == NULL || mqp->front == NULL)
    {
        return NULL;
    }
    for (node_t *p = mqp->front; p != NULL; p = p->next)
    {
        if (searchfn(p->data, skeyp))
        {
            refdata = p->data;
        }
    }
    return refdata;
}

/* search a queue using a supplied boolean function
 * skeyp -- a key to search for
 * searchfn -- a function applied to every element of the queue
 *          -- elementp - a pointer to an element
 *          -- keyp - the key being searched for (i.e. will be
 *             set to skey at each step of the search
 *          -- returns TRUE or FALSE as defined in bool.h
 * returns a pointer to an element, or NULL if not found
 */
/* search a queue using a supplied boolean function (as in qsearch),
 * removes the element from the queue and returns a pointer to it or
 * NULL if not found
 */
void *qremove(queue_t *qp,
              bool (*searchfn)(void *elementp, const void *keyp),
              const void *skeyp)
{
    myQueueType *mqp = (myQueueType *)qp;
    void *refdata = NULL;
    node_t *temp = NULL;

    if (searchfn == NULL || mqp == NULL || skeyp == NULL || mqp->front == NULL)
    {
        printf("Can't remove null element\n");
        return NULL;
    }
    if (searchfn(mqp->front->data, skeyp))
    {
        refdata = mqp->front->data;
        temp = mqp->front;
        mqp->front = mqp->front->next;
        free(temp);
    }
    else
    {
        for (node_t *p = mqp->front; p->next != NULL;)
        {
            if (searchfn(p->next->data, skeyp))
            {
                refdata = p->next->data;
                temp = p->next;
                p->next = temp->next;
                free(temp);
            }
            else
            {
                p = p->next;
            }
        }
    }
    return refdata;
}

// deallocates a queue, frees everything in it
void qclose(queue_t *qp)
{
    myQueueType *mqp = (myQueueType *)qp;

    if (mqp != NULL)
    {
        node_t *currentNode = mqp->front;
        node_t *nextNode;

        while (currentNode != NULL)
        {
            if (currentNode->data != NULL)
            {
                free(currentNode->data);
                currentNode->data = NULL;
            }

            nextNode = currentNode->next;
            free(currentNode);
            currentNode = nextNode;
        }

	mqp->front = NULL;

        free(mqp);
        mqp = NULL;
    }
}

// places an element at the back of the queue
int32_t qput(queue_t *qp, void *elementp)
{
    myQueueType *mqp = (myQueueType *)qp;

    if (elementp == NULL)
    {
        printf("Can't put null element\n");
        return 1;
    }

    if (qp == NULL)
    {
        printf("Queue can't be null\n");
        return 1;
    }

    node_t *node = makeNode(elementp);

    if (mqp->front == NULL)
    {
        mqp->front = node;
        mqp->back = node;
    }
    else
    {
        mqp->back->next = node;
        mqp->back = node;
    }

    return 0;
}

// removes and returns the first element of the queue
void *qget(queue_t *qp)
{
    myQueueType *mqp = (myQueueType *)qp;
    node_t *retNode = mqp->front;

    if (retNode == NULL)
    {
        return NULL;
    }

    mqp->front = mqp->front->next;
    void *data = retNode->data;
    free(retNode);
    return data;
}

// applies a function to all elements in the queue;
void qapply(queue_t *qp, void (*fn)(void *elementp))
{
    myQueueType *mqp = (myQueueType *)qp;
    node_t *currentNode = mqp->front;

    while (currentNode != NULL)
    {
        void *data = currentNode->data;
        fn(data);
        currentNode = currentNode->next;
    }
}

// concatenatenates elements of q2 into q1
// q2 is deallocated, closed, and unusable upon completion
void qconcat(queue_t *q1p, queue_t *q2p)
{
    myQueueType *mq2p = (myQueueType *)q2p;
    node_t *currentNode = mq2p->front;

    while (currentNode != NULL)
    {
        qput(q1p, currentNode->data);
        currentNode->data = NULL;
        currentNode = currentNode->next;
    }

    qclose((queue_t *)mq2p);
}
