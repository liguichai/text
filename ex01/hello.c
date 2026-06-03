#include<stdio.h> 

char* input(const char* prompt, char* buffer, size_t max_len) {
    if (prompt != NULL) {
        printf("%s", prompt);   // 输出提示信息
    }
    // 使用 fgets 读取一行，包括换行符
    if (fgets(buffer, (int)max_len, stdin) != NULL) {
        // 去除末尾的换行符
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        return buffer;
    }
    return NULL;
}

int main() {
    printf("Hello, World!\n");
    
     char str[100];   // 存储输入内容的缓冲区

    // 调用 input 函数读取键盘输入，提示信息为 "请输入内容："
    if (input("请输入内容：", str, sizeof(str)) != NULL) {
        // 显示输出键盘输入的内容
        printf("您输入的内容是：%s\n", str);
    } else {
        printf("读取输入失败。\n");
    }
    
    return 0;
}
