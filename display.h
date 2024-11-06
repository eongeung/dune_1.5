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


extern const POSITION resource_pos;
extern const POSITION map_pos;
extern const POSITION system_message_pos;
extern const POSITION object_info_pos;  // ��ġ ��� extern ����
extern const POSITION commands_pos;

// ������ �ڿ�, ��, Ŀ���� ǥ��
// ������ ȭ�鿡 ǥ���� ����� ���⿡ �߰��ϱ�
void display_system_message(char object);
void display_object_info(char object);
void display_commands(void);
void display_resource(RESOURCE resource);
void display_status_bar(int color);
void handle_double_click(KEY key);
void handle_selection();
void handle_cancel();

void clear_line(POSITION pos, int length);
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor
);

#endif
