#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H


void disAssembly (const char *assembled_file, const char *disassembled_file)
{
    ByteCode bcode = {};
    byteCodeCtor (&bcode, 1);
    readByteCodeFromFile (&bcode, assembled_file);

    FILE *code = fopen (disassembled_file, "w");

    while (bcode.pos < bcode.size) 
    {
        cmd_t IP = getCommand (&bcode);

        char  regn = 0;
        arg_t arg  = 0;

#define breakn fprintf (code, "\n"); break;
        switch (IP & FLAG_OFF) 
        {                                 
            case CMD_PUSH:
                fprintf (code, "push ");
                
                if (IP & REGISTER_FLAG) 
                {
                    regn = getRegisterNum (&bcode);
                    fprintf (code, "r%cx", regn + 'a');
                }
                else 
                { 
                    arg = getArgument (&bcode);
                    fprintf (code, "%lg", arg);
                }
                breakn;            
            case CMD_POP:
                fprintf (code, "pop ");                      
                regn = getRegisterNum (&bcode);
                fprintf (code, "r%cx", regn + 'a');
                breakn;

            case CMD_ADD:
                fprintf (code, "add");
                breakn;

            case CMD_SUB:                       
                fprintf (code, "sub");
                breakn;

            case CMD_MULT:                                
                fprintf (code, "mult");
                breakn;

            case CMD_DIV:                                   
                fprintf (code, "div");
                breakn;

            case CMD_NEG:                                       
                fprintf (code, "neg");
                breakn;
                
            case CMD_SQRT:                                                          
                fprintf (code, "sqrt");
                breakn;

            case CMD_SIN:                                             
                fprintf (code, "sin");
                breakn;

            case CMD_COS:                                           
                fprintf (code, "cos");
                breakn;

            case CMD_OUT:                                                                           
                fprintf (code, "out");
                breakn;

            case CMD_IN:                                      
                fprintf (code, "in");
                breakn;

            case CMD_DUMP:
                fprintf (code, "dump");
                breakn;

            case CMD_HET:                               
                fprintf (code, "het");
                breakn;

            case CMD_END:                             
                fprintf (code, "end");
                breakn;
        }
    }

    fclose (code);
    byteCodeDtor (&bcode);
}

#endif