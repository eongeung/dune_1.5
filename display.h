/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// ǥ���� ���� ����. ���� ���� �״µ�, ���ⲯ �߰��ϰų� �����ϱ�
#define COLOR_DEFAULT	15
#define COLOR_CURSOR	112
#define COLOR_WHITE_ON_BLACK 15       // �� ����, ���� ���
#define COLOR_BLACK_ON_WHITE 240      // ���� ����, �� ���
#define COLOR_WHITE_ON_BLUE 31        // �� ����, �Ķ� ���
#define COLOR_WHITE_ON_RED 79         // �� ����, ���� ���
#define COLOR_WHITE_ON_YELLOW 111     // �� ����, ��� ���
#define COLOR_WHITE_ON_GRAY 112       // �� ����, ȸ�� ���
#define COLOR_YELLOW_ON_RED 108




extern const POSITION resource_pos;
extern const POSITION map_pos;
extern const POSITION system_message_pos;
extern const POSITION object_info_pos;  // ��ġ ��� extern ����
extern const POSITION commands_pos;

// ������ �ڿ�, ��, Ŀ���� ǥ��
// ������ ȭ�鿡 ǥ���� ����� ���⿡ �߰��ϱ�
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
