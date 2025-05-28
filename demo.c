#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int pid;          // Process ID
    int arrival_time; // 도착 시간
    int burst_time;   // CPU 실행 시간
    int remaining_time; // 남은 실행 시간 (선점형에서 사용)
    int io_burst_time; // I/O 시간 (선택사항)
    int io_request_time; // I/O 요청 시간 (선택사항)
    int priority;     // 우선순위 (작을수록 높은 우선순위)
    int waiting_time; // 대기 시간
    int turnaround_time; // 반환 시간
    int completion_time; // 완료 시간
} Process;

Process* processes = NULL;
int num_processes = 0; 

float Comp_Average_waiting_time[7] = {0};  
float Comp_Average_turnaround_time[7] = {0};
int executed_algorithms[6] = {0};

void Create_Process() {
    printf("생성할 프로세스 개수 입력 : \n");
    scanf("%d", &num_processes);
    processes = (Process*)malloc(sizeof(Process) * num_processes);
    if (processes == NULL) {
        printf("메모리 할당 실패 오류임\n");
        exit(1);
    }

    for (int i = 0; i < num_processes; i++) {
        processes[i].pid = i + 1;

        printf("\n프로세스 P%d 입력\n", processes[i].pid);
        printf("도착 시간 : ");
        scanf("%d", &processes[i].arrival_time);

        printf("실행 시간 : ");
        scanf("%d", &processes[i].burst_time);

        printf("우선순위 (작을수록 높음): ");
        scanf("%d", &processes[i].priority);

        processes[i].remaining_time = processes[i].burst_time; // 선점형에 쓸거
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].completion_time = 0;
    }

    printf("\n입력끝\n");


}



void Schedule_Menu() {
    int choice;
    int time_quantum;

    do {
        printf("\n번호로 스케쥴링 알고리즘 선택\n");
        printf("1. FCFS\n");
        printf("2. SJF_NonPreemptive\n");
        printf("3. SJF_Preemptive\n");
        printf("4. Priority_NonPreemptive\n");
        printf("5. Priority_Preemptive\n");
        printf("6. Round Robin\n");
        printf("0. 더이상 안함\n");
        printf("번호 선택: ");
        scanf("%d", &choice);

        switch (choice) {
                case 1:
                    FCFS();
                    executed_algorithms[0] = 1;
                    break;
                case 2:
                    SJF_NonPreemptive();
                    executed_algorithms[1] = 1;
                    break;
                case 3:
                    SJF_Preemptive();
                    executed_algorithms[2] = 1;
                    break;
                case 4:
                    Priority_NonPreemptive();
                    executed_algorithms[3] = 1;
                    break;
                case 5:
                    Priority_Preemptive();
                    executed_algorithms[4] = 1;
                    break;
                case 6:
                    printf("RR의 Time Quantum: ");
                    scanf("%d", &time_quantum);
                    RoundRobin(time_quantum);
                    executed_algorithms[5] = 1;
                    break;
            case 0:
                printf("프로그램 종료\n");
                break;
            default:
                printf("잘못된 선택입니다.\n");
        }

    } while (choice != 0);
}


void FCFS() {
    //순서 배열
    for (int i = 0; i < num_processes - 1; i++) {
        for (int j = i + 1; j < num_processes; j++) {
            if (processes[i].arrival_time > processes[j].arrival_time) {
                Process temp = processes[i];
                processes[i] = processes[j];
                processes[j] = temp;
            }
        }
    }

    //간트 따져야 하니까 크기는 동적배열, 최대 시간은 단순하게 가장 마지막 도착+총실행시간
    int latest_arrival = 0;
    int total_burst = 0;

    for (int i = 0; i < num_processes; i++) {
        if (processes[i].arrival_time > latest_arrival){
            latest_arrival = processes[i].arrival_time;
        }
        total_burst += processes[i].burst_time;
    }

    int max_possible_time = latest_arrival + total_burst;




    // 간트차트 기록용 배열
    int* gantt = (int*)malloc(sizeof(int) * (max_possible_time + 1));

    if (gantt == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }

    // 0으로 초기화
    for (int i = 0; i < max_possible_time +1; i++) {
        gantt[i] = 0;
    }

    int current_time = 0;
    for (int i = 0; i < num_processes; i++) {
        if (current_time < processes[i].arrival_time)
            current_time = processes[i].arrival_time;

        // 간트에 프로세스 유지 중
        for (int t = current_time; t < current_time + processes[i].burst_time; t++) {
            gantt[t] = processes[i].pid;
        }

        processes[i].waiting_time = current_time - processes[i].arrival_time;
        current_time += processes[i].burst_time;
        processes[i].completion_time = current_time;
        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;

    }

    // 간트차트 출력
    printf("\nFCFS 간트차트 출력\n");
    printf("Time:    ");
    for (int t = 0; t < current_time; t++) printf("%2d ", t);
    printf("\nProcess: ");
    int last_pid = -1;
    for (int t = 0; t < current_time; t++) {
        if (gantt[t] == 0) printf(" - ");
        else {
            if (gantt[t] != last_pid && last_pid != -1) printf("| ");
            printf("P%d ", gantt[t]);
            last_pid = gantt[t];
        }
    }
    printf("| End\n");

    // 결과 출력
    printf("\nPID\tArrival\tBurst\tWaiting\tTurnaround\n");
    float total_wait = 0, total_turn = 0;
    for (int i = 0; i < num_processes; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\n",
               processes[i].pid,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].waiting_time,
               processes[i].turnaround_time);
        total_wait += processes[i].waiting_time;
        total_turn += processes[i].turnaround_time;
    }

    printf("\n평균 대기시간: %.2f\n", total_wait / num_processes);
    printf("평균 반환시간: %.2f\n", total_turn / num_processes);

    Comp_Average_waiting_time[0] = total_wait / num_processes; 
    Comp_Average_turnaround_time[0] = total_turn / num_processes;

    free(gantt);
}


void SJF_NonPreemptive() {
    int latest_arrival = 0;
    int total_burst = 0;

    for (int i = 0; i < num_processes; i++) {
        if (processes[i].arrival_time > latest_arrival){
            latest_arrival = processes[i].arrival_time;
        }
        total_burst += processes[i].burst_time;
    }

    int max_possible_time = latest_arrival + total_burst;
    
    int* gantt = (int*)malloc(sizeof(int) * (max_possible_time + 1));

    if (gantt == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }
    for (int i = 0; i < max_possible_time +1; i++) {
        gantt[i] = 0;
    }
    //가장 짭은 실행시간 찾으려면 위에서부터 비교
    int max_burst_time=0;
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].burst_time > max_burst_time){
            max_burst_time = processes[i].burst_time;
        }
    }



    int current_time = 0, completed = 0;
    int* is_completed = (int*)malloc(sizeof(int) * num_processes);
    if (is_completed == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }
    for (int i = 0; i < num_processes; i++) {
        is_completed[i] = 0;
    }

    while (completed != num_processes) {
        int p_index = -1, min_burst = max_burst_time+1;

        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= current_time && is_completed[i] == 0) {
                if (processes[i].burst_time < min_burst) {
                    min_burst = processes[i].burst_time;
                    p_index= i;
                } else if (processes[i].burst_time == min_burst && processes[i].arrival_time < processes[p_index].arrival_time) {
                    p_index = i;
                }
            }
        }

        if (p_index != -1) {
            for (int t = current_time; t < current_time + processes[p_index].burst_time; t++) {
                gantt[t] = processes[p_index].pid;
            }

            processes[p_index].waiting_time = current_time - processes[p_index].arrival_time;
            current_time += processes[p_index].burst_time;
            processes[p_index].completion_time = current_time;
            processes[p_index].turnaround_time = current_time - processes[p_index].arrival_time;

            is_completed[p_index] = 1;
            completed++;
        } else {
            gantt[current_time] = 0;
            current_time++;
        }
    }

    printf("\nSJF (Non-Preemptive) 간트차트\n");
    printf("Time:    ");
    for (int t = 0; t < current_time; t++) printf("%2d ", t);
    printf("\nProcess: ");
    int last_pid = -1;
    for (int t = 0; t < current_time; t++) {
        if (gantt[t] == 0) printf(" - ");
        else {
            if (gantt[t] != last_pid && last_pid != -1) printf("| ");
            printf("P%d ", gantt[t]);
            last_pid = gantt[t];
        }
    }
    printf("| End\n");

    printf("\nPID\tArrival\tBurst\tWaiting\tTurnaround\tCompletion\n");
    float total_wait = 0, total_turn = 0;
    for (int i = 0; i < num_processes; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n", processes[i].pid, processes[i].arrival_time, processes[i].burst_time, processes[i].waiting_time, processes[i].turnaround_time, processes[i].completion_time);
        total_wait += processes[i].waiting_time;
        total_turn += processes[i].turnaround_time;
    }

    printf("\n평균 대기시간: %.2f\n", total_wait / num_processes);
    printf("평균 반환시간: %.2f\n", total_turn / num_processes);
    
    Comp_Average_waiting_time[1] = total_wait / num_processes; 
    Comp_Average_turnaround_time[1] = total_turn / num_processes;

    free(is_completed);
    free(gantt);
}


void SJF_Preemptive() {
    int latest_arrival = 0;
    int total_burst = 0;

    for (int i = 0; i < num_processes; i++) {
        if (processes[i].arrival_time > latest_arrival){
            latest_arrival = processes[i].arrival_time;
        }
        total_burst += processes[i].burst_time;
    }

    int max_possible_time = latest_arrival + total_burst;
    
    int* gantt = (int*)malloc(sizeof(int) * (max_possible_time + 1));

    if (gantt == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }
    for (int i = 0; i < max_possible_time +1; i++) {
        gantt[i] = 0;
    }

    int max_burst_time=0;
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].burst_time > max_burst_time){
            max_burst_time = processes[i].burst_time;
        }
    }



    int current_time = 0, completed = 0;
    int* is_completed = (int*)malloc(sizeof(int) * num_processes);
    if (is_completed == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }
    for (int i = 0; i < num_processes; i++) {
        is_completed[i] = 0;
    }

    while (completed != num_processes) {
        int p_index = -1;
        int min_remain = max_burst_time;

        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= current_time && is_completed[i] == 0) {
                if (processes[i].remaining_time < min_remain) {
                    min_remain = processes[i].remaining_time;
                    p_index = i;
                }
                else if (processes[i].remaining_time == min_remain) {
                    if (processes[i].arrival_time < processes[p_index].arrival_time) {
                        p_index = i;
                    }
                }
            }
        }

        if (p_index != -1) {
            gantt[current_time] = processes[p_index].pid;
            processes[p_index].remaining_time--;

            if (processes[p_index].remaining_time == 0) {
                processes[p_index].completion_time = current_time + 1;
                processes[p_index].turnaround_time = processes[p_index].completion_time - processes[p_index].arrival_time;
                processes[p_index].waiting_time = processes[p_index].turnaround_time - processes[p_index].burst_time;

                is_completed[p_index] = 1;
                completed++;
            }
        } else {
            gantt[current_time] = 0; 
        }

        current_time++;
    }

    // 간트차트 출력
    printf("\nSJF (Preemptive) 간트차트\n");
    printf("Time:    ");
     for (int t = 0; t < current_time; t++) printf("%2d ", t);
    printf("\nProcess: ");
    int last_pid = -1; //last_pid는 pid의 마지막을 구분짓기 위해서 넣어줌
    for (int t = 0; t < current_time; t++) {
        if (gantt[t] == 0) printf(" - ");
        else {
            if (gantt[t] != last_pid && last_pid != -1) printf("| ");
            printf("P%d ", gantt[t]);
            last_pid = gantt[t];
        }
    }
    printf("| End\n");

    printf("\nPID\tArrival\tBurst\tWaiting\tTurnaround\tCompletion\n");
    float total_wait = 0, total_turn = 0;
    for (int i = 0; i < num_processes; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n", processes[i].pid, processes[i].arrival_time, processes[i].burst_time, processes[i].waiting_time, processes[i].turnaround_time, processes[i].completion_time);
        total_wait += processes[i].waiting_time;
        total_turn += processes[i].turnaround_time;
    }

    printf("\n평균 대기시간: %.2f\n", total_wait / num_processes);
    printf("평균 반환시간: %.2f\n", total_turn / num_processes);

    Comp_Average_waiting_time[2] = total_wait / num_processes; 
    Comp_Average_turnaround_time[2] = total_turn / num_processes;

    free(is_completed);
    free(gantt);
}

void Priority_NonPreemptive() {
    int latest_arrival = 0;
    int total_burst = 0;

    for (int i = 0; i < num_processes; i++) {
        if (processes[i].arrival_time > latest_arrival){
            latest_arrival = processes[i].arrival_time;
        }
        total_burst += processes[i].burst_time;
    }

    int max_possible_time = latest_arrival + total_burst;
    
    int* gantt = (int*)malloc(sizeof(int) * (max_possible_time + 1));

    if (gantt == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }
    for (int i = 0; i < max_possible_time +1; i++) {
        gantt[i] = 0;
    }
    //가장 짭은 실행시간 찾으려면 위에서부터 비교
    int max_priority=0;
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].priority > max_priority){
            max_priority = processes[i].priority;
        }
    }


    int current_time = 0, completed = 0;
    int* is_completed = (int*)malloc(sizeof(int) * num_processes);
    if (is_completed == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }
    for (int i = 0; i < num_processes; i++) {
        is_completed[i] = 0;
    }

    while (completed != num_processes) {
        int p_index = -1;
        int min_priority = max_priority+1;

        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= current_time && is_completed[i] == 0) {
                if (processes[i].priority < min_priority) {
                    min_priority = processes[i].priority;
                    p_index = i;
                }
                else if (processes[i].priority == min_priority) {
                    if (processes[i].arrival_time < processes[p_index].arrival_time) {
                        p_index = i;
                    }
                }
            }
        }

        if (p_index != -1) {
            for (int t = current_time; t < current_time + processes[p_index].burst_time; t++) {
                gantt[t] = processes[p_index].pid;
            }

            processes[p_index].waiting_time = current_time - processes[p_index].arrival_time;
            current_time += processes[p_index].burst_time;
            processes[p_index].completion_time = current_time;
            processes[p_index].turnaround_time = processes[p_index].completion_time - processes[p_index].arrival_time;

            is_completed[p_index] = 1;
            completed++;

        } else {
            gantt[current_time] = 0;
            current_time++;
        }
    }

    printf("\nPriority (Non-Preemptive) 간트차트\n");
    printf("Time : ");
    for (int t = 0; t < current_time; t++) printf("%2d ", t);
    printf("\nProcess: ");
    int last_pid = -1;
    for (int t = 0; t < current_time; t++) {
        if (gantt[t] == 0) printf(" - ");
        else {
            if (gantt[t] != last_pid && last_pid != -1) printf("| ");
            printf("P%d ", gantt[t]);
            last_pid = gantt[t];
        }
    }
    printf("| End\n");

    printf("\nPID\tArrival\tBurst\tPriority\tWaiting\tTurnaround\tCompletion\n");
    float total_wait = 0, total_turn = 0;
    for (int i = 0; i < num_processes; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\t%d\n", processes[i].pid, processes[i].arrival_time, processes[i].burst_time, processes[i].priority, processes[i].waiting_time, processes[i].turnaround_time, processes[i].completion_time);
        total_wait += processes[i].waiting_time;
        total_turn += processes[i].turnaround_time;
    }

    printf("\n평균 대기시간: %.2f\n", total_wait / num_processes);
    printf("평균 반환시간: %.2f\n", total_turn / num_processes);

    Comp_Average_waiting_time[3] = total_wait / num_processes; 
    Comp_Average_turnaround_time[3] = total_turn / num_processes;

    free(is_completed);
    free(gantt);
}


void Priority_Preemptive() {
    int latest_arrival = 0;
    int total_burst = 0;

    for (int i = 0; i < num_processes; i++) {
        if (processes[i].arrival_time > latest_arrival){
            latest_arrival = processes[i].arrival_time;
        }
        total_burst += processes[i].burst_time;
    }

    int max_possible_time = latest_arrival + total_burst;
    
    int* gantt = (int*)malloc(sizeof(int) * (max_possible_time + 1));

    if (gantt == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }
    for (int i = 0; i < max_possible_time +1; i++) {
        gantt[i] = 0;
    }

    int max_priority=0;
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].priority > max_priority){
            max_priority = processes[i].priority;
        }
    }



    int current_time = 0, completed = 0;
    int* is_completed = (int*)malloc(sizeof(int) * num_processes);
    if (is_completed == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }
    for (int i = 0; i < num_processes; i++) {
        is_completed[i] = 0;
    }

    while (completed != num_processes) {
        int p_index = -1;
        int min_priority = max_priority+1;

        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= current_time && is_completed[i] == 0) {
                if (processes[i].priority < min_priority) {
                    min_priority = processes[i].priority;
                    p_index = i;
                }
                else if (processes[i].priority == min_priority) {
                    if (processes[i].arrival_time < processes[p_index].arrival_time) {
                        p_index = i;
                    }
                }
            }
        }

        if (p_index != -1) {
            gantt[current_time] = processes[p_index].pid;
            processes[p_index].remaining_time--;

            if (processes[p_index].remaining_time == 0) {
                processes[p_index].completion_time = current_time + 1;
                processes[p_index].turnaround_time = processes[p_index].completion_time - processes[p_index].arrival_time;
                processes[p_index].waiting_time = processes[p_index].turnaround_time - processes[p_index].burst_time;

                is_completed[p_index] = 1;
                completed++;
            }
        } else {
            gantt[current_time] = 0; 
        }

        current_time++;
    }

    // 간트차트 출력
    printf("\nPriority (Preemptive) 간트차트\n");
    printf("Time:    ");
    for (int t = 0; t < current_time; t++) printf("%2d ", t);
    printf("\nProcess: ");
    int last_pid = -1; //last_pid는 pid의 마지막을 구분짓기 위해서 넣어줌
    for (int t = 0; t < current_time; t++) {
        if (gantt[t] == 0) printf(" - ");
        else {
            if (gantt[t] != last_pid && last_pid != -1) printf("| ");
            printf("P%d ", gantt[t]);
            last_pid = gantt[t];
        }
    }
    printf("| End\n");

    printf("\nPID\tArrival\tBurst\tWaiting\tTurnaround\tCompletion\n");
    float total_wait = 0, total_turn = 0;
    for (int i = 0; i < num_processes; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n", processes[i].pid, processes[i].arrival_time, processes[i].burst_time, processes[i].waiting_time, processes[i].turnaround_time, processes[i].completion_time);
        total_wait += processes[i].waiting_time;
        total_turn += processes[i].turnaround_time;
    }

    printf("\n평균 대기시간: %.2f\n", total_wait / num_processes);
    printf("평균 반환시간: %.2f\n", total_turn / num_processes);

    Comp_Average_waiting_time[4] = total_wait / num_processes; 
    Comp_Average_turnaround_time[4] = total_turn / num_processes;

    free(is_completed);
    free(gantt);
}


void RoundRobin(int time_quantum) {
    int latest_arrival = 0;
    int total_burst = 0;

    for (int i = 0; i < num_processes; i++) {
        if (processes[i].arrival_time > latest_arrival){
            latest_arrival = processes[i].arrival_time;
        }
        total_burst += processes[i].burst_time;
    }

    int max_possible_time = latest_arrival + total_burst;
    
    int* gantt = (int*)malloc(sizeof(int) * (max_possible_time + 1));

    if (gantt == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }
    for (int i = 0; i < max_possible_time +1; i++) {
        gantt[i] = 0;
    }

    int max_burst_time=0;
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].burst_time > max_burst_time){
            max_burst_time = processes[i].burst_time;
        }
    }



    int current_time = 0, completed = 0;
    int* is_completed = (int*)malloc(sizeof(int) * num_processes);
    if (is_completed == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }
    for (int i = 0; i < num_processes; i++) {
        is_completed[i] = 0;
    }

    //여기까진 앞과 유사하는데 이제 큐 써야 함
    
    int queue_size= total_burst/time_quantum + latest_arrival +1;
    int* queue = (int*)malloc(sizeof(int) * queue_size);
    int front = 0, rear = 0;

    // 첫 도착 프로세스 큐에 넣기->하지만 모든 큐의 도착시간이 0이 아닐 수도 있음.
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].arrival_time == 0) {
            queue[rear++] = i;
        }
    }

    while (completed != num_processes) {
        // 모든 큐의 도착시간이 0이 아닐때는 간트에 모두 0이 기록되도록 
        if (front == rear) {
            gantt[current_time] = 0; 
            current_time++;

            // 새로 도착한 프로세스 체크
            for (int i = 0; i < num_processes; i++) {
                if (processes[i].arrival_time == current_time) {
                    queue[rear++] = i;
                }
            }
            continue;
        }

        //넣어줬으면 넣은 순서대로 나와야 하니까 프론트는 방출용

        int p_index = queue[front++];

        //남은 시간이 있을 때 첫번째는 타임퀀텀보다 짧은 경우, 두번째는 길어서 다시 큐로 ㄱ
        //그니까 어떻게 할 지 결정해야 함

        if (processes[p_index].remaining_time > 0) {
            
            int rufwjd_time;
            if (processes[p_index].remaining_time < time_quantum) {
                rufwjd_time = processes[p_index].remaining_time;
            } else {
                rufwjd_time = time_quantum;
            }

            for (int t = 0; t < rufwjd_time; t++) {
                gantt[current_time] = processes[p_index].pid;
                current_time++;

                for (int i = 0; i < num_processes; i++) {
                    if (i != p_index && processes[i].arrival_time == current_time) {
                        queue[rear++] = i;
                    }
                }
            }

            processes[p_index].remaining_time = processes[p_index].remaining_time-rufwjd_time;

            if (processes[p_index].remaining_time == 0) {
                processes[p_index].completion_time = current_time;
                processes[p_index].turnaround_time = processes[p_index].completion_time - processes[p_index].arrival_time;
                processes[p_index].waiting_time = processes[p_index].turnaround_time - processes[p_index].burst_time;

                is_completed[p_index] = 1;
                completed++;
            } else {
                queue[rear++] = p_index; // 다시 큐에 넣기
            }
        }
    }


    // 간트차트 출력
    printf("\nRound Robin 간트차트 (Time Quantum = %d)\n", time_quantum);
    printf("Time:    ");
    for (int t = 0; t < current_time; t++) printf("%2d ", t);
    printf("\nProcess: ");
    int last_pid = -1; //last_pid는 pid의 마지막을 구분짓기 위해서 넣어줌
    for (int t = 0; t < current_time; t++) {
        if (gantt[t] == 0) printf(" - ");
        else {
            if (gantt[t] != last_pid && last_pid != -1) printf("| ");
            printf("P%d ", gantt[t]);
            last_pid = gantt[t];
        }
    }
    printf("| End\n");

    printf("\nPID\tArrival\tBurst\tWaiting\tTurnaround\tCompletion\n");
    float total_wait = 0, total_turn = 0;
    for (int i = 0; i < num_processes; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n", processes[i].pid, processes[i].arrival_time, processes[i].burst_time, processes[i].waiting_time, processes[i].turnaround_time, processes[i].completion_time);
        total_wait += processes[i].waiting_time;
        total_turn += processes[i].turnaround_time;
    }

    printf("\n평균 대기시간: %.2f\n", total_wait / num_processes);
    printf("평균 반환시간: %.2f\n", total_turn / num_processes);

    Comp_Average_waiting_time[5] = total_wait / num_processes; 
    Comp_Average_turnaround_time[5] = total_turn / num_processes;

    free(is_completed);
    free(queue);
    free(gantt);
}

void Evaluation() {

    char* algo_names[6] = {"FCFS", "SJF_비선점", "SJF_선점", "Priority_비선점", "Priority_선점", "RR"};
    float temp_wait[6], temp_turn[6];
    char* temp_wait_names[6], *temp_turn_names[6];

    int count = 0;

    for (int i = 0; i < 6; i++) {
        if (executed_algorithms[i] == 1) {
            temp_wait[count] = Comp_Average_waiting_time[i];
            temp_turn[count] = Comp_Average_turnaround_time[i];
            temp_wait_names[count] = algo_names[i];
            temp_turn_names[count] = algo_names[i];
            count++;
        }
        }
        if (count == 0) {
                printf("\n아직 실행된 알고리즘이 없습니다!\n");
                return;
            }

            // 평균 대기시간 정렬
            for (int i = 0; i < count-1; i++) {
                for (int j = i+1; j < count; j++) {
                    if (temp_wait[i] > temp_wait[j]) {
                        float temp_f = temp_wait[i]; 
                        temp_wait[i] = temp_wait[j]; 
                        temp_wait[j] = temp_f;
                        char* temp_n = temp_wait_names[i]; 
                        temp_wait_names[i] = temp_wait_names[j]; 
                        temp_wait_names[j] = temp_n;
                    }
                }
            }
            printf("\nAverage Waiting Time 짧은 순서부터 나열\n");
            for (int i = 0; i < count; i++) {
                printf("%s (%.2f)\n", temp_wait_names[i], temp_wait[i]);
            }

            // 평균 턴어라운드 정렬
            for (int i = 0; i < count-1; i++) {
                for (int j = i+1; j < count; j++) {
                    if (temp_turn[i] > temp_turn[j]) {
                        float temp_f = temp_turn[i]; 
                        temp_turn[i] = temp_turn[j]; 
                        temp_turn[j] = temp_f;
                        char* temp_n = temp_turn_names[i]; 
                        temp_turn_names[i] = temp_turn_names[j]; 
                        temp_turn_names[j] = temp_n;
                    }
                }
            }
            printf("\nAverage Turnaround Time 짧은 순서부터 나열\n");
            for (int i = 0; i < count; i++) {
                printf("%s (%.2f)\n", temp_turn_names[i], temp_turn[i]);
            }
        }

void Free_Memory() {
    if (processes != NULL) {
        free(processes);
    }
}

int main() {
    Create_Process();  
    Schedule_Menu();    
    Evaluation();   
    Free_Memory(); // 메모리 해제
    return 0;
}