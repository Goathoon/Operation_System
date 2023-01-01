#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    FILE *test_file = fopen("input.txt", "r");
    if (test_file == NULL)
    {
        printf("no such file exit.");
        return 0;
    }

    int process_number = 0;  // 총 프로세스의 개수
    int resource_number = 0; // 총 자원 개수

    if (fscanf(test_file, "%d", &process_number) < 0) // 먼저 프로세스의 개수와 자원 개수를 scanf로 받아온다.
    {
        printf("Invalid input!");
        return 1;
    }
    if (fscanf(test_file, "%d", &resource_number) < 0)
    {
        printf("Invalid input!");
        return 1;
    }
    if (process_number <= 0 || resource_number <= 0)
    {
        printf("Invalid Input!");
        return 1;
    }
    int *resource_array = (int *)malloc(sizeof(int) * resource_number); // resource_array라는 각 resource 별 unit의 개수를 담을 배열 선언

    for (int i = 0; i < resource_number; i++) // 먼저 총 resource별 unit들의 개수를 저장.
    {
        if (fscanf(test_file, "%d", &resource_array[i]) < 0)
        {
            printf("Invalid input!");
            return 1;
        }
        if (resource_array[i] < 0)
        {
            printf("Invalid Input !");
            return 1;
        }
    }

    // process 별로 resource에 대한 요구 개수와 현재 할당 개수, 필요 개수를 담을 struct 구조체를 선언
    typedef struct _process
    {
        int max_claim[process_number];
        int cur_alloc[process_number];
        int need_alloc[process_number];
        int done; // 해당 프로세스가 자원 할당받고 끝났는지 보는 변수.
    } process;

    //----------------- 프로세스 구조체 배열에 해당하는 정보들을 저장 ---------------------------//
    process *processes = (process *)malloc(sizeof(process) * process_number); // 프로세스의 개수만큼 processes라는 프로세스를 담은 구조체 배열 선언

    for (int i = 0; i < process_number; i++) // max_claim 을 프로세스별로 저장하기
    {
        for (int j = 0; j < resource_number; j++)
        {
            if (fscanf(test_file, "%d", &processes[i].max_claim[j]) < 0)
            {
                printf("Invalid input!");
                return 1;
            }
            if (processes[i].max_claim[j] < 0)
            {
                printf("Invalid Input!");
                return 1;
            }
        }
    }

    for (int i = 0; i < process_number; i++) // cur_alloc을 프로세스별로 저장하기
    {
        for (int j = 0; j < resource_number; j++)
        {
            if (fscanf(test_file, "%d", &processes[i].cur_alloc[j]) < 0)
            {
                printf("Invalid input!");
                return 1;
            }
            if (processes[i].cur_alloc[j] < 0)
            {
                printf("Invalid Input!");
                return 1;
            }
        }
    }

    for (int i = 0; i < process_number; i++) // 프로세스 별로 할당 필요한 unit의 개수 배열 저장하기
    {
        for (int j = 0; j < resource_number; j++)
        {
            processes[i].need_alloc[j] = processes[i].max_claim[j] - processes[i].cur_alloc[j];
            if (processes[i].need_alloc[j] < 0)
            {
                printf("Invalid Input ! \n Process (%d)'s current allocation is over the max claim of input", i + 1); // 만약 할당된 자원개수가 최대 개수를 넘으면 출력하는 오류메세지
                return 1;
            }
        }
    }
    for (int i = 0; i < process_number; i++) // 프로세스별 종료여부 done변수 0으로 초기화.
    {
        processes[i].done = 0;
    }
    //---------------------------------------------------------------------------------//

    //--------------------------- deadlock 탐지 ------------------------------------//
    int isDeadlock[process_number]; // 프로세스별로 deadlock인지 아닌지 판단하는 변수 1이면 deadlock상태임.
    memset(isDeadlock, 0, process_number * sizeof(int));
    int done_process = 0;                                   // 종료된 프로세스가 몇개인지 세는 변수, 해당 변수가 총 프로세스 개수와 같으면 deadlock이 아님.
    int stack_process[process_number];                      // 마지막 safe sequence를 위한 배열을 선언, 맨 앞에서부터 출력할 예정
    memset(stack_process, 0, process_number * sizeof(int)); // 해당 배열을 다 0으로 초기화
    int end_process_index = 0;                              // stack_process의 인덱스
    // 1. 먼저 이용가능한 자원의 unit의 개수를 저장
    int unit_alloc = 0; // 각 자원 type 별 이미 할당된 unit의 개수 저장 변수
    for (int i = 0; i < resource_number; i++)
    {
        for (int j = 0; j < process_number; j++)
        {
            unit_alloc += processes[j].cur_alloc[i];
        }
        resource_array[i] = resource_array[i] - unit_alloc;
        unit_alloc = 0;
    }
    // 그 이후 available 한 자원내에서 할당할 수 있는 프로세스 탐지
    // 특정 기준 없이 1번째 프로세스부터 n번째 프로세스까지 돌면서, 할당가능하면 바로 자원 할당하는 방식으로 진행
    int index = 0;
    while (index < process_number) // 만일 i가 process_number보다 넘어간다는 것은 프로세스가 요구하는 자원을 현재 이용가능한 자원내에서 할당하지 못한다는
    // 뜻으로, deadlock상태임을 의미한다. (모두 종료된 상태는 while문 안에서 검사한다)
    {
        if (processes[index].done == 0) // 아직 안끝난 프로세스만 판단
        {
            for (int j = 0; j < resource_number; j++)
            {
                if (processes[index].need_alloc[j] > resource_array[j]) // 필요개수가 할당 가능 자원보다 많으면 넘어가야함.
                {
                    isDeadlock[index] = 1; // 해당 프로세스가 현재 상태에서 deadlock임을 알림
                    break;
                }
            }
            if (isDeadlock[index] != 1) // deadlock이 아닌 경우에만 다음을 실행 deadlock이면 다음 프로세스로 넘어가는 반복문을 거쳐야함.
            {
                // 위 과정을 모두 거친 후라는 것은, 현재 자원의 개수가 해당 프로세스의 요구 자원을 충족한다는 뜻이다.
                // 따라서 해당 프로세스에게 바로 자원을 할당하고 종료될 때 자원의 해당 프로세스의 할당 자원을 현재 자원의 개수에 추가해야한다.
                for (int k = 0; k < resource_number; k++)
                {
                    resource_array[k] += processes[index].cur_alloc[k];
                }
                processes[index].done = 1; // 그후 해당 프로세스 종료시킴.
                done_process++;
                stack_process[end_process_index] = index + 1;        // 종료된 프로세스의 번호를 stack_process(종료된 프로세스를 담은 배열)에 저장
                end_process_index++;                                 // 종료 프로세스를 순서대로 저장하는 배열의 인덱스 증가
                index = 0;                                           // 프로세스들을 저장한 배열의 인덱스 초기화
                memset(isDeadlock, 0, process_number * sizeof(int)); // deadlock 상태체크하는 배열 초기화

                if (done_process == process_number) // 모두 종료되었는지 체크
                {
                    printf("It is safe state\n");
                    // ------------ safe seuqence 출력 부분 ---------------------//
                    int length = sizeof(stack_process) / sizeof(int);
                    printf("Safe Sequence :");
                    for (int i = 0; i < length; i++)
                    {
                        if (i == length - 1)
                        {
                            printf(" P%d.", stack_process[i]);
                            break;
                        }
                        printf(" P%d ->", stack_process[i]);
                    }
                    fclose(test_file);
                    free(resource_array);
                    free(processes);

                    return 0;
                }
            }
            else // 해당 프로세스가 deadlock일 경우 다음 인덱스로 넘어가야함.
            {
                index++;
            }
        }
        else // 해당 프로세스 종료시 프로세스 배열 인덱스 증가
        {
            index++;
        }
    }
    //---------- unsafe 상태 출력 -----------------//
    printf("It is unsafe state\n");
    printf("Because there are deadlock processes :");
    for (int i = 0; i < process_number; i++)
    {
        if (isDeadlock[i] == 1)
        {
            printf(" P%d", i + 1);
        }
    }
    fclose(test_file);
    free(resource_array);
    free(processes);
    return 0;
}
