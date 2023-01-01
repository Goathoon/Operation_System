# 세가지의 프로젝트 진행

# Project-01 [MFQ Scheduling Simulation]

### 1. 목표 설정
3개의 RQ를 갖는 MFQ의 스케줄링 기법을 구현하는 것이 프로젝트의 목표이다. Q0 와 Q1의 RQ는 RR 스케줄링 기법이고, Q2는 SRTN 스케줄링 기법을 사용하여야 한다. 
- 세부 목표 
1. Q0 : Time quantum이 2인 RR 스케줄링 기법 사용
2. Q1 : Time quantum이 4인 RR 스케줄링 기법 사용
3. 2 : SRTN 스케줄링 기법 사용

- 세부 내용
1. 각 프로세스는 최초에 각자에게 지정된 queue로 진입하며, 최초 진입 queue는 입력에 의해 결정됨
2. Qi에서 스케줄받아 실행하고 해당 queue의 time quantum을 소모할 경우 Qi+1로 진입(i= 0, 1)
3. Qi에서 스케줄받아 실행한 프로세스가 IO burst를 마치고 wakeup 되는 경우 Qi-1로 진입(i= 1, 2)
4. Q2의 SRTN 스케줄링은 Q2에 있는 프로세스들만을 대상으로 함
5. 우선순위는 Q0 > Q1 > Q2 순이며, 스케줄링은 항상 높은 우선 순위의 queue에서부터 이루어짐

- 구현

**입력(입력파일명input.txt로작성)**

프로세스 개수 및 각 프로세스별 도착시간(AT), 최초진입queue, # Cycles, 수행트레이스(CPU-BT, IO-BT, …) 등(CPU-BT 및IO-BT는각각CPU burst time, IO burst time을의미함)  (#Cycles는수행트레이스의(CPU-BT, IO-BT) 쌍의개수임; 다만, 마지막Cycle에는CPU-BT만존재함) 

**출력**

1. 스케줄링결과(Gantt chart)
2. 각프로세스별Turnaround Time(TT) 및Waiting Time(WT)
3. 전체 프로세스의 평균 TT 및 평균 WT

![image](https://user-images.githubusercontent.com/95288696/210159132-c16428f1-7079-4a1f-9f3d-89e69fe2b5c0.png)

- 고려 사항

1. 모든 프로세스들의 IO burst는 병렬 진행 가능한 것으로 가정
2. 출력 형태는 각자 자유롭게 결정; 단 출력 결과를 누구나 쉽게 파악할 수 있도록 해야함

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
   

### 5. 실행 결과를 보는 법

![image](https://user-images.githubusercontent.com/95288696/210158920-f4bd99c6-edb9-4da4-9bf1-566540e9f939.png)

Gantt chart를 어떻게 보는지 설명하겠다.
WT TT보는 방법은 보이는 대로이니 생략한다.
첫번째 인풋의 결과를 보면
 
TIME 0 의 줄에 ‘process_1 in’ 이 있는데 이 때는 TIME 0에 1번 PID의 프로세스가 RQ에 할당 되는 것이고, | 1 | 사이에 있는 번호는 0초부터 1초까지 즉, TIME ~ TIME +1 초 까지 CPU에 할당된 프로세스의 PID를 의미한다.
정리하자면 0초에 1번 프로세스가 들어오고 0초부터 1초까지 1번프로세스가 할당됐다. 그리고 1초에 2번 프로세스가 들어오고, 1초부터 2초까지 1번프로세스가 할당된다. 2초에는 들어온 프로세스가 없고, 2초부터 3초까지 2번 프로세스가 할당된다. 3초에는 들어온 프로세스가 없고 3초부터 4초까지 1번프로세스가 할당된다. 4초에는 3번 프로세스가 들어오고, 4초부터 5초까지 3번 프로세스가 즉시 할당된다. 이런식으로 Gantt Chart를 작성했으며, 마지막에는 23초부터 24초까지 4번 프로세스가 CPU를 할당 받다가 결국 끝이 난다.
 
또한 이러한 상태는 CPU가 할당할 프로세스가 없다는 의미로 112초부터 113초까지 아무런 프로세스가 할당되지 않은 idle 한 상태임을 의미한다.

### 6. 아쉬운 점

예외처리를 아예 신경쓰지 않았다. 그저 Valid 한 INPUT만 온다고 가정했었는데, 이 부분에서 점수가 많이 낮아진 프로젝트였다. 이후 프로젝트에서는 예외처리를 반드시 진행했다.

<hr/>

# Project-02 [Deadlock Avoidance]

### 1. 목표 설정
N개의 process와 M개의 resource type을 갖는 시스템에서 주어진 Max-claim matrix와 Current-allocmatrix 를 기준으로 deadlock avoidance 를수행하기 위한 기본 기법 구현
(N과 M값, 각 resource unit 개수등을 비롯하여 Max-claim matrix 와 Current-allocmatrix 등은 입력으로 주어짐)
- 세부 내용
1. Multiple resource types & multiple resource units가정
2. 주어진 Current-alloc 상태가 safe 인지 unsafe 인지 확인/판정 (safe인 경우 safe sequence 출력, Unsafe인 경우 이유를 출력

- 구현

**입력(입력파일명input.txt로작성)**

1. 프로세스개수(N) 및resource type 개수(M),각resource type 별resource unit 개수(t1, t2, …, tM)
2. Max-claim matrix (N×M matrix)
3. Current-allocmatrix (N×M matrix)

**출력**
1. Safe 상태 여부
2. Safe sequence 또는 unsafe state인 이유

![image](https://user-images.githubusercontent.com/95288696/210159196-0fdde6d2-9746-437a-a1dd-b6f139607fbe.png)

### 2. 설계 / 구현 내용

-1 <input.txt 처리 및 예외처리>

인풋으로 주어지는 ”input.txt” 파일을 ‘fscanf’ 로 받아온다. 프로세스의 수, 자원의 수를 받아올 때, 0보다 작은 수이면 의미가 없으므로 “Invalid Input”을 출력한다. 그후에 주어지는 input에 해당하는 것들 (resource unit의 수, 프로세스별 max_claim, current_allocation의 경우)에서는 해당 내용들이 음수이거나, input.txt에 내용이 없는 경우 “Invalid Input”을 출력하는 식으로 예외를 처리했다. 

-2 <input을 저장하는 방법>

input.txt로 받아온 자원의 개수만큼, 각 자원의 수를 담을 배열인 ’resource_array'를 동적 할당하여 저장한다.
그리고, 프로세스별로 정의된 ‘max_claim’ , ‘current_alloc’을 우선적으로 저장해야 하므로, process 구조체를 선언하여 이를 구조체 멤버로 정의하였다. 또한, deadlock avoidance를 위해 이들이 현재 상태에서 필요로 하는 자원 별 개수에 해당하는 정보가 필요하므로, need_alloc이라는 배열을 하나 더 멤버로 추가하였다. 이는 max_claim 에서 current_alloc 배열의 각 원소를 빼서 need_alloc의 원소로 저장하면 된다. 또한 해당 프로세스의 종료 여부를 체크하는 done이라는 변수를 만들어서 deadlock avoidance를 체크할 시에, 무의미한 반복을 하지 않게 하였다. 그리고 이들을 process의 개수만큼 process 배열을 할당한 구조체 배열인 processes를 정의하였다.

-3 <deadlock avoidance를 체크하는 방법 (setting) >

먼저 ‘isDeadlock’이라는 배열을 선언하였다. 이 배열은 프로세스별로, deadlock인지 아닌지를 판단하는 배열이고, 이 배열의 원소의 index는 프로세스의 넘버와 순서가 같다. (P1이면 0번째, P2이면 1번째 인덱스). 1이면 deadlock이고 0이면 deadlock이 아니다. 이를 시작부분에서 0으로 다 초기화를 해주었다.
‘done_process’라는 변수는 종료된 프로세스가 몇 개인지 세는 변수이다. 
‘stack_process’라는 배열은 추후에 safe sequence를 print하기 위한 배열로, 먼저 종료된 프로세스들을 stack처럼 배열에 담기 위해 정의하였다. 이름에만 stack이 들어가 있지, stack으로 구현하지는 않았고, stack의 느낌만 가져갔다. 어차피 safe sequence를 위해 존재하는 배열이므로 원소의 개수는 process의 총 개수로 할당시켰다. 프로세스가 종료될 때마다, 인덱스를 하나씩 늘려가면서 다음 번 종료된 프로세스가 해당 인덱스에 저장되게 하는 방식을 사용하여, 종료된 프로세스 순서대로 해당 배열에 저장된다.
‘end_process_index’는 stack_process 배열의 인덱스이다.

-4 <deadlock avoidance를 체크하는 방법 (실행) >

1. 이용가능한 자원의 unit의 개수를 resource_array에 저장

총 프로세스에서 이미 할당된 자원의 개수를 type별로 for문을 돌면서 더하여, 해당 resource_array에서 이들을 빼주어 이용 가능한 unit의 개수를 저장하였다.

2. available 한 자원(resource_array에 저장된 정보)를 통해 프로세스 배열을 순회하며 해당 프로세스에게 자원을 할당할 수 있는지 확인

프로세스가 자원을 할당 받는 순서에 대해서는 언급이 없으므로, 프로세스를 처음부터 순서대로 돌면서, 먼저 종료 가능한 프로세스에게 바로 해당 자원을 주고, 종료 후 자원을 반납하는 방식으로 구현하였다.
이를 while문을 통해서 반복하였는데, 프로세스 배열의 인덱스를 ‘index’로 선언하였고, 이를 증가시키면서 프로세스 배열을 순회하였다.물론 이 때, 프로세스가 종료되었는지 확인하는 과정을 거쳐서, 의미없는 반복을 피하였다. 또한 만일 해당하는 프로세스가 이용가능한 자원 내에서 충분하게 자원을 받지 못하는 경우(종료할 수 없는 경우) 해당하는 프로세스를 ‘isDeadlock’배열에서 1로 설정하여 Deadlock 유발 프로세스임을 정의하였다.
만일, 프로세스 배열을 순회하는 와중에, 해당 프로세스가 deadlock도 아니고, 종료된 프로세스도 아니라면 이는 avilable한 자원에서 해당 프로세스의 need를 충족시킬 수 있는 상태이므로, 이용 가능한 자원을 프로세스에게 모두 건네 주고, 해당 프로세스가 종료함을 가정하였다. 그러면 프로세스가 가지고 있는 모든 자원을 반환할 것이고, 이를 resource_array에 update 시켰다. 그리고 해당 프로세스가 종료됨을 밝혔고 (done 변수) done_procee(종료된 프로세스의 개수)를 증가시키고, 종료된 프로세스의 순서대로 stack_process에 업데이트 시켜야하므로, end_proces_index에 해당하는 인덱스에 stack_process에 해당 프로세스의 번호를 저장시켰다. (이 때, 인덱스는 프로세스의 번호-1 이므로 이를 반영)그리고 다음에 종료할 프로세스를 위해 인덱스를 하나 증가시킨다. 이 다음에는 처음 프로세스부터 다시 순회해야 하므로, 프로세스 배열의 인덱스인 ‘index’ 또한 0으로 초기화 한다. 
그 후에, 모든 프로세스가 종료되었는지 확인하고, 만일 종료되었다면(인덱스 == 모든 프로세스의 개수) safe한 상태이므로 이를 stack_process 배열을 처음부터 순회하며 적절한 형태로 print 하였다. 
만일 while문에서 프로세스를 순회하는데 모든 프로세스를 다 순회 했음에도, index가 0으로 초기화되지 않았다는 것은, deadlock 상태임을 의미한다. 따라서 isDeadlock 배열에서 deadlock 상태인 ‘1’인 인덱스에 해당하는 프로세스들의 정보(index+1)를 꺼내어 프린트한다. 

### 3. 다양한 입력에 대한 실행 결과

![image](https://user-images.githubusercontent.com/95288696/210158969-b0c485f9-3575-4fc3-9dad-b64f95cda199.png)

# Project-03 [Virtual Memory Management 기법 구현]

### 1. 목표 설정
Demand paging system을 위한 page replacement 기법 구현 및 검증

- 세부 목표 
주어진 page reference string을 입력받아 아래의 각 replacement 기법으로 처리했을 경우의 memory residence set 변화 과정 및 page fault 발생 과정 추적/출력
1. MIN
2. LRU
3. LFU
4. WS Memory Management

- 세부 내용
1. Pageframe 할당량 및 window size 등은 입력으로 결정
2. 초기 할당된 page frame들은 모두 비어있는 것으로 가정
3. 각 기법의 실행 결과에 대한 출력방법은 각자 design 하여 진행

- 구현

**입력(입력파일명input.txt로작성)**

![image](https://user-images.githubusercontent.com/95288696/210159289-0f9a075f-d165-42c0-bb24-447afc86a309.png)

**출력**

![image](https://user-images.githubusercontent.com/95288696/210159291-22ca231f-012f-4ce3-a383-c9d9660b0f36.png)

1. 스케줄링결과(Gantt chart)
2. 각프로세스별Turnaround Time(TT) 및Waiting Time(WT)
3. 전체 프로세스의 평균 TT 및 평균 WT

### 2. 설계 / 구현 내용

#### 1. <input.txt 처리 및 예외처리>

인풋으로 주어지는 ”input.txt” 파일을 ‘fscanf’ 로 받아온다. 필요한 입력 값들인 process 가 갖는 page의 개수, 할당 page frame의 개수, window size, page reference string의 길이 값들이 0보다 작은 수이거나 해당 입력값이 없거나, 혹은 최대 지정 개수를 초과하면 에러 메세지를 출력하고 종료한다. 또한 reference string의 입력값들은 주어진 reference string의 길이보다 작거나 큰 경우에도 에러 메시지를 출력하고 프로세스를 종료하는 예외처리를 하였다.

#### 2. <FA(Fixed Allocation) 처리 방법>

해당 방법은 각 프로세스마다 할당하는 프레임의 수를 고정시켜서, 프레임에 ref 된 page string을 저장시키는 방식이다.
따라서, input으로 받은 page frame의 수를 받아와 그 만큼 memory 배열에 동적할당 해 주었다. 또한 frame에 저장된 페이지의 정보들을 저장하는 ref_info 라는 배열도 page의 수만큼 동적할당 해 주었다. 물론 굳이 메모리에 올라와 있는 페이지, 즉 page frame의 수만큼만 동적할당해서 해당 페이지의 정보를 저장할 수도 있겠지만, 구현이 꽤 까다로워 page의 수만큼 동적 할당하였다. 여기서, 페이지의 정보란, FA의 방법 중 MIN의 알고리즘에서는 각 page별 forward distance의 정보를, LRU 알고리즘에서는 backward distance의 정보를, LFU방식에서는 여태까지 참조된 횟수의 정보를 담는다. Variable Allocation 방법인 Working Set 에서는 해당 배열을 사용할 필요가 없다. 여기서 LFU방식에서는 tie의 상황이 존재할 수 있는데, 그러한 상황에서 교체될 page를 정하기 위한 방법에 활용되는 배열인 tie 배열을 총 page의 수만큼 동적 할당했다. 해당 활용 방안은 후에 LFU방식에서 자세히 밝히겠다.

1. MIN(FA)

기본적으로 FA방식중 MIN 과 LRU 방식의 초반부는 동일하다. 따라서초반부의 과정을 do_same함수를 정의하여 실행했다. 여기서 언급하는 초반부는, 모든 page frame이 page에 의해서 꽉 차있는 상태까지를 말한다. 
해당 함수는 각 기법별로 공통으로 사용되는 page fault수(fault_count), memory 배열의 index(empty_index, 비어있는 pageframe의 index이다.)를 0으로 초기화하고, 모든 page frame에 담긴 정보들을 초기화하며. 각 page의 정보를 담은 ref_info(page_info)를 초기화 시키는 과정에서 시작된다. 이유는, 다른 알고리즘과 기법에서 같은 배열과 정보들이 참조되어 잘못된 정보 공유를 막기 위함이다.  
page frame을 꽉 채울 때까지 해당 frame에 page를 load하기 위해서 ref 된 page string (배열로 이미 치환했음)의 ref_index를 0부터 시작하여 해당 index를 증가시킴과 동시에 empty_index가 pageframe의 수보다 커지면 해당 함수를 종료한다. 만일 그렇지 않다면 page frame에 이미 참조된 page가 존재하는지 여부를 검사하고 (check 함수로 체크하였다. 해당 함수는 참조된 string과 memory 정보들을 index별로 선형적으로 체크하였다.) 만일 참조된 page가 page frame에 존재하지 않다면, 바로 memory 배열에 해당 page string을 추가하고 page fault의 수를 1만큼 증가시킨다. 이렇게 ref된 page가 생기고 이를 처리할 때마다, 해당 상황을 print 하여 변화 과정을 추적하였다.
해당 함수가 종료하면 index를 반환하는데, 해당 index는 마지막으로처리한 refstring의 index로, 그 다음 refstring의 index 부터 page를 load하는 과정을 시작하게 한다.
다시 MIN함수로 돌아와서, 반환된 index부터 마지막 refstring의 index까지 증가시키면서 replacement될 page를 고르고 교체하는 과정을 MIN방식으로 진행한다. MIN은 forward distance방식을 사용하므로, page fault가 생길 경우, memory (page frame)에 올라와 있는 모든 page string을 index로 참조하면서 지금 참조되고 있는 refstring의 인덱스부터 해당 memory에 load 된 page string이 미래에 다시 참조될 때까지 인덱스를 증가시킨다. 만일 동일한 page string을 찾았다면 찾은 index와 지금 참조되고 있는 refstring의 index의 차를 page_info 배열의 해당 page string의 인덱스 위치로 forward distance를 저장한다. 만일 동일한 page string을 refstring에서 찾지 못했다는 것은, 가장 우선순위로 교체해야 할 page라는 것(후에 쓰이지 않을 page)을 의미하므로, 사실상 max의 distance인 1001로 저장한다. 그후 return_Big함수를 사용하여, memory(page frame)에 존재하는 page string들을 index로 이용하여 page_info 배열을 돌면서 가장 큰 page_info의 값을 가지고 있는 memory(page frame)의 index를 반환하여, 해당 index를 교체하게끔 한다. 이 때, tie-breaking rule을 큰 page number를 가진 frame 을 교체하는 방식으로 진행했다.

2. LRU(FA)

LRU의 과정은 MIN과 거의 동일하다. 차이점은 page_info 에 저장할 정보들이 다를 뿐이다. 저장할 정보는 backward distance 이므로, MIN에서는 참조된 ref_index 부터 refstring의 끝까지 이동하면서 미래에 참조될 page를 찾았다면, LRU는 ref_index 부터 refstring의 맨 앞까지 거꾸로 이동하면서 과거에 참조된 page를 찾는다. 따라서 LRU와 매우 흡사한 코드로 진행된다.

3. LFU(FA)

LFU의 과정은 앞서 본 두가지 알고리즘과 다른 부분이 있다. 일단, 초기과정은 비슷하지만, LFU는 맨 처음부터 계속해서 해당 page가 몇 번 참조되었는지를 기록해야 하므로, do_same함수를 약간 변형시켜야 한다. 먼저 page_info 배열을 모두 0으로 초기화 한다. 이는 각 page 별, 참조된 횟수를 초기화 하는 과정이므로 0을 초기화 하는 것이다. 그 후, 참조된 page string을 인덱스로 하는 page_info 배열의 값들을 ref_index를 증가시키면서 1씩 증가시킨다.
그 후에, replacement할 page를 고르는 과정 속에서, 가장 작은 page info 값(가장 적게 참조된) 페이지를 교체해야 한다. 이를 return_Big_LFU 함수로 확인하였다. 하지만 여기서 유의해야 하는 점이, tie의 상황이다. 이를 return_Big_LFU 함수를 통해 알 수 있는데, 만일 min(최소 참 조 횟수)가 갱신된 상황속에서, 참조 횟수가 동일한 page가 존재하면 isTie 라는 tie가 존재함을 알려주는 변수를 1로 둔다. 동시에 인자로 받은 tie배열에 해당 page의 인덱스 위치의 값에 1로 설정하여, tie가 발생한 page 번호를 알 수 있게 한다. 물론, 새로운 min값이 갱신되면 tie값과 isTie는 초기값인 0으로 다 초기화 되어야 할 것이다. 마지막 page_info index까지 돌고 나서도 isTie가 1이라면, 같은 minimal count값을 가진 page가 존재하므로, return index를  -1로 반환한다. 그리고, LFU함수에서 반환한 값이 -1이라면 tie를 처리하는 과정을 거친다. 이 때 tie-breaking rule은 LRU이므로, tie 배열을 선형적으로 탐색하면서 1인 index (tie가 발생한 page)와 동일한 page를 ref index를 감소하면서 LRU알고리즘을 적용한다. backward distance를 다시 tie 배열에 해당 index(page string)에 저장한다. 그리고 이중 가장 값이 큰 tie index를 찾고, memory (page frame)에 해당 index가 존재하는 memory (page frame) 의 index를 찾는다. 이 index가 교체되어야 할 index이므로, 참조된 page string을 해당 memory index에 넣는다.

4. WS(FA)

WS memory management는 window size로 진행되는 management로 앞선 FA방식과 많이 다르다. 여기서 자료구조 Queue를 활용하였다. 그 이유는, working set이 Queue 자료구조와 닮은 구석이 있기 때문이다. 만약 0,1,2,3 의 residence set이 있는 와중에 (해당 순서대로 ref가 되었고 window size가 3이라고 가정하자), 4가 들어오면 가장 먼저 참조된 0이 replacement가 될 것이다. 하지만, 0,1,2,3,0,4가 순서대로 ref되었다고 가정하면, residence set은 (순서가 있다고 가정하자) 1,2,3,0이 되어 1이 replacement가 될 것이다. 즉 FIFO방식이 적용된다는 것이다. 따라서 Queue 자료구조을 마치 residence set처럼 생각했다. 
하지만 한가지 더 고려해야할 점은, window size만큼의 시간 차이만큼에 해당하는 ref 된 page 들만 memory에 올라와 있어야 하므로, 해당 과정을 반드시 구현해야 한다. 이 상황은 오직 ref_index가 3보다 큰 상황에서만 고려해야하고, queue의 가장 front에 있는 page 정보는 가장 먼저 memory에 들어온 page이다. 따라서 해당 page string이 현재 ref_index보다 3만큼 작은 refstring의 index 까지 같은 page 가 참조되었다면 해당 page는 중복된 페이지이므로 반드시 window size안에 참조되었을 것이다. 하지만 그렇지 않다면 해당 page를 pop하면 된다. 이제 본격적인 working set management를 큐 관점에서 보자.
만일 해당 큐가 비어 있다는 것은 page frame에 어떤 page도 load가 되지 않았다는 것이므로 (ref_index가 0), queue에 해당 ref page를 넣는다. 그후에 ref_index를 1부터 끝까지 증가시킨다. 
이 때, 반드시 memory(page frame)에 이미 존재한 page를 ref 했다면, 아무것도 하지 않을 것이 아니라, 해당 page를 queue에서 삭제시키고 (page frame에서 빼내고), 다시 삽입하는 과정을 거쳐, ref된 순서를 반영해야만 한다. 해당 부분이 
```c
if (ptr->page == refstring[ref_index])
```
이 부분이다. 이를 구현하기 위해 queue를 ptr로 탐색할때, ptr 이전의Node 또한 preptr로 저장하였다. 이 외의 상황들은 일반적인 queue를 다룰 때와 크게 다르지 않다. 
만일 fault가 발생한 상황이라면, 두가지 상황이 존재할 수 있는데, queue 존재하는 노드의 수가 window size + 1의 상황이라면 window size를 넘어가는 상황이므로, queue에서 노드를 pop하고 ref된 page를 push하는 과정을 거친다. 만일 그렇지 않다면 그냥 push만 하면 된다.
 
### 3. 가정
MIN 알고리즘의 tie-breaking rule을 큰 page number를 가진 frame 을 교체하는 방식으로 진행했다.

### 4. 출력물 및 출력 형태

- input 1

억지로 MIN의 TIE상황을 만듦

6 4 3 14 <br>
0 1 5 0 3 4 4 2 4 3 4 5 3 4

![image](https://user-images.githubusercontent.com/95288696/210159373-87092135-3f9e-4de7-9112-0e1eb813c89f.png)
![image](https://user-images.githubusercontent.com/95288696/210159375-1efd31f6-3a91-46e9-bcc8-608b5da64192.png)

MIN 에서 0153->0453 의 residence set이 변화하는 과정속에서 0과 1이 TIE인 상황에서 TIE breaking rule의 가정에 따라 큰 page string인 1이 교체 되었다.

LRU 에서 0153->0453에서 오랜 시간동안 참조되지 않은 1이 교체되었다.

LFU 에서 0153->0453에서 참조수가 1로 가장 적은 page 1이 교체되었다. 

WS 에서 window size에 따라서 memory 에서 out (342->24)와 같은 현상이 일어난다. 

- input 2

억지로 TIE 상황을 만듦

6 4 3 14 <br/>
0 1 5 0 3 4 0 1 0 3 4 5 3 4

![image](https://user-images.githubusercontent.com/95288696/210159398-dff01e2b-332f-44a5-a129-c2654d591401.png)
![image](https://user-images.githubusercontent.com/95288696/210159399-17a05333-4d58-4fcb-b376-0ef9b04207f3.png)

마찬가지로 MIN에서 0143->0543에서 0과 1이 TIE인 상황에서 TIE breaking rule에 따라 큰 page인 1이 교체되었다.

LFU에서 0413->0453 에서 1이 두 번 3이 두 번 TIE인 상황에서 1이 3보다 더 오랜시간 참조되지 않았으므로 1이 replacement 되었다.

- input 3

WINDOW SIZE를 증가시켜 봄.

6 4 4 15 <br/>
0 1 2 3 4 5 4 5 4 1 3 4 3 4 5

![image](https://user-images.githubusercontent.com/95288696/210159416-e109215c-69e4-4cda-988e-9b7199393478.png)
![image](https://user-images.githubusercontent.com/95288696/210159417-0f39bb55-eb62-44ab-ae02-03179f655b11.png)

WS에서 앞서 다르게 window size가 1만큼 늘어나다 보니, 5개까지 page가 residence set에 올라온 것을 볼 수 있다.

- input 4

5 5 3 13
4 3 0 2 2 3 1 2 4 2 4 0 3

![image](https://user-images.githubusercontent.com/95288696/210159426-23296bd3-9978-411f-8f2f-a127b241d08c.png)
![image](https://user-images.githubusercontent.com/95288696/210159428-6f3ec6c9-2f84-4acc-afb8-be0fc6aa45d3.png)

- input 5

6 2 3 15 <br/>
0 1 2 3 2 3 4 5 4 1 3 4 3 4 5

![image](https://user-images.githubusercontent.com/95288696/210159466-d28e52ba-5234-481b-98a2-ad8217aedaf2.png)
![image](https://user-images.githubusercontent.com/95288696/210159468-1cba78b3-6c00-49e2-8ad0-a1c39451d38e.png)

