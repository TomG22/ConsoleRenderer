#include <iostream>
#include <stdio.h>
#include <math.h>
#include <chrono>
#include <thread>
#include <Windows.h>

const float PI = 3.14159265f;

const int REFRESH_TIME = 40; //ms
const float TIME_RANGE = 2 * PI; // unregulated units; 0 = still-image
const float TIME_STEP = .05;
const float CALC_STEP = 0.01f; // lower value -> higher calculation precision

const std::string FUNCTION = "VertexObject";

const int RADIUS = 31;

const int INNER_RADIUS = floor(RADIUS * .5);
const int TUBE_RADIUS = floor(RADIUS * .25);

float(*MatRx (float theta))[3] {
	float Rx[3][3] = {
		{1, 0, 0},
		{0, cosf(theta), -sinf(theta)},
		{0, sinf(theta), cosf(theta)}
	};
	return Rx;
}

float(*MatRy(float theta))[3] {
	float Ry[3][3] = {
		{cosf(theta), 0, sinf(theta)},
		{0, 1, 0},
		{-sinf(theta), 0, cosf(theta)}
	};
	return Ry;
}

float(*(MatRz)(float theta))[3] {
	float Rz[3][3] = {
		{cosf(theta), -sinf(theta), 0},
		{sinf(theta), cosf(theta), 0},
		{0, 0, 1}
	};
	return Rz;
}

float* MatTransform(float vector[3], float time) {
	float transformed_vector[3];
	for (int rowIdx = 0; rowIdx < 3; rowIdx++) {
		float totalTransform = 0;
		for (int colIdx = 0; colIdx < 3; colIdx++) {
			float(*transformMatrix)[3];
			transformMatrix = MatRx(time);
			totalTransform += transformMatrix[rowIdx][colIdx] * vector[colIdx];
		}
		transformed_vector[rowIdx] = totalTransform;
	}

	float transformed_vector_2[3];
	for (int rowIdx = 0; rowIdx < 3; rowIdx++) {
		float totalTransform = 0;
		for (int colIdx = 0; colIdx < 3; colIdx++) {
			float(*transformMatrix)[3];
			transformMatrix = MatRy(time);
			totalTransform += transformMatrix[rowIdx][colIdx] * transformed_vector[colIdx];
		}
		transformed_vector_2[rowIdx] = totalTransform;
	}
	return transformed_vector_2;
}

std::string VertexObject(float time) {
	std::string buffer[RADIUS * 2] = {};
	for (int i = 0; i < RADIUS; i++) {
		std::string buffer_row = "";
		for (int j = 0; j < RADIUS * 2; j++) {
			buffer_row.push_back(' ');
		}
		buffer[i] = buffer_row;
	}

	float vertices[8][3] = {
		{10,10,10},
		{10,10,-10},
		{10,-10,10},
		{10,-10,-10},
		{-10,10,10},
		{-10,10,-10},
		{-10,-10,10},
		{-10,-10,-10}
	};

	for (int i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i++) {
		float* vertexPtr = MatTransform(vertices[i], time);

		int x = floor(vertexPtr[0] + RADIUS);
		int y = floor((vertexPtr[1] + RADIUS) *.5);
		int z = floor(vertexPtr[2] + RADIUS);

		if (x >= 0 && y >= 0 && x < RADIUS * 2 && y < RADIUS) {
			char map[] = "-=+^#%@";
			int div = floor((z / float(RADIUS * 2 + 1)) * (sizeof(map) - 1));

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
		int half_radius = floor(RADIUS * .5);
		buffer[half_radius][RADIUS] = '+';

	}

	std::string outputString;
	for (int i = 0; i < RADIUS; i++) {
		outputString.append(buffer[i]);
		outputString.push_back('\n');
	}

	return outputString;
}

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
	for (int i = 0; i < RADIUS; i++) {
		std::string buffer_row = "";
		for (int j = 0; j < RADIUS * 2; j++) {
			buffer_row.push_back('*');
		}
		buffer[i] = buffer_row;
	}

	for (float i = 0; i <= 2 * PI; i += CALC_STEP) {
		int x = floor(time * RADIUS * cosf(i) + RADIUS);
		int y = floor(time * RADIUS * .5 * sinf(i) + RADIUS * .5);

		if (x >= 0 && y >= 0 && x < RADIUS * 2 && y < RADIUS) {
			buffer[y][x] = '#';
		}
	}

	std::string outputString;
	for (int i = 0; i < RADIUS; i++) {
		outputString.append(buffer[i]);
		outputString.push_back('\n');
	}

	return outputString;
}

std::string ParamTorus(float time) {
	std::string buffer[RADIUS * 2] = {};
	std::string outputString;

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

			int x = floor((INNER_RADIUS + TUBE_RADIUS * cosf(v)) * cosf(u) + RADIUS);
			int y = floor(.5 * (INNER_RADIUS + TUBE_RADIUS * cosf(v)) * sinf(u) + RADIUS * .5);
			int z = floor(TUBE_RADIUS * sinf(v) + TUBE_RADIUS);

			if (x >= 0 && y >= 0 && x < RADIUS * 2 && y < RADIUS) {
				char map[]  = "-=+*#%@";
				int div = floor((z / float(TUBE_RADIUS*2 + 1)) * (sizeof(map)-1));

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
		outputString.append(buffer[i]);
		outputString.push_back('\n');
	}

	return outputString;
}

void Clear() {
	system("cls");
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
			std::cout << VertexObject(time);
			std::this_thread::sleep_for(std::chrono::milliseconds(REFRESH_TIME));
			Clear();
			time += TIME_STEP * inc;
		}
	} else {
		std::cout << "Rendered \"" + FUNCTION << "\" (Image):\n";
		std::cout << VertexObject(0);
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