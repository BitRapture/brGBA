#include "../include/cpu_test.h"

int main()
{
    br::gba::cpu_test test;

    test.load_directives_file("./directives.txt");
    test.run();

    return 0;
}