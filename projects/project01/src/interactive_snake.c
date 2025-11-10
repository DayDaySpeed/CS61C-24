// 启用 POSIX.1b 实时扩展功能，例如 nanosleep
#define _POSIX_C_SOURCE 199506L

// 引入自定义头文件，定义游戏逻辑和状态结构
#include "snake_utils.h"
#include "state.h"

// 标准库头文件
#include <errno.h>      // 错误处理
#include <pthread.h>    // POSIX 线程支持
#include <stdio.h>      // 输入输出
#include <stdlib.h>     // 内存分配、转换函数
#include <string.h>     // 字符串处理
#include <termios.h>    // 终端控制（用于禁用回显和规范模式）
#include <time.h>       // 时间控制（如 nanosleep）
#include <unistd.h>     // POSIX 系统调用（如 read）

// 警告：此代码使用了一些不推荐的技巧，仅供学习参考

// 游戏帧间隔，初始为 1 秒
struct timespec game_interval = {1, 0L};

// 全局游戏状态指针
game_state_t *state = NULL;

// 用于线程同步的互斥锁
pthread_mutex_t state_mutex;

// 获取用户输入的单个字符（禁用终端规范模式和回显）
int get_raw_char() {
  // 如果不是终端设备（例如输入被重定向），使用标准 getchar
  if (!isatty(STDIN_FILENO)) {
    return getchar();
  }

  unsigned char buf = 0;
  struct termios old = {0};  // 保存原始终端设置

  // 获取当前终端设置
  if (tcgetattr(STDIN_FILENO, &old) < 0) {
    perror("Error getting terminal attributes");
  }

  // 禁用规范模式和回显
  tcflag_t old_lflag = old.c_lflag;
  old.c_lflag &= (tcflag_t)~ICANON;
  old.c_lflag &= (tcflag_t)~ECHO;

  // 设置读取条件：至少一个字符，无超时
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;

  // 应用新设置
  if (tcsetattr(STDIN_FILENO, TCSANOW, &old) < 0) {
    perror("Error disabling terminal canonical mode");
  }

  // 读取一个字符
  if (read(STDIN_FILENO, &buf, 1) < 0) {
    perror("Error reading char from terminal");
  }

  // 恢复原始终端设置
  old.c_lflag = old_lflag;
  if (tcsetattr(STDIN_FILENO, TCSADRAIN, &old) < 0) {
    perror("Error re-enabling terminal canonical mode");
  }

  return (int)(unsigned char)buf;
}

// 清屏并打印游戏界面
void print_fullscreen_board(game_state_t *state) {
  fprintf(stdout, "\033[2J\033[H");  // ANSI 转义码：清屏并将光标移到左上角
  print_board(state, stdout);       // 打印当前游戏状态
}

// 游戏主循环（运行在子线程中）
void *game_loop(void *_) {
  unsigned int timestep = 0;  // 时间步计数器
  print_fullscreen_board(state);  // 打印初始界面

  while (1) {
    nanosleep(&game_interval, NULL);  // 等待一个时间间隔

    pthread_mutex_lock(&state_mutex);  // 加锁访问游戏状态
    int live_snakes = 0;

    // 遍历所有蛇，统计存活蛇数量
    for (int j = 0; j < state->num_snakes; j++) {
      if (state->snakes[j].live) {
        live_snakes += 1;

        // 非玩家控制的蛇每 6 步随机转向
        if (j >= 1 && timestep % 6 == 0) {
          random_turn(state, j);
        }
      }
    }

    // 更新游戏状态（移动蛇、吃食物等）
    update_state(state, deterministic_food);
    pthread_mutex_unlock(&state_mutex);  // 解锁

    print_fullscreen_board(state);  // 打印新状态
    timestep += 1;

    // 所有蛇死亡则退出循环
    if (live_snakes == 0) {
      break;
    }
  }

  return NULL;
}

// 用户输入循环（运行在主线程中）
void input_loop() {
  while (1) {
    char key = (char)get_raw_char();  // 获取用户输入
    pthread_mutex_lock(&state_mutex); // 加锁访问游戏状态

    // 调整游戏速度
    if (key == '[') {
      if (game_interval.tv_nsec >= 900000000L) {
        game_interval.tv_sec++;
        game_interval.tv_nsec = 0L;
      } else {
        game_interval.tv_nsec += 100000000L;
      }
    } else if (key == ']') {
      if (game_interval.tv_nsec == 0L) {
        game_interval.tv_sec--;
        game_interval.tv_nsec = 900000000L;
      } else if (game_interval.tv_sec > 0 || game_interval.tv_nsec > 100000000L) {
        game_interval.tv_nsec -= 100000000L;
      }
    } else {
      // 控制玩家蛇的方向
      redirect_snake(state, key);
    }

    pthread_mutex_unlock(&state_mutex);  // 解锁
    print_fullscreen_board(state);       // 刷新界面
  }
}

// 主函数入口
int main(int argc, char *argv[]) {
  char *in_filename = NULL;  // 输入文件名（可选）

  // 解析命令行参数
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-i") == 0 && i < argc - 1) {
      in_filename = argv[i + 1];  // 读取输入文件名
      i++;
      continue;
    }
    if (strcmp(argv[i], "-d") == 0 && i < argc - 1) {
      double delay = strtod(argv[i + 1], NULL);  // 读取延迟时间
      if (delay == 0.0 && errno != 0) {
        perror("Error parsing delay");
      }
      game_interval.tv_sec = (time_t)(unsigned int)delay;
      game_interval.tv_nsec = (long)(delay * 1000000000) % 1000000000L;
      i++;
      continue;
    }

    // 参数错误，打印用法提示
    fprintf(stderr, "Usage: %s [-i filename] [-d delay]\n", argv[0]);
    return 1;
  }

  // 加载游戏状态
  if (in_filename != NULL) {
    FILE *fp = fopen(in_filename, "r");
    state = load_board(fp);           // 从文件加载地图
    state = initialize_snakes(state); // 初始化蛇的位置
  } else {
    state = create_default_state();   // 创建默认地图和蛇
  }

  // 创建游戏线程
  pthread_t thread_id;
  pthread_create(&thread_id, NULL, game_loop, NULL);

  // 启动用户输入循环（阻塞）
  input_loop();

  // 游戏结束后取消游戏线程
  pthread_cancel(thread_id);

  return 0;
}
