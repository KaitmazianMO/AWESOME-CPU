
#include "CPU.h"
#include <GL/glut.h>


// g++ -fpermissive cpu_main.cpp CPU.h CPU.cpp ../Assembler/assembler.h ../Assembler/comands.h  ../Assembler/label.h ../Assembler/label.cpp ../CommonFiles/text.h ../CommonFiles/text.cpp ../CommonFiles/errors.h ../Assembler/byte_code.h ../Assembler/byte_code.cpp ../CommonFiles/listing.h ../CommonFiles/listing.cpp stack.h CPU_commands_processing.cpp -o TheCPU -lglut -lGLU -lGL
main(int argc, char *argv[]) 
{
    printf ("CPU start..\n");
    CPU cpu = {};
    CPUctor (&cpu, argv[1]);

    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_SINGLE);
    glutInitWindowSize (VIDEO_RAM_AXIS_X_SIZE, VIDEO_RAM_AXIS_Y_SIZE);
    glutInitWindowPosition ((1920 - VIDEO_RAM_AXIS_X_SIZE) / 2, (1080 - VIDEO_RAM_AXIS_Y_SIZE) / 2);

    glutCreateWindow ("Assembler window");
    glClearColor (1.0, 1.0, 1.0, 0);    
    glClear (GL_COLOR_BUFFER_BIT);    

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D (0.0, 2*VIDEO_RAM_AXIS_X_SIZE, 2*VIDEO_RAM_AXIS_Y_SIZE, 0.0);    

    int err = CPURun (&cpu);

    CPUdtor (&cpu);

    getchar();
    printf ("CPU finish(%d)\n", err);
    return err;
}
 