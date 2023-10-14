#include <stdio.h>

#include "opcode.h"

#define PRINT_TRUTH_VAL(condition) printf((condition) ? "T\n" : "F\n")

int main(int argc, char **argv)
{
    OpStack stack;
    OpResult res;
    int pc = 0; // program counter
    OpBytecode bytecode;

    uint16_t bytecode_bytes[] = {
        LOWORD(MAGIC_WORD), HIWORD(MAGIC_WORD),
        0, 0, 0
        //0x740a, 0x4300, OP_MOVE_VT_I,
        //OP_MOVE_HZ,
        //0x770a, 0x4355, OP_PUSH_NUMBER_I
    };

    // op_init(&bytecode, (uint8_t*)bytecode_bytes, sizeof(bytecode_bytes));

    bytecode.bytes = bytecode_bytes;
    bytecode.pc = 5;

    stack_init(&stack);

    opcode_t opcode;
    uint32_t uarg;
    float farg;
    int ret;

    while (ret != EOF)
    {
        printf(">> ");
        ret = scanf("%hu %f", &opcode, &farg);

        uarg = *(uint32_t*)&farg;

        bytecode.bytes[4] = opcode;
        bytecode.bytes[3] = HIWORD(uarg);
        bytecode.bytes[2] = LOWORD(uarg);
        
        bytecode.pc = 5;

        res = execute_one(&bytecode, &stack);

        printf("\n");
    }


    //res = op_init(&bytecode, (uint8_t*)bytecode_bytes, sizeof(bytecode_bytes));

    
    // while (res == RES_SUCCESS)
    // {
    //     res = execute_one(&bytecode, &stack);
    //     if (res != RES_SUCCESS && res != RES_BYTECODE_EOF)
    //     {
    //         fprintf(stderr, "execute_one failed! : %x\n", res);
    //     }
    // }

    // if (res != RES_BYTECODE_EOF)
    // {
    //     fprintf(stderr, "\nScript exited with Error(s)!");
    // }

    return 0;
}
