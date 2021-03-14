
#include "CPU.h"


//g++ -fpermissive cpu_main.cpp CPU.h CPU.cpp ../Assembler/assembler.h ../Assembler/comands.h  ../Assembler/label.h ../Assembler/label.cpp ../CommonFiles/text.h ../CommonFiles/text.cpp ../CommonFiles/errors.h ../Assembler/byte_code.h ../Assembler/byte_code.cpp ../CommonFiles/listing.h ../CommonFiles/listing.cpp stack.h -o CPUTestint 
main(int argc, char *argv[]) {
    printf ("CPU start..\n");
    CPU cpu = {};
    CPUctor (&cpu, argv[1]);

    
    int err = CPURun (&cpu);

    CPUdtor (&cpu);
    printf ("CPU finish(%d)\n", err);
    return err;
}
 