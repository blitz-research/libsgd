# Original code by Seyha, tweaked by Mark

# Python requirements
# $ pip3 install tree-sitter tree-sitter-c

# Generate 'sgd.lua' binding:
# $ python3 bindgen.py inc_dir output_file

import os, io, sys
from tree_sitter import Language, Parser
import tree_sitter_c

sgd_excludes = [
    'SGD_API',
    'SGD_DECL',
    'SGD_EXTERN',
    'SGD_KEYCODES',
    'SGD_SGD',
    'SGD_TRUE',
    'SGD_FALSE',
    'SGD_NULL',
]

out = {
    'cdef': {
        'typedefs': [],
        'funcs': []
    },
    'lua': {
        'consts': {},
        'funcs': {}
    }
}

#// Functions

# ast

def preproc_def(node):
    ident, value = node.children[1].text.decode(), ''
    if len(node.children) >= 3:
        value = node.children[2].text.decode()
        value = value.replace('SGD_', '')
    if not any(ex in ident for ex in sgd_excludes):
        #print(f'lua.consts.{ident[4:]} = "{value.replace('SGD_', '')}"')
        out['lua']['consts'][ident[4:]] = value if not value in out['lua']['consts'] else out['lua']['consts'][value]

def type_definition(node):
    text = node.text.decode()
    #print(f'cdef.typedefs[{len(out['cdef']['typedefs'])}] = "{text}"')
    out['cdef']['typedefs'].append(text)

def declaration(node):
    functype, funcname = '', ''
    text = node.text.decode()
    if 'SGD_API' in text:
        for child in node.children:
            if child.type == 'identifier':
                functype = child.text.decode()
                break

    node = node.next_sibling
    text = node.text.decode()
    if 'SGD_DECL' in text:
        for child in node.children:
            if child.type == 'function_declarator':
                funcproto = f'{functype} {child.text.decode()}'
                funcproto = funcproto.replace('SGD_DECL', '') #!FIXME: remove SGD_DECL in callback
                funcproto = ' '.join(funcproto.split()) # remove extra whitespaces
                #print(f'cdef.funcs[{len(out['cdef']['funcs'])}] = "{funcproto};"')
                out['cdef']['funcs'].append(funcproto + ';')

                for func in child.children:
                    if func.type == 'identifier':
                        text = func.text.decode()
                        funcname = text[4].lower() + text[5:] # camelcase
                        #print(f'lua.funcs.{funcname} = "{text}"')
                        out['lua']['funcs'][funcname] = text
                        break

def traverse(node, indent=0):
    #print('    ' * indent + f'{node.start_point.row}| type: "{node.type.replace(os.linesep, '\\n')}", text: "{node.text.decode().replace(os.linesep, '\\n')}"')
    if node.type == 'preproc_def':
        preproc_def(node)
    elif node.type == 'type_definition':
        type_definition(node)
    elif node.type == 'declaration':
        declaration(node)

    for child in node.children:
        traverse(child, indent + 1)

# utils

def writeln(io: io.TextIOWrapper, text = ''):
    io.write(text + os.linesep)


#// MAIN

def main(inc_dir, output_file):

    with open(inc_dir+"/sgd/sgd.h", "r") as sgd_file:
        source = sgd_file.read()
        sgd_file.close()

    with open(inc_dir+"/sgd/keycodes.h", "r") as keycodes_file:
        keycodes = keycodes_file.read()
        keycodes_file.close()

    # hack
    source = source.replace('#include "keycodes.h"', keycodes)


    language = Language(tree_sitter_c.language())
    parser = Parser(language)
    ast = parser.parse(bytes(source, 'utf8'))

    traverse(ast.root_node, 0)


    with open(output_file, 'w') as sgd:
        writeln(sgd, 'local ffi = require("ffi")\n')
        writeln(sgd, 'ffi.cdef[[')

        writeln(sgd, '\t// Typedefs')
        for typedef in out['cdef']['typedefs']:
            if typedef == 'typedef float SGD_Real;': #!FIXME:
                writeln(sgd, f'\t//{typedef}')
            else:
                writeln(sgd, f'\t{typedef}')

        writeln(sgd, '\n\t// Functions')
        for func in out['cdef']['funcs']:
            writeln(sgd, f'\t{func}')
        writeln(sgd, ']]\n')


        writeln(sgd, 'local lib = ffi.load("sgd_dynamic")\n')

        writeln(sgd, 'local sgd = {')
        writeln(sgd, '\t-- Constants')
        for k, v in out['lua']['consts'].items():
            writeln(sgd, f'\t{k} = {v},')

        writeln(sgd, '\n\t-- Functions')
        for k, v in out['lua']['funcs'].items():
            writeln(sgd, f'\t{k} = lib.{v},')

        writeln(sgd, '}\n')
        writeln(sgd, 'return sgd')

        sgd.close()


if __name__ == "__main__":

    if len(sys.argv) != 3:
        print("Usage: python bindgen.py <inc_dir> <output_file>")
        sys.exit(1)

    main(sys.argv[1], sys.argv[2])
