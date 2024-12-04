#include "display.h"
#include "io.h"
#define MAX_MESSAGES 5  // 최대 표시할 메시지 수
#define MESSAGE_LENGTH 80  // 메시지 길이 제한

char message_log[MAX_MESSAGES][MESSAGE_LENGTH];  // 메시지 로그 버퍼
int message_count = 0;  // 현재 메시지 수
int message_colors[MAX_MESSAGES]; // 메시지 색상 정보 저장

// 화면 버퍼
char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };  // 새로운 프레임을 저장할 버퍼
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH];     // 맵 레이어 저장
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // 현재 화면에 표시 중인 버퍼
OBJECT_SAND worm1;
OBJECT_SAND worm2;

// 위치 설정
const POSITION resource_pos = { 0, 0 };                          // 자원 상태 표시 위치
const POSITION map_pos = { 1, 0 };                               // 맵 표시 위치
const POSITION system_message_pos = { MAP_HEIGHT + 2, 0 };       // 시스템 메시지 표시 위치
const POSITION object_info_pos = { 2, MAP_WIDTH + 2 };           // 오른쪽 상단 상태창 위치
const POSITION commands_pos = { MAP_HEIGHT + 2, MAP_WIDTH + 1 }; // 오른쪽 하단 명령창 위치
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
void copy_back_to_front(void);
void update_display(void);
void clear_line(POSITION pos, int length, int lines);
void add_system_message(const char* message, int type);
void display_system_message(void);

// 색상을 설정하는 함수
extern void set_color(int color);

// 개체별 색상 설정 함수
void set_object_color(char object, int row, int col) {
    if (object == 'B') {
        if ((row >= 15 && row <= 16) && (col >= 1 && col <= 2)) {
            set_color(COLOR_WHITE_ON_BLUE); // PLAYER의 Base는 파란 배경에 흰 글자
        }
        else if ((row >= 1 && row <= 2) && (col >= 57 && col <= 58)) {
            set_color(COLOR_WHITE_ON_RED); // AI의 Base는 빨간 배경에 흰 글자
        }
        else {
            set_color(COLOR_WHITE_ON_BLACK); // 기본 색상
        }
    }
    else if (object == 'H') {
        if ((row == 14 && col == 1) || (row == 14 && col == 2)) {
            set_color(COLOR_WHITE_ON_BLUE); // PLAYER의 Harvester는 파란 배경에 흰 글자
        }
        else if ((row == 3 && col == 57) || (row == 3 && col == 58)) {
            set_color(COLOR_WHITE_ON_RED); // AI의 Harvester는 빨간 배경에 흰 글자
        }
        else {
            set_color(COLOR_WHITE_ON_BLACK); // 기본 색상
        }
    }
    else if (object == 'P') {
        set_color(COLOR_WHITE_ON_BLACK); // 장판(Plate)은 흰색 배경에 검은 글자
    }
    else if (object == 'R') {
        set_color(COLOR_WHITE_ON_GRAY); // 바위(Rock)은 흰 글자, 회색 배경
    }
    else if ( isdigit(object)) {
        set_color(COLOR_YELLOW_ON_RED); // 스파이스는 빨간 글자, 노란 배경 (주황색 구현 불가)
    }
    else if (object == 'W') {
        set_color(COLOR_WHITE_ON_YELLOW); // Worm은 노란 배경에 흰 글자
    }
    else {
        set_color(COLOR_WHITE_ON_BLACK); // 기본 색상 (흰 글자, 검은 배경)
    }
}

// W 표시 및 커서 표시 함수
void display_worms(void) {
    // worm1 표시
    gotoxy(padd(map_pos, worm1.pos));
    set_color(COLOR_WHITE_ON_YELLOW);
    printf("%c", worm1.repr);

    // worm2 표시
    gotoxy(padd(map_pos, worm2.pos));
    set_color(COLOR_WHITE_ON_YELLOW);
    printf("%c", worm2.repr);

    set_color(COLOR_WHITE_ON_BLACK);
}

// backbuf를 frontbuf로 복사하는 함수
void copy_back_to_front() {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            frontbuf[i][j] = backbuf[i][j];
        }
    }
}

// 변경된 부분만 업데이트하는 함수
void update_display() {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (backbuf[i][j] != frontbuf[i][j]) {  // 버퍼 내용이 다를 때만 업데이트
                char object = backbuf[i][j];
                set_object_color(object, i, j);
                gotoxy((POSITION) { i + map_pos.row, j + map_pos.column });
                printf("%c", object);
            }
        }
    }
    
    copy_back_to_front();  // 업데이트 후 backbuf를 frontbuf로 복사
    set_color(COLOR_WHITE_ON_BLACK);  // 기본 색상으로 복구
    
}

// 맵을 화면에 출력하는 함수 (backbuf에 내용 저장)
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            for (int k = 0; k < N_LAYER; k++) {
                if (src[k][i][j] >= 0) {
                    dest[i][j] = src[k][i][j];
                }
            }
        }
    }
}

void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    project(map, backbuf);

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (frontbuf[i][j] != backbuf[i][j]) {
                char object = backbuf[i][j];
                set_object_color(object, i, j);  // 개체별 색상 설정
                gotoxy((POSITION) { i + map_pos.row, j + map_pos.column });
                printf("%c", object);
            }
            frontbuf[i][j] = backbuf[i][j];
        }
    }
}

// 특정 줄을 지우는 함수
void clear_line(POSITION pos, int length, int lines) {
    for (int i = 0; i < lines; i++) {
        gotoxy((POSITION) { pos.row + i, pos.column });
        for (int j = 0; j < length; j++) {
            printf(" ");
        }
    }
    gotoxy(pos);  // 원래 위치로 복구
}

// 커서 위치 표시 함수 (기존 문자를 그대로 유지하면서 커서 표시)
void display_cursor(CURSOR cursor) {
    POSITION prev = cursor.previous;
    POSITION curr = cursor.current;

    // 이전 위치의 문자와 색상 복원
    char prev_char = backbuf[prev.row][prev.column];  // 이전 위치의 원래 문자
    set_object_color(prev_char, prev.row, prev.column);  // 이전 위치의 색상 복원
    gotoxy((POSITION) { prev.row + map_pos.row, prev.column + map_pos.column });
    printf("%c", prev_char);

    // 현재 위치에 커서 표시 (흰색 배경, 검정 글씨)
    char curr_char = backbuf[curr.row][curr.column];  // 현재 위치의 원래 문자
    set_color(COLOR_BLACK_ON_WHITE);  // 커서 색상: 흰 배경, 검정 글씨
    gotoxy((POSITION) { curr.row + map_pos.row, curr.column + map_pos.column });
    printf("%c", curr_char);

    // 기본 색상으로 복구
    set_color(COLOR_WHITE_ON_BLACK);
}
// 시스템 메시지를 화면에 표시하는 함수
void display_system_message() {
    gotoxy((POSITION) { system_message_pos.row - 1, system_message_pos.column });
    printf("┌──────────────────────────────────────────────────────┐");
    gotoxy((POSITION) { system_message_pos.row + MAX_MESSAGES, system_message_pos.column });
    printf("└──────────────────────────────────────────────────────┘");
    clear_line(system_message_pos, 60, MAX_MESSAGES); // 메시지 영역 초기화 후 표시
    for (int i = 0; i < message_count; i++) {
        set_color(message_colors[i]); // 각 메시지에 맞는 색상 설정
        gotoxy((POSITION) { system_message_pos.row + i, system_message_pos.column });
        printf(" %-60s", message_log[i]); // 각 메시지를 60자로 제한하여 세로로 출력
    }
    set_color(COLOR_WHITE_ON_BLACK); // 기본 색상 복구
}

// 시스템 메시지 추가 및 스크롤링 처리
void add_system_message(const char* message, int type) {
    // 메시지가 MAX_MESSAGES 이상일 때 가장 오래된 메시지를 제거하고 위로 스크롤
    if (message_count >= MAX_MESSAGES) {
        // 모든 메시지를 한 칸씩 앞으로 이동
        for (int i = 1; i < MAX_MESSAGES; i++) {
            strncpy(message_log[i - 1], message_log[i], MESSAGE_LENGTH);
            message_colors[i - 1] = message_colors[i]; // 색상 정보도 함께 이동
        }
        // 마지막 위치에 새 메시지 추가
        strncpy(message_log[MAX_MESSAGES - 1], message, MESSAGE_LENGTH - 1);
        message_log[MAX_MESSAGES - 1][MESSAGE_LENGTH - 1] = '\0';
        message_colors[MAX_MESSAGES - 1] = type; // 마지막 위치에 색상 정보 추가
    }
    else {
        // MAX_MESSAGES에 도달하기 전까지는 새로운 메시지를 순차적으로 추가
        strncpy(message_log[message_count], message, MESSAGE_LENGTH - 1);
        message_log[message_count][MESSAGE_LENGTH - 1] = '\0';
        message_colors[message_count] = type; // 색상 정보 저장
        message_count++;
    }

    // 메시지 타입에 따른 색상 설정
    switch (type) {
    case 0: message_colors[message_count - 1] = COLOR_WHITE_ON_BLACK; break; // 기본 메시지
    case 1: message_colors[message_count - 1] = COLOR_RED; break;            // 오류 메시지
    case 2: message_colors[message_count - 1] = COLOR_YELLOW; break;         // 경고 메시지
    case 3: message_colors[message_count - 1] = COLOR_GREEN; break;          // 정보 메시지
    }

    // 메시지 갱신하여 화면에 표시
    display_system_message();
}

// 상태창: 선택된 유닛 또는 건물의 정보를 표시
void display_object_info(char symbol, CURSOR cursor) {
    // 상태창 헤더 출력
    gotoxy((POSITION) { object_info_pos.row - 1, object_info_pos.column });
    printf("┌────────────────────────────────── 상태창 ──────────────────────────────────┐");

    clear_line(object_info_pos, 80, 6);  // 상태창 초기화

    char unitSymbol = map[1][cursor.current.row][cursor.current.column];
    char terrainSymbol = map[0][cursor.current.row][cursor.current.column];  // 지형 정보 확인

    // 유닛 정보 표시
    for (int i = 0; i < UNIT_COUNT; i++) {
        if (units[i].symbol == unitSymbol) {
            gotoxy(object_info_pos);
            printf("      < 유닛 > 이름: %s, 체력: %d", units[i].name, units[i].health);

            gotoxy((POSITION) { object_info_pos.row + 1, object_info_pos.column });
            printf("      공격력: %d, 이동 주기: %d", units[i].attack_damage, units[i].move_period);
            return;
        }
    }

    // 건물 정보 표시
    for (int i = 0; i < BUILDING_COUNT; i++) {
        if (buildings[i].symbol == symbol) {
            gotoxy(object_info_pos);
            printf("< 건물 정보 > 이름: %s, 비용: %d, 설명: %s\n",
                buildings[i].name, buildings[i].cost, buildings[i].description);
            return;
        }
    }

    // 지형 정보에 따른 처리
    if (terrainSymbol == 'P') {  // Plate가 깔려있으면
        gotoxy(object_info_pos);
        printf("      <건물 지을 수 있음> Plate가 깔려 있습니다.\n");
        return;
    }
    else if (terrainSymbol == 'R') {  // Rock이면 건물을 지을 수 없음
        gotoxy(object_info_pos);
        printf("      <건물 지을 수 없음> 이곳은 Rock 지형으로 건물을 지을 수 없습니다.\n");
        return;
    }

    // Plate도 없고, Rock도 아니면 Plate를 설치하라는 메시지
    gotoxy(object_info_pos);
    printf("      <건물 지을 수 없음> Plate를 우선 설치해주십시오.\n");
}


// 명령창: 선택된 유닛 또는 건물의 명령어 표시
void display_commands(char symbol, char unitSymbol) {
    gotoxy((POSITION){commands_pos.row - 1, commands_pos.column});
    printf("┌────────────────────────────────── 명령창 ──────────────────────────────────┐");
    clear_line(commands_pos, 80, 1);  // 명령창 초기화

    // 유닛 명령어 표시
    for (int i = 0; i < UNIT_COUNT; i++) {
        if (units[i].symbol == unitSymbol) {
            gotoxy(commands_pos);
            printf("  명령어: %s\n", units[i].command);
            return;
        }
    }

    // 건물 명령어 표시
    for (int i = 0; i < BUILDING_COUNT; i++) {
        if (buildings[i].symbol == symbol) {
            gotoxy(commands_pos);
            printf("  명령어: %s\n", buildings[i].command);
            return;
        }
    }

    // 기본 명령어 안내
    gotoxy(commands_pos);
    printf("     [스페이스]: 선택, [방향키]: 이동, [ESC]: 취소");
}

// 자원 상태를 표시하는 함수
void display_resource(RESOURCE resource) {
    set_color(COLOR_BLACK_ON_WHITE);  // 흰색 배경, 검정 글씨로 설정
    gotoxy(resource_pos);
    printf("spice = %d/%d, population=%d/%d\n",
           resource.spice, resource.spice_max,
           resource.population, resource.population_max);
    set_color(COLOR_WHITE_ON_BLACK);  // 기본 색상 복구
}

// 메인 display 함수
void display(RESOURCE resource, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], CURSOR cursor) {
    display_resource(resource);
    display_map(map);
    display_cursor(cursor); 
    display_system_message(); // 시스템 메시지 재출력
    int should_update_status = 0;
    

    // 유닛 또는 건물 상태와 명령어 창 표시
    if (should_update_status) {
        char symbol = map[0][cursor.current.row][cursor.current.column];
        char unitSymbol = map[1][cursor.current.row][cursor.current.column];
        display_object_info(symbol, cursor);   // 상태창에 정보 표시
        display_commands(symbol, unitSymbol);  // 명령창에 명령어 표시
        should_update_status = 0;  // 상태 업데이트 플래그 초기화
    }
    update_display();
}