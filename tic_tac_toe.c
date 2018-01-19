#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const int BOARD_SIZE = 15;
const char MARKER[3] = {' ', 'O', 'X'};
int computer_player;
int** board;
int player_number;

void shuffle(int *array, size_t n) {
	if (n > 1) {
		size_t i;
		for (i = 0; i < n - 1; i++) {
			  size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
			  int t = array[j];
			  array[j] = array[i];
			  array[i] = t;
		}
    	}
}

void print_break_line() {
	for (int i=0; i<4*(BOARD_SIZE+1); i++)
		printf("*");
	printf("\n");
}

void print_horizontal_title_line() {
	printf("   ");
	for (int i=0; i<BOARD_SIZE; i++)
		printf("| %c ", 'A' + i);
	printf("|\n");	
}

void print_horizontal_line() {
	for (int i=0; i<4*(BOARD_SIZE+1); i++)
		printf("-");
	printf("\n");
}

void print_board() {
	printf("Special commands: load, save, new, exit, finish\nYou can use special commands in each moment\n");
	print_horizontal_title_line();
	for (int i=0; i<BOARD_SIZE; i++) {
		print_horizontal_line();
		printf("%3d", i + 1);
		for (int j=0; j<BOARD_SIZE; j++)
			printf("| %c ", MARKER[board[i][j]]);
		printf("|\n");
	}
	print_horizontal_line();
}

void clear_board() {
	player_number = 0;
	for (int i=0; i<BOARD_SIZE; i++)
		for (int j=0; j<BOARD_SIZE; j++)
			board[i][j] = 0;
}

int check_main_command(char* command) {
	if (strcmp(command, "exit") == 0)
		return -1;
	if (strcmp(command, "save") == 0)
		return -2;
	if (strcmp(command, "load") == 0)
		return -3;
	if (strcmp(command, "new") == 0)
		return -4;
	if (strcmp(command, "finish") == 0)
		return -5;
	return 0;
}

int get_column_from_user() {
	char user_input[20];
	while (1) {
		printf("Choose column: ");
		scanf("%s", user_input);
		int main_command = check_main_command(user_input);
		if (main_command != 0)
			return main_command;
		char column = user_input[0];
		if (strlen(user_input) ==1 && column >= 'A' && column <= 'A' + BOARD_SIZE - 1)
			return column - 'A';
		printf("Incorrect input, please put only one character from A..%c\n", 'A' + BOARD_SIZE - 1);
	}
}

int get_row_from_user() {
	char user_input[20];
	char* end;
	while (1) {
		printf("Choose row: ");
		scanf("%s", user_input);
		int main_command = check_main_command(user_input);
		if (main_command != 0)
			return main_command;
		int row = strtol(user_input, &end, 0);
		if (strlen(end) == 0 && row > 0 && row <= BOARD_SIZE)
			return row - 1;
		printf("Incorrect input, please put a number from 1 to %d\n", BOARD_SIZE);
	}
}

int execute_move() {
	printf("Player %c move:\n", MARKER[player_number + 1]);
	int column, row;
	while (1) {
		if (player_number == computer_player) {
			int rows[BOARD_SIZE];
			int cols[BOARD_SIZE];
			for (int i=0; i<BOARD_SIZE; i++) {
				rows[i] = i;
				cols[i] = i;
			}
			shuffle(rows, BOARD_SIZE);
			shuffle(cols, BOARD_SIZE);
			row = -1;
			column = -1;
			for (int i=0; i<BOARD_SIZE; i++)
				for (int j=0; j<BOARD_SIZE; j++) {
					if (board[rows[i]][cols[j]] == 0) {
						row = rows[i];
						column = cols[j];
						break;
					}
				if (row != -1)
					break;
				}
			printf("Choose column: %c\nChoose row: %d\n", 'A' + column, row + 1);
		}
		else {
			column = get_column_from_user();
			if (column < 0)
				return column;
			row = get_row_from_user();
			if (row < 0)
				return row;
		}
		if (board[row][column] == 0) {
			board[row][column] = player_number + 1;
			return 0;
		}
		printf("This field is already occupied, please select another field\n");
	}
}

int** create_board() {
	int** board;
	board = malloc(BOARD_SIZE * sizeof(*board));
	for (int i=0; i<BOARD_SIZE; i++)
		board[i] = malloc(BOARD_SIZE * sizeof(*board[i]));
	return board;
}

int check_draw() {
	for (int i=0; i<BOARD_SIZE; i++)
		for (int j=0; j<BOARD_SIZE; j++)
			if (board[i][j] == 0)
				return 0;
	print_board();
	printf("No more moves -- draw\n");
	print_break_line();
	return 1;
}

int check_dir_win_from(int i, int j, int di, int dj, int n) {
	if (n == 0) {
		while (i < BOARD_SIZE && j < BOARD_SIZE && board[i][j] == player_number + 1) {
			i += di;
			j += dj;
		}
		i -= di;
		j -= dj;
		int count = 0;
		while (i >= 0 && j >= 0 && board[i][j] == player_number + 1) {
			i -= di;
			j -= dj;
			count++;
		}
		return count == 5;
	}
	if (i >= BOARD_SIZE || j >= BOARD_SIZE || board[i][j] != player_number + 1)
		return 0;
	return check_dir_win_from(i + di, j + dj, di, dj, n - 1);
}

int check_win_from(int i, int j) {
	return check_dir_win_from(i + 1, j, 1, 0, 4) || check_dir_win_from(i, j + 1, 0, 1, 4) || check_dir_win_from(i + 1, j + 1, 1, 1, 4);
}

int check_win() {
	for (int i=0; i<BOARD_SIZE; i++)
		for (int j=0; j<BOARD_SIZE; j++)
			if (board[i][j] == player_number + 1 && check_win_from(i, j)) {
				print_board();
				printf("Player %c won\n", MARKER[player_number + 1]);
				print_break_line();
				return 1;
			}
	return 0;
}

int check_game_finished() {
	return check_win() || check_draw();
}

void play_with_computer() {
	char user_input[20];
	while (1) {
		printf("Do you want to play with computer? [y/n]");
		scanf("%s", user_input);
		if (strlen(user_input) == 1 && (user_input[0] == 'y' || user_input[0] == 'n')) {
			if (user_input[0] == 'y') {
				computer_player = rand() % 2;
				printf("Computer plays %c\n", MARKER[computer_player + 1]);
			}
			else
				computer_player = 2;
			return;
		}
		printf("Incorrect input, please answer y or n\n");
	}
}

void start_new_game() {
	clear_board();
	play_with_computer();
}

void save_command() {
	FILE *f = fopen("save", "w");
	for (int i=0; i<BOARD_SIZE; i++)
		for (int j=0; j<BOARD_SIZE; j++)
			fprintf(f, "%d ", board[i][j]);
	fprintf(f, "%d ", player_number);
	fprintf(f, "%d ", computer_player);
	fclose(f);
	printf("Game saved\n");
}

void exit_command() {
	printf("Do you want to save the game? [y/n] ");
	char user_input[20];
	while (1) {
		scanf("%s", user_input);
		char save = user_input[0];
		if (strlen(user_input) == 1 && (save == 'y' || save == 'n')) {
			if (save == 'y')
				save_command();
			exit(0);
		}
		printf("Incorrect input, please answer y or n\n");
	}
}

void load_command() {
	FILE *f = fopen("save", "r");
	if (!f) {
		printf("There is no saved game, starting new game\n");
		start_new_game();
		return;
	}
	for (int i=0; i<BOARD_SIZE; i++)
		for (int j=0; j<BOARD_SIZE; j++)
			fscanf(f, "%d", &board[i][j]);
	fscanf(f, "%d", &player_number);
	fscanf(f, "%d", &computer_player);
	fclose(f);
	printf("Game loaded\n");
}

int main_menu() {
	char user_input[20];
	while (1) {
		printf("Menu:\n1) New game\n2) Load game\n3) Exit\n");
		scanf("%s", user_input);
		if (strlen(user_input) == 1 && (user_input[0] == '1' || user_input[0] == '2' || user_input[0] == '3')) {
			print_break_line();
			switch (user_input[0]) {
				case '1':
					start_new_game();
					return 0;
				case '2':
					load_command();
					return 0;
				case '3':
					return 1;
			}
		}
		printf("Invalid input, please put 1, 2 or 3\n");
	}
}

int main() {
	srand(time(NULL));
	board = create_board();
	int finish_program = 0;
	while (!finish_program) {
		finish_program = main_menu();
		if (!finish_program) {
			int game_finished = 0;
			while (!game_finished) {
				print_board();
				int main_command = execute_move();
				print_break_line();
				switch (main_command) {
					case -1:
						exit_command();
					case -2:
						save_command();
						continue;
					case -3:
						load_command();
						continue;
					case -4:
						clear_board();
						continue;
					case -5:
						game_finished = 1;
						printf("Game finished\n");
						break;
				
				}
				game_finished = game_finished || check_game_finished();
				player_number = (player_number + 1) % 2;
			}
		}
	}
	return 0;
}
