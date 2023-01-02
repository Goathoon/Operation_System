#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

int page_number = 0;
int pageframe_number = 0;
int window_size = 0;
int refstring_length = 0;

int fault_count = 0;
int empty_index = 0;

//--------------Working Set 구현을 위한 큐 선언 ------------------//
typedef struct Node
{
    int page;
    struct Node *next;
} Node;

typedef struct Queue
{
    Node *front;
    Node *rear;
    int count;
} Queue;

void InitQueue(Queue *queue);
int isEmpty(Queue *queue);
void enqueue(Queue *queue, int refpage);
void dequeue(Queue *queue);
void print_WS(Queue *queue);

//--------------Working Set 구현을 위한 큐 선언 끝------------------//

int MIN(int *memory, int *refstring, int *page_info);
int LRU(int *memory, int *refstring, int *page_info);
int LFU(int *memory, int *refstring, int *page_info, int *tie);
int WS(int *refstring, int window_size);
int check(int *memory, int check_fault);
int return_Big(int *page_info, int *memory);
int return_Big_LFU(int *page_info, int *memory, int *tie);
int do_same(int *memory, int *refstring, int *page_info);
int do_same_LFU(int *memory, int *refstring, int *page_info);

int main()
{
    int buffer_for_error = 0;
    FILE *test_file = fopen("input.txt", "r");

    //---------------- 예외 처리 부분--------------------//
    if (test_file == NULL)
    {
        printf("no such file exit.");
        return 0;
    }

    if (fscanf(test_file, "%d", &page_number) < 0)
    {
        printf("Invalid input! Check your first input in first line");
        return 0;
    }
    if (fscanf(test_file, "%d", &pageframe_number) < 0)
    {
        printf("Invalid input! Check your second input in first line");
        return 0;
    }
    if (fscanf(test_file, "%d", &window_size) < 0)
    {
        printf("Invalid input! Check your third input in first line");
        return 0;
    }
    if (fscanf(test_file, "%d", &refstring_length) < 0)
    {
        printf("Invalid input! Check your fourth input in first line");
        return 0;
    }

    if (page_number < 0 || page_number > 100)
    {
        printf("Invalid input! You must check range of input number");
        return 0;
    }
    if (pageframe_number < 0 || pageframe_number > 20)
    {
        printf("Invalid input! You must check range of input number");
        return 0;
    }
    if (window_size < 0 || window_size > 100)
    {
        printf("Invalid input! You must check range of input number");
        return 0;
    }
    if (refstring_length < 0 || refstring_length > 1000)
    {
        printf("Invalid input! You must check range of input number");
        return 0;
    }

    int *refstring = (int *)malloc(sizeof(int) * refstring_length); // refstring을 배열로 동적할당
    for (int i = 0; i < refstring_length; i++)
    {
        if (fscanf(test_file, "%d", &refstring[i]) < 0)
        {
            printf("Invalid input! You must check refstring input");
            return 0;
        };
        if (refstring[i] >= page_number) // page number보다 큰 ref string이 온 경우 예외처리
        {
            printf("Invalid input! You must check refstring number is below the page number");
            return 0;
        }
    }
    if (!(fscanf(test_file, "%d", &buffer_for_error) < 0)) // 받은 refstring length 보다 긴 refstring이 올 경우 예외 처리
    {
        printf("Invalid input! You must check refstring input's length");
        return 0;
    }
    //---------------- 예외 처리 부분 끝--------------------//

    int *memory = (int *)malloc(sizeof(int) * pageframe_number);
    int *ref_info = (int *)malloc(sizeof(int) * page_number);
    /* ref_info 해당 동적할당 배열은 ref_string으로 받아온 page에 해당하는 정보를 담는 배열로,
    MIN 알고리즘에서는 각 page 별 forward distance의 정보를,
    LRU 알고리즘 에서는 각 page 별 backword distance의 정보를,
    LFU 알고리즘 에서는 각 page 별 참조된 횟수의 정보를 담는다.
    VA방식의 WS memory manage ment에서는 해당 배열을 이용하지 않는다.
    */
    int *tie = (int *)malloc(sizeof(int) * page_number);
    memset(tie, 0, sizeof(int) * page_number);
    // 해당 tie 배열은 LFU상황에서 tie 인 상황이 존재할 때, 해당 page string을 골라내기 위한 배열이다.
    printf("==MIN==\n");
    MIN(memory, refstring, ref_info);
    printf("==LRU==\n");
    LRU(memory, refstring, ref_info);
    printf("==LFU==\n");
    LFU(memory, refstring, ref_info, tie);
    printf("==W S==\n");
    WS(refstring, window_size);
    fclose(test_file);
    free(memory);
    free(ref_info);
    free(tie);
}

int check(int *memory, int check_fault) // residence set에 page가 존재하는지 fault를 체크하는 함수 fault라면 1반환
{
    for (int i = 0; i < pageframe_number; i++)
    {
        if (memory[i] == check_fault)
            return 0;
    }
    return 1;
}
int MIN(int *memory, int *refstring, int *page_info)
{
    // page fault 초기화 과정을 거치고, 정적 할당에서 모든 page frame이 다 차기 전까지는 동일한 과정을 거침.
    // 해당 함수를 do_same() 함수로 정의, 그리고 ref_index즉, refstring의 index를 조절해가며 page ref를 변경함.
    int ref_index = do_same(memory, refstring, page_info);
    // 그 후 해당 알고리즘에 따라 replacement 할 page 고르고 교체
    for (ref_index; ref_index < refstring_length;)
    {

        if (check(memory, refstring[ref_index])) // residence set에 해당 time에 ref된 page가 존재하지않는가?
        {
            for (int mem_index = 0; mem_index < pageframe_number; mem_index++)
            {
                for (int start_index = ref_index; start_index < refstring_length; start_index++)
                {
                    if (memory[mem_index] == refstring[start_index])
                    {
                        page_info[memory[mem_index]] = start_index - ref_index;
                        break;
                        // 메모리에 존재하는 page별
                        // forward distance를 구해서 page_info 배열의 해당 page 번호 index에 저장
                    }
                    // 만일 forward distance를 구할 수 없다면 (후에 해당 string page가 등장하지 않는다면)
                    // 당연히 가장 우선순위로 교체해야할 페이지이다. 따라서 해당 index의 값을 1001로 조정한다.
                    if (start_index == refstring_length - 1)
                    {
                        page_info[memory[mem_index]] = 1001;
                    }
                }
            }
            int replace_index = return_Big(page_info, memory); // residence set에 올라와있는 page 중에 가장 distance가 큰 page 번호 반환.
            memory[replace_index] = refstring[ref_index];
            fault_count++;
            printf("%5d |%11d |%11C |", ref_index + 1, refstring[ref_index], 'F');
        }
        else
        {
            printf("%5d |%11d |%11C |", ref_index + 1, refstring[ref_index], ' ');
        }
        for (int i = 0; i < pageframe_number; i++)
        {
            if (memory[i] == -1)
            {
                break;
            }
            printf("%2d", memory[i]);
        }
        printf("\n");
        ref_index++;
    }

    printf("---- How many page faults occured? : %d ----\n", fault_count);
};

int LRU(int *memory, int *refstring, int *page_info)
{
    // page fault 초기화 과정을 거치고, 정적 할당에서 모든 page frame이 다 차기 전까지는 동일한 과정을 거침.
    // 해당 함수를 do_same() 함수로 정의, 그리고 ref_index즉, refstring의 index를 조절해가며 page ref를 변경함.
    int ref_index = do_same(memory, refstring, page_info);
    // 그 후 해당 알고리즘에 따라 replacement 할 page 고르고 교체
    for (ref_index; ref_index < refstring_length;)
    {
        if (check(memory, refstring[ref_index])) // residence set에 해당 time에 ref된 page가 존재하지않는가?
        {
            for (int mem_index = 0; mem_index < pageframe_number; mem_index++)
            {
                for (int start_index = ref_index; start_index >= 0; start_index--) // backward distance를 고려해야함.
                {
                    if (memory[mem_index] == refstring[start_index])
                    {
                        page_info[memory[mem_index]] = ref_index - start_index;
                        break;
                        // 메모리에 존재하는 page별
                        // forward distance를 구해서 page_info 배열의 해당 page 번호 index에 저장
                    }
                    // 만일 backward distance를 구할 수 없다면 (이전에 해당 string page가 등장하지 않는다면 즉, 지금이 첫 등장이라면)
                    // 당연히 가장 우선순위로 교체해야할 페이지이다. 따라서 해당 index의 값을 1001로 조정한다.
                    if (start_index == 0)
                    {
                        page_info[memory[mem_index]] = 1001;
                    }
                }
            }
            int replace_index = return_Big(page_info, memory); // residence set에 올라와있는 page 중에 가장 distance가 큰 page 번호 반환.
            memory[replace_index] = refstring[ref_index];
            fault_count++;
            printf("%5d |%11d |%11C |", ref_index + 1, refstring[ref_index], 'F');
        }
        else
        {
            printf("%5d |%11d |%11C |", ref_index + 1, refstring[ref_index], ' ');
        }
        for (int i = 0; i < pageframe_number; i++)
        {
            if (memory[i] == -1)
            {
                break;
            }
            printf("%2d", memory[i]);
        }
        printf("\n");
        ref_index++;
    }

    printf("---- How many page faults occured? : %d ----\n", fault_count);
};

int LFU(int *memory, int *refstring, int *page_info, int *tie)
{
    // page fault 초기화 과정을 거치고, 정적 할당에서 모든 page frame이 다 차기 전까지는 동일한 과정을 거침.
    // 해당 함수를 do_same() 함수로 정의, 그리고 ref_index즉, refstring의 index를 조절해가며 page ref를 변경함.
    // 하지만 LFU 는 처음부터 각 ref string page 별 참조 횟수를 세어야 하므로, 같은 do_same 함수에 해당 과정을 추가한 함수를 정의함.
    int ref_index = do_same_LFU(memory, refstring, page_info);

    // 그 후 해당 알고리즘에 따라 replacement 할 page 고르고 교체
    for (ref_index; ref_index < refstring_length;)
    {
        page_info[refstring[ref_index]]++;       // 해당 ref string page 참조 횟수 증가
        if (check(memory, refstring[ref_index])) // residence set에 해당 time에 ref된 page가 존재하지않는가?
        {
            int replace_index = return_Big_LFU(page_info, memory, tie); // residence set에 올라와있는 page 중에 가장 distance가 큰 page 번호 반환.
            if (replace_index == -1)                                    // 만약 tie가 발생한다면, ref string 순서의 가장 앞순서에 존재하는 해당 tie 발생한 string을 교체하면 된다.
            {                                                           // 해당 과정을 위해 tie 배열을 활용하여 LFU과정을 거침
                for (int i = 0; i < page_number; i++)
                {
                    if (tie[i] == 1) // 인덱스가 곧 count가 중복된 ref string이다.
                    {
                        int j = ref_index - 1;
                        for (j; j >= 0; j--)
                        {
                            if (refstring[j] == i)
                            {
                                tie[i] = ref_index - j; // 값이 클수록 LRU임.
                                break;
                            }
                        }
                    }
                }
                int max_tie = 0;
                for (int i = 0; i < page_number; i++)
                {
                    if (tie[i] == 0)
                        continue;
                    max_tie = tie[i] > tie[max_tie] ? i : max_tie;
                    for (int m = 0; m < pageframe_number; m++)
                    {
                        if (memory[m] == max_tie)
                        {
                            replace_index = m;
                        }
                    }
                }
            }
            memory[replace_index] = refstring[ref_index];
            fault_count++;
            printf("%5d |%11d |%11C |", ref_index + 1, refstring[ref_index], 'F');
        }
        else
        {
            printf("%5d |%11d |%11C |", ref_index + 1, refstring[ref_index], ' ');
        }
        for (int i = 0; i < pageframe_number; i++)
        {
            if (memory[i] == -1)
            {
                break;
            }
            printf("%2d", memory[i]);
        }
        printf("\n");
        ref_index++;
    }

    printf("---- How many page faults occured? : %d ----\n", fault_count);
};

int WS(int *refstring, int window_size)
{
    fault_count = 0;
    Queue queue;     // residence set을 곧 queue 자료 구조라고 가정했음.
    int isFault = 0; // page fault가 존재하는지 판단하는 변수.
    queue.front = queue.rear = NULL;
    queue.count = 0;
    printf(" Time | Ref_string | Page fault | Residence set\n");
    if (isEmpty(&queue))
    {
        enqueue(&queue, refstring[0]);
        printf("%5d |%11d |%11C |", 1, refstring[0], 'F');
        fault_count++;
        print_WS(&queue);
    }
    Node *ptr;
    Node *preptr = NULL;

    for (int ref_index = 1; ref_index < refstring_length; ref_index++)
    {
        isFault = 0;
        // queue 들어간 (residence set에 존재하는) 페이지 중 ref된 page가 있는지 검사.
        ptr = queue.front;
        int isRemove = ptr->page;
        if (ref_index > window_size)
        {
            int index = ref_index;
            for (index; index >= ref_index - window_size; index--)
            {
                if (isRemove == refstring[index])
                    break;
                else
                {
                    if (index == ref_index - window_size)
                    {
                        dequeue(&queue);
                        break;
                    }
                }
            }
        }
        ptr = queue.front;

        while (1)
        {
            if (ptr->page == refstring[ref_index]) // 이미 memory에 올라간 경우, 해당 page를 pop했다가 다시 push하는 과정을 거쳐야 순서가 유지됨.
            {
                if (ptr == queue.front)
                {
                    dequeue(&queue);
                    enqueue(&queue, refstring[ref_index]);
                }
                else if (ptr != queue.rear)
                { // 바로 이전에 들어온 page라면 상태 유지
                    preptr->next = ptr->next;
                    queue.count--;
                    enqueue(&queue, refstring[ref_index]);
                }
                printf("%5d |%11d |%11C |", ref_index + 1, refstring[ref_index], ' ');
                break;
            }
            else
            {
                if (ptr != queue.front)
                {
                    preptr = preptr->next;
                }
                else
                {
                    preptr = ptr;
                }
                ptr = ptr->next;
                if (ptr == NULL)
                {
                    isFault = 1;
                    break;
                }
            }
        }

        if (isFault && queue.count >= window_size + 1)
        { // Fault가 존재하고, window size +1보다 크므로, replacement 해야함. residence set을 queue구조로 가정했으므로,
            // 가장 앞에있는 node를 제거하면 됨. FIFO와 동일하므로.
            dequeue(&queue);
            enqueue(&queue, refstring[ref_index]);
            printf("%5d |%11d |%11C |", ref_index + 1, refstring[ref_index], 'F');
            fault_count++;
        }
        else if (isFault)
        {
            enqueue(&queue, refstring[ref_index]);
            printf("%5d |%11d |%11C |", ref_index + 1, refstring[ref_index], 'F');

            fault_count++;
        }
        print_WS(&queue);
    }
    printf("---- How many page faults occured? : %d ----\n", fault_count);
    free(ptr);
};
int return_Big(int *page_info, int *memory) // page_info 배열에서 값이 가장 큰 memory index를 반환하는 함수
{
    int max = -1;
    int max_index = 0;
    for (int index = 0; index < pageframe_number; index++)
    {
        if (page_info[memory[index]] > max)
        {
            max = page_info[memory[index]];
            page_info[memory[index]] = -1; //-1 로 초기화
            max_index = index;
        }
        else if (page_info[memory[index]] == max) // tie breaking
        {
            max_index = memory[index] > memory[max_index] ? index : max_index;
        }
    }
    return max_index; // max 값을 가진 residence set의 index 를 반환. Tie-breaking rule은 큰 page number로 잡음.
}

int return_Big_LFU(int *page_info, int *memory, int *tie) // page_info 배열에서 값이 가장 작은 memory index를 반환하는 함수
{
    int min = 1002;
    int min_index = 0;
    int is_Tie = 0; // ref된 횟수가 같은 page 가 존재하는지 여부를 알려주는 변수.
    for (int index = 0; index < pageframe_number; index++)
    {
        if (page_info[memory[index]] < min)
        {
            memset(tie, 0, sizeof(int) * page_number); // 모든 page 정보들을 0으로 초기화.새로운 min이 갱신됨.
            min = page_info[memory[index]];
            tie[memory[index]] = 1;
            min_index = index;
            is_Tie = 0;
        }
        else if (page_info[memory[index]] == min)
        {
            tie[memory[index]] = 1;
            is_Tie = 1;
        }
    }
    if (is_Tie)
    {
        return -1;
    }
    return min_index; // min 값을 가진 residence set의 index 를 반환.
};
int do_same(int *memory, int *refstring, int *page_info)
{

    fault_count = 0;
    empty_index = 0;
    //-----------memory state,page_info 초기화 과정---------------//
    for (int i = 0; i < pageframe_number; i++)
    {
        memory[i] = -1;
    }
    for (int i = 0; i < page_number; i++)
    {
        page_info[i] = -1;
    }
    //-----------memory state,page_info 초기화 과정 끝---------------//

    // refstring 배열을 참조하는 인덱스를 마지막 인덱스까지 증가시키며 알고리즘 구현
    printf(" Time | Ref_string | Page fault | Residence set\n");
    for (int ref_index = 0; ref_index < refstring_length;)
    {
        //------- 비어있는 page frame이 없을 때까지 실행------------//
        if (empty_index < pageframe_number)
        {
            if (check(memory, refstring[ref_index]))
            {
                memory[empty_index] = refstring[ref_index]; // page frame에 해당 ref string page 삽입
                fault_count++;
                printf("%5d |%11d |%11C |", ref_index + 1, refstring[ref_index], 'F');
                empty_index++;
            }
            else
            {
                printf("%5d |%11d |%11C |", ref_index + 1, refstring[ref_index], ' ');
            }
            for (int i = 0; i < pageframe_number; i++)
            {
                if (memory[i] == -1)
                {
                    continue;
                }
                printf("%2d", memory[i]);
            }
            printf("\n");

            ref_index++;
            continue;
        }
        //------- 비어있는 page frame이 없을 때까지 실행 끝------------//
        return ref_index;
    }
};

int do_same_LFU(int *memory, int *refstring, int *page_info)
{

    fault_count = 0;
    empty_index = 0;
    //-----------memory state,page_info 초기화 과정---------------//
    for (int i = 0; i < pageframe_number; i++)
    {
        memory[i] = -1; // 메모리에 들어와 있지 않으면 -1을 저장
    }
    for (int i = 0; i < page_number; i++)
    {
        page_info[i] = 0;
    }
    //-----------memory state,page_info 초기화 과정 끝---------------//

    // refstring 배열을 참조하는 인덱스를 마지막 인덱스까지 증가시키며 알고리즘 구현
    printf(" Time | Ref_string | Page fault | Residence set\n");
    for (int ref_index = 0; ref_index < refstring_length;)
    {
        //------- 비어있는 page frame이 없을 때까지 실행------------//
        if (empty_index < pageframe_number)
        {
            page_info[refstring[ref_index]]++; // 해당 ref string page 참조 횟수 증가
            if (check(memory, refstring[ref_index]))
            {
                memory[empty_index] = refstring[ref_index]; // page frame에 해당 ref string page 삽입
                fault_count++;
                printf("%5d |%11d |%11C |", ref_index + 1, refstring[ref_index], 'F');
                empty_index++;
            }
            else
            {
                printf("%5d |%11d |%11C |", ref_index + 1, refstring[ref_index], ' ');
            }
            for (int i = 0; i < pageframe_number; i++)
            {
                if (memory[i] == -1)
                {
                    continue;
                }
                printf("%2d", memory[i]);
            }
            printf("\n");

            ref_index++;
            continue;
        }
        //------- 비어있는 page frame이 없을 때까지 실행 끝------------//
        return ref_index;
    }
};

void print_WS(Queue *queue)
{
    Node *ptr = queue->front;
    for (int i = 0; i < queue->count; i++)
    {
        printf("%2d", ptr->page);
        ptr = ptr->next;
    }
    printf("\n");
}
void initQueue(Queue *queue)
{
    queue->front = NULL;
    queue->rear = NULL;
    queue->count = 0;
}
int isEmpty(Queue *queue)
{
    return queue->count == 0;
};

void enqueue(Queue *queue, int refpage)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->page = refpage;
    newNode->next = NULL;
    if (isEmpty(queue))
    {
        queue->front = newNode;
    }
    else
    {
        queue->rear->next = newNode;
    }
    queue->rear = newNode;
    queue->count++;
}
void dequeue(Queue *queue)
{
    int page;
    Node *ptr;
    if (isEmpty(queue))
    {
        printf("error \n");
        return;
    }
    ptr = queue->front;
    queue->front = ptr->next;
    free(ptr);
    queue->count--;
}
