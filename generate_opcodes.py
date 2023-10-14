import sys
from collections import namedtuple

OpcodeDef = namedtuple('OpcodeDef', ['opcode', 'cat', 'nargs', 'code', 'impl'])

opcodes = []

def main():
    filename = 'opcodes.def'

    assert(len(sys.argv) > 1)

    mode = sys.argv[1]

    if len(sys.argv) > 2:
        filename = sys.argv[2]

    with open(filename, 'r') as file:
        parse_def_file(file)

    if mode == 'opcodes':
        max_len = 0
        for opcode,_c,_n,_d,_i in opcodes:
            max_len = max(max_len, len(opcode))

        for opcode, cat, nargs, code, impl in opcodes:
            spaces = ''.join([' '] * (max_len - len(opcode)))
            print(f'{opcode}{spaces}    = MAKE_OPCODE({cat}, {nargs}, {code}),')

def parse_def_file(file):
    import re
    pattern = re.compile(r'^DEFINE\((OP_[A-Z_0-9]+)\)\s+MAKE_OPCODE\((CAT_IMM|CAT_STK|CAT_HST),\s*([0-3]),\s*(0x[0-9a-f]{1,3})\)\s+IMPL\(([a-zA-Z_][a-zA-Z_0-9]*)\)$')

    for line in file.readlines():
        match = re.findall(pattern, line)[0]
        opcode = OpcodeDef(*match)
        opcodes.append(opcode)
        
main()
