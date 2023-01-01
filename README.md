## Project-01 [MFQ Scheduling Simulation]

### 1. 목표 설정
3개의 RQ를 갖는 MFQ의 스케줄링 기법을 구현하는 것이 프로젝트의 목표이다. Q0 와 Q1의 RQ는 RR 스케줄링 기법이고, Q2는 SRTN 스케줄링 기법을 사용하여야 한다. 

### 2. 사용할 자료 구조 및 정의한 구조체

- 자료구조
RR 스케줄링 기법은 프로세스들의 arrival time을 기준으로 CPU가 할당되며, process 실행 시간이 time quantum을 초과하면 CPU에서 preemption된다. 여기서 떠올릴 수 있는 구현의 방법은 ‘Queue’이다. arrival time을 기준으로 하기 때문에, 전체 CPU 실행 시간인 time을 0 부터 증가시키면서, 해당 process의 arrival time이 time과 같아질 때, RR 스케줄링 기법을 사용하는 Queue 자료 구조에 Push하는 방식을 가장 먼저 떠올렸다. 이러면 arrival time 순서대로 정렬되며, First In First Out의 큐의 특징을 그대로 적용시킬 수 있다. RR 스케줄링 기법을 사용하는 큐의 프로세스를 CPU에서 할당하면 그 큐에서 Pop을 시킨다. 그리고, 해당 프로세스가 CPU를 할당 받은 구간마다의 실행시간(process_runtime)을 따로 선언하여 time이 증가할 때, 같이 증가하고, CPU에서 preemption이 되면 process_runtime을 0으로 초기화 해서 다음 프로세스의 process_runtime을 재 측정한다.
SRTN 스케줄링 기법을 사용하는 Q2의 경우는 Shortest Remain Time Next이므로, 들어오는 순서와 상관없이, 사이클 내에 가장 짧은 시간의 CPU burst time을 가지는 프로세스를 먼저 실행시키기 때문에, Priority Queue의 자료구조를 사용했다. 물론 이를 Min-heap 구조로 구현하였으며, 가장 짧은 시간의 프로세스가 root node에 존재하게 구현하였다. 따라서, Q2에 존재하는 프로세스가 CPU에 할당되어 실행되는 경우에, 그것보다 더 짧은 CPU burst time이 Q2에 Push되면(자동으로 root node에 자리매김할 것이다.), 할당중인 프로세스를 다시 Q2에 Push(이미 Pop 되었기 때문)하고, 새롭게 Q2에 들어온 프로세스는 Q2의 root node에 존재하므로 다시 Q2에서 Pop 하면, SRTN을 구현할 수 있다. 

- 정의한 구조체
기본적인 큐와 우선순위 큐에 필요한 노드와 큐, 우선순위 큐를 위한 구조체는 보편적인 구현이므로 생략하겠다. 단지 해당 노드와 heap에 에 직접 정의한 구조체인 process를 넣은 차이만 존재한다.
프로세스마다 PID, Arrival Time, Init Queue, Number Cycles, burst time들의 시간 쌍의 정보들이 input.txt로 주어진다. 이를 프로세스마다 달리 저장하기 위하여, process라는 구조체를 선언했다. 이 구조체 안에 input으로 주어진 정보들을 당연히 정의하였고, 이 외에 선언한 것을 설명하겠다.

1. time_index 
burst time 시간이 쌍으로 주어진 정보를 순서대로 ‘time_arr’ 라는 시간 배열을 선언하여 저장하였는데, 각 time이 끝날 때 다음 time을 어떤 방식으로 넘어가게 할지 궁리했다. 처음에는 stack 자료구조형을 떠올렸는데, 또 stack을 구조체 안에 선언하여 넣는 것 보다 time_index 즉, 배열의 index를 하나씩 올리는 방법으로 하면 더 쉬울 것 같아서 다음의 변수를 선언하였다. 

2. BT, WT, IO
해당 프로젝트의 출력사항에는 프로세스별 Turnaround Time과 Waiting Time을 구현해야만 했다. Turnaround Time을 직접 구현하기에는 조금 복잡할 것 같아서, Turnaround time을 Burst Time + Waiting Time + I/O time을 구간별로 구해서 더하는 쪽을 선택했다.

3. cpu_alloc
해당 변수는 이름이 cpu_alloc이라 CPU로부터 할당 받았는지 아닌지를 알려주는 변수이긴 하지만, 코드를 구현하다 보니 더 기능이 많아 졌다. 다음 변수는 여섯 가지의 상태를 정의한다.
'-1'은 CPU에 할당중인 프로세스가 없는 상황을 나타낸다.
'0'은 아직 도착하지 않은 프로세스를 의미한다.
'1'은 CPU가 현재 할당중인 프로세스를 의미한다.
'2'는 queue에 올라가 있는 ready 상태의 프로세스를 의미한다.
'3'은 I/O burst time 이 진행중인 프로세스를 의미한다.
'100'은 종료된 프로세스를 의미한다.

### 3. 정의한 함수들

Queue 나 Priority Queue 의 구현을 위한 기본적인 함수들은 보편적이므로 설명하지 않겠다. 

1. void ReadProcess(FILE *filename, int iteration, process *arr)
텍스트 파일을 불러와서 해당 파일을 읽어가면서, arr 배열에 input 정보를 입력하는 것이다. 추가로 프로세스의 할당되지 않은 변수에 초기화도 해주었다. 이 때 iteration은 전체 프로세스의 개수일 것이고, arr배열은 process를 원소로 하는 배열이다.

2. int UpdateProcess(process *be_update_process)
이 함수는 매우 중요한 함수이므로 조금 자세히 설명하겠다.
프로세스 개별을 업데이트 하는 함수이다. 이 때, 업데이트의 기준은 단 1초이며, 1초 후에 프로세스의 상황이 어떻게 되는지를 int로 반환한다. 이 함수 역시 반환 값마다 의미가 있다.
'0'은 해당 프로세스가 계속 CPU에 할당될 '수' 있음을 의미
'1'은 해당 프로세스가 종료되었음을 의미
'2'는 해당 프로세스가 I/O 상태에 접어들었음을 의미,
'3'은 해당 프로세스가 I/O 에서 wakeup 했음을 의미
'4'는 해당 프로세스가 계속해서 I/O를 진행중임을 의미
'5'는 해당 프로세스가 계속해서 큐에 대기중인 상태

- 만일 업데이트 해야 하는 프로세스의 cpu_alloc 이 '1' 이라면 현재 cpu에 할당중인 프로세스이므로, 이 프로세스가 해당 cpu burst time을 다 쓰지는 않았는지, 그리고 완전히 종료상태가 된 프로세스인지 확인하는 과정을 거친다. 각 프로세스별 저장된 time_arr(cpu time 과 I/O time을 순서대로 저장해 놓은 배열) 정보와 이 배열의 index를 정해주는 time_index 변수를 통해 해당 burst time이 종료되면 time_index를 하나씩 올라가게 하였다. 이를 이용하여 time index가 결국 마지막 time_arr의 인덱스까지 도달하게 되었을 때(cycle number * 2 -1 하면 마지막 인덱스임), 종료 상태의 값 '100'을 cpu_alloc에 할당하였다. 이러한 상황에서는 함수에서 1(프로세스 종료)을 반환하게 하였다. 만약 프로세스 자체가 종료되지 않고, 하나의 cpu burst time이 끝나고 다음 I/O burst time에 도달할 때는 time_index만 하나 올려주고 I/O가 진행중임을 의미하는 '3'을 cpu_alloc에 할당하며, 함수 자체에서는 2(I/O상태 접어듦)를 반환하여 종료한다. 만일 cpu burst time이 끝나지 않았다면 (지금 상황은 CPU에 할당중인 상황이므로), 이 프로세스는 계속 CPU를 할당 받을 자격이 있는 프로세스이다. 따라서, 해당 프로세스가 계속 진행할 수 있으므로 함수는 '0'을 반환한다. 하지만 time quantum의 조건에 어긋날 수도 있다. 이러한 조건은 main함수에서 직접 걸러낸다. 

- 만일 업데이트 해야 하는 프로세스의 cpu_alloc이 '2'라면 현재 queue에 올라가 있는 프로세스의 상태이므로 wating time만 update해주면 된다. 그리고 함수는 5를 반환한다.

- 만일 업데이트 해야 하는 프로세스의 cpu_alloc이 '3'이라면 I/O burst time이 진행중인 프로세스이므로, 해당 burst time을 1씩 줄여 나가야 한다. 해당 과정을 잘 따라왔다면, 해당 프로세스의 time_index는 cpu burst time이 끝나고, I/O burst time을 가리키고 있을 것이다. 그리고, 해당 I/O time이 0이되면, wake up이 되어야 한다. 이 때도, time index를 하나 올리고, cpu_alloc 에 2(해당 프로세스가 I/O상태에 접어듦)를 할당한다.그리고 함수자체는 3을 반환한다. 만일 I/O time이 0이 되지 않은 상태이면, 계속해서 I/O를 진행해야 하므로, cpu_alloc에는 변화가 없고, 함수 자체는 4를 반환한다.

- 그리고 혹시나 모를 버그를 위해, 100을 반환하였다.

3. int SelectProcess(Queue *q0, Queue *q1, priority_queue *q2)
다음 함수는 다음 프로세스를 선택하기 위한 큐를 정하는 함수인데, 반환형이 int 이다. 이 int 반환형은 각 큐의 넘버를 의미하며, 0이면 Q0, 1이면 Q1, 2이면 Q2를 의미한다고 가정했다.

4. int WhatIsTimeQuantum(int queue_number);
해당 큐(큐의 넘버가 인자)의 time quantum을 알려주는 함수이다.

### 4. 구현 내용 설명 (main 함수 흐름)

**<가정>**
먼저 input.txt파일은 PID 순서대로 arrival time이 정렬된다고 가정한다. 즉 PID가 낮을수록 빨리 도착하고, 높을수록 늦게 도착한다. 그리고 최초로 등록되는 PID 1은 반드시 0초에 Ready Queue에 도달한다고 한다. 
또한 큐에서 꺼낼 때에는 프로세스가 시작될 때 꺼낸다고 정의한다.

<구현> - 해당 단계별로 주석으로 작성

#### 1. 세팅
input 내용을 받아오는 것이 급선무이다. input 내용중 맨 앞에는 process의 수가 존재하므로, 이를 위한 변수 number_process를 선언하고, 해당 txt파일을 읽어서 할당한다. 그 후에는 계속 프로세스의 정보들이 존재하므로, 먼저 프로세스를 원소로 하는 배열 (processes) 을 선언한다. 이 때 프로세스의 개수를 알고 있으므로 동적 할당이 필요가 없다. 그후 ReadProcess 함수로 해당 정보들을 processes 배열로 가져온다. 
큐들을 선언한 후에, 여러가지 변수들을 정의하고 선언했다.

1. process_index 
arrival_time이 순서대로 정렬된 processes 배열의 인덱스를 의미한다.

2. process_runtime
해당 프로세스가 CPU를 할당 받은 구간마다의 실행시간

3. time_quantum 
각 큐에서 정의된 time quantum

4. process_state
메모리에 등록(도착한 process들)된 프로세스 별로 UpdateProcess를 진행한 결과값을 할당하는 함수이다. 즉 process별 상황 그대로를 의미한다. 의미하는 바는 해당 함수 설명에 작성했다.

5. end_process_num
종료된 프로세스의 개수를 의미한다. 모든 프로세스가 종료되었는지 확인하는 용도이다.

6. now_queue_num
현재 실행중인 프로세스가 존재하는 큐의 번호를 의미한다. 어떠한 프로세스도 RQ에 존재하지 않으면 -1을 반환한다.

7. now_process
현재 실행중인 프로세스를 의미한다.

8. new_process_in
새로운 프로세스가 등록되어야 하는지 여부를 검사하는 변수이다. 이 변수는 프로세스가 종료되거나, CPU burst time이 끝날 때 1이 된다.

9. time
전체 프로세스의 시간을 의미한다. 이 시간이 지나면서 프로세스별 arrival time에 따라 Queue에 Push한다.

10. idle_time
어떠한 프로세스도 CPU에 의해 할당 받고 있지 않는 시간을 의미한다.

11. while_idle_push
어떠한 프로세스도 CPU에 의해 할당 받고 있지 않을 때(idle), 프로세스가 큐에 들어오면 해당 변수는 1로 설정된다. 따라서 바로 해당 프로세스를 현재 프로세스로 올린다.

12. already_print
Gantt Chart를 출력하기 위한 변수인데, 이를 설정하지 않으면 같은 시간대에 동일한 Gantt가 한 번 더 출력되어서 설정한 변수이다. 이러한 상황은 process가 arrival time에 다다라서 큐에 들어올 때 정보를 표시할 때와, 프로세스별 상태에 따라서 Gantt Chart의 흐름이 달라지는 정보를 동시에 표시되는 상황을 막기 위해 설정하였다.

#### 2. While 반복문 (모든 프로세스가 끝날 때 까지)

time을 증가시킨다.

<while 초반 if문>
time을 증가해가며 process가 arrive 될 때 그 프로세스의 init_queue 정보를 보고 적절한 큐에 삽입한다. init_queue 가 2냐 1이냐 0이냐에 따라 다른 큐에 삽입되어야 한다. 그리고 큐에 Push 되고 Pop되는 과정에서 변화하는 프로세스의 상태를 적절히 조절한다. 해당 if문에 걸리면 무조건 idle_time은 -1로 설정이 된다. time 변수 값에 process가 도착했다는 것은 CPU가 할당해야 할 프로세스가 그 time에 무조건 존재한다는 뜻이기 때문이다. 물론 now_queue_num 이 -1(프로세스 없음)이 아닌 경우에는 굳이 idle_time을 -1로 설정하지 않아도 되겠다. 이미 -1이기 때문이다.
그리고, 가장 처음의 프로세스가 할당 받을 때, 현재 실행되고 있는 프로세스가 없으므로 SelectProcess로 프로세스가 존재하는 큐에서 프로세스를 Pop하는 과정을 거친다. 그리고 이를 now_process에 할당한다. 
2번째 프로세스가 할당 받을 때부터는 이미 실행되고 있는 프로세스가 있는 경우가 있고 없는 경우가 있는데, 이는 while문 후반에서 now_process_in이 1일 때, 새로운 프로세스를 현재 존재하는 큐에서 Pop하는 과정을 거치는데, 아무런 프로세스도 큐에 존재하지 않을 때, now_queue_num이라는 변수 (SelectProcess의 반환 값)에 -1이들어간다. 이 변수의 정보에 따라서 push하고 pop해야 하는지(Q2의 스케줄링 기법에 의해 preemtion 체크) 아니면 그냥 해당 프로세스를 Push만 할지(이미 프로세스가 실행중), 아니면 해당 프로세스를 굳이 Queue에 Push하지 않고 바로 now_process로 할당할지 결정한다.(프로세스가 실행중이지 않으므로) 그리고, now_queue_num이 -1일 때, idle_time과 while_idle_push를 조절하여서 idle time이 끝났고, 새로운 프로세스가 idle time중간에 들어왔는지 여부를 알려줌으로써 해당 if문이 끝난 후 process_runtime이 다시 초기화 되어야 하는지 결정할 수 있게 한다. 특히 Push될 프로세스가 Q2에 들어갈 경우 현재 프로세스가 Q2의 스케줄링을 받고 있는지 여부를 파악해야만 한다. 그래야 preemption을 할 지 말지 알 수 있기 때문이다.
마지막에는 해당 프로세스가 arrived 되면 index를 하나 더 옮겨서 다음 도착할 프로세스의 시간이 언제 인지 파악한다.

<if문이 끝나고>

if 문이 끝나고, now_process_in을 0으로 설정한다. 이미 해당 시간에 arrived 된 프로세스는 앞서서 다 처리했기 때문이다. 최종적으로 결정된 idle_time과 while_idle_push 변수에 따라서, process runtime을 조절하거나 Gantt 정보를 출력하고 말지를 결정한다. (이미 해당 시간 Gantt 정보가 앞에서 출력된 경우가 존재) 그 후, 알맞게 idle_time 과 while_idle_push를 다음 시간의 상태를 위하여 초기화 하거나 유지/변화하는 과정을 거친다.

<time을 증가시키고 for문을 돌면서 arrived 된 프로세스 정보들 상태 조회하며 Push, Pop, 큐 이동, 새로운 프로세스 선택 등의 상태를 파악>

이 때는 앞선 상태와 다른 상태이다. time을 증가시켰기 때문이다. 현재 process_index 정보를 통하여 해당 인덱스부터 0까지 돌면 그 시간까지 arrived 된 프로세스를 for문으로 다 돌 수 있다. 앞서서 process_index를 -1로 설정한 이유도 여기에서 있었다. time++를 증가 시켰을 때, 시간을 동기화 시켜주기 위함이다.
각 프로세스마다 UpdateProcess를 거치면, 해당 프로세스가 1초후에 어떤 상태로 변화했는지 process_state에 저장한다.
이 때, process가 종료된 경우 (cpu_alloc 이 100)인 경우에는 다음 프로세스의 인덱스로 바로 넘어가서 업데이트 과정을 거치지 않게 한다.
해당 프로세스의 상태 값에 따라서 적절히 상황을 조절한다. 
0 인 경우에, time quantum을 넘어섰는지 조사하고, 넘어섰으면, 큐의 변화와 프로세스 cpu_alloc의 변화, now_process의 변화 모든 것이 고려사항에 맞게 조절한다 특히 여기서는 하위 큐로 이동시켜야 할 것이다. 
1 인 경우에, process가 종료되었으므로, 혹여나 모든 프로세스가 종료될 수 있으니 이를 체크하고, 종료하며 결과물을 출력한다. 그렇지 않다면, process runtime 을 0으로 초기화 하고, 새로운 프로세스를 할당시켜주어야 한다는 의미로 new_process_in을 1로 설정한다.
2와 3인 경우에도 적절히 구현해주면 된다.
특히 3인 경우에는 해당 프로세스가 wakeup 한 경우이므로, queue 를 상위 큐로 옮긴다.

<for문이 종료된 후에 new_process_in이 1인 경우를 판단>

이 경우에는 새로운 프로세스를 할당시켜주어야 하므로, SelectProcess 함수를 통해 가장 상위 큐부터 프로세스가 존재하는 지 그 여부를 조사해 존재하는 큐의 숫자를 now_queue_num에 할당후, 해당 num에 따라 적절히 pop을 하며 now_process를 바꾼다.
   

5. 실행 결과를 보는 법
Gantt chart를 어떻게 보는지 설명하겠다.
WT TT보는 방법은 보이는 대로이니 생략한다.
첫번째 인풋의 결과를 보면
 
TIME 0 의 줄에 ‘process_1 in’ 이 있는데 이 때는 TIME 0에 1번 PID의 프로세스가 RQ에 할당 되는 것이고, | 1 | 사이에 있는 번호는 0초부터 1초까지 즉, TIME ~ TIME +1 초 까지 CPU에 할당된 프로세스의 PID를 의미한다.
정리하자면 0초에 1번 프로세스가 들어오고 0초부터 1초까지 1번프로세스가 할당됐다. 그리고 1초에 2번 프로세스가 들어오고, 1초부터 2초까지 1번프로세스가 할당된다. 2초에는 들어온 프로세스가 없고, 2초부터 3초까지 2번 프로세스가 할당된다. 3초에는 들어온 프로세스가 없고 3초부터 4초까지 1번프로세스가 할당된다. 4초에는 3번 프로세스가 들어오고, 4초부터 5초까지 3번 프로세스가 즉시 할당된다. 이런식으로 Gantt Chart를 작성했으며, 마지막에는 23초부터 24초까지 4번 프로세스가 CPU를 할당 받다가 결국 끝이 난다.
 
또한 이러한 상태는 CPU가 할당할 프로세스가 없다는 의미로 112초부터 113초까지 아무런 프로세스가 할당되지 않은 idle 한 상태임을 의미한다.
