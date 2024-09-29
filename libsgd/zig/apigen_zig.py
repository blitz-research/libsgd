import sys
from datetime import datetime

template = """// Zig wrapper for LibSGD
//
// Auto-generated : ${NOW}
 
pub const String = [*c]const u8;
pub const Handle = i32;
pub const Bool = i32;
pub const Real = f64;

${CONSTS}

${FUNCS}

"""


##### ZIG Backend #####

# ZIG builtin types
#
# pub const String = [*c]const u8;
# pub const Handle = i32;
# pub const Bool = i32;
# pub const Real = f64;

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
			return "f32"

		if name == "int":
			return "i32"

		if name == "void":
			return "void"

		print("Invalid type", name)
		sys.exit(1)

	CONSTS = []
	FUNCS = []

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

		name = transName(func.name)
		if name == "error":
			name = "error_"

		params = []
		for p in func.params:
			pname = p.name
			if pname == "error":
				pname = "error_"

			params.append(pname + ":" + transType(p.type))

		# pub extern fn sgd_Init(...) void;
		# pub extern fn sgd_SetConfigVar(name: String, value: String) void;

		args = ", ".join(params)
		if len(args) == 0:
			args = "..."

		FUNCS.append("pub extern fn " + func.name + "(" + args + ") " + transType(func.retType) + ";")
		FUNCS.append("pub const " + name + " = " + func.name + ";")

	r = template
	r = r.replace("${NOW}", str(datetime.now()))
	r = r.replace("${CONSTS}", "\n".join(CONSTS))
	r = r.replace("${FUNCS}", "\n".join(FUNCS))

	return r
