#include <stdio.h>
#include <assert.h>

#include "opcode.h"

// Stack functions

void stack_init(OpStack* stack)
{
    stack->data[0] = MAGIC_WORD;
    stack->ptr = MAX_STACK_SIZE;
}

OpResult stack_push(OpStack *stack, uint32_t value)
{
#if CHECK_MAGIC_WORD
    if (MAGIC_WORD != stack->data[0])
        return RES_STACK_NOT_INITIALIZED;
#endif

    if (stack->ptr == 1) // 0th value is reserved for marker
        return RES_STACK_OVERFLOW;

    --stack->ptr;
    stack->data[stack->ptr] = value;
    return RES_SUCCESS;
}

OpResult stack_pop(OpStack *stack, uint32_t *out_value)
{
#if CHECK_MAGIC_WORD
    if (MAGIC_WORD != stack->data[0])
        return RES_STACK_NOT_INITIALIZED;
#endif

    if (stack->ptr == MAX_STACK_SIZE)
        return RES_STACK_UNDERFLOW;

    if (out_value != NULL)
        *out_value = stack->data[stack->ptr];
    stack->ptr++;
    return RES_SUCCESS;
}

int stack_empty(OpStack *stack)
{
#if CHECK_MAGIC_WORD
    if (MAGIC_WORD != stack->data[0])
        return TRUE;
#endif

    return stack->ptr == MAX_STACK_SIZE;
}

size_t stack_count(OpStack *stack)
{
    return MAX_STACK_SIZE - stack->ptr;
}

// Bytecode functions

OpResult op_init(OpBytecode *bytecode, uint8_t *bytes, size_t size_in_bytes)
{
    bytecode->bytes = (uint16_t*)bytes;
    bytecode->pc = size_in_bytes / sizeof(opcode_t);

#if CHECK_MAGIC_WORD
    if (MAGIC_WORD != MAKE_OPERAND(bytecode->bytes[1], bytecode->bytes[0]))
        return RES_BYTECODE_INVALID;
#endif

    return RES_SUCCESS;
}

OpResult op_read_word(OpBytecode *bytecode, uint16_t *out_word)
{
#if CHECK_MAGIC_WORD
    if (MAGIC_WORD != MAKE_OPERAND(bytecode->bytes[1], bytecode->bytes[0]))
        return RES_BYTECODE_INVALID;
#endif

    if (bytecode->pc == 2)
        return RES_BYTECODE_EOF;

    --bytecode->pc;
    if (out_word != NULL)
        *out_word = bytecode->bytes[bytecode->pc];
    return RES_SUCCESS;
}

OpResult op_read_uint(OpBytecode *bytecode, uint32_t *out_uint)
{
#if CHECK_MAGIC_WORD
    if (MAGIC_WORD != MAKE_OPERAND(bytecode->bytes[1], bytecode->bytes[0]))
        return RES_BYTECODE_INVALID;
#endif

    if (bytecode->pc < 4)
    {
        if (bytecode->pc == 3)
            return RES_BYTECODE_INSUFFICIENT;
        else
            return RES_BYTECODE_EOF;
    }
    bytecode->pc -= 2;
    if (out_uint != NULL)
        *out_uint = *(uint32_t*)&bytecode->bytes[bytecode->pc];
    return RES_SUCCESS;
}

// Opcode functions

OpResult execute_one(OpBytecode *bytecode, OpStack *stack)
{
    OpResult res;
    opcode_t opcode; // opcode
    uint32_t op[3];  // operands (OPTIONAL)
    int opcat, nargs; // opcode category, # of operands

    res = op_read_word(bytecode, &opcode);
    if (res != RES_SUCCESS)
    {
        if (res != RES_BYTECODE_EOF)
            LOG_ERROR_MSG(res, "while reading opcode");
        return res;
    }

    opcat = OPCODE_CAT(opcode);
    nargs = OPCODE_NARGS(opcode);

    if (opcat == CAT_IMM)
    {
        for (int i = 0; i < nargs; i++)
        {
            res = op_read_uint(bytecode, &op[i]);
            printf("read op[%d] : %x\n", i, op[i]);

            if (res != RES_SUCCESS) 
            {
                LOG_ERROR_MSG(res, "while reading op[%d]", i,);
            }
        }
    }

    res = RES_SUCCESS;

    switch (opcode)
    {
    case OP_NOP:
        break;
    case OP_PUSH_NUMBER_I:
        res = stack_push(stack, op[0]);
        break;
    case OP_POP_NUMBER:
        res = stack_pop(stack, &op[0]);
        break;

    case OP_MOVE_HZ_I:
        res = stack_push(stack, op[0]);
        if (res != RES_SUCCESS) break;
    case OP_MOVE_HZ:
        res = impl_move_horizontal(stack);
        break;

    case OP_MOVE_VT_I:
        res = stack_push(stack, op[0]);
        if (res != RES_SUCCESS) break;
    case OP_MOVE_VT:
        res = impl_move_vertical(stack);
        break;
    }

    return res;
}

OpResult impl_move_horizontal(OpStack *stack)
{
    OpResult res;
    uint32_t uval;
    float fval;
    
    res = stack_pop(stack, &uval);
    if (res != RES_SUCCESS)
        return res;

    fval = *(float*)&uval;

    if (fval > 0)
        printf("Moving right by %f units\n", fval);
    else
        printf("Moving left by %f units\n", -fval);

    return RES_SUCCESS;
}

OpResult impl_move_vertical(OpStack *stack)
{
    OpResult res;
    uint32_t uval;
    float fval;
    
    res = stack_pop(stack, &uval);
    if (res != RES_SUCCESS)
        return res;

    fval = *(float*)&uval;

    if (fval > 0)
        printf("Moving up by %f units\n", fval);
    else
        printf("Moving down by %f units\n", -fval);

    return RES_SUCCESS;
}
