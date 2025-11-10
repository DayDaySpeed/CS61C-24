#include <stdio.h>
#include <string.h>

#include "snake_utils.h"
#include "state.h"

int main(int argc, char *argv[]) {
  bool io_stdin = false;
  char *in_filename = NULL;
  char *out_filename = NULL;
  game_state_t *state = NULL;

  // Parse arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-i") == 0 && i < argc - 1) {
      if (io_stdin) {
        fprintf(stderr, "Usage: %s [-i filename | --stdin] [-o filename]\n", argv[0]);
        return 1;
      }
      in_filename = argv[i + 1];
      i++;
      continue;
    } else if (strcmp(argv[i], "--stdin") == 0) {
      if (in_filename != NULL) {
        fprintf(stderr, "Usage: %s [-i filename | --stdin] [-o filename]\n", argv[0]);
        return 1;
      }
      io_stdin = true;
      continue;
    }
    if (strcmp(argv[i], "-o") == 0 && i < argc - 1) {
      out_filename = argv[i + 1];
      i++;
      continue;
    }
    fprintf(stderr, "Usage: %s [-i filename | --stdin] [-o filename]\n", argv[0]);
    return 1;
  }

  // Do not modify anything above this line.

  /* Task 7 */

  // 📥 读取棋盘：从文件、stdin 或创建默认状态
  if (in_filename != NULL) {
    FILE *fp = fopen(in_filename, "r");
    if (fp == NULL) {
      fprintf(stderr, "Error: Cannot open input file %s\n", in_filename);
      return -1;
    }
    state = load_board(fp);              // 从文件读取棋盘
    state = initialize_snakes(state);    // 初始化蛇信息
    fclose(fp);                          // 关闭文件
  } else if (io_stdin) {
    state = load_board(stdin);           // 从标准输入读取棋盘
    state = initialize_snakes(state);    // 初始化蛇信息
  } else {
    state = create_default_state();      // 创建默认棋盘和蛇
  }

  // 🐍 更新游戏状态：移动蛇、吃果实、死亡判定
  update_state(state, deterministic_food);

  // 📤 输出棋盘：保存到文件或打印到 stdout
  if (out_filename != NULL) {
    save_board(state, out_filename);     // 保存到指定文件
  } else {
    print_board(state, stdout);          // 打印到标准输出
  }

  // 🧹 释放内存
  free_state(state);

  return 0;
}
