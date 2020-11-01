#include <bangtal.h>
#include <string>
#include <iostream>

using namespace std;
using namespace bangtal;

ObjectPtr board[8][8];
ObjectPtr whiteScore[2];
ObjectPtr blackScore[2];

enum class State {
	BLANK,
	POSSIBLE,
	BLACK,
	WHITE
};
State state[8][8];

enum class Turn {
	BLACK,
	WHITE
};
Turn turn = Turn::BLACK;

void setState(int x, int y, State s)
{
	switch (s) {
	case State::BLANK: board[y][x]->setImage("Images/blank.png"); break;
	case State::POSSIBLE: board[y][x]->setImage(turn == Turn::BLACK ? "Images/black possible.png" : "Images/white possible.png"); break;
	case State::BLACK: board[y][x]->setImage("Images/black.png"); break;
	case State::WHITE: board[y][x]->setImage("Images/white.png"); break;
	}
	state[y][x] = s;
}

bool checkPossible(int x, int y, int dx, int dy)
{
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (state[y][x] == other) {
			possible = true;
		}
		else if (state[y][x] == self) {
			return possible;
		}
		else {
			return false;
		}
	}

	return false;
}


bool checkPossible(int x, int y)
{
	if (state[y][x] == State::BLACK) return false;
	if (state[y][x] == State::WHITE) return false;
	setState(x, y, State::BLANK);

	int delta[8][2] = {
		{0,1},
		{1,1},
		{1,0},
		{1,-1},
		{0,-1},
		{-1,-1},
		{-1,0},
		{-1,1},
	};

	bool possible = false;
	for (auto d : delta) {
		if (checkPossible(x, y, d[0], d[1])) possible = true;
	}
	return possible;
}

void reverse(int x, int y, int dx, int dy)
{
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (state[y][x] == other) {
			possible = true;
		}
		else if (state[y][x] == self) {
			if (possible) {
				for (x -= dx, y -= dy; x >= 0 && x < 8 && y >= 0 && y < 8; x -= dx, y -= dy) {
					if (state[y][x] == other) {
						setState(x, y, self);
					}
					else {
						return;
					}
				}
			}
			else {
				return;
			}
			return;
		}
	}
}

void reverse(int x, int y)
{
	int delta[8][2] = {
	{0,1},
	{1,1},
	{1,0},
	{1,-1},
	{0,-1},
	{-1,-1},
	{-1,0},
	{-1,1},
	};

	bool possible = false;
	for (auto d : delta) {
		reverse(x, y, d[0], d[1]);
	}
}


bool setPossible()
{
	bool possible = false;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (checkPossible(x, y)) {
				setState(x, y, State::POSSIBLE);
				possible = true;
			}
		}
	}
	return possible;
}

void score()
{
	int bScore = 0;
	int wScore = 0;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (state[y][x] == State::BLACK) {
				bScore++;
			}
			else if (state[y][x] == State::WHITE) {
				wScore++;
			}
			blackScore[0]->setImage("Images/L" + to_string(bScore / 10) + ".png");
			blackScore[1]->setImage("Images/L" + to_string(bScore % 10) + ".png");
			whiteScore[0]->setImage("Images/L" + to_string(wScore / 10) + ".png");
			whiteScore[1]->setImage("Images/L" + to_string(wScore % 10) + ".png");
		}
	}
}

int main()
{
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

	auto scene = Scene::create("Othello", "Images/background.png");

	blackScore[0] = Object::create("Images/L0.png", scene, 745, 225);
	blackScore[1] = Object::create("Images/L2.png", scene, 835, 225);
	whiteScore[0] = Object::create("Images/L0.png", scene, 1065, 225);
	whiteScore[1] = Object::create("Images/L2.png", scene, 1155, 225);

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			board[y][x] = Object::create("Images/blank.png", scene, 40 + x * 80, 40 + y * 80);
			board[y][x]->setOnMouseCallback([&, x, y](ObjectPtr, int, int, MouseAction)->bool {
				if (state[y][x] == State::POSSIBLE) {
					if (turn == Turn::BLACK) {
						setState(x, y, State::BLACK);
						reverse(x, y);
						turn = Turn::WHITE;
						score();
					}
					else {
						setState(x, y, State::WHITE);
						reverse(x, y);
						turn = Turn::BLACK;
						score();
					}
					if (!setPossible()) {
						turn = turn == Turn::BLACK ? Turn::WHITE : Turn::BLACK;

						if (!setPossible()) {
							showMessage("End Game!");
						}
					}
				}
				return true;
				});
			state[y][x] = State::BLANK;
		}
	}

	setState(3, 3, State::BLACK);
	setState(4, 4, State::BLACK);
	setState(3, 4, State::WHITE);
	setState(4, 3, State::WHITE);

	setPossible();

	startGame(scene);
}