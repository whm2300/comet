#include <stdio.h>

//#pragma pack(1)
struct stu
{
    char sex;
    int length;
    char name[10];
};

int main(int argc, char **argv)
{
    int b;
    int i;
    struct stu a;
    printf("%ld, %ld, %ld, %ld, %ld, %d\n", &a.sex, &a.length, a.name, &i, &b, sizeof(a));
    return 0;
}
