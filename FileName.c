#include <stdio.h>
#include <conio.h>   
#include <windows.h> 

#define TICK 100  // 100ms �ֱ�� ������ ������ ���� ����
#define MAX_CLOCK 100000  // �ӽ÷� ������ clock �ִ� ��
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
    k_undef, // ���ǵ��� ���� Ű �Է�
} KEY; //�Է� Ű ����

typedef enum {
    d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION; //�̵�����

typedef struct {
    int spice; // ���� ������ �����̽�
    int spice_max; // �����̽� �ִ� ���差
    int population; // ���� �α� ��
    int population_max; // ���� ������ �α� ��
} RESOURCE;


// �밭 ����� ����. ��� �߰��ϸ鼭 ���� ������ ��
typedef struct {
    POSITION pos; // ���� ��ġ(position)
    POSITION dest; // ������(destination)
    char repr; // ȭ�鿡 ǥ���� ����(representation)
    int move_period; // '�� ms���� �� ĭ �����̴���'�� ����
    int next_move_time; // ������ ������ �ð�
} OBJECT_SAMPLE;


KEY get_key(void) {
    if (!_kbhit()) { // �Էµ� Ű�� �ִ��� Ȯ��
        return k_none;
    }
    int byte = _getch(); // �Էµ� Ű�� ����
    switch (byte) {
    case 'q': return k_quit;  // 'q'�� ������ ����
    case 224:  // ����Ű�� ù ����Ʈ�� 224
        byte = _getch(); // �� ��° ����Ʈ�� ����
        switch (byte) {
        case 72: return k_up;    // ���� ����Ű
        case 80: return k_down;  // �Ʒ��� ����Ű
        case 75: return k_left;  // ���� ����Ű
        case 77: return k_right; // ������ ����Ű
        default: return k_undef;
        }
    default: return k_undef;
    }
}

int main(void) {
    int clock_value = 0;  // �ð� ���� ������ ����

    // �⺻ ���� ����
    while (1) {
        KEY key = get_key();  // Ű �Է� Ȯ��

        // �Է¿� ���� ó��
        switch (key) {
        case k_quit:
            printf("exiting...\n");
            return 0;  // 'q'�� ������ ���� ����
        case k_up:
        case k_down:
        case k_left:
        case k_right:
            printf("Ű �Է�: %d\n", key);  // ����Ű �Է� Ȯ��
            break;
        default: break;
        }

        // ���� ������ ���� (TICK ����)
        Sleep(TICK);  // TICK �ֱ�� ������ ����
        clock_value += TICK;  // �ð� �� ����

        // ���� ���� ����
        if (clock_value >= MAX_CLOCK) {
            printf("������ ����Ǿ����ϴ�.\n");
            break;
        }
    }

    return 0;
}