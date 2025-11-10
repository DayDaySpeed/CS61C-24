#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t *create_default_state() {
  // 分配 game_state_t 结构体
  game_state_t *state = malloc(sizeof(game_state_t));
  if (state == NULL) {
    return NULL;  // 分配失败
  }

  // 设置棋盘大小为 10x10
  state->num_rows = 18;
  state->board = malloc(sizeof(char *) * state->num_rows);
  if (state->board == NULL) {
    free(state);
    return NULL;
  }

  // 初始化每一行
  for (unsigned int i = 0; i < state->num_rows; i++) {
    state->board[i] = malloc(sizeof(char) * 19);  // 10列 + 1个 null terminator
    if (state->board[i] == NULL) {
      // 如果某一行分配失败，释放之前分配的所有行和 state
      for (unsigned int j = 0; j < i; j++) {
        free(state->board[j]);
      }
      free(state->board);
      free(state);
      return NULL;
    }
  }

  // 设置棋盘内容
  // 每行初始化为空格，并在末尾加 '\0' 作为字符串终止符
  for (unsigned int row = 0; row < state->num_rows; row++) {
    for (unsigned int col = 0; col < 10; col++) {
      state->board[row][col] = ' ';
    }
    state->board[row][10] = '\0';  // null terminator
  }

  // 设置果实位置
  state->board[2][2] = '*';

  // 设置蛇的身体：尾 -> 身体 -> 头
  state->board[4][4] = 'd';  // 尾部向右
  state->board[4][5] = '>';  // 身体向右
  state->board[4][6] = 'D';  // 头部向右

  // 分配蛇数组
  state->num_snakes = 1;
  state->snakes = malloc(sizeof(snake_t));
  if (state->snakes == NULL) {
    for (unsigned int i = 0; i < state->num_rows; i++) {
      free(state->board[i]);
    }
    free(state->board);
    free(state);
    return NULL;
  }

  // 初始化蛇的位置
  state->snakes[0].tail_row = 4;
  state->snakes[0].tail_col = 4;
  state->snakes[0].head_row = 4;
  state->snakes[0].head_col = 6;
  state->snakes[0].live = true;

  return state;
}


/* Task 2 */
void free_state(game_state_t *state) {
  if (state == NULL) return;

  // 释放每一行的棋盘
  for (unsigned int i = 0; i < state->num_rows; i++) {
    free(state->board[i]);
  }

  // 释放棋盘数组和蛇数组
  free(state->board);
  free(state->snakes);

  // 释放整个状态结构体
  free(state);
}


/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
  for (unsigned int i = 0; i < state->num_rows; i++) {
    fprintf(fp, "%s\n", state->board[i]);
  }
}


/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col) { return state->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  return c == 'w' || c == 's' || c == 'a' || c == 'd';
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  return c == 'W' || c == 'S' || c == 'A' || c == 'D' || c == 'x';
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  return is_tail(c) || is_head(c) || c == '^' || c == '<' || c == 'v' || c == '>';
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  if (c == '^') return 'w';
  if (c == '<') return 'a';
  if (c == 'v') return 's';
  if (c == '>') return 'd';
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  if (c == 'W') return '^';
  if (c == 'A') return '<';
  if (c == 'S') return 'v';
  if (c == 'D') return '>';
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  if (c == 'v' || c == 's' || c == 'S') return cur_row + 1;
  if (c == '^' || c == 'w' || c == 'W') return cur_row - 1;
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  if (c == '>' || c == 'd' || c == 'D') return cur_col + 1;
  if (c == '<' || c == 'a' || c == 'A') return cur_col - 1;
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum) {
  snake_t snake = state->snakes[snum];
  char head = get_board_at(state, snake.head_row, snake.head_col);
  unsigned int next_row = get_next_row(snake.head_row, head);
  unsigned int next_col = get_next_col(snake.head_col, head);
  return get_board_at(state, next_row, next_col);
}


/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t *state, unsigned int snum) {
  snake_t *snake = &state->snakes[snum];
  char head = get_board_at(state, snake->head_row, snake->head_col);
  char body = head_to_body(head);

  // 在原位置放置身体
  set_board_at(state, snake->head_row, snake->head_col, body);

  // 计算新位置
  unsigned int new_row = get_next_row(snake->head_row, head);
  unsigned int new_col = get_next_col(snake->head_col, head);

  // 更新蛇头位置
  snake->head_row = new_row;
  snake->head_col = new_col;

  // 设置新头字符
  set_board_at(state, new_row, new_col, head);
}


/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum) {
  snake_t *snake = &state->snakes[snum];
  char tail = get_board_at(state, snake->tail_row, snake->tail_col);

  // 清除旧尾部
  set_board_at(state, snake->tail_row, snake->tail_col, ' ');

  // 计算新尾部位置
  unsigned int new_row = get_next_row(snake->tail_row, tail);
  unsigned int new_col = get_next_col(snake->tail_col, tail);

  // 将新位置从身体变为尾部
  char body = get_board_at(state, new_row, new_col);
  set_board_at(state, new_row, new_col, body_to_tail(body));

  // 更新蛇尾位置
  snake->tail_row = new_row;
  snake->tail_col = new_col;
}


/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state)) {
  for (unsigned int i = 0; i < state->num_snakes; i++) {
    char next = next_square(state, i);

    if (next == '#' || is_snake(next)) {
      // 撞墙或撞蛇，死亡
      snake_t *snake = &state->snakes[i];
      set_board_at(state, snake->head_row, snake->head_col, 'x');
      snake->live = false;
    } else if (next == '*') {
      // 吃果实，增长
      update_head(state, i);
      add_food(state);
    } else {
      // 正常移动
      update_head(state, i);
      update_tail(state, i);
    }
  }
}


/* Task 5.1 */
/*
  从给定的文件指针 fp 中读取一行文本（不包括换行符），
  并返回一个以 '\0' 结尾的动态分配字符串。
  如果读取失败或遇到 EOF，返回 NULL。
  调用者需要手动 free 返回的字符串。
*/
char *read_line(FILE *fp) {
  char *line = NULL;     // 初始化指针，开始时没有分配内存
  size_t size = 0;       // 当前已读取的字符数
  int ch;                // 用于存储每次读取的字符（必须是 int，因为 fgetc 可能返回 EOF）

  // 循环读取字符，直到遇到换行符 '\n' 或文件结束 EOF
  while ((ch = fgetc(fp)) != '\n' && ch != EOF) {
    // 每读取一个字符，就扩展 line 的内存空间，多分配一个字符 + '\0'
    line = realloc(line, size + 2);  // +1 是新字符，+1 是终止符 '\0'
    if (!line) return NULL;          // 如果 realloc 失败，返回 NULL

    line[size++] = (char)ch;              // 将当前字符存入 line，并递增 size
  }

  // 如果至少读取了一个字符，就添加字符串终止符 '\0'
  if (line) line[size] = '\0';

  // 返回构造好的字符串（调用者需要 free）
  return line;
}



/* Task 5.2 */
game_state_t *load_board(FILE *fp) {
  game_state_t *state = malloc(sizeof(game_state_t));
  state->num_rows = 0;
  state->board = NULL;

  char *line;
  while ((line = read_line(fp)) != NULL) {
    state->board = realloc(state->board, sizeof(char *) * (state->num_rows + 1));
    state->board[state->num_rows++] = line;
  }

  return state;
}


/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum) {
  snake_t *snake = &state->snakes[snum];
  unsigned int row = snake->tail_row;
  unsigned int col = snake->tail_col;

  while (true) {
    char c = get_board_at(state, row, col);
    unsigned int next_row = get_next_row(row, c);
    unsigned int next_col = get_next_col(col, c);
    char next = get_board_at(state, next_row, next_col);

    if (is_head(next)) {
      snake->head_row = next_row;
      snake->head_col = next_col;
      return;
    }

    row = next_row;
    col = next_col;
  }
}


/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state) {
  state->num_snakes = 0;
  state->snakes = NULL;

  for (unsigned int row = 0; row < state->num_rows; row++) {
    for (unsigned int col = 0; col < strlen(state->board[row]); col++) {
      char c = get_board_at(state, row, col);
      if (is_tail(c)) {
        state->snakes = realloc(state->snakes, sizeof(snake_t) * (state->num_snakes + 1));
        snake_t *snake = &state->snakes[state->num_snakes++];
        snake->tail_row = row;
        snake->tail_col = col;
        snake->live = true;
        find_head(state, state->num_snakes - 1);
      }
    }
  }

  return state;
}

