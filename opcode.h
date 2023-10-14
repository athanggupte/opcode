#ifndef PRAGMA_ONCE_OPCODE_H
#define PRAGMA_ONCE_OPCODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>

#define FALSE 0
#define TRUE !(FALSE)

#define LOWORD(uint) ((uint) & 0xffff)
#define HIWORD(uint) (((uint) >> 16) & 0xffff)

#define MAX_STACK_SIZE 1048576 // 1024 * 1024

#ifdef NDEBUG
#   define CHECK_MAGIC_WORD 0
#else
#   define CHECK_MAGIC_WORD 1
#endif

#if CHECK_MAGIC_WORD
#   define MAGIC_WORD 0xb01ec0de
#endif

typedef enum OpResult
{
    RES_SUCCESS                 = 0x0,
    
    // Bytecode errors
    RES_BYTECODE_INVALID        = 0xc0000000,
    RES_BYTECODE_EOF            = 0xc0000001,
    RES_BYTECODE_INSUFFICIENT   = 0xc0000002,

    // Stack errors
    RES_STACK_NOT_INITIALIZED   = 0xe0000000,
    RES_STACK_OVERFLOW          = 0xe0000001,
    RES_STACK_UNDERFLOW         = 0xe0000002,

    // Opcode errors
    RES_INVALID_OPCODE          = 0xf0000001,
    RES_INVALID_OPERAND         = 0xf0000002,
} OpResult;

#define BYTECODE_ERROR_SIGN 0xc0000000
#define STACK_ERROR_SIGN    0xe0000000
#define OPCODE_ERROR_SIGN   0xf0000000

#define LOG_ERROR(res) fprintf(stderr, "<%s:%d> ERROR : 0x%x\n", __func__, __LINE__, (res))
#define LOG_ERROR_MSG(res, msg, ...) fprintf(stderr, "<%s:%d> ERROR " msg " : 0x%x\n", __func__, __LINE__, __VA_ARGS__ (res))

typedef uint16_t opcode_t;

/// @brief Virtual machine stack space for intermediate variables
typedef struct OpStack
{
    uint32_t data[MAX_STACK_SIZE];
    uint32_t ptr;
} OpStack;

void        stack_init  (OpStack *stack);
OpResult    stack_push  (OpStack *stack, uint32_t value);
OpResult    stack_pop   (OpStack *stack, uint32_t *out_value);
int         stack_empty (OpStack *stack);
size_t      stack_count (OpStack *stack);

typedef struct OpBytecode
{
    opcode_t *bytes;
    int pc;
} OpBytecode;

OpResult op_init(OpBytecode *bytecode, uint8_t *bytes, size_t size_in_bytes);
OpResult op_read_word(OpBytecode *bytecode, uint16_t *out_word);
OpResult op_read_uint(OpBytecode *bytecode, uint32_t *out_uint);

#define OPCODE_MASK_CAT 0xc000
#define OPCODE_MASK_NARGS 0x3000
#define OPCODE_MASK_CODE 0x0fff

#define OPCODE_NSHIFT_CAT 14
#define OPCODE_NSHIFT_NARGS 12
#define OPCODE_NSHIFT_CODE 0

#define OPCODE_NBITS_CAT 2
#define OPCODE_NBITS_NARGS 2
#define OPCODE_NBITS_CODE 12

#define OPCODE_CAT(opcode) ((opcode & OPCODE_MASK_CAT) >> OPCODE_NSHIFT_CAT)
#define OPCODE_NARGS(opcode) ((opcode & OPCODE_MASK_NARGS) >> OPCODE_NSHIFT_NARGS)
#define OPCODE_CODE(opcode) ((opcode & OPCODE_MASK_CODE) >> OPCODE_NSHIFT_CODE)

#define MAKE_OPCODE(category, nargs, code) \
                    ((category & (OPCODE_MASK_CAT >> OPCODE_NSHIFT_CAT)) << OPCODE_NSHIFT_CAT) | \
                    ((nargs & (OPCODE_MASK_NARGS >> OPCODE_NSHIFT_NARGS)) << OPCODE_NSHIFT_NARGS) | \
                    ((code & (OPCODE_MASK_CODE >> OPCODE_NSHIFT_CODE)) << OPCODE_NSHIFT_CODE)

#define MAKE_OPERAND(hiword, loword) (((hiword) << 16) | (loword))

enum OpcodeCategories
{
    CAT_INV = 0x0, // invalid
    CAT_IMM = 0x1, // immediate mode addressing - operands are in bytecode
    CAT_STK = 0x2, // stack addressing - operands are in the stack
    CAT_HST = 0x3, // host addressing - interacts with host memory (uses C pointers)
};

enum Opcodes
{
    OP_NOP          = 0x0,
    
    OP_PUSH_NUMBER_I    = MAKE_OPCODE(CAT_IMM, 1, 0x01), // push a number on the stack
    OP_POP_NUMBER       = MAKE_OPCODE(CAT_STK, 0, 0x02), // pop a number from the stack
    
    OP_PUSH_ENTITY_I    = MAKE_OPCODE(CAT_IMM, 1, 0x03), // push an entity id on the stack

    // Custom opcodes for game demo

    OP_MOVE_HZ_I        = MAKE_OPCODE(CAT_IMM, 1, 0x71), // move horizontally by x units
    OP_MOVE_HZ          = MAKE_OPCODE(CAT_STK, 0, 0x71), // move horizontally by x units
    OP_MOVE_VT_I        = MAKE_OPCODE(CAT_IMM, 1, 0x72), // move vertically by y units
    OP_MOVE_VT          = MAKE_OPCODE(CAT_STK, 0, 0x72), // move vertically by y units

    OP_SHOOT_I          = MAKE_OPCODE(CAT_IMM, 1, 0x73),
    OP_SHOOT            = MAKE_OPCODE(CAT_STK, 0, 0x73),
    OP_CHOOSE_WEAPON_I  = MAKE_OPCODE(CAT_IMM, 1, 0x74),
    OP_CHOOSE_WEAPON    = MAKE_OPCODE(CAT_STK, 0, 0x74),

};

enum OperandTypes
{
    INVALID_TYPE = 0,
    NUMBER,
};

int is_nop(opcode_t);

OpResult execute_one(OpBytecode *bytecode, OpStack *stack);

OpResult impl_move_horizontal(OpStack *stack);
OpResult impl_move_vertical(OpStack *stack);


#ifdef __cplusplus
}
#endif

#endif // PRAGMA_ONCE_OPCODE_H
