import sys
from datetime import datetime

template = """// LuaJIT wrapper for LibSGD
//
// Auto-generated : ${NOW}
 
typedef const char* String;
typedef int Handle;
typedef int Bool;
typedef double Real;

${CONSTS}

${FUNCS}

"""


##### LUA Backend #####

def genApi(enums, funcs):
	def transName(name):
		if name.startswith("sgd_"):
			return name[4:5].lower() + name[5:]

		print("Invalid name", name)
		sys.exit(1)

	def transType(name):
		if name == "SGD_String" or name == "SGD_Handle" or name == "SGD_Bool" or name == "SGD_Real":
			return name[4:]

		if name.startswith("SGD_"):
			return "Handle"

		if name == "float":
			return "float"

		if name == "int":
			return "int"

		if name == "void":
			return "void"

		print("Invalid type", name)
		sys.exit(1)

	CONSTS = []
	FUNCS = []
	DECLS = []

	for enum in enums:

		name = enum.name
		if name.startswith("SGD_"):
			name = name[4:]

		CONSTS.append("// enum " + name)

		for m in enum.members:
			mname = m.name
			if mname.startswith("SGD_"):
				mname = mname[4:]

			# pub const SGD_KEY_SPACE = @as(i32, 32);

			CONSTS.append("pub const " + mname + " = @as(i32, " + m.value + ");")

	for func in funcs:

		params = []

		for p in func.params:
			params.append(transType(p.type) + " " + p.name)

		FUNCS.append(transType(func.retType) + " " + func.name + "(" + (", ".join(params)) + ");")
		DECLS.append(transName(func.name) + " = lib." + func.name + ",")

	r = template
	r = r.replace("${NOW}", str(datetime.now()));
	r = r.replace("${CONSTS}", "\n".join(CONSTS))
	r = r.replace("${FUNCS}", "\n".join(FUNCS))
	r = r.replace("${DECLS}", "\n".join(DECLS))

	return r
