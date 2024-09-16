# Original code by Seyha, tweaked by Mark

import subprocess
import sys

def main(inc_dir, output_file):

    sgd_include_path = inc_dir + '/sgd/sgd.h'

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

    sgd_prefix_const = 'pub const SGD_'
    sgd_prefix_func = 'pub extern fn sgd_'

    # process

    out = {
        'types': {},
        'consts': {},
        'funcs': {},
    }

    capi = subprocess.run(['zig', 'translate-c', sgd_include_path], capture_output=True, text=True)

    # read from 'translate-c' output
    for line in capi.stdout.splitlines():

        line = line.replace('c_int', 'i32')
        line = line.replace('@"u0"', 'u0_')
        line = line.replace('@"error"', 'error_')

        if not any(ex in line for ex in sgd_excludes):

            # consts
            if line.startswith(sgd_prefix_const):
                var = line[len(sgd_prefix_const):].split(' ')[0]

                # consts
                if var.isupper():
                    out['consts']['SGD_' + var] = {
                        'decl': line,
                        'alias': 'pub const ' + var + ' = SGD_' + var + ';'
                    }
                # types
                else:
                    out['types']['SGD_' + var] = {
                        'decl': line,
                        'alias': 'pub const ' + var + ' = SGD_' + var + ';'
                    }
                continue

            # funcs
            if line.startswith(sgd_prefix_func):
                func = line[len(sgd_prefix_func):].split('(')[0]

                func_cc = func[0].lower() + func[1:]  # camelcase
                if func_cc == 'error': func_cc = 'error_'  # exception

                out['funcs']['sgd_' + func] = {
                    'decl': line.replace('SGD_', ''),
                    'alias': 'pub const ' + func_cc + ' = sgd_' + func + ';'
                }
                continue

    # write 'out' structure in the 'sgd.json' file (debug)
    # with open('sgd.json', 'w') as sgd:
    #    sgd.write(json.dumps(out, indent=2))
    #    sgd.close()


    # write 'sgd.zig' file
    with open(output_file, 'w') as sgd:
        sgd.write('//------------------\n')
        sgd.write('// Decls\n')
        sgd.write('//------------------\n\n\n')

        sgd.write('// Types\n')
        for k, v in out['types'].items():
            sgd.write(v['decl'] + '\n')
        sgd.write('\n')

        sgd.write('// Constants\n')
        for k, v in out['consts'].items():
            sgd.write(v['decl'] + '\n')
        sgd.write('\n')

        sgd.write('// Functions\n')
        for k, v in out['funcs'].items():
            sgd.write(v['decl'] + '\n')
        sgd.write('\n\n')

        sgd.write('//------------------\n')
        sgd.write('// Aliases\n')
        sgd.write('//------------------\n\n\n')

        sgd.write('// Types\n')
        for k, v in out['types'].items():
            sgd.write(v['alias'] + '\n')
        sgd.write('\n')

        sgd.write('// Constants\n')
        for k, v in out['consts'].items():
            sgd.write(v['alias'] + '\n')
        sgd.write('\n')

        sgd.write('// Functions\n')
        for k, v in out['funcs'].items():
            sgd.write(v['alias'] + '\n')

        sgd.close()


if __name__ == "__main__":

    if len(sys.argv) != 3:
        print("Usage: python bindgen.py <inc_dir> <output_file>")
        sys.exit(1)

    main(sys.argv[1], sys.argv[2])
