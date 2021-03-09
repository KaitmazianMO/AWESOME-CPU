
#include "CPU.h"


//  g++ -fpermissive cpu_main.cpp CPU.h CPU.cpp assembler.h comands.h buffer.h label.h label.cpp text.h text.cpp errors.h byte_code.h byte_code.cpp listing.h listing.cpp stack.h -o TheCPU
int main(int argc, char *argv[]) {
    printf ("CPU start..\n");
    CPU cpu = {};
    CPUctor (&cpu, argv[1]);

    
    int err = CPURun (&cpu);

    CPUdtor (&cpu);
    printf ("CPU finish(%d)\n", err);
    return err;
}
 