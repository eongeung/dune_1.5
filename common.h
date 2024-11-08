#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <conio.h>
#include <assert.h>

/* ================= system parameters =================== */
#define TICK 10		// time unit(ms)

#define N_LAYER 2
#define MAP_WIDTH	60
#define MAP_HEIGHT	18
#define DOUBLE_CLICK_THRESHOLD 200  // ���� Ŭ�� �ν� ���� �ð�(ms)
#define SPACE_KEY 32  // �����̽��� �߰�
#define ESC_KEY 27    // ESC Ű �߰�
#define BUILDING_COUNT 8
#define UNIT_COUNT 6

#define SPICE_GENERATION_INTERVAL 5000
#define SPICE_PROBABILITY 10         //���� �ֱ� 10%
#define SPICE_INITIAL_AMOUNT 5      // ù ��° �輳 �� �����̽� ��
#define SPICE_MIN_AMOUNT 1           // �ּ� �����̽� ��
#define SPICE_MAX_AMOUNT 9           // �ִ� �����̽� ��




/* ================= ��ġ�� ���� =================== */
// �� ���� ��ġ�� ��Ÿ��
typedef struct {
	int row, column;
} POSITION;

// Ŀ�� ��ġ
typedef struct {
	POSITION previous;  // ���� ��ġ
	POSITION current;   // ���� ��ġ

} CURSOR;

// KEY : �Է� Ű ����
typedef enum {
	// k_none: �Էµ� Ű�� ����. d_stay(�� �����̴� ���)�� ����
	k_none = 0, k_up, k_right, k_left, k_down,k_h,
	k_quit, k_space = SPACE_KEY, k_esc = ESC_KEY,
	k_undef, // ���ǵ��� ���� Ű �Է�	
} KEY;


// DIRECTION : �̵� ����
typedef enum {
	d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION;


/* ================= ��ġ�� ����(2) =================== */
// ���Ǽ��� ���� �Լ���. KEY, POSITION, DIRECTION ����ü���� ���������� ��ȯ

// �� POSITION�� ���ϴ� �Լ�
inline POSITION padd(POSITION p1, POSITION p2) {
	POSITION p = { p1.row + p2.row, p1.column + p2.column };
	return p;
}

// p1 - p2
inline POSITION psub(POSITION p1, POSITION p2) {
	POSITION p = { p1.row - p2.row, p1.column - p2.column };
	return p;
}

// ����Ű���� Ȯ���ϴ� �Լ�
#define is_arrow_key(k)		(k_up <= (k) && (k) <= k_down)

// ȭ��ǥ 'Ű'(KEY)�� '����'(DIRECTION)���� ��ȯ. ���� ���� �Ȱ����� Ÿ�Ը� �ٲ��ָ� ��
#define ktod(k)		(DIRECTION)(k)

// ������ POSITION���� ��ȯ�ϴ� �Լ�
inline POSITION dtop(DIRECTION d) {
	static POSITION direction_vector[] = { {0, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 0} };
	return direction_vector[d];
}

// p�� d �������� �̵���Ű�� �Լ�
inline POSITION pmove(POSITION p, DIRECTION d) {
	return padd(p, dtop(d));  // `padd`�� `dtop`�� ����Ͽ� ��ġ ���
}

/* ================= game data =================== */
typedef struct {
	int spice;		// ���� ������ �����̽�
	int spice_max;  // �����̽� �ִ� ���差
	int population; // ���� ���� ��
	int population_max;  // �ִ� ���� ��
} RESOURCE;


// �밭 ����� ����. ��� �߰��ϸ鼭 ���� ������ ��
typedef struct {
	POSITION pos;		// ���� ��ġ(position)
	POSITION dest;		// ������(destination)
	char repr;
	int speed;
	int next_move_time;
} OBJECT_SAND;

// �ǹ� ����ü ����
typedef struct {
	char symbol;           // �ǹ��� �ɺ� ����
	const char* name;      // �ǹ� �̸�
	const char* description;  // ����
	int cost;              // �Ǽ� ���
	int durability;        // ������
	const char* command;   // ��ɾ�
} BUILDING;

// ���� ����ü ����
typedef struct {
	char symbol;            // ������ �ɺ� ����
	const char* name;       // ���� �̸�
	int cost;               // ���� ���
	int population;         // �ʿ��� �α� ��
	int move_period;        // �̵� �ֱ�(ms)
	int attack_damage;      // ���ݷ�
	int attack_period;      // ���� �ֱ�(ms)
	int health;             // ü��
	int vision;             // �þ�
	const char* command;    // ��ɾ�
	POSITION pos;           //���� ��ġ
} UNIT;

typedef struct {
	POSITION positions[4];  // ���̽��� 4�� ĭ�� ��ǥ
} BASE_GROUP;

// �޽��� Ÿ�� ����
typedef enum {
	MSG_DEFAULT,
	MSG_ERROR,
	MSG_WARNING,
	MSG_INFO,
} MessageType;

extern BUILDING buildings[BUILDING_COUNT];
extern UNIT units[UNIT_COUNT];
extern RESOURCE resource;
extern CURSOR cursor;
extern char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH];
extern char backbuf[MAP_HEIGHT][MAP_WIDTH];



#endif
