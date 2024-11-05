#include "display.h"
#include "io.h"

#define COLOR_WHITE_ON_BLACK 15       // �� ����, ���� ���
#define COLOR_BLACK_ON_WHITE 240      // ���� ����, �� ���
#define COLOR_WHITE_ON_BLUE 31        // �� ����, �Ķ� ���
#define COLOR_WHITE_ON_RED 79         // �� ����, ���� ���
#define COLOR_WHITE_ON_YELLOW 111     // �� ����, ��� ���
#define COLOR_WHITE_ON_GRAY 112       // �� ����, ȸ�� ���

// �ǹ� ���� �迭
BUILDING buildings[] = {
    {'B', "Base", "����", 50, 0, "H: �Ϻ����� ����"},
    {'P', "Plate", "�ǹ� ���� ���� ���", 1, 0, "����"},
    {'D', "Dormitory", "�α� �ִ�ġ ����(10)", 2, 10, "����"},
    {'G', "Garage", "�����̽� ���� �ִ�ġ ����(10)", 4, 10, "����"},
    {'B', "Barracks", "���� ����", 4, 20, "���� ����(S: Soldier)"},
    {'S', "Shelter", "Ư������ ����", 5, 30, "������ ����(F: Fremen)"},
    {'A', "Arena", "���� ����", 3, 15, "���� ����(F: Fighter)"},
    {'F', "Factory", "Ư������ ����", 5, 30, "������ ����(T: heavy Tank)"}
};

// ���� ���� �迭
UNIT units[] = {
    {'H', "Harvester", 5, 5, 2000, 0, 0, 70, 0, "H: Harvest, M: Move"},
    {'F', "Fremen", 5, 2, 400, 15, 200, 25, 8, "M: �̵�, P: ����"},
    {'S', "Soldier", 1, 1, 1000, 5, 800, 15, 1, "M: �̵�"},
    {'F', "Fighter", 1, 1, 1200, 6, 600, 10, 1, "M: �̵�"},
    {'T', "Heavy Tank", 12, 5, 3000, 40, 4000, 60, 4, "M: �̵�"},
    {'W', "Sandworm", 0, 0, 2500, 0, 10000, 0, 0, "����"}
};

// ȭ�� ����


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };  // ���ο� �������� ������ ����
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]; // �� ���̾� ����
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // ���� ȭ�鿡 ǥ�� ���� ����

// ��ġ ����
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
const POSITION system_message_pos = { MAP_HEIGHT + 1, 0 };
const POSITION object_info_pos = { MAP_HEIGHT + 1, 80 };
const POSITION commands_pos = { MAP_HEIGHT + 3, 80 };

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
    else if (object == 'W' || object == '5') {
        set_color(COLOR_WHITE_ON_YELLOW); // Worm�� Spice�� ��� ��濡 �� ����
    }
    else {
        set_color(COLOR_WHITE_ON_BLACK); // �⺻ ���� (�� ����, ���� ���)
    }
}

// �ǹ� �� ���� ������ �迭�� ���� ��ȸ�ϰ� ǥ���ϴ� �Լ�
void display_building_info(char symbol) {
    for (int i = 0; i < sizeof(buildings) / sizeof(buildings[0]); i++) {
        if (buildings[i].symbol == symbol) {
            gotoxy(object_info_pos);
            printf("�ǹ�: %s\n����: %s\n���: %d\n������: %d\n��ɾ�: %s\n",
                buildings[i].name, buildings[i].description,
                buildings[i].cost, buildings[i].durability,
                buildings[i].command);
            return;
        }
    }
    printf("�ǹ� ������ �����ϴ�.\n");
}

// ���� ������ ǥ���ϴ� �Լ�
void display_unit_info(char symbol) {
    for (int i = 0; i < sizeof(units) / sizeof(units[0]); i++) {
        if (units[i].symbol == symbol) {
            gotoxy(object_info_pos);
            printf("����: %s\n���: %d\n�α���: %d\n�̵� �ֱ�: %d\n���ݷ�: %d\n���� �ֱ�: %d\n"
                "ü��: %d\n�þ�: %d\n��ɾ�: %s\n",
                units[i].name, units[i].cost, units[i].population,
                units[i].move_period, units[i].attack_damage,
                units[i].attack_period, units[i].health,
                units[i].vision, units[i].command);
            return;
        }
    }
    printf("���� ������ �����ϴ�.\n");
}
// ������Ʈ ���� ǥ�� �Լ� (�ǹ� �Ǵ� ���� ���� ���)
void display_object_info(char object) {
    gotoxy(object_info_pos); // ������Ʈ ������ �ϴ� �����ʿ� ��ġ
    switch (object) {
    case 'B':
        printf("����: HP=1000, ���� �ӵ�=10");
        break;
    case 'H':
        printf("�ڿ� ������: ���� �ӵ�=5");
        break;
    default:
        printf("�ش� ��ġ�� ����/�ǹ� ������ �����ϴ�.");
    }
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
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            backbuf[i][j] = map[0][i][j];  // layer 0�� �������� ���� backbuf�� ����
        }
    }
}

// Ŀ���� ȭ�鿡 ����ϴ� �Լ� (backbuf�� ���� ����)
void display_cursor(CURSOR cursor) {
    // ���� ��ġ�� �������� ǥ��
    gotoxy((POSITION) { cursor.previous.row + map_pos.row, cursor.previous.column + map_pos.column });
    printf(" ");

    // ���� ��ġ�� �������� ǥ��
    gotoxy((POSITION) { cursor.current.row + map_pos.row, cursor.current.column + map_pos.column });
    printf(" ");

    set_color(COLOR_WHITE_ON_BLACK);  // �⺻ �������� ����
}

void display_system_message(char symbol) {
    gotoxy(system_message_pos);
    switch (symbol) {
    case 'B': printf("Base selected: �⺻ �����Դϴ�."); break;
    case 'H': printf("Harvester selected: �ڿ��� �����մϴ�."); break;
    case 'P': printf("Plate selected: �ǹ��� ���� �� �ִ� �ڸ��Դϴ�."); break;
    default: printf("Empty");
    }
}


// ������Ʈ ���� ǥ�� �Լ�
void display_object_info(char object);

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
        resource.population, resource.population_max
    );
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