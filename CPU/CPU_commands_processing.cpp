
#include "CPU.h"

void CPU :: jumpCommandsProcessing (cmd_t jmp)
{
    arg_t rarg = stack_pop (&stack);
    arg_t larg = stack_pop (&stack);
    size_t pos  = getLabelPointer (&bcode);

    if (pos > bcode.size)
        throw std :: out_of_range ("Segmentation fault! Oh yeah!");

    bool condition = false;
    if (jmp == CMD_JB)  condition = larg < rarg;
    if (jmp == CMD_JBE) condition = larg <= rarg;
    if (jmp == CMD_JA)  condition = larg > rarg;
    if (jmp == CMD_JAE) condition = larg >= rarg;
    if (jmp == CMD_JE)  condition = larg == rarg;
    if (jmp == CMD_JNE) condition = larg != rarg;
    if (jmp == CMD_JMP) condition = true;

    bcode.pos = (condition) ? pos : bcode.pos + sizeof (size_t);
}

void CPU :: popCommandProcessing (cmd_t pop)
{
    if ((pop & FORMAT_BYTES) == REGISTER_ARGUMENT_TYPE)
    {
        cmd_t regn = getRegisterNum (&bcode);
        if (regn < 0)
            throw std :: runtime_error ("Wrong register number");

        registers [regn] = stack_pop (&stack);
    }

    else if ((pop & FORMAT_BYTES) == RAM_ACCESS_ARGUMENT_TYPE)
    {
        cmd_t regn = getRegisterNum (&bcode);
        if (regn < 0)
            throw std :: runtime_error ("Wrong register number");

        int index = (int)registers [regn];
        if (index < 0 || index > RAM_SIZE)
            throw std :: out_of_range ("Segmentation fault! Oh yeah!");

        *(arg_t *)(RAM + index) = stack_pop (&stack);
    }

    else if ((pop & FORMAT_BYTES) == VIDEO_RAM_ACCESS_ARGUMENT_TYPE)
    {
        cmd_t regn1 = getRegisterNum (&bcode);
        cmd_t regn2 = getRegisterNum (&bcode);
        if (regn1 < 0 || regn2 < 0)
            throw std :: runtime_error ("Wrong register number"); 

        int x = (int) registers [regn1],
            y = (int) registers [regn2];

        if (abs (x) > VIDEO_RAM_AXIS_X_SIZE || abs (y) > VIDEO_RAM_AXIS_Y_SIZE)
            throw std :: out_of_range ("Segmentation fault! Oh yeah!");

        // Want to access negative indices
        VideoRAM [y + VIDEO_RAM_AXIS_Y_SIZE] [x + VIDEO_RAM_AXIS_X_SIZE] = false;
    }

    else
    {
        throw std :: invalid_argument ("Wrong pop argument format");
    }

    if (stk_err)
        throw std :: runtime_error ("Stack error: " + std :: string (str_error (stk_err)));
}

void CPU :: pushCommandProcessing (cmd_t push)
{
    if ((push & FORMAT_BYTES) == REGISTER_ARGUMENT_TYPE)
    {
        cmd_t regn = getRegisterNum (&bcode);
        if (regn < 0)
            throw std :: runtime_error ("Wrong register number");

        stack_push (&stack, registers [regn]);
    }

    else if ((push & FORMAT_BYTES) == RAM_ACCESS_ARGUMENT_TYPE)
    {
        cmd_t regn = getRegisterNum (&bcode);
        if (regn < 0)
            throw std :: runtime_error ("Wrong register number");

        int index = (int)registers [regn];
        if (index < 0 || index > RAM_SIZE)
            throw std :: out_of_range ("Segmentation fault! Oh yeah!");

        stack_push (&stack, *(arg_t *)(RAM + index));
    }

    else if ((push & FORMAT_BYTES) == VIDEO_RAM_ACCESS_ARGUMENT_TYPE)
    {
        cmd_t regn1 = getRegisterNum (&bcode);
        cmd_t regn2 = getRegisterNum (&bcode);
        if (regn1 < 0 || regn2 < 0)
            throw std :: runtime_error ("Wrong register number"); 

        int x = (int) registers [regn1],
            y = (int) registers [regn2];

        if (abs (x) > VIDEO_RAM_AXIS_X_SIZE || abs (y) > VIDEO_RAM_AXIS_Y_SIZE)
            throw std :: out_of_range ("Segmentation fault! Oh yeah!");

        VideoRAM [y + VIDEO_RAM_AXIS_Y_SIZE] [x + VIDEO_RAM_AXIS_X_SIZE] = true;
    }

    else
    {
        arg_t arg = getNumberArgument (&bcode);
        stack_push (&stack, arg);
    }

    if (stk_err)
        throw std :: runtime_error ("Stack error: " + std :: string (str_error (stk_err)));
}

void CPU :: subCommandProcessing()
{
    arg_t rarg = stack_pop (&stack);
    arg_t larg = stack_pop (&stack);
    stack_push (&stack, larg - rarg);
}

void CPU:: divCommandProcessing()
{
    arg_t rarg = stack_pop (&stack);
    arg_t larg = stack_pop (&stack);
    
    if (fabs (rarg) < DBL_EPSILON) 
        throw std :: invalid_argument ("Division by zero!");
    
    stack_push (&stack, larg / rarg);    
}

void CPU :: sqrtCommandProcessing()
{
    arg_t arg = stack_pop (&stack);
    if (arg < 0) 
        throw std :: invalid_argument ("Root of a negative number!");

    stack_push (&stack, sqrt (arg));
}

void CPUDump (const CPU *cpu)
{
    for (size_t i = 0; i < NREGISTERS; ++i)
        printf ("r%cx:    %lg\n", 'a' + i, cpu->registers[i]);

    for (int i = 0; i < cpu->stack.capacity; ++i)
        printf ("stk[%d]: %lg\n", i, cpu->stack.data[i]);
    
    for (int i = 0; i < cpu->call_stack.capacity; ++i)
        printf ("call_stk[%d]: %zu\n", i, cpu->call_stack.data[i]);
}

void CPU :: retCommandProcessing()
{
    size_t ret_pos = stack_pop (&call_stack);

    if (ret_pos > bcode.size)
        throw std :: out_of_range ("Segmentation fault! Oh yeah!");

    bcode.pos = ret_pos;
}

void CPU :: callCommandProcessing()
{
    stack_push (&call_stack, (bcode.pos + sizeof (size_t)));
    size_t pos = getLabelPointer (&bcode);

    if (pos > bcode.size)
        throw std :: out_of_range ("Segmentation fault! Oh yeah!");   

    bcode.pos = pos;  
}

void CPU :: inCommandProcessing()
{
    arg_t arg = 0;
    if (scanf ("%lg", &arg) != 1)
        throw std :: invalid_argument ("Can't read the argument!");

    stack_push (&stack, arg);
}

void CPU :: drawCommandProcessing()
{
    glPointSize (3);
    
    glBegin (GL_POINTS);
        glColor3f (1, 0, 0);
        for (int y = 0; y < 2*VIDEO_RAM_AXIS_Y_SIZE; ++y)
            for (int x = 0; x < 2*VIDEO_RAM_AXIS_Y_SIZE; ++x)
                if (VideoRAM [y] [x])                  
                    glVertex2i (x, y);   

    glEnd();      

    glFlush(); 
}

void CPU :: addCommandProcessing()
{
    stack_push (&stack, stack_pop (&stack) + stack_pop (&stack));
}

void CPU :: sinCommandProcessing()
{
    stack_push (&stack, sin (stack_pop (&stack)));
}

void CPU :: cosCommandProcessing()
{
    stack_push (&stack, cos (stack_pop (&stack)));
}

void CPU :: multCommandProcessing()
{
    stack_push (&stack, stack_pop (&stack) * stack_pop (&stack));
}

void CPU :: negCommandProcessing()
{
    stack_push (&stack, -(stack_pop (&stack)));
}

void CPU :: outCommandProcessing()
{
    printf ("%lg\n", stack_peek (&stack));
}