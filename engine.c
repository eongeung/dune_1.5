﻿#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);
void update_worm_position(OBJECT_SAND* worm);
POSITION get_next_position(OBJECT_SAND* obj);
void choose_alternative_direction(OBJECT_SAND* worm, POSITION* next_pos); // 이 버전만 남깁니다.

/* ================= control =================== */
int sys_clock = 0;  // system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };

/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };
OBJECT_SAND worm1 = { {1, 1}, {MAP_HEIGHT - 2, MAP_WIDTH - 2}, 'W', 300, 300 };
OBJECT_SAND worm2 = { {5, 5}, {MAP_HEIGHT - 3, MAP_WIDTH - 3}, 'W', 300, 300 };

RESOURCE resource = {
    .spice = 0,
    .spice_max = 0,
    .population = 0,
    .population_max = 0
};

/* ================= main() =================== */
int main(void) {
    srand((unsigned int)time(NULL));

    init();
    intro();
    display(resource, map, cursor);

    while (1) {
        KEY key = get_key();

        if (is_arrow_key(key)) {
            cursor_move(ktod(key)); // 방향키로 커서 이동
        }
        else {
            switch (key) {
            case k_quit:
                outro();
            case k_none:
            case k_undef:
            default:
                break;
            }
        }

        // worm 오브젝트 동작
        update_worm_position(&worm1);
        update_worm_position(&worm2);

        // 화면 출력
        display(resource, map, cursor);
        Sleep(TICK);
        sys_clock += 10;
    }
}

/* ================= subfunctions =================== */
void intro(void) {
    printf("DUNE 1.5\n");
    Sleep(2000);
    system("cls");
}

void outro(void) {
    printf("exiting...\n");
    exit(0);
}

void init(void) {
    // layer 0(map[0])에 지형 생성
    for (int j = 0; j < MAP_WIDTH; j++) {
        map[0][0][j] = '#';
        map[0][MAP_HEIGHT - 1][j] = '#';
    }

    for (int i = 1; i < MAP_HEIGHT - 1; i++) {
        map[0][i][0] = '#';
        map[0][i][MAP_WIDTH - 1] = '#';
        for (int j = 1; j < MAP_WIDTH - 1; j++) {
            map[0][i][j] = ' ';
        }
    }

    // layer 1(map[1])은 비워 두기(-1로 채움)
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            map[1][i][j] = -1;
        }
    }

    //AI 및 플레이어 오브젝트 초기화
    map[0][1][58] = 'B';  map[0][1][57] = 'B';
    map[0][2][58] = 'B';  map[0][2][57] = 'B';
    map[0][1][56] = 'P';  map[0][1][55] = 'P';
    map[0][2][56] = 'P';  map[0][2][55] = 'P';
    map[0][3][58] = 'H';  // Harvester
    map[0][5][58] = '5';  // Spice

    map[0][15][1] = 'B';  map[0][15][2] = 'B';
    map[0][16][1] = 'B';  map[0][16][2] = 'B';
    map[0][15][3] = 'P';  map[0][15][4] = 'P';
    map[0][16][3] = 'P';  map[0][16][4] = 'P';
    map[0][14][1] = 'H';  // Harvester
    map[0][12][1] = '5';  // Spice

    map[1][worm1.pos.row][worm1.pos.column] = worm1.repr;
    map[1][worm2.pos.row][worm2.pos.column] = worm2.repr;
    map[0][12][37] = 'R';  // Rock1
    map[0][3][6] = 'R';  // Rock1
    map[0][8][49] = 'R';  map[0][8][50] = 'R';//Rock 2
    map[0][9][49] = 'R';  map[0][9][50] = 'R';
    map[0][6][23] = 'R';  map[0][6][24] = 'R';//Rock 2
    map[0][7][23] = 'R';  map[0][7][24] = 'R';

}

// 커서 이동 함수
void cursor_move(DIRECTION dir) {
    POSITION curr = cursor.current;
    POSITION new_pos = pmove(curr, dir);

    if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
        1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {
        cursor.previous = cursor.current;
        cursor.current = new_pos;
    }
}

/* ================= worm 이동 =================== */
void update_worm_position(OBJECT_SAND* worm) {
    if (sys_clock < worm->next_move_time) return;

    POSITION next_pos = get_next_position(worm);  // next_pos 설정

    // Rock 또는 벽(#)에 부딪혔을 경우
    if (map[0][next_pos.row][next_pos.column] == 'R' || map[0][next_pos.row][next_pos.column] == '#') {
        // Rock이나 벽에 막혀있는 경우 우회 방향을 선택
        choose_alternative_direction(worm, &next_pos);
    }

    // 이동 후 위치 업데이트
    map[1][worm->pos.row][worm->pos.column] = ' '; // 이전 위치 지움
    worm->pos = next_pos; // worm의 위치 업데이트
    map[1][worm->pos.row][worm->pos.column] = worm->repr; // 새로운 위치에 worm 표시

    worm->next_move_time = sys_clock + worm->speed; // 다음 이동 시간 설정
}

/* 다음 이동할 위치 계산 함수 */
POSITION get_next_position(OBJECT_SAND* obj) {
    POSITION diff = psub(obj->dest, obj->pos);
    DIRECTION preferred_dir;

    // 주 방향 설정: 목적지와의 거리 차이를 우선적으로 고려
    if (abs(diff.row) >= abs(diff.column)) {
        preferred_dir = (diff.row > 0) ? d_down : d_up;
    }
    else {
        preferred_dir = (diff.column > 0) ? d_right : d_left;
    }

    return pmove(obj->pos, preferred_dir);  // 다음 위치 반환
}

void choose_alternative_direction(OBJECT_SAND* worm, POSITION* next_pos) {
    POSITION curr_pos = worm->pos;

    // 상하좌우 이동 방향 설정
    DIRECTION directions[4] = { d_up, d_down, d_left, d_right };

    // 방향 무작위화
    for (int i = 0; i < 4; i++) {
        int j = rand() % 4;
        DIRECTION temp = directions[i];
        directions[i] = directions[j];
        directions[j] = temp;
    }

    // 이동할 수 있는 방향 찾기
    for (int i = 0; i < 4; i++) {
        POSITION new_pos = pmove(curr_pos, directions[i]);

        // 벽(#)이나 Rock('R')이 없는 위치를 찾으면 이동
        if (map[0][new_pos.row][new_pos.column] != 'R' &&
            map[0][new_pos.row][new_pos.column] != '#' &&
            new_pos.row >= 1 && new_pos.row < MAP_HEIGHT - 1 &&
            new_pos.column >= 1 && new_pos.column < MAP_WIDTH - 1) {
            *next_pos = new_pos;
            return;
        }
    }

    // 벽에 부딪혔을 경우 반대 방향으로 이동 시도
    if (curr_pos.row <= 1) {
        *next_pos = pmove(curr_pos, d_down); // 위쪽 벽에 부딪힘 -> 아래로 이동
    }
    else if (curr_pos.row >= MAP_HEIGHT - 2) {
        *next_pos = pmove(curr_pos, d_up); // 아래쪽 벽에 부딪힘 -> 위로 이동
    }
    else if (curr_pos.column <= 1) {
        *next_pos = pmove(curr_pos, d_right); // 왼쪽 벽에 부딪힘 -> 오른쪽으로 이동
    }
    else if (curr_pos.column >= MAP_WIDTH - 2) {
        *next_pos = pmove(curr_pos, d_left); // 오른쪽 벽에 부딪힘 -> 왼쪽으로 이동
    }
}
