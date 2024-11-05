#include "display.h"
#include "io.h"

#define COLOR_WHITE_ON_BLACK 15       // 흰 글자, 검은 배경
#define COLOR_BLACK_ON_WHITE 240      // 검은 글자, 흰 배경
#define COLOR_WHITE_ON_BLUE 31        // 흰 글자, 파란 배경
#define COLOR_WHITE_ON_RED 79         // 흰 글자, 빨간 배경
#define COLOR_WHITE_ON_YELLOW 111     // 흰 글자, 노란 배경
#define COLOR_WHITE_ON_GRAY 112       // 흰 글자, 회색 배경

// 건물 정보 배열
BUILDING buildings[] = {
    {'B', "Base", "없음", 50, 0, "H: 하베스터 생산"},
    {'P', "Plate", "건물 짓기 전에 깔기", 1, 0, "없음"},
    {'D', "Dormitory", "인구 최대치 증가(10)", 2, 10, "없음"},
    {'G', "Garage", "스파이스 보관 최대치 증가(10)", 4, 10, "없음"},
    {'B', "Barracks", "보병 생산", 4, 20, "보병 생산(S: Soldier)"},
    {'S', "Shelter", "특수유닛 생산", 5, 30, "프레멘 생산(F: Fremen)"},
    {'A', "Arena", "투사 생산", 3, 15, "투사 생산(F: Fighter)"},
    {'F', "Factory", "특수유닛 생산", 5, 30, "중전차 생산(T: heavy Tank)"}
};

// 유닛 정보 배열
UNIT units[] = {
    {'H', "Harvester", 5, 5, 2000, 0, 0, 70, 0, "H: Harvest, M: Move"},
    {'F', "Fremen", 5, 2, 400, 15, 200, 25, 8, "M: 이동, P: 순찰"},
    {'S', "Soldier", 1, 1, 1000, 5, 800, 15, 1, "M: 이동"},
    {'F', "Fighter", 1, 1, 1200, 6, 600, 10, 1, "M: 이동"},
    {'T', "Heavy Tank", 12, 5, 3000, 40, 4000, 60, 4, "M: 이동"},
    {'W', "Sandworm", 0, 0, 2500, 0, 10000, 0, 0, "없음"}
};

// 화면 버퍼


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };  // 새로운 프레임을 저장할 버퍼
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]; // 맵 레이어 저장
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // 현재 화면에 표시 중인 버퍼

// 위치 설정
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
const POSITION system_message_pos = { MAP_HEIGHT + 1, 0 };
const POSITION object_info_pos = { MAP_HEIGHT + 1, 80 };
const POSITION commands_pos = { MAP_HEIGHT + 3, 80 };

// 색상을 설정하는 함수
void set_color(int color);

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
        if (row == 14 && col == 1) {
            set_color(COLOR_WHITE_ON_BLUE); // PLAYER의 Harvester는 파란 배경에 흰 글자
        }
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

// 건물 및 유닛 정보를 배열을 통해 조회하고 표시하는 함수
void display_building_info(char symbol) {
    for (int i = 0; i < sizeof(buildings) / sizeof(buildings[0]); i++) {
        if (buildings[i].symbol == symbol) {
            gotoxy(object_info_pos);
            printf("건물: %s\n설명: %s\n비용: %d\n내구도: %d\n명령어: %s\n",
                buildings[i].name, buildings[i].description,
                buildings[i].cost, buildings[i].durability,
                buildings[i].command);
            return;
        }
    }
    printf("건물 정보가 없습니다.\n");
}

// 유닛 정보를 표시하는 함수
void display_unit_info(char symbol) {
    for (int i = 0; i < sizeof(units) / sizeof(units[0]); i++) {
        if (units[i].symbol == symbol) {
            gotoxy(object_info_pos);
            printf("유닛: %s\n비용: %d\n인구수: %d\n이동 주기: %d\n공격력: %d\n공격 주기: %d\n"
                "체력: %d\n시야: %d\n명령어: %s\n",
                units[i].name, units[i].cost, units[i].population,
                units[i].move_period, units[i].attack_damage,
                units[i].attack_period, units[i].health,
                units[i].vision, units[i].command);
            return;
        }
    }
    printf("유닛 정보가 없습니다.\n");
}
// 오브젝트 정보 표시 함수 (건물 또는 유닛 정보 출력)
void display_object_info(char object) {
    gotoxy(object_info_pos); // 오브젝트 정보를 하단 오른쪽에 배치
    switch (object) {
    case 'B':
        printf("기지: HP=1000, 생산 속도=10");
        break;
    case 'H':
        printf("자원 수집기: 수집 속도=5");
        break;
    default:
        printf("해당 위치에 유닛/건물 정보가 없습니다.");
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
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            backbuf[i][j] = map[0][i][j];  // layer 0을 기준으로 맵을 backbuf에 저장
        }
    }
}

// 커서를 화면에 출력하는 함수 (backbuf에 내용 저장)
void display_cursor(CURSOR cursor) {
    // 이전 위치를 공백으로 표시
    gotoxy((POSITION) { cursor.previous.row + map_pos.row, cursor.previous.column + map_pos.column });
    printf(" ");

    // 현재 위치를 공백으로 표시
    gotoxy((POSITION) { cursor.current.row + map_pos.row, cursor.current.column + map_pos.column });
    printf(" ");

    set_color(COLOR_WHITE_ON_BLACK);  // 기본 색상으로 복구
}

void display_system_message(char symbol) {
    gotoxy(system_message_pos);
    switch (symbol) {
    case 'B': printf("Base selected: 기본 기지입니다."); break;
    case 'H': printf("Harvester selected: 자원을 수집합니다."); break;
    case 'P': printf("Plate selected: 건물을 놓을 수 있는 자리입니다."); break;
    default: printf("Empty");
    }
}


// 오브젝트 정보 표시 함수
void display_object_info(char object);

void display_commands() {
    gotoxy(commands_pos); // 명령어 안내를 하단 오른쪽에 배치
    printf("[스페이스]: 선택, [방향키]: 이동, [ESC]: 취소");
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

// 상단바를 표시하는 함수 (직접 출력)
void display_status_bar(int color) {
    set_color(color);
    gotoxy((POSITION) { 0, 0 });
    printf("Status: Player HP: 100 | Resources: Spice=50, Population=20\n");
    set_color(COLOR_WHITE_ON_BLACK);  // 기본 색상 복구
}

// 메인 display 함수
void display(
    RESOURCE resource,
    char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
    CURSOR cursor
) {
    display_resource(resource);
    display_map(map);
    display_cursor(cursor);
    display_system_message(map[0][cursor.current.row][cursor.current.column]);
    display_building_info(map[0][cursor.current.row][cursor.current.column]);
    display_commands();
    update_display();
}