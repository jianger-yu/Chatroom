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

//key为0显式输入，否则隐式输入,返回读取字符的长度，返回-1说明用户需要ESC
int enter(char *arr, int key){
    char a = 0;
    int i;
    for (i = 0; i < 20; i++){
        arr[i] = charget();
        if(arr[i] == 27) return -1;
        if (arr[i] == '\n'){
            arr[i] = '\0';
            break;
        }
        else if (arr[i] == '\b'||arr[i] == 127){
            if (i>0){
                i--;
                printf("\b \b");
            }
            i--;
            continue;
        }
        else if (i == 19){
            arr[i] = a;
            i--;
        }
        else{
            if(key == 1)
                putchar('*');
            else printf("%c",arr[i]);
        }
        a = arr[i];
    }
    printf("\n");
    return i;
}

//初始化数组内元素
void chu(char* arr){
    memset(arr, 0, sizeof arr);
}