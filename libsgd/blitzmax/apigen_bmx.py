import sys

template = """// BlitzMax wrapper for LibSGD
//
// Auto-generated : ${NOW}

Type SGD
	${ENUMS}
	${FUNCS}

	Global _lib:Byte Ptr

	Function LoadSGD(dll_path:String = "sgd_dynamic.dll")

		Local sgd_lib:Byte Ptr = LoadLibraryA(dll_path)
		if Not sgd_lib
			Local msg:String = "Couldn't load " + RealPath (dll_path) + "... check path is correct!"
			?Not Win64
			msg = msg + Chr (10) + Chr (10) + "Note standard DLL is 64-bit: make sure you are compiling in 64-bit mode!" + Chr (10) + Chr (10) + "(MaxIDE 'Program' menu -> Architecture -> x64.)"
			?
			Notify msg,True
			RuntimeError msg
		EndIf
		${INITS}
	
	End Function
	
End Type
"""


##### BMX Backend #####


def genApi(enums, funcs):
	def transName(name):
		if name.startswith("sgd_"):
			return name[4:]

		print("Invalid name", name)
		sys.exit(1)

	def transType(name):
		if name == "SGD_String":
			return ":String"

		# TODO: Need to pass arch to apigen?
		if name == "SGD_Real":
			return ":Double"

		if name.startswith("SGD_"):
			return ":Int"

		if name == "int" or name == "float":
			return ":" + name.capitalize()

		if name == "void":
			return ""

		print("Invalid name", name)
		sys.exit(1)

	# Outputs
	ENUMS = []
	FUNCS = []
	INITS = []

	for enum in enums:

		name = enum.name
		if name.startswith("SGD_"):
			name = name[4:]

			ENUMS.append("\t'Enum " + name)

		for m in enum.members:
			mname = m.name
			if mname.startswith("SGD_"):
				mname = mname[4:]

			value = m.value
			if value.startswith("0x"):
				value = "$" + value[2:]

			ENUMS.append("\tConst " + mname + ":Int = " + value)

	for func in funcs:

		name = transName(func.name)

		params = []
		hasStrParams = (func.retType == "SGD_String")

		for p in func.params:
			params.append(p.name + transType(p.type))
			hasStrParams |= (p.type == "SGD_String")

		proto = name + transType(func.retType) + "(" + (", ".join(params)) + ")"

		if hasStrParams:

			iname = name + "_native"
			iparams = []
			args = []

			for p in func.params:
				if p.type == "SGD_String":
					iparams.append(p.name + ":Byte Ptr")
					args.append(p.name + ".ToUTF8String()")
				else:
					iparams.append(p.name + transType(p.type))
					args.append(p.name)

			iproto = ""
			if func.retType == "SGD_String":
				iproto = iname + ":Byte Ptr(" + (", ".join(iparams)) + ")"
			else:
				iproto = iname + transType(func.retType) + "(" + (", ".join(iparams)) + ")"

			fcall = iname + "(" + (", ".join(args)) + ")"

			# Native function
			FUNCS.append("\tGlobal " + iproto + " \"os\"")
			INITS.append("\t\t" + iname + " = GetProcAddress(sgd_lib, \"" + func.name + "\")")

			# Wrapper function
			FUNCS.append("\tFunction " + proto)
			if func.retType == "SGD_String":
				FUNCS.append("\t\tReturn String.FromUTF8String(" + fcall + ")")
			elif func.retType != "void":
				FUNCS.append("\t\tReturn " + fcall)
			else:
				FUNCS.append("\t\t" + fcall)

			FUNCS.append("\tEnd Function")

		else:

			FUNCS.append("\tGlobal " + proto + " \"os\"")
			INITS.append("\t\t" + name + " = GetProcAddress(sgd_lib, \"" + func.name + "\")")

	r = template
	r = r.replace("${ENUMS}", "\n" + "\n".join(ENUMS))
	r = r.replace("${FUNCS}", "\n" + "\n".join(FUNCS))
	r = r.replace("${INITS}", "\n" + "\n".join(INITS))

	return r
