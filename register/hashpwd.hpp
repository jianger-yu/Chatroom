#pragma once
#include <sodium.h>
#include <cstring>
#include <iostream>

class PasswordHasher {
public:
    PasswordHasher() {
        if (sodium_init() < 0) {
            throw std::runtime_error("libsodium 初始化失败");
        }
    }

    // 生成密码哈希，覆盖原密码内容（原地修改）
    // 输入：char* passwd，必须保证缓冲区至少 crypto_pwhash_STRBYTES 字节大小
    void hashPassword(char* passwd) {
        // 先保存原密码
        std::string orig(passwd);

        // 生成哈希，覆盖原内存
        if (crypto_pwhash_str(
                passwd,
                orig.c_str(),
                orig.size(),
                crypto_pwhash_OPSLIMIT_INTERACTIVE,
                crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
            throw std::runtime_error("生成密码哈希失败");
        }
    }

    // 验证密码是否和哈希匹配
    // passwd: 明文密码
    // hashed: 哈希字符串
    // 匹配返回1(true)，不匹配返回0(false)
    bool verifyPassword(const char* passwd, const char* hashed) const {
        return crypto_pwhash_str_verify(hashed, passwd, std::strlen(passwd)) == 0;
    }
};
