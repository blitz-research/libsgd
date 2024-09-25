import sys

from pycparser import parse_file, c_ast

import apigen_bmx


class EnumMember:
	name: ""
	value: ""


class Enum:
	name: ""
	members: []


class Param:
	name: ""
	type: ""


class Func:
	name: ""
	retType: ""
	params: []


class Enumerator(c_ast.NodeVisitor):
	enums = []
	funcs = []

	def visit_Enum(self, node):

		enum = Enum()
		enum.name = node.name
		enum.members = []

		for e in node.values.enumerators:
			m = EnumMember()
			m.name = e.name
			m.value = e.value.value

			enum.members.append(m)

		self.enums.append(enum)

	def visit_FuncDecl(self, node):

		# TODO: Add support for ptrs.
		if node.type.declname == "sgd_SetErrorHandler":
			return

		func = Func()
		func.name = node.type.declname
		func.retType = node.type.type.names[0]
		func.params = []

		if node.args:

			for p in node.args.params:

				param = Param()

				if hasattr(p.type, "declname"):
					param.name = p.type.declname
				else:
					# Probably a pointer type?
					param.name = "?"

				if hasattr(p.type.type, "names"):
					param.type = p.type.type.names[0]
				else:
					# Probably a pointer type.
					param.type = "?"

				func.params.append(param)

		self.funcs.append(func)


def main(includeDir, outputFile):
	ast = parse_file(includeDir + "/sgd/sgd.h", use_cpp=True, cpp_args="-DSGD_GENAPI=1")

	enumer = Enumerator()

	enumer.visit(ast)

	r = apigen_bmx.genApi(enumer.enums, enumer.funcs)

	with open(outputFile, 'w') as file:
		file.write(r)


if __name__ == "__main__":
	if len(sys.argv) != 3:
		print("Usage: apigen.py includeDir outputFile")
		sys.exit(1)

	main(sys.argv[1], sys.argv[2])
