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
#define DOUBLE_CLICK_THRESHOLD 200  // 더블 클릭 인식 기준 시간(ms)
#define SPACE_KEY 32  // 스페이스바 추가
#define ESC_KEY 27    // ESC 키 추가
#define BUILDING_COUNT 8
#define UNIT_COUNT 6

#define SPICE_GENERATION_INTERVAL 5000
#define SPICE_PROBABILITY 10         //생성 주기 10%
#define SPICE_INITIAL_AMOUNT 5      // 첫 번째 배설 시 스파이스 양
#define SPICE_MIN_AMOUNT 1           // 최소 스파이스 양
#define SPICE_MAX_AMOUNT 9           // 최대 스파이스 양




/* ================= 위치와 방향 =================== */
// 맵 상의 위치를 나타냄
typedef struct {
	int row, column;
} POSITION;

// 커서 위치
typedef struct {
	POSITION previous;  // 직전 위치
	POSITION current;   // 현재 위치

} CURSOR;

// KEY : 입력 키 정의
typedef enum {
	// k_none: 입력된 키가 없음. d_stay(안 움직이는 경우)에 대응
	k_none = 0, k_up, k_right, k_left, k_down,k_h,
	k_quit, k_space = SPACE_KEY, k_esc = ESC_KEY,
	k_undef, // 정의되지 않은 키 입력	
} KEY;


// DIRECTION : 이동 방향
typedef enum {
	d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION;


/* ================= 위치와 방향(2) =================== */
// 편의성을 위한 함수들. KEY, POSITION, DIRECTION 구조체들을 유기적으로 변환

// 두 POSITION을 더하는 함수
inline POSITION padd(POSITION p1, POSITION p2) {
	POSITION p = { p1.row + p2.row, p1.column + p2.column };
	return p;
}

// p1 - p2
inline POSITION psub(POSITION p1, POSITION p2) {
	POSITION p = { p1.row - p2.row, p1.column - p2.column };
	return p;
}

// 방향키인지 확인하는 함수
#define is_arrow_key(k)		(k_up <= (k) && (k) <= k_down)

// 화살표 '키'(KEY)를 '방향'(DIRECTION)으로 변환. 정수 값은 똑같으니 타입만 바꿔주면 됨
#define ktod(k)		(DIRECTION)(k)

// 방향을 POSITION으로 변환하는 함수
inline POSITION dtop(DIRECTION d) {
	static POSITION direction_vector[] = { {0, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 0} };
	return direction_vector[d];
}

// p를 d 방향으로 이동시키는 함수
inline POSITION pmove(POSITION p, DIRECTION d) {
	return padd(p, dtop(d));  // `padd`와 `dtop`을 사용하여 위치 계산
}

/* ================= game data =================== */
typedef struct {
	int spice;		// 현재 보유한 스파이스
	int spice_max;  // 스파이스 최대 저장량
	int population; // 현재 유닛 수
	int population_max;  // 최대 유닛 수
} RESOURCE;


// 대강 만들어 봤음. 기능 추가하면서 각자 수정할 것
typedef struct {
	POSITION pos;		// 현재 위치(position)
	POSITION dest;		// 목적지(destination)
	char repr;
	int speed;
	int next_move_time;
} OBJECT_SAND;

// 건물 구조체 정의
typedef struct {
	char symbol;           // 건물의 심볼 문자
	const char* name;      // 건물 이름
	const char* description;  // 설명
	int cost;              // 건설 비용
	int durability;        // 내구도
	const char* command;   // 명령어
} BUILDING;

// 유닛 구조체 정의
typedef struct {
	char symbol;            // 유닛의 심볼 문자
	const char* name;       // 유닛 이름
	int cost;               // 생산 비용
	int population;         // 필요한 인구 수
	int move_period;        // 이동 주기(ms)
	int attack_damage;      // 공격력
	int attack_period;      // 공격 주기(ms)
	int health;             // 체력
	int vision;             // 시야
	const char* command;    // 명령어
	POSITION pos;           //유닛 위치
} UNIT;

typedef struct {
	POSITION positions[4];  // 베이스의 4개 칸의 좌표
} BASE_GROUP;

// 메시지 타입 정의
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
