#pragma once
#include<iostream>
#include "login/allfucs.h"


void startmenu(void){
  printf("\033[0;36m==========================================================\033[0m\n");
  printf("           \033[0;33m欢迎进入聊天室,请选择要进行的操作\033[0m\n");
  printf("       \033[0;32m------------1.登录------------\033[0m\n");
  printf("       \033[0;32m------------2.注册------------\033[0m\n");
  printf("       \033[0;32m-----------3.找回密码----------\033[0m\n");
  printf("       \033[0;32m------------4.退出------------\033[0m\n");
  printf("\033[0;36m==========================================================\033[0m\n");

  printf("\033[0;32m请输入要选择功能的序号:>\033[0m");
  fflush(stdout); // 手动刷新标准输出缓冲区
}


void logmenu(){
    printf("\033[0;32m已选择登录选项(按下ESC可返回上一级菜单)\033[0m\n");
    printf("\033[0;32m请选择您的登录方式:\n\033[0m");
    printf("\033[0;33m1.用户名、密码登录\n\033[0m");
    printf("\033[0;33m2.邮箱、验证码登录\n\033[0m");
    printf("\033[0;32m请输入要选择的序号:>\033[0m");
    fflush(stdout); // 手动刷新标准输出缓冲区
}

void usermainmenu(user u, void *p){
  reportfucs rpf(u, p);
  printf("\033[0;32m数据请求中...\033[0m\n");
  bool ret = rpf.Getrpt();
  fflush(stdout); // 手动刷新标准输出缓冲区
  system("clear");
  printf("\033[0;36m==========================================================\033[0m\n");
  printf("                \033[0;33m用户%s,欢迎使用聊天室\033[0m\n", u.name.c_str());
  if(ret){
    if(rpf.rpt.friendapply.size())
  printf("           \033[0;34m[new] 有%ld条未处理的好友申请\033[0m\n", rpf.rpt.friendapply.size());
    if(rpf.rpt.chatfriend.size())
  printf("           \033[0;34m[new] 有%ld条未处理的好友申请\033[0m\n", rpf.rpt.chatfriend.size());
    if(rpf.rpt.chatgroup.size())
  printf("           \033[0;34m[new] 有%ld条未处理的好友申请\033[0m\n", rpf.rpt.chatgroup.size());
    if(rpf.rpt.groupapply.size())
  printf("           \033[0;34m[new] 有%ld条未处理的好友申请\033[0m\n", rpf.rpt.groupapply.size());
    if(rpf.rpt.notice.size())
  printf("           \033[0;34m[new] 有%ld条未处理的好友申请\033[0m\n", rpf.rpt.notice.size());
  }
  printf("\033[0;32m[1] 好友功能\033[0m\n");
  printf("\033[0;32m[2] 群聊功能\033[0m\n");
  printf("\033[0;32m[3] 聊天记录\033[0m\n");
  printf("\033[0;32m[4] 查看通知\033[0m\n");
  printf("\033[0;32m[5] 设置与退出\033[0m\n");
  printf("\033[0;36m==========================================================\033[0m\n");

  printf("\033[0;32m请输入要选择功能的序号:>\033[0m");
  fflush(stdout); // 手动刷新标准输出缓冲区
}

void friendmenu(){
  printf("\033[0;36m==========================================================\033[0m\n");
  printf("                \033[0;33m好友功能\033[0m\n");
  printf("\033[0;32m[1] 添加好友\033[0m\n");
  printf("\033[0;32m[2] 删除好友\033[0m\n");
  printf("\033[0;32m[3] 查看好友列表\033[0m\n");
  printf("\033[0;32m[4] 私聊好友\033[0m\n");
  printf("\033[0;32m[5] 屏蔽/解除屏蔽好友\033[0m\n");
  printf("                    \033[0;33m(tip:ESC可返回上一级菜单)\033[0m\n");

  printf("\033[0;36m==========================================================\033[0m\n");

  printf("\033[0;32m请输入要选择功能的序号:>\033[0m");
  fflush(stdout); // 手动刷新标准输出缓冲区
}

void groupmenu(){
  printf("\033[0;36m==========================================================\033[0m\n");
  printf("                \033[0;33m群聊功能\033[0m\n");
  printf("\033[0;32m[1] 创建群组\033[0m\n");
  printf("\033[0;32m[2] 加入群组\033[0m\n");
  printf("\033[0;32m[3] 查看已加入群组\033[0m\n");
  printf("\033[0;32m[4] 开始群聊\033[0m\n");
  printf("\033[0;32m[5] 退出群组\033[0m\n");
  printf("\033[0;32m[6] 查看群成员\033[0m\n");
  printf("\033[0;32m[7] 管理员操作\033[0m\n");
  printf("                    \033[0;33m(tip:ESC可返回上一级菜单)\033[0m\n");

  printf("\033[0;36m==========================================================\033[0m\n");

  printf("\033[0;32m请输入要选择功能的序号:>\033[0m");
  fflush(stdout); // 手动刷新标准输出缓冲区
}

void recordmenu(){
  printf("\033[0;36m==========================================================\033[0m\n");
  printf("                \033[0;33m聊天记录\033[0m\n");
  printf("\033[0;32m[1] 查看与某好友的聊天记录\033[0m\n");
  printf("\033[0;32m[2] 查看某群组的聊天记录\033[0m\n");
  printf("                    \033[0;33m(tip:ESC可返回上一级菜单)\033[0m\n");
  printf("\033[0;36m==========================================================\033[0m\n");

  printf("\033[0;32m请输入要选择功能的序号:>\033[0m");
}

void reportmenu(user u, void *p){
  reportfucs rpf(u, p);
  printf("\033[0;32m数据请求中...\033[0m\n");
  bool ret = rpf.Getrpt();
  fflush(stdout); // 手动刷新标准输出缓冲区
  system("clear");
  printf("\033[0;36m==========================================================\033[0m\n");
  printf("                \033[0;33m查看通知\033[0m\n");

  printf("\033[0;32m[1] 查看好友申请      \033[0m");
  if(ret && rpf.rpt.friendapply.size()) printf("\033[0;31m(%ld)\033[0m\n", rpf.rpt.friendapply.size());
  else printf("\n");

  printf("\033[0;32m[2] 查看未读好友消息   \033[0m");
  if(ret && rpf.rpt.chatfriend.size()) printf("\033[0;31m(%ld)\033[0m\n", rpf.rpt.chatfriend.size());
  else printf("\n");

  printf("\033[0;32m[3] 查看未读群聊消息   \033[0m");
  if(ret && rpf.rpt.chatgroup.size()) printf("\033[0;31m(%ld)\033[0m\n", rpf.rpt.chatgroup.size());
  else printf("\n");

  printf("\033[0;32m[4] 查看群组邀请      \033[0m");
  if(ret && rpf.rpt.groupapply.size()) printf("\033[0;31m(%ld)\033[0m\n", rpf.rpt.groupapply.size());
  else printf("\n");

  printf("\033[0;32m[5] 查看事务通知      \033[0m");
  if(ret && rpf.rpt.notice.size()) printf("\033[0;31m(%ld)\033[0m\n", rpf.rpt.notice.size());
  else printf("\n");

  printf("                    \033[0;33m(tip:ESC可返回上一级菜单)\033[0m\n");
  printf("\033[0;36m==========================================================\033[0m\n");

  printf("\033[0;32m请输入要选择功能的序号:>\033[0m");
}

void  setupmenu(){
  
  printf("\033[0;36m==========================================================\033[0m\n");
  printf("                \033[0;33m设置与退出\033[0m\n");
  printf("\033[0;32m[1] 修改密码\033[0m\n");
  printf("\033[0;32m[2] 注销账号\033[0m\n");
  printf("\033[0;32m[3] 退出登录\033[0m\n");
  printf("                    \033[0;33m(tip:ESC可返回上一级菜单)\033[0m\n");
  printf("\033[0;36m==========================================================\033[0m\n");

  printf("\033[0;32m请输入要选择功能的序号:>\033[0m");
}



