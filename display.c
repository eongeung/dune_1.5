#include "display.h"
#include "io.h"
#define MAX_MESSAGES 5  // �ִ� ǥ���� �޽��� ��
#define MESSAGE_LENGTH 80  // �޽��� ���� ����

char message_log[MAX_MESSAGES][MESSAGE_LENGTH];  // �޽��� �α� ����
int message_count = 0;  // ���� �޽��� ��
int message_colors[MAX_MESSAGES]; // �޽��� ���� ���� ����

// ȭ�� ����
char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };  // ���ο� �������� ������ ����
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH];     // �� ���̾� ����
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // ���� ȭ�鿡 ǥ�� ���� ����
OBJECT_SAND worm1;
OBJECT_SAND worm2;

// ��ġ ����
const POSITION resource_pos = { 0, 0 };                          // �ڿ� ���� ǥ�� ��ġ
const POSITION map_pos = { 1, 0 };                               // �� ǥ�� ��ġ
const POSITION system_message_pos = { MAP_HEIGHT + 2, 0 };       // �ý��� �޽��� ǥ�� ��ġ
const POSITION object_info_pos = { 2, MAP_WIDTH + 2 };           // ������ ��� ����â ��ġ
const POSITION commands_pos = { MAP_HEIGHT + 2, MAP_WIDTH + 1 }; // ������ �ϴ� ���â ��ġ
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
void copy_back_to_front(void);
void update_display(void);
void clear_line(POSITION pos, int length, int lines);
void add_system_message(const char* message, int type);
void display_system_message(void);

// ������ �����ϴ� �Լ�
extern void set_color(int color);

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
        if ((row == 14 && col == 1) || (row == 14 && col == 2)) {
            set_color(COLOR_WHITE_ON_BLUE); // PLAYER�� Harvester�� �Ķ� ��濡 �� ����
        }
        else if ((row == 3 && col == 57) || (row == 3 && col == 58)) {
            set_color(COLOR_WHITE_ON_RED); // AI�� Harvester�� ���� ��濡 �� ����
        }
        else {
            set_color(COLOR_WHITE_ON_BLACK); // �⺻ ����
        }
    }
    else if (object == 'P') {
        set_color(COLOR_WHITE_ON_BLACK); // ����(Plate)�� ��� ��濡 ���� ����
    }
    else if (object == 'R') {
        set_color(COLOR_WHITE_ON_GRAY); // ����(Rock)�� �� ����, ȸ�� ���
    }
    else if ( isdigit(object)) {
        set_color(COLOR_YELLOW_ON_RED); // �����̽��� ���� ����, ��� ��� (��Ȳ�� ���� �Ұ�)
    }
    else if (object == 'W') {
        set_color(COLOR_WHITE_ON_YELLOW); // Worm�� ��� ��濡 �� ����
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
void clear_line(POSITION pos, int length, int lines) {
    for (int i = 0; i < lines; i++) {
        gotoxy((POSITION) { pos.row + i, pos.column });
        for (int j = 0; j < length; j++) {
            printf(" ");
        }
    }
    gotoxy(pos);  // ���� ��ġ�� ����
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
// �ý��� �޽����� ȭ�鿡 ǥ���ϴ� �Լ�
void display_system_message() {
    gotoxy((POSITION) { system_message_pos.row - 1, system_message_pos.column });
    printf("����������������������������������������������������������������������������������������������������������������");
    gotoxy((POSITION) { system_message_pos.row + MAX_MESSAGES, system_message_pos.column });
    printf("����������������������������������������������������������������������������������������������������������������");
    clear_line(system_message_pos, 60, MAX_MESSAGES); // �޽��� ���� �ʱ�ȭ �� ǥ��
    for (int i = 0; i < message_count; i++) {
        set_color(message_colors[i]); // �� �޽����� �´� ���� ����
        gotoxy((POSITION) { system_message_pos.row + i, system_message_pos.column });
        printf(" %-60s", message_log[i]); // �� �޽����� 60�ڷ� �����Ͽ� ���η� ���
    }
    set_color(COLOR_WHITE_ON_BLACK); // �⺻ ���� ����
}

// �ý��� �޽��� �߰� �� ��ũ�Ѹ� ó��
void add_system_message(const char* message, int type) {
    // �޽����� MAX_MESSAGES �̻��� �� ���� ������ �޽����� �����ϰ� ���� ��ũ��
    if (message_count >= MAX_MESSAGES) {
        // ��� �޽����� �� ĭ�� ������ �̵�
        for (int i = 1; i < MAX_MESSAGES; i++) {
            strncpy(message_log[i - 1], message_log[i], MESSAGE_LENGTH);
            message_colors[i - 1] = message_colors[i]; // ���� ������ �Բ� �̵�
        }
        // ������ ��ġ�� �� �޽��� �߰�
        strncpy(message_log[MAX_MESSAGES - 1], message, MESSAGE_LENGTH - 1);
        message_log[MAX_MESSAGES - 1][MESSAGE_LENGTH - 1] = '\0';
        message_colors[MAX_MESSAGES - 1] = type; // ������ ��ġ�� ���� ���� �߰�
    }
    else {
        // MAX_MESSAGES�� �����ϱ� �������� ���ο� �޽����� ���������� �߰�
        strncpy(message_log[message_count], message, MESSAGE_LENGTH - 1);
        message_log[message_count][MESSAGE_LENGTH - 1] = '\0';
        message_colors[message_count] = type; // ���� ���� ����
        message_count++;
    }

    // �޽��� Ÿ�Կ� ���� ���� ����
    switch (type) {
    case 0: message_colors[message_count - 1] = COLOR_WHITE_ON_BLACK; break; // �⺻ �޽���
    case 1: message_colors[message_count - 1] = COLOR_RED; break;            // ���� �޽���
    case 2: message_colors[message_count - 1] = COLOR_YELLOW; break;         // ��� �޽���
    case 3: message_colors[message_count - 1] = COLOR_GREEN; break;          // ���� �޽���
    }

    // �޽��� �����Ͽ� ȭ�鿡 ǥ��
    display_system_message();
}

// ����â: ���õ� ���� �Ǵ� �ǹ��� ������ ǥ��
void display_object_info(char symbol, CURSOR cursor) {
    // ����â ��� ���
    gotoxy((POSITION) { object_info_pos.row - 1, object_info_pos.column });
    printf("���������������������������������������������������������������������� ����â ����������������������������������������������������������������������");

    clear_line(object_info_pos, 80, 6);  // ����â �ʱ�ȭ

    char unitSymbol = map[1][cursor.current.row][cursor.current.column];
    char terrainSymbol = map[0][cursor.current.row][cursor.current.column];  // ���� ���� Ȯ��

    // ���� ���� ǥ��
    for (int i = 0; i < UNIT_COUNT; i++) {
        if (units[i].symbol == unitSymbol) {
            gotoxy(object_info_pos);
            printf("      < ���� > �̸�: %s, ü��: %d", units[i].name, units[i].health);

            gotoxy((POSITION) { object_info_pos.row + 1, object_info_pos.column });
            printf("      ���ݷ�: %d, �̵� �ֱ�: %d", units[i].attack_damage, units[i].move_period);
            return;
        }
    }

    // �ǹ� ���� ǥ��
    for (int i = 0; i < BUILDING_COUNT; i++) {
        if (buildings[i].symbol == symbol) {
            gotoxy(object_info_pos);
            printf("< �ǹ� ���� > �̸�: %s, ���: %d, ����: %s\n",
                buildings[i].name, buildings[i].cost, buildings[i].description);
            return;
        }
    }

    // ���� ������ ���� ó��
    if (terrainSymbol == 'P') {  // Plate�� ���������
        gotoxy(object_info_pos);
        printf("      <�ǹ� ���� �� ����> Plate�� ��� �ֽ��ϴ�.\n");
        return;
    }
    else if (terrainSymbol == 'R') {  // Rock�̸� �ǹ��� ���� �� ����
        gotoxy(object_info_pos);
        printf("      <�ǹ� ���� �� ����> �̰��� Rock �������� �ǹ��� ���� �� �����ϴ�.\n");
        return;
    }

    // Plate�� ����, Rock�� �ƴϸ� Plate�� ��ġ�϶�� �޽���
    gotoxy(object_info_pos);
    printf("      <�ǹ� ���� �� ����> Plate�� �켱 ��ġ���ֽʽÿ�.\n");
}


// ���â: ���õ� ���� �Ǵ� �ǹ��� ��ɾ� ǥ��
void display_commands(char symbol, char unitSymbol) {
    gotoxy((POSITION){commands_pos.row - 1, commands_pos.column});
    printf("���������������������������������������������������������������������� ���â ����������������������������������������������������������������������");
    clear_line(commands_pos, 80, 1);  // ���â �ʱ�ȭ

    // ���� ��ɾ� ǥ��
    for (int i = 0; i < UNIT_COUNT; i++) {
        if (units[i].symbol == unitSymbol) {
            gotoxy(commands_pos);
            printf("  ��ɾ�: %s\n", units[i].command);
            return;
        }
    }

    // �ǹ� ��ɾ� ǥ��
    for (int i = 0; i < BUILDING_COUNT; i++) {
        if (buildings[i].symbol == symbol) {
            gotoxy(commands_pos);
            printf("  ��ɾ�: %s\n", buildings[i].command);
            return;
        }
    }

    // �⺻ ��ɾ� �ȳ�
    gotoxy(commands_pos);
    printf("     [�����̽�]: ����, [����Ű]: �̵�, [ESC]: ���");
}

// �ڿ� ���¸� ǥ���ϴ� �Լ�
void display_resource(RESOURCE resource) {
    set_color(COLOR_BLACK_ON_WHITE);  // ��� ���, ���� �۾��� ����
    gotoxy(resource_pos);
    printf("spice = %d/%d, population=%d/%d\n",
           resource.spice, resource.spice_max,
           resource.population, resource.population_max);
    set_color(COLOR_WHITE_ON_BLACK);  // �⺻ ���� ����
}

// ���� display �Լ�
void display(RESOURCE resource, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], CURSOR cursor) {
    display_resource(resource);
    display_map(map);
    display_cursor(cursor); 
    display_system_message(); // �ý��� �޽��� �����
    int should_update_status = 0;
    

    // ���� �Ǵ� �ǹ� ���¿� ��ɾ� â ǥ��
    if (should_update_status) {
        char symbol = map[0][cursor.current.row][cursor.current.column];
        char unitSymbol = map[1][cursor.current.row][cursor.current.column];
        display_object_info(symbol, cursor);   // ����â�� ���� ǥ��
        display_commands(symbol, unitSymbol);  // ���â�� ��ɾ� ǥ��
        should_update_status = 0;  // ���� ������Ʈ �÷��� �ʱ�ȭ
    }
    update_display();
}