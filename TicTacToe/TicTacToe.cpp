#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <array>
#include <vector>

using namespace std;

class Terminal {
public:
	template <class... Ts>
	void output(std::ostream& os, Ts const&... args) {
		using expander = int[];
		(void)expander {
			0, (void(os << args), 0)...
		};
	}

	template <class... Ts>
	void input(std::istream& is, Ts &... args) {
		using expander = int[];
		(void)expander {
			0, (void(is >> args), 0)...
		};
	}
};

constexpr int TABLE_SIZE = 81;

class BoardMatcher {

public:
	template<int A, int B, int C, int D>
	struct ValuesMatcher {
		enum {
			result = false
		};
	};

	template<int V>
	struct ValuesMatcher<V, V, V, V> {
		enum {
			result = V != NULL ? true : false
		};
	};

	template<int INDEX = 0, int ...D>
	struct LookupTableCreator :
		LookupTableCreator<INDEX + 1, D..., ValuesMatcher<INDEX % 3, INDEX % 9 / 3, INDEX % 27 / 9, INDEX / 27>::result> {
	};

	template<int ...D>
	struct LookupTableCreator<TABLE_SIZE, D...> {
		static constexpr array<bool, TABLE_SIZE> lookupTable = { D... };
	};
};

constexpr array<bool, TABLE_SIZE> lookupTable = BoardMatcher::LookupTableCreator<>::lookupTable;

class Game {
public:
	enum class status { success, taken, invalid };

	char board[3][3];
	char next = 'X';
	char winner = NULL;

	Game() {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				board[i][j] = '*';
			}
		}
	}

	status place(int posX, int posY) {
		if (posX < 0 || posX > 2 || posY < 0 || posY > 2) {
			return status::invalid;
		}

		if (board[posX][posY] != '*') {
			return status::taken;
		}

		board[posX][posY] = next;

		getWinner();
		getNextTurn();

		return status::success;
	}

	void getNextTurn() {
		next = next == 'X' ? 'O' : 'X';
	}

	void getWinner() {
		winner = checkWinner(board);
	}

	char checkWinner(char board[3][3]) {
		for (int i = 0; i < 3; i++) {
			if (checkValues(board[i][0], board[i][1], board[i][2], 'X')
				|| checkValues(board[0][i], board[1][i], board[2][i], 'X')) {
				return 'X';
			}

			if (checkValues(board[i][0], board[i][1], board[i][2], 'O')
				|| checkValues(board[0][i], board[1][i], board[2][i], 'O')) {
				return 'O';
			}
		}

		if (checkValues(board[0][0], board[1][1], board[2][2], 'X')
			|| checkValues(board[0][2], board[1][1], board[2][0], 'X')) {
			return 'X';
		}

		if (checkValues(board[0][0], board[1][1], board[2][2], 'O')
			|| checkValues(board[0][2], board[1][1], board[2][0], 'O')) {
			return 'O';
		}

		return NULL;
	}

	bool checkValues(char a, char b, char c, char d) {
		return lookupTable[
			getSymbolIndex(a) +
			3 * getSymbolIndex(b) +
			9 * getSymbolIndex(c) +
			27 * getSymbolIndex(d)
		];
	}

	int getSymbolIndex(char symbol) {
		return symbol == '*' ? 0 : (symbol == 'X' ? 1 : 2);
	}
};

class CLI {
public:
	Terminal terminal;
	Game game;

	CLI(Terminal terminal) : terminal(terminal) {}

	void printHelpMessage() {
		terminal.output(
			cout,
			"The game supports following commands:\n\n",
			"  place - format: \"place X, Y\". Places the mark into coordinates (X, Y).\n",
			"  quit - Quits the simulator.\n\n"
		);
	}

	void printPlayerTurn() {
		terminal.output(cout, "Player ", game.next, " turn.\n");
	}

	void printBoard() {
		terminal.output(
			cout,
			"| - - - |\n"
		);

		for (int i = 0; i < 3; i++) {
			terminal.output(
				cout,
				"| "
			);

			for (int j = 0; j < 3; j++) {
				terminal.output(
					cout,
					game.board[i][j], " "
				);
			}

			terminal.output(cout, "|\n");
		}

		terminal.output(cout, "| - - - |\n\n");
	}

	void printWinner() {
		terminal.output(cout, "Player ", game.winner, " won!!\n");
	}

	void receiveCommand() {
		string command = "";
		int posX = -1;
		int posY = -1;

		terminal.output(cout, "> ");
		terminal.input(cin, command);

		transform(
			command.begin(),
			command.end(),
			command.begin(),
			[](unsigned char c) { return tolower(c); }
		);

		if (command == "place") {
			if (posX == -1) {
				terminal.input(cin, posX);
			}

			if (posY == -1) {
				terminal.input(cin, posY);
			}
		}

		executeCommand(command, posX, posY);
	}

	void executeCommand(string command, int posX, int posY) {
		if (command == "quit") {
			terminal.output(
				cout,
				"Are you sure? Y/N\n",
				"> "
			);

			string exitCommand;
			terminal.input(cin, exitCommand);

			transform(
				exitCommand.begin(),
				exitCommand.end(),
				exitCommand.begin(),
				[](unsigned char c) { return tolower(c); }
			);

			executeExitCommand(exitCommand);
		}
		else if (command == "place") {
			Game::status status = game.place(posX, posY);

			if (status == Game::status::success) {
				printBoard();

				if (game.winner != NULL) {
					printWinner();
				}
				else {
					printPlayerTurn();
					receiveCommand();
				}
			}
			else if (status == Game::status::taken) {
				terminal.output(
					cout,
					"The position: (", posX, ", ", posY, ") already has the symbol.\n"
				);
				receiveCommand();
			}
			else if (status == Game::status::invalid) {
				terminal.output(
					cout,
					"You passed wrong position. Check if you are passing positions between 0 and 2.\n"
				);
				receiveCommand();
			}
		}
		else {
			terminal.output(
				cout,
				"Invalid command. I don't know what to do.\n"
			);
			printHelpMessage();
			receiveCommand();
		}
	}

	void executeExitCommand(string exitCommand) {
		if (exitCommand == "y") {
			terminal.output(
				cout,
				"Bye!\n"
			);
		}
		else if (exitCommand == "n") {
			receiveCommand();
		}
		else {
			executeCommand(exitCommand, -1, -1);
		}
	}
};

int main() {
	Terminal terminal;
	terminal.output(cout, "Welcome to the Tic-Tac-Toe game!\n\n");

	CLI cli(terminal);
	cli.printHelpMessage();

	Game game;
	cli.game = game;

	terminal.output(cout, "New game started!\n");

	cli.printPlayerTurn();
	cli.printBoard();

	cli.receiveCommand();
}
