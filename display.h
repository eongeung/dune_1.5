/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// 표시할 색상 정의. 대충 맞춰 뒀는데, 취향껏 추가하거나 변경하기
#define COLOR_DEFAULT	15
#define COLOR_CURSOR	112
#define COLOR_WHITE_ON_BLACK 15       // 흰 글자, 검은 배경
#define COLOR_BLACK_ON_WHITE 240      // 검은 글자, 흰 배경
#define COLOR_WHITE_ON_BLUE 31        // 흰 글자, 파란 배경
#define COLOR_WHITE_ON_RED 79         // 흰 글자, 빨간 배경
#define COLOR_WHITE_ON_YELLOW 111     // 흰 글자, 노란 배경
#define COLOR_WHITE_ON_GRAY 112       // 흰 글자, 회색 배경
#define COLOR_YELLOW_ON_RED 108




extern const POSITION resource_pos;
extern const POSITION map_pos;
extern const POSITION system_message_pos;
extern const POSITION object_info_pos;  // 위치 상수 extern 선언
extern const POSITION commands_pos;

// 지금은 자원, 맵, 커서만 표시
// 앞으로 화면에 표시할 내용들 여기에 추가하기
void display_system_message(const char* message);
void display_object_info(char symbol,CURSOR cursor);
void display_commands(char symbol, char unitSymbol);
void display_resource(RESOURCE resource);
void display_status_bar(int color);
void handle_double_click(KEY key);
void handle_selection(KEY key);
void handle_cancel();
void display_cursor(CURSOR cursor);
void generate_spice_at_position(int row, int col);
void process_unit_commands(UNIT* unit, char command);
void produce_unit(char unit_type, POSITION base_pos);


void clear_line(POSITION pos, int length, int lines);
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor
);

#endif
