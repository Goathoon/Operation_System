#define _CRT_SECURE_NO_WARNINGS
#define MAXSIZE 10000

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct process
{
    int pid;
    int arrival_time;
    int init_queue;
    int number_cycles;
    int *time_arr;
    int time_index;
    int BT;
    int WT;
    int IO;
    int cpu_alloc;
} process;

typedef struct Node
{
    process process_in_q;
    struct Node *next;
} Node;

typedef struct Queue
{
    Node *front;
    Node *rear;
    int count;
} Queue;

typedef struct priority_queue
{
    process heap[MAXSIZE];
    int size;
} priority_queue;

//---------------- scheduling 관련 함수들 ------------------------
// process별 input 정보를 읽어와서 각 프로세스(구조체) 배열을 불러와 값을 저장하는 함수
void ReadProcess(FILE *filename, int iteration, process *arr);
// process의 cpu time, I/O time 을 1초씩 갱신하는 함수 -> CPU 진행중이면 0, 종료되면 1 반환, I/O로 전환되면 2, I/O에서 CPU로 전환되면 3, I/O진행중이면 4
int UpdateProcess(process *be_update_process);
/* time quantum이 끝나거나, 프로그램이 실행 다 되었거나, I/O가 끝날때
큐를 들여다보고 다음 프로세스를 CPU에 할당해주는 함수 */
int SelectProcess(Queue *q0, Queue *q1, priority_queue *q2);
//해당 큐에 timequantum을 알려주는 함수
int WhatIsTimeQuantum(int queue_number);

// --------------- ready queue 작성에 해당하는 함수들-------------------------
void InitQueue(Queue *queue);
int IsEmpty(Queue *queue);
void PushQueue(Queue *queue, process prc);
process PopQueue(Queue *queue);
void InitPq(priority_queue *pq);
process PopPriorityQueue(priority_queue *pq);
void PushPriorityQueue(priority_queue *pq, process prc);
void swap(process *a, process *b);
//-------------------------------------------------------------------------

int WhatIsTimeQuantum(int queue_number)
{
    if (queue_number == 0)
    {
        return 2;
    }
    else if (queue_number == 1)
    {
        return 4;
    }
    else
    {
        return 987654321;
    }
}
int SelectProcess(Queue *q0, Queue *q1, priority_queue *q2)
{
    if (q0->count != 0)
    {
        return 0;
    }
    else if (q1->count != 0)
    {
        return 1;
    }
    else if (q2->size != 0)
    {
        return 2;
    }
    else
    {
        return -1;
    }
};

void InitQueue(Queue *queue)
{
    queue->front = queue->rear = NULL;
    queue->count = 0;
};
void InitPq(priority_queue *pq)
{
    pq->size = 0;
}
int IsEmpty(Queue *queue)
{
    return queue->count == 0;
};

void PushQueue(Queue *queue, process prc) //역방향으로 연결됨
{
    Node *NewNode = (Node *)malloc(sizeof(Node));
    NewNode->process_in_q = prc;
    if (IsEmpty(queue))
    {
        queue->front = NewNode;
    }
    else
    {
        queue->rear->next = NewNode;
    }
    queue->rear = NewNode;
    queue->count++;
};

process PopQueue(Queue *queue)
{
    process pop_process;
    Node *pop_node;

    if (IsEmpty(queue))
    {
        exit;
    }
    pop_node = queue->front;
    pop_process = pop_node->process_in_q;
    queue->front = pop_node->next;
    free(pop_node);
    queue->count--;
    return pop_process;
}

void swap(process *a, process *b)
{
    process temp = *a;
    *a = *b;
    *b = temp;
};
void PushPriorityQueue(priority_queue *pq, process prc)
{
    if (pq->size >= MAXSIZE)
        return;
    int now_size = pq->size;
    pq->heap[now_size] = prc;
    int parent = (now_size - 1) / 2;

    while (now_size > 0 && pq->heap[now_size].time_arr[pq->heap[now_size].time_index] < pq->heap[parent].time_arr[pq->heap[parent].time_index])
    {
        swap(&pq->heap[now_size], &pq->heap[parent]);
        now_size = parent;
        parent = (now_size - 1) / 2;
    }
    pq->size++;
};

process PopPriorityQueue(priority_queue *pq)
{
    if (pq->size <= 0)
    {
        exit;
    }
    process pop_prc = pq->heap[0];
    if (pq->size == 1)
    {
        pq->size--;
        return pop_prc;
    }
    pq->size--;
    pq->heap[0] = pq->heap[pq->size];
    int now = 0;
    int left = 1;
    int right = 2;

    int changed_prc_index;
    while (now <= pq->size)
    {
        changed_prc_index = now;
        if (left <= pq->size && pq->heap[now].time_arr[pq->heap[now].time_index] >
                                    pq->heap[left].time_arr[pq->heap[left].time_index])
            changed_prc_index = left;
        if (right <= pq->size && pq->heap[now].time_arr[pq->heap[now].time_index] >
                                     pq->heap[right].time_arr[pq->heap[right].time_index])
            changed_prc_index = right;
        if (changed_prc_index == now)
            break;
        swap(&pq->heap[now], &pq->heap[changed_prc_index]);
        now = changed_prc_index;
        left = now * 2 + 1;
        right = now * 2 + 2;
    }
    return pop_prc;
};

void ReadProcess(FILE *filename, int iteration, process *arr)
{
    for (int i = 0; i < iteration; i++)
    {
        fscanf(filename, "%d %d %d %d", &arr[i].pid, &arr[i].arrival_time, &arr[i].init_queue, &arr[i].number_cycles);
        arr[i].time_arr = (int *)malloc(sizeof(int) * (arr[i].number_cycles * 2 - 1));
        for (int j = 0; j < arr[i].number_cycles * 2 - 1; j++)
        {
            fscanf(filename, "%d", &arr[i].time_arr[j]);
        };
        arr[i].time_index = 0;
        arr[i].BT = 0;
        arr[i].WT = 0;
        arr[i].IO = 0;
        arr[i].cpu_alloc = 0;
    };
};

int UpdateProcess(process *be_update_process) // 1초마다 갱신
{
    if (be_update_process->cpu_alloc == 1)
    {
        if (be_update_process->number_cycles * 2 - 1 >= (be_update_process->time_index) + 1)
        {
            //마지막 burst time실행시, 종료되었는지 확인해야함.
            be_update_process->time_arr[be_update_process->time_index] -= 1;
            be_update_process->BT++;
            if (be_update_process->time_arr[be_update_process->time_index] == 0) // cpu time 하나가 끝났음. ->마지막인지 체크
            {
                if (be_update_process->number_cycles * 2 - 1 == be_update_process->time_index + 1)
                { //마지막 time burst 였으므로 종료
                    be_update_process->cpu_alloc = 100;
                    return 1; // 다음프로세스 선택은 main함수에서 진행
                }
                be_update_process->time_index++; //하나의 burst time이 종료되었으므로, time_index 옮김
                //이후에는 i/o에 진입한다. cpu_alloc 해제
                be_update_process->cpu_alloc = 3; // 0이면 arrival time 안올라간 친구, 1이면 cpu 할당중인 친구, 2이면 queue 에 올라갔는데 cpu 할당안된친구. 3이면 i/o진행중인 친구.
                return 2;
            }

            return 0; //종료도 안하고, 해당 종류의 time이 계속 진행중 -> timequantum 체크는 메인함수에서.
        }
    }
    else if (be_update_process->cpu_alloc == 2)
    {
        be_update_process->WT += 1;
        return 5;
    }

    else if (be_update_process->cpu_alloc == 3)
    {
        be_update_process->time_arr[be_update_process->time_index] -= 1;
        be_update_process->IO += 1;
        if (be_update_process->time_arr[be_update_process->time_index] == 0) // I/O 종료
        {
            be_update_process->time_index++; //하나의 burst time이 종료되었으므로, time_index 옮김
            be_update_process->cpu_alloc = 2;
            return 3;
        }
        return 4;
    }
    else
    {
        return 100;
    }
}

int main()
{
    int number_process;
    int scheduling_time = 0;

    FILE *test_file = fopen("input_5.txt", "r");
    if (test_file == NULL)
    {
        printf("no such file exit.");
        return 0;
    }
    fscanf(test_file, "%d", &number_process);

    struct process processes[number_process];
    ReadProcess(test_file, number_process, processes);

    //큐 규칙: 큐에서 꺼낼때는 해당 프로세스가 시작 될 때
    Queue q0;
    Queue q1;
    priority_queue q2;
    InitQueue(&q0);
    InitQueue(&q1);
    InitPq(&q2);

    int process_index = -1;  // arrival_time이 순서대로 정렬된 processes 배열이므로, 처음에 0으로 설정
    int process_runtime = 0; //각 프로세스가 실행된 시간 (quantum 마다 초기화)
    int time_quantum = -1;   // q0, q1의 timequantum 설정 변수.
    int process_state = -1;  //각각의 프로세스의 상태(io 인지, 아직 cpu에 할당 안되어있는지 판단)
    int end_process_num = 0;
    int now_queue_num = -1;
    process now_process;
    int new_process_in = 0; //새로운 프로세스가 등록되어야 하는지 여부를 검사
    int time = 0;
    int idle_time = -1;      // CPU가 노는 시간
    int while_idle_push = 0; // idle한 상태에서 동시간 대에 새로운 프로세스가 등록되었는지 확인하는 변수
    int already_print = 0;   // Gantt 차트 프린트시, 이미 프린트한 거 처리하는 변수.
    while (1)
    { // scheduling time을 증가시키는 중.
        //<while 초반 if문>======================================================
        if (processes[process_index + 1].arrival_time == time)
        { // time 증가시키면서 해당 time과 동일한 arrival time이 존재하면 바로 큐에 삽입해야함.

            //프로세스들을 init_queue 정보에 따라 큐 정하고 push
            if (process_index == -1)
            { //처음프로세스는 바로 실행해야함.
                printf("START   -----\n");
                processes[process_index + 1].cpu_alloc = 1;
                if (processes[process_index + 1].init_queue == 2)
                {
                    PushPriorityQueue(&q2, processes[process_index + 1]);
                }
                else
                {
                    if (processes[process_index + 1].init_queue == 1)
                    {
                        PushQueue(&q1, processes[process_index + 1]);
                    }
                    else
                    {
                        PushQueue(&q0, processes[process_index + 1]);
                    }
                }
                now_queue_num = SelectProcess(&q0, &q1, &q2);
                if (now_queue_num == 2)
                {
                    now_process = PopPriorityQueue(&q2); // cpu alloc 설정 다 되어있는친구
                }
                else if (now_queue_num == 1)
                {
                    now_process = PopQueue(&q1); // cpu alloc 설정 다 되어있는친구
                }
                else
                {
                    now_process = PopQueue(&q0); // cpu alloc 설정 다 되어있는친구
                }
                printf("TIME %d, | %d | <- process_%d in \n", time, now_process.pid, now_process.pid);
                already_print = 1;
            }
            else
            {
                if (processes[process_index + 1].init_queue == 2)
                {
                    processes[process_index + 1].cpu_alloc = 2;
                    PushPriorityQueue(&q2, processes[process_index + 1]);
                    if (now_queue_num != -1) //실행할 프로세스가 있는 경우에.
                    {
                        if (now_process.time_arr[now_process.time_index] > q2.heap[0].time_arr[q2.heap[0].time_index])
                        {
                            //현재 q2에서 실행중인 프로세스보다 짧은 cpu time을 가진 프로세스가 들어오면 그걸로 교체
                            now_process.cpu_alloc = 2; //기존 프로세스 cpu 할당 빼앗고,
                            processes[now_process.pid - 1].cpu_alloc = 2;
                            PushPriorityQueue(&q2, now_process); //그상태로 다시 q2에 삽입(preemption)
                            now_process = PopPriorityQueue(&q2);
                            process_runtime = 0;
                            processes[process_index + 1].cpu_alloc = 1;
                            printf("TIME %d, | %d | <- process_%d in \n", time, now_process.pid, now_process.pid);
                            already_print = 1;
                        }
                        else
                        {
                            printf("TIME %d, | %d | <- process_%d in \n", time, now_process.pid, processes[process_index + 1].pid);
                            already_print = 1;
                        }
                    }
                    else //그저 pop하면 됨. 실행프로세스 없으므로, push한거 그대로 pop
                    {
                        now_process = PopPriorityQueue(&q2);
                        now_process.cpu_alloc = 1;
                        now_process.init_queue = 2;
                        processes[now_process.pid - 1].cpu_alloc = 1;
                        idle_time = -1;
                        while_idle_push = 1;
                        printf("TIME %d, | %d | <- process_%d in \n", time, now_process.pid, now_process.pid);
                        already_print = 1;
                    }
                }
                else
                {
                    if (now_queue_num != -1)
                    {
                        processes[process_index + 1].cpu_alloc = 2;
                        if (processes[process_index + 1].init_queue == 1)
                        {
                            PushQueue(&q1, processes[process_index + 1]);
                        }
                        else
                        {
                            PushQueue(&q0, processes[process_index + 1]);
                        }
                        printf("TIME %d, | %d | <- process_%d in \n", time, now_process.pid, processes[process_index + 1].pid);
                        already_print = 1;
                    }
                    else
                    {
                        idle_time = -1;
                        now_process = processes[process_index + 1];
                        now_process.cpu_alloc = 1;
                        now_queue_num = now_process.init_queue;
                        processes[process_index + 1].cpu_alloc = 1;
                        while_idle_push = 1;
                        printf("TIME %d, | %d | <- process_%d in \n", time, now_process.pid, now_process.pid);
                        already_print = 1;
                    }
                }
            }
            process_index += 1;
        }
        //다음 코드들은 time quantum이 끝났는지 혹은 I/O 과정을 하는지 확인하고,
        //혹시나 이러한 상황이라면, 다음 cpu에 할당될 프로세스를 선택해야한다.
        //=================================================================
        //<if문이 끝나고>==============================================
        new_process_in = 0;
        if (idle_time == 0 && while_idle_push == 1)
        {
            idle_time = -1; //동시간대에 새로운 프로세스가 들어왔으므로 idle_time을 -1로 조정
            new_process_in = 0;
            while_idle_push = 0;
            process_runtime = 1;
        }
        else if (idle_time != -1 && while_idle_push == 0)
        { // idle한 상태이고, 그 때 새로 등록된 프로세스도 없다.
            idle_time++;
            new_process_in = 1; //새로 프로세스가 등록되어야하는 상태
            printf("TIME %d, | x |   = means 'idle'\n", time);
        }
        else if (idle_time != -1 && while_idle_push == 1)
        {
            idle_time = -1;
            while_idle_push = 0;
            printf("TIME %d, | x |   = means 'idle'\n", time);
        }
        else
        { // idle_time 이 -1 즉, idle한 상태가 아닐때
            new_process_in = 0;
            process_runtime++;
            if (already_print != 1)
            {
                printf("TIME %d, | %d |   '\n", time, now_process.pid);
            }
            already_print = 0;
            while_idle_push = 0;
        }
        time++;

        //===========================================================================================
        // ready 상태에 돌입한 상태가 한번이라도 있었던 프로세스들만 반복문돌려서 상태파악.
        //<for문을 돌면서 arrived 된 프로세스 정보들 상태 조회하며 Push Pop 큐 이동, 새로운 프로세스 선택 등의 상태를 파악>
        for (int running_process_index = process_index; running_process_index >= 0; running_process_index--)
        { //종료된 프로세스는 update 안함. 이를 프로세스(구조체)별 cpu_alloc 변수로 확인
            if (processes[running_process_index].cpu_alloc == 100)
            {
                continue;
            }
            process_state = UpdateProcess(&(processes[running_process_index]));

            //프로세스별 process_state 상태에 따라 조절함.
            if (process_state == 0)
            {
                if (processes[running_process_index].init_queue != 2)
                {
                    if (process_runtime >= WhatIsTimeQuantum(now_process.init_queue)) // timequantum을 넘어간 순간
                    {
                        now_process.cpu_alloc = 2;
                        now_process.init_queue += 1; // time quantum 초과이므로, 다음 하위 큐에 삽입시키기위해 init_queue를 조정
                        if (now_process.init_queue == 3)
                            now_process.init_queue = 2;
                        processes[now_process.pid - 1].init_queue = now_process.init_queue; // 큐정보 갱신
                        if (now_process.init_queue == 2)
                        {
                            PushPriorityQueue(&q2, now_process);
                        }
                        else
                        {
                            PushQueue(&q1, now_process);
                        }
                        processes[now_process.pid - 1].cpu_alloc = 2;
                        process_runtime = 0;
                        new_process_in = 1; //새로운 프로세스가 등록되어야함
                    }
                }
            }
            else if (process_state == 1)
            { //종료된 프로세스 수 세기
                end_process_num++;
                if (end_process_num == number_process) //모든 프로세스가 끝나면 종료
                {
                    printf("FINISH  ------\n");
                    int average_tt = 0;
                    int average_wt = 0;
                    for (int i = 0; i < number_process; i++)
                    {
                        printf("PID_%d : TT = %d, WT = %d\n", processes[i].pid, processes[i].BT + processes[i].WT + processes[i].IO, processes[i].WT);
                        average_tt += processes[i].BT + processes[i].WT + processes[i].IO;
                        average_wt += processes[i].WT;
                    }
                    printf("AVERAGE OF TT : %lf, AVERAGE OF WT : %lf", (float)average_tt / (float)number_process, (float)average_wt / (float)number_process);
                    return 0;
                }
                process_runtime = 0;
                new_process_in = 1; //새로운프로세스가 등록되어야함.
            }
            else if (process_state == 2)
            {
                process_runtime = 0;
                new_process_in = 1; //새로운 프로세스가 등록되어야함.
            }
            else if (process_state == 3)
            {
                processes[running_process_index].init_queue -= 1;
                if (processes[running_process_index].init_queue == -1)
                {
                    processes[running_process_index].init_queue = 0;
                }

                if (processes[running_process_index].init_queue == 0)
                    PushQueue(&q0, processes[running_process_index]);
                else
                {
                    PushQueue(&q1, processes[running_process_index]);
                }
                idle_time = -1; // wake up 했으므로, 큐에 삽입됨 ->따라서 idle한 상태가 아닐것임.
            }
        }
        //=============================================================================
        //<for문이 종료된 후에 new_process_in이 1인 경우를 판단>=====================
        if (new_process_in == 1)
        {
            //새로운 프로세스 cpu할당 now_process를 실제 processes 배열에서 해당 프로세스 찾고 cpu_alloc 변경
            now_queue_num = SelectProcess(&q0, &q1, &q2);
            if (now_queue_num == 2)
            {
                now_process = PopPriorityQueue(&q2);
            }
            else if (now_queue_num == -1)
            {
                idle_time = 0;
                continue;
            }
            else
            {
                if (now_queue_num == 0)
                    now_process = PopQueue(&q0);
                else
                {
                    now_process = PopQueue(&q1);
                }
            }
            now_process.cpu_alloc = 1;
            processes[now_process.pid - 1].cpu_alloc = 1;
        }
    }
}
