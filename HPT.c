#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "HPT.h"

void initializeTable(HashPageTable **pt, int size, char *algor)
{
    HashPageTable *pt1;
    pt1 = (HashPageTable *)malloc(sizeof(HashPageTable));
    pt1->page_faults = 0;
    pt1->reads = 0;
    pt1->writes = 0;
    pt1->disk_writes = 0;
    pt1->algor = algor;
    pt1->numOfpages = 0;
    for (int i = 0; i < size; i++)
    {
        pt1->array[i] = (Page *)malloc(sizeof(Page) * size);
        pt1->array[i]->next = NULL;
        pt1->array[i]->page_number = -1;
        pt1->array[i]->count = -1;
    }
    *pt = pt1;
}

void simulation(HashPageTable *pt, HashPageTable *pt2, unsigned int PageNumber, char mode, int c, int size, int prId, int frames, queueMemory *q)
{
    int index = hashFunc(PageNumber, size);
    int exist;
    if (prId == 1)
    {
        exist = PageExists(q, PageNumber);
        if (exist == 1)
        {
            //printf("Page Already exist \n");
            updatePage(pt, q, PageNumber, mode, c, size, prId);
        }
        else
        {
            if (availableFrame(q, frames) == 1)
            {

                insertQueue(q, PageNumber, mode, c, prId);
                insertPage(pt, PageNumber, mode, c, size, prId);
                pt->numOfpages++;
                increase(pt, mode);
            }
            else
            {
                Page *victim = malloc(sizeof(Page));
                victim = findVictim(pt, q, size);
                replaceVictim(pt, pt2, q, PageNumber, mode, c, victim->page_number, size, prId, victim->mode, victim->processID);
                increase(pt, mode);
            }
            pt->page_faults++;
        }
    }
    else
    {
        exist = PageExists(q, PageNumber);
        if (exist == 1)
        {
            //printf("Page Already exist \n");
            updatePage(pt2, q, PageNumber, mode, c, size, prId);
        }
        else
        {
            if (availableFrame(q, frames) == 1)
            {

                insertQueue(q, PageNumber, mode, c, prId);
                insertPage(pt2, PageNumber, mode, c, size, prId);
                pt2->numOfpages++;

                increase(pt2, mode);
            }
            else
            {
                Page *victim = malloc(sizeof(Page));
                victim = findVictim(pt2, q, size);
                replaceVictim(pt, pt2, q, PageNumber, mode, c, victim->page_number, size, prId, victim->mode, victim->processID);

                increase(pt2, mode);
            }
            pt2->page_faults++;
        }
    }
}

int hashFunc(int key, int size)
{
    return key % size;
}

void insertPage(HashPageTable *pt, unsigned int PageNumber, char mode, int c, int size, int prId)
{

    int index = hashFunc(PageNumber, size);

    Page *position;
    Page *newPage;
    newPage = (Page *)malloc(sizeof(Page));
    newPage->processID = prId;
    newPage->count = c;
    newPage->mode = mode;
    newPage->page_number = PageNumber;
    position = pt->array[index];

    if (newPage != NULL)
    {

        if (pt->array[index]->next == NULL)
        {
            pt->array[index]->next = newPage;
            newPage->next = NULL;
        }
        else
        {
            while (position->next != NULL)
                position = position->next;

            position->next = newPage;
            newPage->next = NULL;
        }
    }
}

int PageExists(queueMemory *q, unsigned int PageNumber)
{
    queueNode *tmp = malloc(sizeof(queueNode));
    tmp = q->head;

    while (tmp != NULL)
    {
        if (tmp->pg->page_number == PageNumber)
            return 1;
        tmp = tmp->next;
    }
    return 0;
}

int availableFrame(queueMemory *q, int frame)
{
    if (q->size >= frame)
        return 0;
    return 1;
}

void printTable(HashPageTable *pt, int size)
{

    for (int i = 0; i < size; i++)
    {

        Page *tmp = pt->array[i];

        while (tmp != NULL)
        {
            printf("(%d %c )", tmp->page_number, tmp->mode);
            tmp = tmp->next;
        }
        printf("\n");
    }
    printf("\n");
}

void increase(HashPageTable *pt, char mode)
{
    if (mode == 'R')
    {
        pt->reads++;
    }
    else
    {
        pt->writes++;
    }
}

void deletePage(HashPageTable *pt, int PageNumber, int size)
{
    //temp is used to freeing the memory
    int index = hashFunc(PageNumber, size);
    Page *temp;
    Page *head = pt->array[index];

    //key found on the head node.
    //move to head node to the next and free the head.
    if (head->page_number == PageNumber)
    {
        temp = head; //backup to free the memory
        pt->array[index] = pt->array[index]->next;
        free(temp);
    }

    else
    {
        Page *current = head;
        while (current->next != NULL)
        {
            //if yes, we need to delete the current->next node
            if (current->next->page_number == PageNumber)
            {
                temp = current->next;
                //node will be disconnected from the linked list.
                current->next = current->next->next;
                free(temp);
                break;
            }
            //Otherwise, move the current node and proceed
            else
                current = current->next;
        }
    }
}

void updatePage(HashPageTable *pt, queueMemory *q, unsigned int PageNumber, char mode, int c, int size, int prId)
{
    if (strcmp(pt->algor, "LRU") == 0)
    {
        increase(pt, mode);

        deleteNode(q, PageNumber);
        insertQueue(q, PageNumber, mode, c, prId);
    }
    else if (strcmp(pt->algor, "SC") == 0)
    {
        updateNode(q, PageNumber);
        increase(pt, mode);
    }
}

Page *findVictim(HashPageTable *pt, queueMemory *q, int size)
{
    Page *victim;
    victim = malloc(sizeof(Page));

    if (strcmp(pt->algor, "LRU") == 0)
    {
        victim = queuePop(q);
        // printf("victimNum is %d\n", victim->page_number);
        // printf("victim id is %c\n",victim->processID);
        // printf("victim mode is %c\n",victim->mode);
        return victim;
    }
    else if (strcmp(pt->algor, "SC") == 0)
    {
        queueNode *victimSc = malloc(sizeof(queueNode));

        queueNode *tmp = q->head;
        while (tmp != NULL)
        {
            if (tmp->pg->R == 0)
            {
                victimSc = tmp;
                //printf("victim is %d",victimSc->pg->page_number);
                return victimSc->pg;
            }
            else
            {
                tmp->pg->R = 0;
                if (tmp->next == NULL)
                    tmp = q->head;
                else
                    tmp = tmp->next;
            }
        }
    }
    printf("Error: Wrong use FindVictim Function\n");
    return 0;
}

void replaceVictim(HashPageTable *pt, HashPageTable *pt2, queueMemory *q, unsigned int PageNumber, char mode, int c, int victim, int size, int prId, char vmode, int victimId)
{
    if (prId == 1)
    {
        if (vmode == 'W')
        {
            pt->disk_writes++;
        }
    }
    if (prId == 2)
    {
        if (vmode == 'W')
        {
            pt2->disk_writes++;
        }
    }

    if (victimId == 1)
        deletePage(pt, victim, size);
    if (victimId == 2)
        deletePage(pt2, victim, size);

    if (prId == 1)
        insertPage(pt, PageNumber, mode, c, size, prId);
    if (prId == 2)
        insertPage(pt2, PageNumber, mode, c, size, prId);

    if (strcmp(pt->algor, "LRU") == 0)
    {
        insertQueue(q, PageNumber, mode, c, prId);
    }
    else if (strcmp(pt->algor, "SC") == 0)
    {

        queueNode *victimReplace = malloc(sizeof(queueNode));
        victimReplace = q->head;
        while (victimReplace != NULL)
        {

            if (victimReplace->pg->page_number == victim)
            {
                victimReplace->pg->page_number = PageNumber;
                victimReplace->pg->mode = mode;
                victimReplace->pg->processID = prId;
                victimReplace->pg->R = 0;
            }
            else
            {
                victimReplace = victimReplace->next;
            }
        }
    }
}

char getVictimMode(queueMemory *q, int VictmNumber)
{
    char victimMode = '0';

    queueNode *tmp = malloc(sizeof(queueNode));
    tmp = q->head;

    while (tmp != NULL)
    {
        if (tmp->pg->page_number == VictmNumber)
        {
            victimMode = tmp->pg->mode;
        }
        tmp = tmp->next;
    }

    return victimMode;
}

void printStats(HashPageTable *pt, int max_traces, int frames)
{

    printf("%d reads, %d writes, %d writes on disk\n", pt->reads, pt->writes, pt->disk_writes);
    printf("%d page faults happend.\n", pt->page_faults);
    printf("%d traces were examined\n", max_traces);
    printf("\n");
}

void insertQueue(queueMemory *q, unsigned int PageNumber, char mode, int c, int prId)
{

    queueNode *newNode = malloc(sizeof(queueNode));
    newNode->pg = malloc(sizeof(Page));
    newNode->next = NULL;
    newNode->pg->count = c;
    newNode->pg->mode = mode;
    newNode->pg->page_number = PageNumber;
    newNode->pg->processID = prId;
    newNode->pg->R = 0;

    if (q->tail == NULL)
    {
        q->head = newNode;
        q->tail = newNode;
        q->size++;
        return;
    }

    q->tail->next = newNode;
    q->tail = newNode;
    q->size++;
}

queueMemory *initQueue()
{
    queueMemory *q = malloc(sizeof(queueMemory));
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

void printfQueue(queueMemory *q)
{
    queueNode *tmp = malloc(sizeof(queueNode));
    tmp = q->head;

    while (tmp != NULL)
    {
        printf("( %d %d)", tmp->pg->page_number, tmp->pg->R);
        tmp = tmp->next;
    }
    printf(" size is %d\n\n", q->size);
}

void deleteNode(queueMemory *q, unsigned int PageNumber)
{

    queueNode *tmp = malloc(sizeof(queueNode));
    tmp = q->head;

    if (tmp->pg->page_number == PageNumber)
    {
        tmp = q->head;
        q->head = q->head->next;
        free(tmp);
        q->size--;
    }
    else
    {
        queueNode *current = malloc(sizeof(queueNode));

        current = q->head;

        while (current->next != NULL)
        {

            if (current->next->pg->page_number == PageNumber)
            {
                tmp = current->next;
                if (tmp->next == NULL)
                    q->tail = current;
                current->next = current->next->next;
                free(tmp);
                break;
            }
            else
            {
                current = current->next;
            }
        }
        q->size--;
    }
}

Page *queuePop(queueMemory *q)
{
    Page *pageNum;
    pageNum = malloc(sizeof(Page));

    if (q->head == NULL)
        return NULL;

    queueNode *tmp = q->head;

    q->head = q->head->next;

    if (q->head == NULL)
        q->tail = NULL;

    pageNum = tmp->pg;
    free(tmp);

    q->size--;

    return pageNum;
}

int getProcessId(queueMemory *q)
{

    queueNode *tmp = malloc(sizeof(queueNode));
    tmp = q->head;

    return tmp->pg->processID;
}

void updateNode(queueMemory *q, unsigned int PageNumber)
{
    queueNode *tmp = malloc(sizeof(queueNode));
    tmp = q->head;

    while (tmp != NULL)
    {
        if (tmp->pg->page_number == PageNumber)
        {
            tmp->pg->R = 1;
        }
        tmp = tmp->next;
    }
}