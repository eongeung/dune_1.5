#include "display.h"
#include "io.h"

#define COLOR_WHITE_ON_BLACK 15       // �� ����, ���� ���
#define COLOR_BLACK_ON_WHITE 240      // ���� ����, �� ���
#define COLOR_WHITE_ON_BLUE 31        // �� ����, �Ķ� ���
#define COLOR_WHITE_ON_RED 79         // �� ����, ���� ���
#define COLOR_WHITE_ON_YELLOW 111     // �� ����, ��� ���
#define COLOR_WHITE_ON_GRAY 112       // �� ����, ȸ�� ���

// ȭ�� ����
char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };  // ���ο� �������� ������ ����
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // ���� ȭ�鿡 ǥ�� ���� ����

// ��ġ ����
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };

// ������ �����ϴ� �Լ�
void set_color(int color);

// ��ü�� ���� ���� �Լ�
void set_object_color(char object, int row, int col) {
    if (object == 'B') {
        // PLAYER�� Base (���� �ϴ�)
        if ((row >= 15 && row <= 16) && (col >= 1 && col <= 2)) {
            set_color(COLOR_WHITE_ON_BLUE); // PLAYER�� Base�� �Ķ� ��濡 �� ����
        }
        // AI�� Base (������ ���)
        else if ((row >= 1 && row <= 2) && (col >= 57 && col <= 58)) {
            set_color(COLOR_WHITE_ON_RED); // AI�� Base�� ���� ��濡 �� ����
        }
        else {
            set_color(COLOR_WHITE_ON_BLACK); // �⺻ ����
        }
    }
    else if (object == 'H') {
        // PLAYER�� Harvester (���� �ϴ�)
        if (row == 14 && col == 1) {
            set_color(COLOR_WHITE_ON_BLUE); // PLAYER�� Harvester�� �Ķ� ��濡 �� ����
        }
        // AI�� Harvester (������ ���)
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

                // ��ü�� �´� ������ ����
                set_object_color(object, i, j);

                // �ش� ��ġ�� �̵��ϰ� ���
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
    set_color(COLOR_BLACK_ON_WHITE);  // Ŀ���� �� ���, ���� ���ڷ� ǥ��
    backbuf[cursor.current.row][cursor.current.column] = 'O';  // Ŀ���� 'O'�� ǥ��
    set_color(COLOR_WHITE_ON_BLACK);  // �⺻ �������� ����
}

// ��ܹٸ� ǥ���ϴ� �Լ� (���� ���)
void display_status_bar(int color) {
    set_color(color);
    gotoxy((POSITION) { 0, 0 });
    printf("Status: Player HP: 100 | Resources: Spice=50, Population=20\n");
    set_color(COLOR_WHITE_ON_BLACK);  // �⺻ ���� ����
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

// ���� display �Լ�
void display(
    RESOURCE resource,
    char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],  // �Ű������� map�� ���޹���
    CURSOR cursor
) {
    display_status_bar(COLOR_WHITE_ON_BLACK);  // ��ܹ� ǥ��
    display_resource(resource);                // �ڿ� ���� ���
    display_map(map);                          // ���� backbuf�� ���
    display_cursor(cursor);                    // Ŀ���� backbuf�� ���
    update_display();                          // ȭ�鿡 ���� ���� ����
}
