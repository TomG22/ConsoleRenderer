#include <iostream>
#include <stdio.h>
#include <math.h>
#include <chrono>
#include <thread>
#include <Windows.h>

const float PI = 3.14159265f;

const int REFRESH_TIME = 20; //ms
const float TIME_RANGE = 1.0f; // unregulated units; 0 = still-image
const float TIME_STEP = 0.02f;
const float CALC_STEP = 0.05f; // lower value -> higher calculation precision

const std::string FUNCTION = "ParamTorus";

const int RADIUS = 30;

const int INNER_RADIUS = floor(RADIUS * .5);
const int TUBE_RADIUS = floor(RADIUS * .25);

std::string ForCircle(float time) {
	std::string buffer = "";
	for (int y = -RADIUS + 1; y < RADIUS; y += 2) {
		for (int x = -RADIUS + 1; x < RADIUS; x++) {
			if ((x * x + y * y >= pow(time*RADIUS - 1, 2)) && (x * x + y * y < pow(time*RADIUS + 1, 2)))
				buffer.push_back('#');
			else
				buffer.push_back('*');
		}
		buffer += "\n";
	}
	return buffer;
}

std::string ParamCircle(float time) {
	std::string buffer[RADIUS * 2] = {};
	std::string string_buffer;

	for (int i = 0; i < RADIUS; i++) {
		std::string buffer_row = "";
		for (int j = 0; j < RADIUS * 2; j++) {
			buffer_row.push_back('*');
		}
		buffer[i] = buffer_row;
	}

	for (float i = 0; i <= 2 * PI; i += CALC_STEP) {
		if (TIME_RANGE == 0) {
			time = 1.0f;
		}

		int x = floor(time * RADIUS * cos(i) + RADIUS);
		int y = floor(time * RADIUS * .5 * sin(i) + RADIUS * .5);

		if (x >= 0 && y >= 0 && x < RADIUS * 2 && y < RADIUS) {
			buffer[y][x] = '#';
		}
	}

	for (int i = 0; i < RADIUS; i++) {
		string_buffer.append(buffer[i]);
		string_buffer.push_back('\n');
	}

	return string_buffer;
}

std::string ParamTorus(float time) {
	std::string buffer[RADIUS * 2] = {};
	std::string string_buffer;

	for (int i = 0; i < RADIUS; i++) {
		std::string buffer_row = "";
		for (int j = 0; j < RADIUS * 2; j++) {
			buffer_row.push_back('*');
		}
		buffer[i] = buffer_row;
	}

	for (float u = 0; u <= 2 * PI; u += CALC_STEP) {
		for (float v = 0; v <= 2 * PI; v += CALC_STEP) {
			if (TIME_RANGE == 0) {
				time = 1.0f;
			}

			int x = floor((INNER_RADIUS + TUBE_RADIUS * cos(v)) * cos(u) + RADIUS);
			int y = floor(.5 * (INNER_RADIUS + TUBE_RADIUS * cos(v)) * sin(u) + RADIUS * .5);
			int z = floor(TUBE_RADIUS * sin(v) + TUBE_RADIUS);

			if (x >= 0 && y >= 0 && x < RADIUS * 2 && y < RADIUS) {
				char map[]  = ".'!?xyWN";
				int div = floor((z / float(TUBE_RADIUS*2 + 1)) * (sizeof(map)-1));
				// std::cout << (z/(float(TUBE_RADIUS*2 + 1))) << "|";
				if (buffer[y][x] != '*') {
					for (int i = 0; i < sizeof(map); i++) {
						if (buffer[y][x] == map[i]) {
							if (div < i) {
								div = i;
							}
						}
					}
				}
				buffer[y][x] = map[div];
			}
		}
	}

	for (int i = 0; i < RADIUS; i++) {
		string_buffer.append(buffer[i]);
		string_buffer.push_back('\n');
	}

	return string_buffer;
}

void Clear() {
	system("cls");
}

std::string DrawFunction(float time) {
	if (FUNCTION == "ForCircle") {
		return ForCircle(time);
	}
	else if (FUNCTION == "ParamCircle") {
		return ParamCircle(time);
	}
	else if (FUNCTION == "ParamTorus") {
		return ParamTorus(time);
	}
	else {
		return "Unknown Function: \"" + FUNCTION + '"';
	}
}

void DrawLoop() {
	float time = -TIME_RANGE;

	if (TIME_RANGE != 0) {
		int inc = 1;
		while (true) {
			if (time < -TIME_RANGE) {
				inc = 1;
				time = -TIME_RANGE;
			}
			else if (time > TIME_RANGE) {
				inc = -1;
				time = TIME_RANGE;
			}
			std::cout << "Rendering \"" << FUNCTION << "\" (" << REFRESH_TIME << "ms Video | Time: " << time << "):\n";
			std::cout << DrawFunction(time);
			std::this_thread::sleep_for(std::chrono::milliseconds(REFRESH_TIME));
			Clear();
			time += TIME_STEP * inc;
		}
	}
	else {
		std::cout << "Rendered \"" + FUNCTION << "\" (Image):\n";
		std::cout << DrawFunction(0);
	}
}

void InitWindow() {
	// Maximize Window
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_SHOWMAXIMIZED);

	// Invis Cursor
	CONSOLE_CURSOR_INFO curInfo;
	HANDLE hStdOut = NULL;
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleCursorInfo(hStdOut, &curInfo);
	curInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hStdOut, &curInfo);
}


int main() {
	InitWindow();
	DrawLoop();
}