/*#include <sodium.h>
#include <iostream>
#include <string>

int main() {
    if (sodium_init() < 0) {
        std::cerr << "libsodium 初始化失败" << std::endl;
        return 1;
    }

    std::string password = "my_password";
    char hashed[crypto_pwhash_STRBYTES];

    // 生成哈希
    if (crypto_pwhash_str(
            hashed, 
            password.c_str(), 
            password.size(),
            crypto_pwhash_OPSLIMIT_INTERACTIVE,
            crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        std::cerr << "哈希生成失败" << std::endl;
        return 1;
    }

    std::cout << "密码哈希: " << hashed << std::endl;

    // 验证密码
    std::string input;
    std::cout << "请输入密码进行验证: ";
    std::cin >> input;

    if (crypto_pwhash_str_verify(hashed, input.c_str(), input.size()) == 0) {
        std::cout << "密码正确 ✅" << std::endl;
    } else {
        std::cout << "密码错误 ❌" << std::endl;
    }

    return 0;
}
*/

// #include <stdio.h>
// #include <termios.h>
// #include <unistd.h>

// char charget(void){
//     struct termios oldt, newt;
//     char ch;
//     tcgetattr(STDIN_FILENO, &oldt);
//     newt = oldt;
//     newt.c_lflag &= ~(ICANON | ECHO);
//     tcsetattr(STDIN_FILENO, TCSANOW, &newt);
//     ch = getchar();
//     tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
//     return ch;
// }

// int enter(char *arr, int key){
//     char ch;
//     int i = 0;

//     while (i < 20){
//         ch = charget();
//         if (ch == 27) return -1; // ESC键退出

//         if (ch == '\n'){ // Linux 回车
//             arr[i] = '\0';
//             break;
//         } else if (ch == '\b' || ch == 127){ // 127 是 Delete 键
//             if (i > 0){
//                 i--;
//                 printf("\b \b"); // 删除一个字符
//             }
//             continue;
//         } else {
//             arr[i++] = ch;
//             if (key)
//                 putchar('*');
//             else
//                 putchar(ch);
//         }
//     }
//     printf("\n");
//     arr[i] = '\0'; // 最后确保字符串结尾
//     return i;
// }

// int main() {
//     char input[21];
//     printf("请输入密码（最多20个字符，按回车结束）:\n");
//     int len = enter(input, 1);
//     printf("你输入的是：%s\n", input);
//     return 0;
// }


#include <iostream>
#include "userdata.hpp"

int main() {
    // 创建一个 UserData 实例并赋值
    UserData user;
    user.name = "Alice";
    user.email = "alice@example.com";
    user.pwd = "hashed_password";
    user.stat = "online";
    user.frd = {"uid1001", "uid1002"};
    user.group = {"gid2001", "gid2002"};

    // 转为 JSON 字符串
    std::string jsonStr = user.toJson();
    std::cout << "Serialized JSON:\n" << jsonStr << "\n\n";

    // 从 JSON 字符串恢复成对象
    UserData restored = UserData::fromJson(jsonStr);

    // 打印恢复出的内容
    std::cout << "Restored UserData:\n";
    std::cout << "Name: " << restored.name << "\n";
    std::cout << "Email: " << restored.email << "\n";
    std::cout << "Password: " << restored.pwd << "\n";
    std::cout << "Status: " << restored.stat << "\n";

    std::cout << "Friends:\n";
    for (const auto& f : restored.frd)
        std::cout << "  - " << f << "\n";

    std::cout << "Groups:\n";
    for (const auto& g : restored.group)
        std::cout << "  - " << g << "\n";

    return 0;
}
