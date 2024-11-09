﻿/* 진행사항 : 1) ~ 5) 완료,7).8) 부분 완료 

문제 애매한 사항
- spice 할 때, 주황색이 지원되지 않아 노랑색에 글씨 색깔을 바꿨습니다.

-- 부탁드립니다..--

시작화면일 때 꼭! 꼭! 꼭! 전체 화면으로 크기를 키워주세요
글씨가 길어서 전체 화면 안하면 맵이 깨지는 현상이 발생합니다..
  ++ 너무 빠르게 조작할 경우 버그가 발생할 수 있으니 마음에 여유를 가지고
     작동 시켜주시면 감사하겠습니다
    */

#include <stdlib.h>
#include <time.h>
#include "io.h"
#include "display.h"

void init(void);
void intro(void);
void outro(void);
//void handle_input(KEY key);
void cursor_move(DIRECTION dir, int steps);
void update_worm_position(OBJECT_SAND* worm);
void choose_alternative_direction(OBJECT_SAND* worm, POSITION* next_pos);
void handle_selection(KEY key);
void produce_unit(char unit_type, POSITION base_pos);
void handle_cancel(void);

bool is_position_empty(int row, int col);
bool is_unit_command(KEY key);
UNIT* get_selected_unit(POSITION pos);
POSITION find_nearby_empty_position(POSITION base_pos);
POSITION get_next_position(OBJECT_SAND* obj);

// 전역 변수로 베이스 그룹 초기화
BASE_GROUP base_groups[] = {
    { { {1, 57}, {1, 58}, {2, 57}, {2, 58} } },  // 첫 번째 베이스 좌표(하코넨)
    { { {15, 1}, {15, 2}, {16, 1}, {16, 2} } }   // 두 번째 베이스 좌표(플레이어)
};
int base_group_count = sizeof(base_groups) / sizeof(base_groups[0]);  // 그룹 수 계산

POSITION find_nearest_harvester(OBJECT_SAND* worm);
/* ================= control =================== */
int sys_clock = 0;  // system-wide clock(ms)
RESOURCE resource = { 95, 100, 5, 50 };
CURSOR cursor;
int should_update_status = 0;
KEY last_arrow_key = k_undef;
int last_arrow_time = 0;
int selection_active = 0;
int first_spice = 1; // 첫 스파이스 양 설정을 위한 플래그
int base_selected;

/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };
OBJECT_SAND worm1 = { {4, 4}, {MAP_HEIGHT - 2, MAP_WIDTH - 2}, 'W', 300, 300 };
OBJECT_SAND worm2 = { {12, 42}, {MAP_HEIGHT - 3, MAP_WIDTH - 3}, 'W', 300, 300 };

BUILDING buildings[] = {
    {'B', "Base", "없음", 50, 0, "H: 하베스터 생산"},
    {'P', "Plate", "건물 부지", 1, 0, "없음"},
    {'D', "Dormitory", "인구 최대치 증가(10)", 2, 10, "없음"},
    {'G', "Garage", "스파이스 보관 최대치 증가(10)", 4, 10, "없음"},
    {'B', "Barracks", "보병 생산", 4, 20, "보병 생산(S: Soldier)"},
    {'S', "Shelter", "특수유닛 생산", 5, 30, "프레멘 생산(F: Fremen)"},
    {'A', "Arena", "투사 생산", 3, 15, "투사 생산(F: Fighter)"},
    {'F', "Factory", "특수유닛 생산", 5, 30, "중전차 생산(T: heavy Tank)"}
};

UNIT units[] = {
    {'H', "Harvester", 5, 5, 2000, 0, 0, 70, 0, "H: Harvest, M: Move"},
    {'F', "Fremen", 5, 2, 400, 15, 200, 25, 8, "M: 이동, P: 순찰"},
    {'S', "Soldier", 1, 1, 1000, 5, 800, 15, 1, "M: 이동"},
    {'F', "Fighter", 1, 1, 1200, 6, 600, 10, 1, "M: 이동"},
    {'T', "Heavy Tank", 12, 5, 3000, 40, 4000, 60, 4, "M: 이동"},
    {'W', "Sandworm", 0, 0, 2500, 19827127, 10000, 19827127, 19827127, "없음"}
};


/* ================= main() =================== */
int main(void) {
    srand((unsigned int)time(NULL));

    init();
    intro();
    display(resource, map, cursor);


    while (1) {
        KEY key = get_key();
        key = tolower(key);

        // Worms는 독립적으로 자동으로 움직임
        if (sys_clock >= worm1.next_move_time) {
            update_worm_position(&worm1);
            display(resource, map, cursor);
        }
        if (sys_clock >= worm2.next_move_time) {
            update_worm_position(&worm2);
            display(resource, map, cursor);
        }

        // 방향키, 스페이스, ESC 키에 따른 추가 작업
        if (is_arrow_key(key)) {
            handle_double_click(key);
            display(resource, map, cursor);
        }
        else if (key == k_h && base_selected) {  // Base가 선택된 상태에서 H 키로 하베스터 생산
            produce_unit('H', cursor.current);  // 현재 커서 위치 (Base 위치)에서 유닛 생산
            display(resource, map, cursor);
        }
        else if (is_unit_command(key)) {  // 유닛 명령어 입력
            UNIT* selected_unit = get_selected_unit(cursor.current);  // 현재 선택된 유닛 가져오기
            if (selected_unit != NULL) {
                process_unit_commands(selected_unit, key);  // 유닛 명령어 처리
            }
        }
        else if (key == k_space) {  // 스페이스 키로 Base 선택 처리
            handle_selection(key);
            display(resource, map, cursor);

        }
        else if (key == ESC_KEY) {
            handle_cancel();  // ESC 키로 선택 취소
            display(resource, map, cursor);
        }
        else if (key == k_quit) {
            outro();
            display(resource, map, cursor);
        }

        if (should_update_status) {
            clear_line(object_info_pos, 80,6);  // 상태창 지우기
            display_object_info(map[0][cursor.current.row][cursor.current.column],cursor); // 현재 위치의 정보 출력
            should_update_status = 0; // 플래그 초기화
        }
        // 자원 및 인구 상태 표시
        display_resource(resource);

        Sleep(TICK);
        sys_clock += 10;
    }
}

/* ================= subfunctions =================== */ 

bool is_position_empty(int row, int col) {
    // map[1]이 빈 공간이면 true로 간주 (map[0] 확인 생략)
    return (map[0][row][col] == ' ' || map[0][row][col] == -1) &&
    (map[1][row][col] == ' ' || map[1][row][col] == -1);
}

bool is_unit_command(KEY key) {
    return (key == 'M' || key == 'P');  // 이동(M) 또는 순찰(P)이면 true 반환
}
UNIT* get_selected_unit(POSITION pos) {
    char unitSymbol = map[1][pos.row][pos.column];  // 현재 위치의 유닛 심볼 가져오기
    for (int i = 0; i < UNIT_COUNT; i++) {
        if (units[i].symbol == unitSymbol) {
            return &units[i];  // 심볼이 일치하는 유닛을 찾으면 해당 유닛의 주소 반환
        }
    }
    return NULL;  // 유닛이 없으면 NULL 반환
}

// 베이스 그룹에서 빈 위치를 찾는 함수
POSITION find_nearby_empty_position_from_group(POSITION selected_pos) {
    for (int i = 0; i < base_group_count; i++) {
        for (int j = 0; j < 4; j++) {
            POSITION pos = base_groups[i].positions[j];
            if (selected_pos.row == pos.row && selected_pos.column == pos.column) {
                // 선택된 위치가 현재 그룹에 속함
                for (int k = 0; k < 4; k++) {
                    POSITION spawn_pos = base_groups[i].positions[k];
                    POSITION empty_pos = find_nearby_empty_position(spawn_pos);
                    if (empty_pos.row != -1 && empty_pos.column != -1) {
                        return empty_pos;  // 빈 공간 위치 반환
                    }
                }
            }
        }
    }
    return (POSITION) { -1, -1 };  // 빈 공간을 찾지 못하면 -1 반환
}

POSITION find_nearby_empty_position(POSITION base_pos) {
    POSITION directions[4] = { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } }; // 상하좌우 방향

    for (int i = 0; i < 4; i++) {
        POSITION new_pos = { base_pos.row + directions[i].row, base_pos.column + directions[i].column };

        // 맵 경계 내, 빈 공간인지, 그리고 유닛 레이어에 아무 것도 없는지 확인
        if (new_pos.row >= 1 && new_pos.row < MAP_HEIGHT - 1 &&
            new_pos.column >= 1 && new_pos.column < MAP_WIDTH - 1 &&
            is_position_empty(new_pos.row, new_pos.column)) { // 유닛 레이어가 비어있는지 확인
            return new_pos;  // 빈 공간 위치 반환
        }
    }
    return (POSITION) { -1, -1 };  // 빈 공간을 찾지 못하면 -1 반환
}

// 기존 produce_unit 함수에서 빈 위치 찾기 부분 수정
void produce_unit(char unit_type, POSITION base_pos) {
    int required_spice = -1;
    int population_increase = 0;

    // 유닛 종류에 따라 필요한 스파이스 양 결정
    for (int i = 0; i < UNIT_COUNT; i++) {
        if (units[i].symbol == unit_type) {
            required_spice = units[i].cost;
            population_increase = units[i].population;
            break;
        }
    }

    if (required_spice == -1) {
        display_system_message("잘못된 유닛 유형입니다");
        return;
    }

    // 커서가 BASE 위에 있는지 확인
    bool is_cursor_on_base = false;
    for (int i = 0; i < base_group_count; i++) {
        for (int j = 0; j < 4; j++) {
            if (base_groups[i].positions[j].row == base_pos.row &&
                base_groups[i].positions[j].column == base_pos.column) {
                is_cursor_on_base = true;
                break;
            }
        }
        if (is_cursor_on_base) break;
    }

    // 커서가 BASE 위에 있을 때만 유닛 생성 가능
    if (!is_cursor_on_base) {
        display_system_message("기지 위에 커서가 있어야 유닛을 생성할 수 있습니다");
        return;
    }

    // 자원 확인 및 생산 가능 여부 체크
    if (resource.spice >= required_spice && resource.population < resource.population_max) {
        POSITION spawn_pos = find_nearby_empty_position_from_group(base_pos);  // Base 그룹 내 빈 공간 찾기

        if (spawn_pos.row != -1 && spawn_pos.column != -1) {
            resource.spice -= required_spice;               // 스파이스 차감
            resource.population += population_increase;     // population 값 증가
            map[1][spawn_pos.row][spawn_pos.column] = unit_type; // 빈 위치에 유닛 생성

            display_system_message("기지 근처에 새로운 유닛이 준비되었습니다");
        }
        else {
            display_system_message("기지 근처에 유닛을 생성할 빈 공간이 없습니다");
        }
    }
    else if (resource.spice < required_spice) {
        display_system_message("스파이스가 부족합니다");
    }
    else {
        display_system_message("인구 한도에 도달했습니다");
    }
}

void process_unit_commands(UNIT* unit, char command) {
    switch (unit->symbol) {
    case 'H':  // 하베스터
        if (command == 'H') {
            display_system_message("하베스터가 스파이스 채취를 시작합니다");
            // TODO: 스파이스 채취 로직 추가
        }
        else if (command == 'M') {
            display_system_message("하베스터가 이동 중입니다");
            // TODO: 이동 로직 추가
        }
        else {
            display_system_message("잘못된 명령어입니다");
        }
        break;

    case 'F':  // 프레멘
        if (command == 'P') {
            display_system_message("프레멘이 순찰을 시작합니다");
            // TODO: 순찰 로직 추가
        }
        else if (command == 'M') {
            display_system_message("프레멘이 이동 중입니다");
            // TODO: 이동 로직 추가
        }
        else {
            display_system_message("잘못된 명령어입니다");
        }
        break;

    default:
        display_system_message("이 유닛은 명령을 처리할 수 없습니다");
        break;
    }
}

void intro(void) {
    printf("DUNE 1.5\n");
    Sleep(5000);
    system("cls");
}

void outro(void) {
    printf("exiting...\n");
    exit(0);
}

void init(void) {
    cursor.previous.row = 1;
    cursor.previous.column = 1;
    cursor.current.row = 1;
    cursor.current.column = 1;

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

    //MAP-건들면 안됨 절대 안됨 맵 사라짐
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            map[1][i][j] = -1;
        }
    }
    // 각 베이스 그룹의 위치 초기화
    for (int i = 0; i < base_group_count; i++) {
        for (int j = 0; j < 4; j++) {
            POSITION pos = base_groups[i].positions[j];
            map[0][pos.row][pos.column] = 'B';  // 각 베이스의 위치에 'B' 표시
        }
    }
    //AI 및 플레이어 오브젝트 초기화
    map[0][1][56] = 'P';  map[0][1][55] = 'P';
    map[0][2][56] = 'P';  map[0][2][55] = 'P';
    map[1][3][58] = 'H';  // Harvester
    map[0][5][58] = '5';  // Spice

    //player
    map[0][15][3] = 'P';  map[0][15][4] = 'P';
    map[0][16][3] = 'P';  map[0][16][4] = 'P';
    map[1][14][1] = 'H';  // Harvester
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

void cursor_move(DIRECTION dir, int steps) {
    for (int i = 0; i < steps; i++) {
        POSITION curr = cursor.current;
        POSITION new_pos = pmove(curr, dir);


        // 맵의 경계를 넘지 않도록 체크
        if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
            1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {
            cursor.previous = cursor.current; // 이전 위치 저장
            cursor.current = new_pos;         // 현재 위치 업데이트

            // 매 이동마다 화면 갱신을 위해 display_cursor 호출
            display_cursor(cursor);
            Sleep(TICK);  // 이동 시 딜레이 적용하여 움직임 시각화
        }
    }
}

void handle_double_click(KEY key) {
    int now = sys_clock;
    int steps = 1;

    // 키가 이전에 눌린 키와 동일하고, 클릭 간의 시간 차이가 기준 이하일 때
    if (key == last_arrow_key && (now - last_arrow_time) < DOUBLE_CLICK_THRESHOLD) {
        steps  = 4;  // 두 번 누를 시 네 칸 이동
    }
    cursor_move(ktod(key), steps);  // 이동 처리
    last_arrow_key = key;
    last_arrow_time = now;
}

void handle_selection(KEY key) {
    should_update_status = 1;
   
    // 현재 커서 위치에 있는 유닛 또는 건물 심볼을 가져옴
    char symbol = map[0][cursor.current.row][cursor.current.column];
    char unitSymbol = map[1][cursor.current.row][cursor.current.column];

    //BASE->UNIT 생산
    base_selected = 0;  // 기본값으로 0 설정
    for (int i = 0; i < sizeof(buildings) / sizeof(buildings[0]); i++) {
        if (buildings[i].symbol == symbol && strcmp(buildings[i].name, "Base") == 0) {
            base_selected = 1;  // Base가 선택되었음을 표시
            break;
        }
    }


    // 선택된 객체 정보를 각 창에 전달만 함
    display_object_info(symbol, cursor);   // 상태창에 정보 전달
    display_commands(symbol, unitSymbol);  // 명령창에 명령어 전달
}

void handle_cancel() {
    selection_active = 0;  // 선택 상태 비활성화
    clear_line(object_info_pos, 80,6);  // 상태창 지우기
}

/* ================= worm 이동 =================== */
void update_worm_position(OBJECT_SAND* worm) {
    if (sys_clock < worm->next_move_time) return;

    // 가장 가까운 하베스터(H)를 찾아 이동할 위치 설정
    POSITION target_pos = find_nearest_harvester(worm);

    POSITION next_pos;
    if (target_pos.row == -1 && target_pos.column == -1) {
        // 타겟이 없으면 기존 방식대로 랜덤 이동
        next_pos = get_next_position(worm);
    }
    else {
        // 타겟이 있으면 그 방향으로 이동
        next_pos = worm->pos;
        if (target_pos.row < worm->pos.row) next_pos.row--;       // 위쪽 이동
        else if (target_pos.row > worm->pos.row) next_pos.row++;  // 아래쪽 이동

        if (target_pos.column < worm->pos.column) next_pos.column--;  // 왼쪽 이동
        else if (target_pos.column > worm->pos.column) next_pos.column++;  // 오른쪽 이동
    }

    // Rock, 벽(#), 또는 스파이스(숫자)에 부딪혔을 경우
    if (next_pos.row >= 0 && next_pos.row < MAP_HEIGHT && next_pos.column >= 0 && next_pos.column < MAP_WIDTH) {
        if (map[0][next_pos.row][next_pos.column] == 'R' || map[0][next_pos.row][next_pos.column] == '#' || isdigit(map[0][next_pos.row][next_pos.column])) {
            choose_alternative_direction(worm, &next_pos);
        }
    }

    // 하베스터를 만난 경우 처리
    if (next_pos.row >= 0 && next_pos.row < MAP_HEIGHT && next_pos.column >= 0 && next_pos.column < MAP_WIDTH) {
        if (map[1][next_pos.row][next_pos.column] == 'H') {
            // 해당 유닛을 잡아먹고 population 감소
            for (int i = 0; i < UNIT_COUNT; i++) {
                if (units[i].symbol == 'H') {  // Harvester의 symbol이 'H'라고 가정
                    resource.population -= units[i].population;  // population 감소
                    break;
                }
            }
            map[1][next_pos.row][next_pos.column] = -1;
            display_system_message("샌드웜이 하베스터를 잡아먹었습니다!");
        }
    }

    // 10% 확률로 샌드웜이 지나간 위치의 오른쪽에 스파이스 배설
    if (rand() % 100 < SPICE_PROBABILITY) {  // 0부터 99 사이의 난수로 10% 확률
        int spice_row = worm->pos.row;
        int spice_col = worm->pos.column + 1;  // 오른쪽에 배설
        if (spice_col < MAP_WIDTH) {           // 맵의 경계를 넘지 않도록 체크
            generate_spice_at_position(spice_row, spice_col);
        }
    }

    // 샌드웜이 이동하기 전 위치를 지우고, 다음 위치로 이동
    map[1][worm->pos.row][worm->pos.column] = ' '; // 이전 위치 지움
    worm->pos = next_pos; // worm의 위치 업데이트
    map[1][worm->pos.row][worm->pos.column] = worm->repr; // 새로운 위치에 worm 표시

    worm->next_move_time = sys_clock + worm->speed; // 다음 이동 시간 설정
}

/* 가장 가까운 하베스터(H) 위치를 찾는 함수 */
POSITION find_nearest_harvester(OBJECT_SAND* worm) {
    POSITION nearest_pos = { -1, -1 };
    int min_distance = MAP_WIDTH + MAP_HEIGHT;

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            char symbol = map[1][i][j];
            if (symbol == 'H') {  // 하베스터(H)만 추적
                int distance = abs(worm->pos.row - i) + abs(worm->pos.column - j);
                if (distance < min_distance) {
                    min_distance = distance;
                    nearest_pos.row = i;
                    nearest_pos.column = j;
                }
            }
        }
    }

    return nearest_pos;
}

/* 특정 위치에 스파이스 생성 */
void generate_spice_at_position(int row, int col) {
    int spice_amount;

    // 첫 생성 시 스파이스 양을 5로 설정, 이후에는 1-9 사이의 난수
    if (first_spice) {
        spice_amount = SPICE_INITIAL_AMOUNT;
        first_spice = 0;  // 첫 생성 후 플래그를 0으로 변경하여 난수 생성 활성화
    }
    else {
        spice_amount = (rand() % (SPICE_MAX_AMOUNT - SPICE_MIN_AMOUNT + 1)) + SPICE_MIN_AMOUNT;
    }

    // 스파이스를 지정된 위치에 생성
    if (map[0][row][col] == ' ') {  // 빈 공간에만 생성
        map[0][row][col] = spice_amount + '0';  // 숫자를 문자로 변환해 맵에 저장
        display_system_message("샌드웜이 스파이스를 배설했습니다!");
    }
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

/* 장애물 우회 이동 */
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

        // Rock, 벽(#), 또는 스파이스가 없는 위치를 찾으면 이동
        if (map[0][new_pos.row][new_pos.column] != 'R' &&
            map[0][new_pos.row][new_pos.column] != '#' &&
            !isdigit(map[0][new_pos.row][new_pos.column]) &&
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