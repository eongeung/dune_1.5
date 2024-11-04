#include "display.h"
#include "io.h"

#define COLOR_WHITE_ON_BLACK 15       // 흰 글자, 검은 배경
#define COLOR_BLACK_ON_WHITE 240      // 검은 글자, 흰 배경
#define COLOR_WHITE_ON_BLUE 31        // 흰 글자, 파란 배경
#define COLOR_WHITE_ON_RED 79         // 흰 글자, 빨간 배경
#define COLOR_WHITE_ON_YELLOW 111     // 흰 글자, 노란 배경
#define COLOR_WHITE_ON_GRAY 112       // 흰 글자, 회색 배경

// 화면 버퍼
char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };  // 새로운 프레임을 저장할 버퍼
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // 현재 화면에 표시 중인 버퍼

// 위치 설정
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };

// 색상을 설정하는 함수
void set_color(int color);

// 개체별 색상 설정 함수
void set_object_color(char object, int row, int col) {
    if (object == 'B') {
        // PLAYER의 Base (왼쪽 하단)
        if ((row >= 15 && row <= 16) && (col >= 1 && col <= 2)) {
            set_color(COLOR_WHITE_ON_BLUE); // PLAYER의 Base는 파란 배경에 흰 글자
        }
        // AI의 Base (오른쪽 상단)
        else if ((row >= 1 && row <= 2) && (col >= 57 && col <= 58)) {
            set_color(COLOR_WHITE_ON_RED); // AI의 Base는 빨간 배경에 흰 글자
        }
        else {
            set_color(COLOR_WHITE_ON_BLACK); // 기본 색상
        }
    }
    else if (object == 'H') {
        // PLAYER의 Harvester (왼쪽 하단)
        if (row == 14 && col == 1) {
            set_color(COLOR_WHITE_ON_BLUE); // PLAYER의 Harvester는 파란 배경에 흰 글자
        }
        // AI의 Harvester (오른쪽 상단)
        else if (row == 3 && col == 58) {
            set_color(COLOR_WHITE_ON_RED); // AI의 Harvester는 빨간 배경에 흰 글자
        }
        else {
            set_color(COLOR_WHITE_ON_BLACK); // 기본 색상
        }
    }
    else if (object == 'P') {
        set_color(COLOR_BLACK_ON_WHITE); // 장판(Plate)은 흰색 배경에 검은 글자
    }
    else if (object == 'R') {
        set_color(COLOR_WHITE_ON_GRAY); // 바위(Rock)은 흰 글자, 회색 배경
    }
    else if (object == 'W' || object == '5') {
        set_color(COLOR_WHITE_ON_YELLOW); // Worm과 Spice는 노란 배경에 흰 글자
    }
    else {
        set_color(COLOR_WHITE_ON_BLACK); // 기본 색상 (흰 글자, 검은 배경)
    }
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

                // 개체에 맞는 색상을 설정
                set_object_color(object, i, j);

                // 해당 위치로 이동하고 출력
                gotoxy((POSITION) { i + map_pos.row, j + map_pos.column });
                printf("%c", object);
            }
        }
    }
    copy_back_to_front();  // 업데이트 후 backbuf를 frontbuf로 복사
    set_color(COLOR_WHITE_ON_BLACK);  // 기본 색상으로 복구
}

// 맵을 화면에 출력하는 함수 (backbuf에 내용 저장)
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            backbuf[i][j] = map[0][i][j];  // layer 0을 기준으로 맵을 backbuf에 저장
        }
    }
}

// 커서를 화면에 출력하는 함수 (backbuf에 내용 저장)
void display_cursor(CURSOR cursor) {
    set_color(COLOR_BLACK_ON_WHITE);  // 커서를 흰 배경, 검은 글자로 표시
    backbuf[cursor.current.row][cursor.current.column] = 'O';  // 커서를 'O'로 표시
    set_color(COLOR_WHITE_ON_BLACK);  // 기본 색상으로 복구
}

// 상단바를 표시하는 함수 (직접 출력)
void display_status_bar(int color) {
    set_color(color);
    gotoxy((POSITION) { 0, 0 });
    printf("Status: Player HP: 100 | Resources: Spice=50, Population=20\n");
    set_color(COLOR_WHITE_ON_BLACK);  // 기본 색상 복구
}

// 자원 상태를 표시하는 함수 (직접 출력)
void display_resource(RESOURCE resource) {
    set_color(COLOR_BLACK_ON_WHITE);  // 흰색 배경, 검은 글씨로 설정
    gotoxy(resource_pos);
    printf("spice = %d/%d, population=%d/%d\n",
        resource.spice, resource.spice_max,
        resource.population, resource.population_max
    );
    set_color(COLOR_WHITE_ON_BLACK);  // 기본 색상 복구
}

// 메인 display 함수
void display(
    RESOURCE resource,
    char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],  // 매개변수로 map을 전달받음
    CURSOR cursor
) {
    display_status_bar(COLOR_WHITE_ON_BLACK);  // 상단바 표시
    display_resource(resource);                // 자원 상태 출력
    display_map(map);                          // 맵을 backbuf에 출력
    display_cursor(cursor);                    // 커서를 backbuf에 출력
    update_display();                          // 화면에 변경 사항 적용
}
