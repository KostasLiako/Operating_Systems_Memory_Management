#include <time.h>
#define HASH_SIZE 10

typedef struct Page{
    int processID;
    unsigned int page_number;
    char mode;
    int count, R;
    struct Page* next;
} Page;

typedef struct page_table{
    int page_faults, reads, writes, disk_writes, maxFrames , numOfpages;
    char *algor;
    Page* array[HASH_SIZE];
}HashPageTable;

typedef struct queueNode{
    Page* pg;
    struct queueNode* next;
}queueNode;

typedef struct queueMemory{
    int size;
    queueNode* head;
    queueNode* tail;
}queueMemory;



void initializeTable(HashPageTable **pt, int maxFrames, char *algor);
queueMemory* initQueue();
void simulation(HashPageTable *pt,HashPageTable* pt2, unsigned int PageNumber, char mode, int c,int size, int prId,int frames,queueMemory* q);
int PageExists(queueMemory* q, unsigned int PageNumber);
void insertPage(HashPageTable *pt, unsigned int PageNumber, char mode, int c , int size, int prId);
Page* findVictim(HashPageTable *pt,queueMemory* q,int c);
void replaceVictim(HashPageTable *pt,HashPageTable* pt2,queueMemory* q,unsigned int PageNumber, char mode, int c ,int victim, int size, int prId,char vmode,int victimId);
void updatePage(HashPageTable *pt,queueMemory* q, unsigned int PageNumber, char mode, int c, int size, int prId);
void increase(HashPageTable *pt, char mode);
void printStats(HashPageTable *pt,int maxTraces,int frames);
int hashFunc(int key, int size);
void printTable(HashPageTable* pt, int size);
int availableFrame(queueMemory* q,int frame);
void deletePage(HashPageTable* pt, int PageNumber,int size);
char getVictimMode(queueMemory* q,int VictmNumber);
void insertQueue(queueMemory* q,unsigned int PageNumber, char mode, int c,int prId);
void deleteNode(queueMemory* q,unsigned int PageNumber);
void printfQueue(queueMemory* q);
Page* queuePop(queueMemory* q);
int getProcessId(queueMemory* q);
void updateNode(queueMemory* q,unsigned int PageNumber);