#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);
void sample_obj_move(void);
POSITION sample_obj_next_position(void);


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = { 
	.spice = 0,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
};

OBJECT_SAMPLE obj = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'o',
	.speed = 300,
	.next_move_time = 300
};

/* ================= main() =================== */
// engine.c 파일의 main 함수
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();
	display(resource, map, cursor);

	while (1) {
		KEY key = get_key();

		// 방향키 입력을 커서 이동에 사용
		if (is_arrow_key(key)) {
			cursor_move(ktod(key)); // 방향키로 커서 이동
		}
		else {
			switch (key) {
			case k_quit:
				outro();
			default:
				break;
			}
		}

		// 샘플 오브젝트 동작
		sample_obj_move();

		// 화면 출력
		display(resource, map, cursor);
		Sleep(TICK);
		sys_clock += 10;
	}
}


/* ================= subfunctions =================== */
void intro(void) {
	printf("DUNE 1.5\n");		
	Sleep(2000);
	system("cls");
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}

void init(void) {
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

	// layer 1(map[1])은 비워 두기(-1로 채움)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	//AI
	map[0][1][58] = 'B';  map[0][1][57] = 'B';
	map[0][2][58] = 'B';  map[0][2][57] = 'B';
	map[0][1][56] = 'P';  map[0][1][55] = 'P';
	map[0][2][56] = 'P';  map[0][2][55] = 'P';
	map[0][3][58] = 'H';  // Harvester
	map[0][5][58] = '5';  //spice

	//Player
	map[0][15][1] = 'B';  map[0][15][2] = 'B';
	map[0][16][1] = 'B';  map[0][16][2] = 'B'; 
	map[0][15][3] = 'P';  map[0][15][4] = 'P';
	map[0][16][3] = 'P';  map[0][16][4] = 'P';
	map[0][14][1] = 'H';  // Harvester
	map[0][12][1] = '5';  //spice

	// 나머지 오브젝트 배치
	map[0][13][50] = 'W';  // Worm1
	map[0][5][7] = 'W';  // Worm2
	map[0][12][37] = 'R';  // Rock1
	map[0][3][6] = 'R';  // Rock2
	map[0][8][49] = 'R';  map[0][8][50] = 'R';//Rock 2
	map[0][9][49] = 'R';  map[0][9][50] = 'R';
	map[0][6][23] = 'R';  map[0][6][24] = 'R';//Rock 2
	map[0][7][23] = 'R';  map[0][7][24] = 'R';

	// 샘플 오브젝트 배치 (layer 1)
	map[1][obj.pos.row][obj.pos.column] = 'o';
}


// 커서 위치를 공백으로 이동시키는 함수 예제
void cursor_move(DIRECTION dir) {
	POSITION curr = cursor.current;
	POSITION new_pos = pmove(curr, dir);

	// validation check
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 &&
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

		// 이전 위치를 공백으로 설정
		backbuf[curr.row][curr.column] = ' ';

		// 새로운 위치로 커서 이동
		cursor.previous = cursor.current;
		cursor.current = new_pos;

		// 새로운 위치에 커서가 이동했음을 표시
		backbuf[new_pos.row][new_pos.column] = ' ';
	}
}

/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}
	
	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}
	
	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {
		
		return next_pos;
	}
	else {
		return obj.pos;  // 제자리
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// 아직 시간이 안 됐음
		return;
	}

	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.speed;
}