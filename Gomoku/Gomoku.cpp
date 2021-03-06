// Gomoku.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <algorithm>
#include <functional>
#include <cmath>
#include <vector>
#include <iostream>

using namespace std;

int factorial(int n)
{
	if (n == 0) return 1;
	return n * factorial(n - 1);
}

class Board
{
public:
	const int size = 8;
private:
	vector<vector<int>> grid;
	int stones = 0;
public:
	Board(int s) : 
		size(s), grid(s, vector<int>(s))
	{}
	int stone(int row, int col) const
	{
		return valid(row, col) ? 
			grid[row][col] : NULL;
	}
	bool valid(int row, int col) const
	{
		return row >= 0 && row < size && 
			col >= 0 && col < size;
	}
	bool place(int row, int col, bool side)
	{
		if (valid(row, col) && !grid[row][col])
		{
			grid[row][col] = side ? 1 : -1;
			++stones;
			return true;
		}
		return false;
	}
	void reset()
	{
		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < size; ++j)
			{
				grid[i][j] = 0;
			}
		}
		stones = 0;
	}
	bool check(int &result) const
	{
		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < size; ++j)
			{
				if (grid[i][j] == 0) continue;
				if (j + 5 <= size)
				{
					bool gomoku = true;
					for (int k = 0; k < 5; ++k)
					{
						if (grid[i][j] != grid[i][j + k])
						{
							gomoku = false;
							break;
						}
					}
					if (gomoku) return result = grid[i][j];
				}
				if (i + 5 <= size)
				{
					bool gomoku = true;
					for (int k = 0; k < 5; ++k)
					{
						if (grid[i][j] != grid[i + k][j])
						{
							gomoku = false;
							break;
						}
					}
					if (gomoku) return result = grid[i][j];
				}
				if (i + 5 <= size && j + 5 <= size)
				{
					bool gomoku = true;
					for (int k = 0; k < 5; ++k)
					{
						if (grid[i][j] != grid[i + k][j + k])
						{
							gomoku = false;
							break;
						}
					}
					if (gomoku) return result = grid[i][j];
				}
				if (i + 5 <= size && j - 5 >= -1)
				{
					bool gomoku = true;
					for (int k = 0; k < 5; ++k)
					{
						if (grid[i][j] != grid[i + k][j - k])
						{
							gomoku = false;
							break;
						}
					}
					if (gomoku) return result = grid[i][j];
				}
			}
		}
		return stones < size * size ? 0 : INT_MAX;
	}
	void display() const
	{
		system("cls");
		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < size; ++j)
			{
				switch (grid[i][j])
				{
				case 1:
					cout << 'B' << ' '; break;
				case -1:
					cout << 'W' << ' '; break;
				case 0:
					cout << '@' << ' ';
				}
			}
			cout << endl;
		}
	}
};

class Player
{
public:
	virtual void play(Board &board, bool side) const
	{
		for (int i = 0; i < board.size; ++i)
		{
			for (int j = 0; j < board.size; ++j)
			{
				if (board.place(i, j, side))
					return;
			}
		}
	}
};

class Console : public Player
{
public:
	void play(Board &board, bool side) const override
	{
		int row, col;
		do
		{
			cin >> row >> col;
		} while (!board.place(row, col, side));
	}
};

class Minimax : public Player
{
	const int maxDepth;
	const function<double(const Board&)> H;
public:
	Minimax(int d, double (*h)(const Board&)) : 
		maxDepth(d), H(h)
	{}
	void play(Board &board, bool side) const override
	{
		cout << maximize(board, side, 0, -INFINITY, INFINITY) << endl;
	}
private:
	double evaluate(const Board &board, bool side, int result, int depth) const
	{
		int colour = side ? 1 : -1;
		if (result != 0) 
		{
			if (result == colour)
				return INT_MAX - depth;
			else if (result == -colour)
				return -INT_MAX + depth;
			else
				return 0;
		}
		else
			return colour * H(board);
	}
	double maximize(Board &board, bool side, 
		int depth, double alpha, double beta) const
	{
		int result = 0;
		if (board.check(result) || depth == maxDepth)
			return evaluate(board, side, result, depth);
		double maxScore = -INFINITY;
		int m = (board.size - 1) / 2;
		int x = 0, y = 0, dx = 0, dy = -1;
		int row = -1, col = -1;
		while (board.valid(m + x, m + y))
		{
			Board newBoard = board;
			if (newBoard.place(m + x, m + y, side))
			{
				double score = -maximize(newBoard, !side, depth + 1, -beta, -alpha);
				if (score > maxScore)
				{
					row = m + x; col = m + y;
					maxScore = score;
					alpha = max(alpha, maxScore);
					if (alpha >= beta)
						return maxScore;
				}
			}
			if (x == y && x <= 0)
			{
				--x; --y;
			}
			if (x == y || x == -y)
			{
				
				int t = dx; dx = -dy; dy = t;
			}
			x += dx; y += dy;
		}
		board.place(row, col, side);
		return maxScore;
	}
};

class Game
{
	Player *black, *white;
	Board board;
public:
	Game(int s, Player *b, Player *w) :
		board(s), black(b), white(w)
	{}
	void run()
	{
		board.reset();
		bool side = true;
		int result = 0;
		while (!board.check(result))
		{
			board.display();
			cout << (side ? 'B' : 'W') << endl;
			(side ? black : white)->play(board, side);
			side = !side;
		}
		board.display();
		cout << result << endl;
	}
};

class Heuristic
{
public:
	static double expt(const Board &board)
	{
		double val = 0;
		for (int i = 0; i < board.size; ++i)
		{
			for (int j = 0; j < board.size; ++j)
			{
				if (i < board.size - 4)
				{
					int black = 0, white = 0;
					for (int k = 0; k < 5; ++k)
					{
						if (board.stone(i + k, j) == 1)
							++black;
						else if (board.stone(i + k, j) == -1)
							++white;
					}
					if (!white) val += exp2(black);
					if (!black) val -= exp2(white);
				}
				if (j < board.size - 4)
				{
					int black = 0, white = 0;
					for (int k = 0; k < 5; ++k)
					{
						if (board.stone(i, j + k) == 1)
							++black;
						else if (board.stone(i, j + k) == -1)
							++white;
					}
					if (!white) val += exp2(black);
					if (!black) val -= exp2(white);
				}
				if (i < board.size - 4 && j < board.size - 4)
				{
					int black = 0, white = 0;
					for (int k = 0; k < 5; ++k)
					{
						if (board.stone(i + k, j + k) == 1)
							++black;
						else if (board.stone(i + k, j + k) == -1)
							++white;
					}
					if (!white) val += exp2(black);
					if (!black) val -= exp2(white);
				}
				if (i < board.size - 4 && j >= 4)
				{
					int black = 0, white = 0;
					for (int k = 0; k < 5; ++k)
					{
						if (board.stone(i + k, j - k) == 1)
							++black;
						else if (board.stone(i + k, j - k) == -1)
							++white;
					}
					if (!white) val += exp2(black);
					if (!black) val -= exp2(white);
				}
			}
		}
		return val;
	}
	static double fact(const Board &board)
	{
		double val = 0;
		for (int i = 0; i < board.size; ++i)
		{
			for (int j = 0; j < board.size; ++j)
			{
				if (i < board.size - 4)
				{
					int black = 0, white = 0;
					for (int k = 0; k < 5; ++k)
					{
						if (board.stone(i + k, j) == 1)
							++black;
						else if (board.stone(i + k, j) == -1)
							++white;
					}
					if (!white) val += factorial(black);
					if (!black) val -= factorial(white);
				}
				if (j < board.size - 4)
				{
					int black = 0, white = 0;
					for (int k = 0; k < 5; ++k)
					{
						if (board.stone(i, j + k) == 1)
							++black;
						else if (board.stone(i, j + k) == -1)
							++white;
					}
					if (!white) val += factorial(black);
					if (!black) val -= factorial(white);
				}
				if (i < board.size - 4 && j < board.size - 4)
				{
					int black = 0, white = 0;
					for (int k = 0; k < 5; ++k)
					{
						if (board.stone(i + k, j + k) == 1)
							++black;
						else if (board.stone(i + k, j + k) == -1)
							++white;
					}
					if (!white) val += factorial(black);
					if (!black) val -= factorial(white);
				}
				if (i < board.size - 4 && j >= 4)
				{
					int black = 0, white = 0;
					for (int k = 0; k < 5; ++k)
					{
						if (board.stone(i + k, j - k) == 1)
							++black;
						else if (board.stone(i + k, j - k) == -1)
							++white;
					}
					if (!white) val += factorial(black);
					if (!black) val -= factorial(white);
				}
			}
		}
		return val;
	}
};

int main()
{
	Console console;
	auto func1 = Heuristic::expt;
	auto func2 = Heuristic::fact;
	Minimax black(4, func1);
	Minimax white(4, func2);
	Game game(13, &black, &white);
	game.run();
	return 0;
}
