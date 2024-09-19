#include <stdio.h>
#include <conio.h>   
#include <windows.h> 

#define TICK 100  // 100ms 주기로 루프를 돌리기 위해 정의
#define MAX_CLOCK 100000  // 임시로 설정한 clock 최대 값
#define N_LAYER 2
#define MAP_WIDTH 60
#define MAP_HEIGHT 18
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

typedef struct {
    int row;
    int column;
} POSITION;

typedef enum {
    k_none = 0, k_up, k_right, k_left, k_down,
    k_quit,
    k_undef, // 정의되지 않은 키 입력
} KEY; //입력 키 정의

typedef enum {
    d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION; //이동방향

typedef struct {
    int spice; // 현재 보유한 스파이스
    int spice_max; // 스파이스 최대 저장량
    int population; // 현재 인구 수
    int population_max; // 수용 가능한 인구 수
} RESOURCE;


// 대강 만들어 봤음. 기능 추가하면서 각자 수정할 것
typedef struct {
    POSITION pos; // 현재 위치(position)
    POSITION dest; // 목적지(destination)
    char repr; // 화면에 표시할 문자(representation)
    int move_period; // '몇 ms마다 한 칸 움직이는지'를 뜻함
    int next_move_time; // 다음에 움직일 시간
} OBJECT_SAMPLE;


KEY get_key(void) {
    if (!_kbhit()) { // 입력된 키가 있는지 확인
        return k_none;
    }
    int byte = _getch(); // 입력된 키를 받음
    switch (byte) {
    case 'q': return k_quit;  // 'q'를 누르면 종료
    case 224:  // 방향키는 첫 바이트가 224
        byte = _getch(); // 두 번째 바이트를 읽음
        switch (byte) {
        case 72: return k_up;    // 위쪽 방향키
        case 80: return k_down;  // 아래쪽 방향키
        case 75: return k_left;  // 왼쪽 방향키
        case 77: return k_right; // 오른쪽 방향키
        default: return k_undef;
        }
    default: return k_undef;
    }
}

int main(void) {
    int clock_value = 0;  // 시간 값을 저장할 변수

    // 기본 게임 루프
    while (1) {
        KEY key = get_key();  // 키 입력 확인

        // 입력에 따라 처리
        switch (key) {
        case k_quit:
            printf("exiting...\n");
            return 0;  // 'q'를 누르면 게임 종료
        case k_up:
        case k_down:
        case k_left:
        case k_right:
            printf("키 입력: %d\n", key);  // 방향키 입력 확인
            break;
        default: break;
        }

        // 게임 루프를 제어 (TICK 간격)
        Sleep(TICK);  // TICK 주기로 루프를 멈춤
        clock_value += TICK;  // 시간 값 갱신

        // 게임 종료 조건
        if (clock_value >= MAX_CLOCK) {
            printf("게임이 종료되었습니다.\n");
            break;
        }
    }

    return 0;
}