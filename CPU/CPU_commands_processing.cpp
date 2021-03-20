
#include "CPU.h"

void CPU :: jumpCommandsProcessing (cmd_t jmp)
{
    arg_t rarg = stack_pop (&stack);
    arg_t larg = stack_pop (&stack);
    size_t pos  = getLabelPointer (&bcode);

    if (pos > bcode.size)
        throw out_of_range ("Segmentation fault! Oh yeah!");

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
    if (pop & REGISTER_ARGUMENT_TYPE)
    {
        cmd_t regn = getRegisterNum (&bcode);
        if (regn < 0)
            throw runtime_error ("Wrong register number");

        registers [regn] = stack_pop (&stack);
    }

    else if (pop & RAM_ACCESS_ARGUMENT_TYPE)
    {
        cmd_t regn = getRegisterNum (&bcode);
        if (regn < 0)
            throw runtime_error ("Wrong register number");

        int index = (int)registers [regn];
        if (index < 0 || index > RAM_SIZE)
            throw out_of_range ("Segmentation fault! Oh yeah!");

        *(arg_t *)(RAM + index) = stack_pop (&stack);
    }

    else if (pop & VIDEO_RAM_ACCESS_ARGUMENT_TYPE)
    {
        cmd_t regn1 = getRegisterNum (&bcode);
        cmd_t regn2 = getRegisterNum (&bcode);
        if (regn1 < 0 || regn2 < 0)
            throw runtime_error ("Wrong register number"); 

        int x = (int) registers [regn1],
            y = (int) registers [regn2];

        if (abs (x) > VIDEO_RAM_AXIS_X_SIZE || abs (y) > VIDEO_RAM_AXIS_Y_SIZE)
            throw out_of_range ("Segmentation fault! Oh yeah!");

        // Want to access negative indices
        VideoRAM [y + VIDEO_RAM_AXIS_Y_SIZE] [x + VIDEO_RAM_AXIS_X_SIZE] = false;
    }

    else
    {
        throw invalid_argument ("Wrong pop argument format");
    }

    if (stk_err)
        throw runtime_error ("Stack error: " + string (str_error (stk_err)));
}

void CPU :: pushCommandProcessing (cmd_t push)
{
    if (push & REGISTER_ARGUMENT_TYPE)
    {
        cmd_t regn = getRegisterNum (&bcode);
        if (regn < 0)
            throw runtime_error ("Wrong register number");

        stack_push (&stack, registers [regn]);
    }

    else if (push & RAM_ACCESS_ARGUMENT_TYPE)
    {
        cmd_t regn = getRegisterNum (&bcode);
        if (regn < 0)
            throw runtime_error ("Wrong register number");

        int index = (int)registers [regn];
        if (index < 0 || index > RAM_SIZE)
            throw out_of_range ("Segmentation fault! Oh yeah!");

        stack_push (&stack, *(arg_t *)(RAM + index));
    }

    else if (push & VIDEO_RAM_ACCESS_ARGUMENT_TYPE)
    {
        cmd_t regn1 = getRegisterNum (&bcode);
        cmd_t regn2 = getRegisterNum (&bcode);
        if (regn1 < 0 || regn2 < 0)
            throw runtime_error ("Wrong register number"); 

        int x = (int) registers [regn1],
            y = (int) registers [regn2];

        if (abs (x) > VIDEO_RAM_AXIS_X_SIZE || abs (y) > VIDEO_RAM_AXIS_Y_SIZE)
            throw out_of_range ("Segmentation fault! Oh yeah!");

        VideoRAM [y + VIDEO_RAM_AXIS_Y_SIZE] [x + VIDEO_RAM_AXIS_X_SIZE] = true;
    }

    else
    {
        arg_t arg = getNumberArgument (&bcode);
        stack_push (&stack, arg);
    }

    if (stk_err)
        throw runtime_error ("Stack error: " + string (str_error (stk_err)));
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
        throw invalid_argument ("Division by zero!");
    
    stack_push (&stack, larg / rarg);    
}

void CPU :: sqrtCommandProcessing()
{
    arg_t arg = stack_pop (&stack);
    if (arg < 0) 
        throw invalid_argument ("Root of a negative number!");

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
        throw out_of_range ("Segmentation fault! Oh yeah!");

    bcode.pos = ret_pos;
}

void CPU :: callCommandProcessing()
{
    stack_push (&call_stack, (bcode.pos + sizeof (size_t)));
    size_t pos = getLabelPointer (&bcode);

    if (pos > bcode.size)
        throw out_of_range ("Segmentation fault! Oh yeah!");   

    bcode.pos = pos;  
}

void CPU :: inCommandProcessing()
{
    arg_t arg = 0;
    if (scanf ("%lg", &arg) != 1)
        throw invalid_argument ("Can't read the argument!");

    stack_push (&stack, arg);
}