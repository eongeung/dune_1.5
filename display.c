#include "display.h"
#include "io.h"


// ȭ�� ����


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };  // ���ο� �������� ������ ����
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]; // �� ���̾� ����
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // ���� ȭ�鿡 ǥ�� ���� ����
OBJECT_SAND worm1;
OBJECT_SAND worm2;

// ��ġ ����
const POSITION resource_pos = { 0, 0 };                // �ڿ� ���� ǥ�� ��ġ
const POSITION map_pos = { 1, 0 };                     // �� ǥ�� ��ġ
const POSITION system_message_pos = { MAP_HEIGHT + 1, 0 }; // �ý��� �޽��� ǥ�� ��ġ
const POSITION object_info_pos = { 1, MAP_WIDTH + 1 }; // ������ ��� ����â ��ġ
const POSITION commands_pos = { MAP_HEIGHT + 3, MAP_WIDTH + 1 }; // ������ �ϴ� ���â ��ġ

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
void copy_back_to_front(void);
void update_display(void);
void clear_line(POSITION pos, int length);

// ������ �����ϴ� �Լ�
void set_color(int color);

// ��ü�� ���� ���� �Լ�
void set_object_color(char object, int row, int col) {
    if (object == 'B') {
        if ((row >= 15 && row <= 16) && (col >= 1 && col <= 2)) {
            set_color(COLOR_WHITE_ON_BLUE); // PLAYER�� Base�� �Ķ� ��濡 �� ����
        }
        else if ((row >= 1 && row <= 2) && (col >= 57 && col <= 58)) {
            set_color(COLOR_WHITE_ON_RED); // AI�� Base�� ���� ��濡 �� ����
        }
        else {
            set_color(COLOR_WHITE_ON_BLACK); // �⺻ ����
        }
    }
    else if (object == 'H') {
        if (row == 14 && col == 1) {
            set_color(COLOR_WHITE_ON_BLUE); // PLAYER�� Harvester�� �Ķ� ��濡 �� ����
        }
        else if (row == 3 && col == 58) {
            set_color(COLOR_WHITE_ON_RED); // AI�� Harvester�� ���� ��濡 �� ����
        }
        else {
            set_color(COLOR_WHITE_ON_BLACK); // �⺻ ����
        }
    }
    else if (object == 'P') {
        set_color(COLOR_BLACK_ON_WHITE); // ����(Plate)�� ��� ��濡 ���� ����
    }
    else if (object == 'R') {
        set_color(COLOR_WHITE_ON_GRAY); // ����(Rock)�� �� ����, ȸ�� ���
    }
    else if (object == '5'|| object == 'w') {
        set_color(COLOR_WHITE_ON_YELLOW); // Worm�� Spice�� ��� ��濡 �� ����
    }
    else {
        set_color(COLOR_WHITE_ON_BLACK); // �⺻ ���� (�� ����, ���� ���)
    }
}

// W ǥ�� �� Ŀ�� ǥ�� �Լ�
void display_worms(void) {
    // worm1 ǥ��
    gotoxy(padd(map_pos, worm1.pos));
    set_color(COLOR_WHITE_ON_YELLOW);
    printf("%c", worm1.repr);

    // worm2 ǥ��
    gotoxy(padd(map_pos, worm2.pos));
    set_color(COLOR_WHITE_ON_YELLOW);
    printf("%c", worm2.repr);

    set_color(COLOR_WHITE_ON_BLACK);
}

// backbuf�� frontbuf�� �����ϴ� �Լ�
void copy_back_to_front() {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            frontbuf[i][j] = backbuf[i][j];
        }
    }
}

// ����� �κи� ������Ʈ�ϴ� �Լ�
void update_display() {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (backbuf[i][j] != frontbuf[i][j]) {  // ���� ������ �ٸ� ���� ������Ʈ
                char object = backbuf[i][j];
                set_object_color(object, i, j);
                gotoxy((POSITION) { i + map_pos.row, j + map_pos.column });
                printf("%c", object);
            }
        }
    }
    copy_back_to_front();  // ������Ʈ �� backbuf�� frontbuf�� ����
    set_color(COLOR_WHITE_ON_BLACK);  // �⺻ �������� ����
}

// ���� ȭ�鿡 ����ϴ� �Լ� (backbuf�� ���� ����)
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
                set_object_color(object, i, j);  // ��ü�� ���� ����
                gotoxy((POSITION) { i + map_pos.row, j + map_pos.column });
                printf("%c", object);
            }
            frontbuf[i][j] = backbuf[i][j];
        }
    }
}

// Ư�� ���� ����� �Լ�
void clear_line(POSITION pos, int length) {
    gotoxy(pos);  // ���� ���� ��ġ�� �̵�
    for (int i = 0; i < length; i++) {
        printf("             ");  // ���̸�ŭ ���� ���
    }
    gotoxy(pos);  // ��ġ�� �ٽ� ���� ��ġ�� ����
}

// Ŀ�� ��ġ ǥ�� �Լ� (���� ���ڸ� �״�� �����ϸ鼭 Ŀ�� ǥ��)
void display_cursor(CURSOR cursor) {
    POSITION prev = cursor.previous;
    POSITION curr = cursor.current;

    // ���� ��ġ�� ���ڿ� ���� ����
    char prev_char = backbuf[prev.row][prev.column];  // ���� ��ġ�� ���� ����
    set_object_color(prev_char, prev.row, prev.column);  // ���� ��ġ�� ���� ����
    gotoxy((POSITION) { prev.row + map_pos.row, prev.column + map_pos.column });
    printf("%c", prev_char);

    // ���� ��ġ�� Ŀ�� ǥ�� (��� ���, ���� �۾�)
    char curr_char = backbuf[curr.row][curr.column];  // ���� ��ġ�� ���� ����
    set_color(COLOR_BLACK_ON_WHITE);  // Ŀ�� ����: �� ���, ���� �۾�
    gotoxy((POSITION) { curr.row + map_pos.row, curr.column + map_pos.column });
    printf("%c", curr_char);

    // �⺻ �������� ����
    set_color(COLOR_WHITE_ON_BLACK);
}


void display_system_message(char object) {
    gotoxy(system_message_pos);  // �� �ϴ� ���ʿ� ǥ��
    switch (object) {
    case 'B':
        printf("Base selected: �⺻ �����Դϴ�.");
        break;
    case 'H':
        printf("Harvester selected: �ڿ��� �����մϴ�.");
        break;
    case 'P':
        printf("Plate selected: �ǹ��� ���� �� �ִ� �ڸ��Դϴ�.");
        break;
    default:
        printf("Empty");
    }
}


// ����â
void display_object_info(char symbol) {
    for (int i = 0; i < sizeof(buildings) / sizeof(buildings[0]); i++) {
        if (buildings[i].symbol == symbol) {
            gotoxy(object_info_pos);
            printf("< �ǹ� > �̸�: %s, ���: %d, ����: %s\n",
                buildings[i].name, buildings[i].cost, buildings[i].description);
            return;
        }
    }

    for (int i = 0; i < sizeof(units) / sizeof(units[0]); i++) {
        if (units[i].symbol == symbol) {
            gotoxy(object_info_pos);
            printf("< ���� > �̸�: %s, ���: %d, ����: %s\n",
                units[i].name, units[i].cost, units[i].command);
            return;
        }
    }

    gotoxy(object_info_pos);
    printf("�ش� ��ġ�� ����/�ǹ� ������ �����ϴ�.\n");
}

void display_commands() {
    gotoxy(commands_pos); // ��ɾ� �ȳ��� �ϴ� �����ʿ� ��ġ
    printf("[�����̽�]: ����, [����Ű]: �̵�, [ESC]: ���");
}

// �ڿ� ���¸� ǥ���ϴ� �Լ� (���� ���)
void display_resource(RESOURCE resource) {
    set_color(COLOR_BLACK_ON_WHITE);  // ��� ���, ���� �۾��� ����
    gotoxy(resource_pos);
    printf("spice = %d/%d, population=%d/%d\n",
           resource.spice, resource.spice_max,
           resource.population, resource.population_max);
    set_color(COLOR_WHITE_ON_BLACK);  // �⺻ ���� ����
}


// ��ܹٸ� ǥ���ϴ� �Լ� (���� ���)
void display_status_bar(int color) {
    set_color(color);
    gotoxy((POSITION) { 0, 0 });
    printf("Status: Player HP: 100 | Resources: Spice=50, Population=20\n");
    set_color(COLOR_WHITE_ON_BLACK);  // �⺻ ���� ����
}

// ���� display �Լ�
void display(RESOURCE resource, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], CURSOR cursor) {
    display_resource(resource);
    display_map(map);
    display_cursor(cursor);
    display_system_message(map[0][cursor.current.row][cursor.current.column]);
    //display_object_info(map[0][cursor.current.row][cursor.current.column]);
    display_commands();
    update_display();
}