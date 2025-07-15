#pragma once
#include <iostream>
#include <cstring>
#include <termios.h>  // 用于配置终端I/O
#include <unistd.h>   // 提供Unix标准API

char charget(void){
  struct termios oldt, newt;
  char ch;
  // 获取当前终端配置
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  // 设置终端为原始模式：无回显、无需回车
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  // 读取单个字符
  ch = getchar();
  // 恢复终端配置
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
}

// 等待用户输入一个字符，最多等待 timeout_ms 毫秒
// 成功读取返回字符值，超时返回 -1
int tm_charget(int timeout_ms) {
    struct termios oldt, newt;
    int ch;

    // 获取当前终端配置
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    // 设置非规范模式，无回显
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // 设置select等待输入的时间
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);

    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int retval = select(STDIN_FILENO + 1, &rfds, NULL, NULL, &tv);
    if (retval == -1) {
        // select 出错
        ch = -1;
    } else if (retval) {
        // 有输入可读
        ch = getchar();
    } else {
        // 超时
        ch = -1;
    }
    // 恢复原来的终端配置
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}



// 判断 UTF-8 首字节，获取字符字节长度
int utf8_char_length(unsigned char c) {
    if ((c & 0x80) == 0x00) return 1;
    else if ((c & 0xE0) == 0xC0) return 2;
    else if ((c & 0xF0) == 0xE0) return 3;
    else if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

// 判断是否为宽字符（中文等），粗略判断 3字节及以上为宽字符
int is_wide_char(const unsigned char *p) {
    int len = utf8_char_length(p[0]);
    return len >= 3;
}

//key为0显式输入，为1隐式输入,为2则为聊天输入（最多500字）,返回读取字符的长度，返回-1说明用户需要ESC
int enter(char *arr, int key) {
    int i = 0;
    unsigned char ch;

    while (1) {
        ch = charget();
        if(i >= 20 && ch != '\b' && ch != 127 && ch != '\n' && ch != 27) continue;
        if (ch == 27) return -1;  // ESC
        if (ch == '\n') {
            arr[i] = '\0';
            break;
        }
        // 回退：退格或DEL
        if (ch == '\b' || ch == 127) {
            if (i == 0) continue;

            // 向前找 UTF-8 起始字节
            int len = 1;
            while (i - len > 0 && (arr[i - len] & 0xC0) == 0x80)
                len++;

            int char_start = i - len;

            int display_width = is_wide_char((unsigned char *)&arr[char_start]) ? 2 : 1;

            // 删除字符
            i = char_start;
            arr[i] = '\0';

            // 清除终端字符（显示宽度）
            for (int j = 0; j < display_width; j++) {
                printf("\b \b");
            }

            fflush(stdout);
            continue;
        }

        // 正常字符输入
        int len = utf8_char_length(ch);
        if (i + len >= 255) continue;

        arr[i++] = ch;
        for (int j = 1; j < len; ++j) {
            unsigned char next = charget();
            arr[i++] = next;
        }
        arr[i] = '\0';

        if (key == 1) {
            int display_width = is_wide_char((unsigned char *)&arr[i - len]) ? 2 : 1;
            for (int j = 0; j < display_width; j++) putchar('*');
        } else {
            fwrite(&arr[i - len], 1, len, stdout);
        }

        fflush(stdout);
    }

    printf("\n");
    return i;
}




//key为0显式输入，为1隐式输入,返回读取字符的长度，返回-1说明用户需要ESC
// int enter(char *arr, int key){
//     char a = 0;
//     int i;
//     for (i = 0; i < 20; i++){
//         arr[i] = charget();
//         if(arr[i] == 27) return -1;
//         if (arr[i] == '\n'){
//             arr[i] = '\0';
//             break;
//         }
//         else if (arr[i] == '\b'||arr[i] == 127){
//             if (i>0){
//                 i--;
//                 printf("\b \b");
//             }
//             i--;
//             continue;
//         }
//         else if (i == 19){
//             arr[i] = a;
//             i--;
//         }
//         else{
//             if(key == 1)
//                 putchar('*');
//             else printf("%c",arr[i]);
//         }
//         a = arr[i];
//     }
//     printf("\n");
//     return i;
// }

//初始化数组内元素
template <size_t N>
void chu(char (&arr)[N]) {
    memset(arr, 0, N);
}



// 判断一个 UTF-8 字符是否为英文字母、数字或中文（常见汉字范围）
int is_valid_utf8_char(const unsigned char *p, int len) {
    if (len == 1) {
        return ((p[0] >= 'a' && p[0] <= 'z') ||
                (p[0] >= 'A' && p[0] <= 'Z') ||
                (p[0] >= '0' && p[0] <= '9'));
    } else if (len == 3) {
        unsigned char c1 = p[0];
        // 常见汉字 0xE4~0xE9 开头（UTF-8）
        if (c1 >= 0xE4 && c1 <= 0xE9) return 1;
        return 0;
    }

    // 不允许 emoji、日文、俄文等
    return 0;
}

//用户名判断函数，返回实际字符数，若非法则返回-1
int is_valid_username(const char *name) {
    int i = 0;
    int char_count = 0;

    while (name[i] != '\0') {
        int len = utf8_char_length((unsigned char)name[i]);

        if (name[i + len - 1] == '\0') {
            // 字符末尾不完整（UTF-8 截断）
            return -1;
        }

        if (!is_valid_utf8_char((const unsigned char *)&name[i], len)) {
            return -1;
        }

        i += len;
        char_count++;
    }

    return char_count;
}