#include "gba_core.h"

int main()
{
    br::gba::bus gbaBus;
    br::gba::cpu gbaCPU(gbaBus);

    return 0;
}