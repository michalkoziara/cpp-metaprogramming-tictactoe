#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

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
			if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][2] == 'X'
				|| board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[2][i] == 'X') {
				return 'X';
			}

			if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][2] == 'O'
				|| board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[2][i] == 'O') {
				return 'O';
			}
		}

		if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[2][2] == 'X'
			|| board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[2][0] == 'X') {
			return 'X';
		}

		if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[2][2] == 'O'
			|| board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[2][0] == 'O') {
			return 'O';
		}

		return NULL;
	}
};

class CLI {
public:
	Game game;

	void printHelpMessage() {
		cout << "The game supports following commands:" << endl << endl;
		cout << "  place - format: \"place X, Y\". Places the mark into coordinates (X, Y)." << endl;
		cout << "  quit - Quits the simulator." << endl << endl;
	}

	void printPlayerTurn() {
		cout << "Player " << game.next << " turn." << endl;
	}

	void printBoard() {
		cout << "| - - - |" << endl;

		for (int i = 0; i < 3; i++) {
			cout << "| ";

			for (int j = 0; j < 3; j++) {
				cout << game.board[i][j] << " ";
			}

			cout << "|" << endl;
		}

		cout << "| - - - |" << endl << endl;
	}

	void printWinner() {
		cout << "Player " << game.winner << " won!!" << endl;
	}

	void receiveCommand() {
		string command = "";
		int posX = -1;
		int posY = -1;

		cout << "> ";
		cin >> command;

		transform(
			command.begin(),
			command.end(),
			command.begin(),
			[](unsigned char c) { return tolower(c); }
		);

		if (command == "place") {
			if (posX == -1) {
				cin >> posX;
			}

			if (posY == -1) {
				cin >> posY;
			}
		}

		executeCommand(command, posX, posY);
	}

	void executeCommand(string command, int posX, int posY) {
		if (command == "quit") {
			cout << "Are you sure? Y/N" << endl;

			cout << "> ";

			string exitCommand;
			cin >> exitCommand;

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
				cout << "The position: (" << posX << ", " << posY << ") already has the symbol." << endl;
				receiveCommand();
			}
			else if (status == Game::status::invalid) {
				cout << "You passed wrong position. Check if you are passing positions between 0 and 2." << endl;
				receiveCommand();
			}
		}
		else {
			cout << "Invalid command. I don't know what to do." << endl;
			printHelpMessage();
			receiveCommand();
		}
	}

	void executeExitCommand(string exitCommand) {
		if (exitCommand == "y") {
			cout << "Bye!" << endl;
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
	cout << "Welcome to the Tic-Tac-Toe game!" << endl << endl;

	CLI cli;
	cli.printHelpMessage();

	Game game;
	cli.game = game;

	cout << "New game started!" << endl;

	cli.printPlayerTurn();
	cli.printBoard();

	cli.receiveCommand();
}
