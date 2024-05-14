
' For BlitzMax-NG!

' -----------------------------------------------------
' IMPORTANT! Set these MaxIDE options before building:
' -----------------------------------------------------

' Program menu:

' BMX-NG App Options: Build Console App
' BMX-NG Platform: Win32
' BMX-NG Architecture: x64

' sgd2bmx "include_directory_path" "output_file_path.bmx"

' eg. sgd2bmx C:/dev/libsgd/release/include/sgd C:/dev/libsgd/blitzmax/sgd.bmx
' eg. sgd2bmx "C:/my dev folder/libsgd/release/include/sgd" blitzmax/sgd.bmx

' Relative paths fine too.

' Best passing each param in quotes so spaces are handled.

' If only an output path is specified, defaults to filename
' libsgd.bmx; otherwise uses filename passed-in.

Framework BRL.FileSystem
Import BRL.StandardIO
Import BRL.Retro
Import BRL.Stream

Const INCLUDE_KEYCODES:String	= "keycodes.h"
Const INCLUDE_SGD:String		= "sgd.h"

Const OUTPUT_FILE:Int = 1 ' Text to .bmx file
Const OUTPUT_TEXT:Int = 2 ' Print text on screen
Const OUTPUT_64BIT:Int = True
Const PREFIXES:Int = False ' sgd_Prefixes
Const LANG_BMX:Int = 1

Global BmxPath:String
Global OutputFormat:Int = OUTPUT_FILE
'Global OutputFormat:Int = OUTPUT_TEXT ' Just dump to MaxIDE Output tab
Global DLLName:String = "sgd_dynamic.dll"
Global OutStream:TStream
Global Functions:String
Global Div:String = "--------------------------------------------------------------------------------"

' Oh, God, what a mess...

If AppArgs.length <> 3
	RuntimeError "ERROR: Need arguments: input directory and output path (file or directory)."
EndIf

Local input_dir:String = AppArgs [1]
Local output_path:String = AppArgs [2]

If Lower (ExtractExt (output_path)) <> "bmx"
	RuntimeError "ERROR: Expecting output path ending .bmx!"
EndIf

Local output_dir:String = ExtractDir (output_path)
Local output_filename:String = StripDir (output_path)

If FileType (output_dir) <> FILETYPE_DIR
	If Not CreateDir (output_dir)
		RuntimeError "Output directory doesn't exist and couldn't be created!"
	EndIf
EndIf

Local out_filepath:String = output_dir + "/" + output_filename

If FileType (out_filepath) = FILETYPE_FILE
	DeleteFile out_filepath
EndIf

'Local count:Int = 0
'For Local arg:String = EachIn AppArgs
'	Print "Arg " + count + ": " + arg
'	count = count + 1
'Next

If Right (input_dir, 1) = "/" Or Right (input_dir, 1) = "\"
	input_dir = Left (input_dir, Len (input_dir) - 1)
EndIf

Go input_dir, out_filepath

End

Function Go (input_dir:String, out_filepath:String)

	BmxPath = ExtractDir (out_filepath)

	' For OO module/namespace, call per:

	' 	SGD.Init				' IMPORTANT!

	' 	SGD.Function1 ()
	' 	If SGD.Function2 () Then...

	' etc.

	' See demos.

	Global IncPath:String
	Global Inc:String = input_dir

	Local bmx_created:Int = False
	Local lang:Int = LANG_BMX

	Select lang

		Case LANG_BMX
			bmx_created = CreateBMXDir ()

	End Select

	Const WRITING_SGD:Int = 1
	Const WRITING_KEYS:Int = 2

	Local includes:String [2]
	
	includes [0] = input_dir + "/" + INCLUDE_KEYCODES
	includes [1] = input_dir + "/" + INCLUDE_SGD
	
	If bmx_created

		OutStream = WriteStream (out_filepath)
		
		If OutStream

			Local outfile:String = StripDir (out_filepath)

			'Print "Writing to " + outfile
			
			Local indent:String = ""
		
			Output ""
			
			Output "Type SGD"
			Output ""
			
			indent = "~t"

			For Local inc:String = EachIn includes
				ParseInclude inc
			Next

			Output ""
			Output indent + "Global SGDLib:Byte Ptr"
			Output ""
			Output indent + "Function LoadSGD (dll_path:String = ~q" + DLLName + "~q)"

			Output ""
			Output indent + indent + "Local sgd_lib:Byte Ptr = LoadLibraryA (dll_path)"
			Output ""
			Output indent + indent + "If sgd_lib"
			Output ""

			WriteFunctions

			Output ""
			Output indent + indent + "Else"

			Output indent + indent + Tabbed ("Local msg:String = ~qCouldn't load ~q + RealPath (dll_path) + ~q... check path is correct!~q")

			Output indent + indent + Tabbed ("?Not Win64")
			Output indent + indent + Tabbed ("msg = msg + Chr (10) + Chr (10) + ~qNote standard DLL is 64-bit: make sure you are compiling in 64-bit mode!~q + Chr (10) + Chr (10) + ~q(MaxIDE 'Program' menu -> Architecture -> x64.)~q")
			Output indent + indent + Tabbed ("?")
			
			Output indent + indent + Tabbed ("RuntimeError msg")
			Output indent + indent + "EndIf"
			
			Output ""
			Output indent + "End Function"
			Output ""
			Output "End Type"

			CloseStream OutStream

			Local new_file:String = BmxPath + "/" + outfile
			
			If OutputFormat = OUTPUT_FILE
				'OpenURL new_file
				Print "File now ready at " + new_file
			EndIf
			
		EndIf
			
	Else
		RuntimeError "Failed to create 'output' directory! Write-protected?"
	EndIf

End Function

Function RemovePrefix:String (entry:String)

	If Left (entry, 4) = "sgd_"
		entry = Mid (entry, 5)
	Else
		RuntimeError "sgd_ prefix expected! Error in " + entry
	EndIf

	Return entry
	
End Function

Function WriteFunctions ()

	Local indent:String = "~t~t"
	
	Local f:String [] = Functions.Split (Chr (10))
	
	For Local entry:String = EachIn f
		
		If Not entry Then Continue
		
		Local dll_name:String = entry
		
		' More ouchie...
		
		If Right (dll_name, 7) = "_native"
			dll_name = Left (dll_name, Len (dll_name) - 7)
		EndIf
		
		If PREFIXES
			Output indent + Tabbed (entry + " = GetProcAddress (sgd_lib, ~q" + dll_name + "~q)")
		Else
			Output indent + Tabbed (RemovePrefix (entry) + " = GetProcAddress (sgd_lib, ~q" + dll_name + "~q)")
		EndIf
		
	Next
	
End Function

Function ListFunctions ()

	Local f:String [] = Functions.Split (Chr (10))
	
	For Local entry:String = EachIn f
		If entry Then Print "Func: " + entry
	Next
	
End Function

Function Output (t:String)

	Select OutputFormat
		Case OUTPUT_TEXT
			Print t
		Case OUTPUT_FILE
			WriteLine OutStream, t
	End Select
	
End Function

Function CreateBMXDir ()
	Return CreateDir (BmxPath)
End Function

' Actually, not needed -- everything is an int!

Function ParseTypeDef (def:String)

	' Copy/paste template:
	If def = "" Then Return

	'Print Tabbed ("Parsing typedef:", 2)

	'Print Tabbed (Div, 2)
	'Print Tabbed (def, 2)
	'Print Tabbed (Div, 2)
	'Print ""
	
	' Treat const char* as single type...

	def = Replace (def, "const char*", "const_char*")
		
	If Right (def, 1) = ";"
		def = def [..def.length - 1]
	EndIf
	
	Local part:String [] = def.Split (" ")
	
	'For Local p:String = EachIn part
	'	Print p
	'Next
	
End Function

Function ParseDefine (def:String)

	def = TrimWhitespace (def) ' Strip repeat spaces/tabs
	
	' IN: "#define SGD_BLEND_MODE_OPAQUE   1"
	' OUT: "#define SGD_BLEND_MODE_OPAQUE 1"
	
	Local indent:String = "~t"
	
	'If SGDModule Then indent = "~t"

	' Copy/paste template:

	If def = "" Then Return
	
	If def = "SGD_TYPES_H_INCLUDED" Then Return
	If def = "SGD_WINDOW_H_INCLUDED" Then Return
	If def = "SGD_EXTERN extern ~qC~q" Then Return
	If def = "SGD_EXTERN extern" Then Return
	If def = "SGD_DECL __stdcall" Then Return
	If def = "SGD_DECL __cdecl" Then Return
	
	'Print Tabbed ("Parsing #define:", 2)
	'Print Tabbed (Div, 2)
	'Print Tabbed (def, 2)
	'Print Tabbed (Div, 2)
	'Print ""

	' Consts...
	
		' SGD_EVENT_MASK_SIZE_CHANGED 2
		' SGD_PI 3.14159265359f

	Local parts:String [] = def.Split (" ")

	'Strip SGD_ prefix since we're in SGD namespace
	If parts[0].StartsWith("SGD_") parts[0] = parts[0][4..]

	'Oops, fix keywords...maybe remove these entirely?
	Select parts[0]
	Case "TRUE", "FALSE", "PI"
		parts[0] = parts[0] + "_"
	End Select
	
	'Select part.length
	'	Case 1
	'		Print "One"
	'	Case 2
	'		Print "Two"
	'	Case 3
	'		Print "Three"
	'	Default
	'		Print "WTF: " + part.length + ": " + def
	'End Select
	
	Select parts.length
		
		Case 2

			For Local p:String = EachIn parts

				If IsNumber (p)

					If Instr (p, ".") And Right (p, 1) = "f"

						' Handle float

						p = p[.. p.length - 1] ' Strip 3.14159265359f to 3.14159265359
						
						Output indent + "Const " + parts[0] + ":Float = " + p

					Else

						' Handle int

						Output indent + "Const " + parts[0] + ":Int = " + p

					EndIf
				
				Else
					If IsHex (p)
						p = Replace (p, "0x", "$")
						Output indent + "Const " + parts[0] + ":Int = " + p
					EndIf
				EndIf

			Next
	
		Default
			'Print "IGNORED: " + def + " (" + parts.length + " parts)"

	End Select
	
End Function

' Count how many times string 'find' is in string s

Function CountString (s:String, find:String)

	Local count:Int = 0
	
	For Local loop:Int = 0 Until Len (s)
		If s[loop..loop + Len (find)] = find
			count = count + 1
		EndIf
	Next
	
	Return count
	
End Function

Function IsHex (s:String)

	Local hex_num:Int = False
	
	' Not exactly ISO-compliant, but, as they
	' say, "close enough for government work"
	
	If Left (s, 2) = "0x" Then hex_num = True
	
	Return hex_num
	
End Function

Function IsNumber (s:String)

	' 0-9 = chars 48-57
	
	Local num:Int = False
	
	For Local char:Int = 0 Until s.length
	
		If s[char] > 47 And s[char] < 58
		
			' Digit
			num = True
			
		Else
		
			If s[char] = 46 Or (s[char] = 102 And s.EndsWith ("f"))
				num = True ' Decimal point, or ends with f for const float number
			Else
			
				' Anything besides '.', 'f'-ending or digit!
				
				num = False
				Exit
				
			EndIf
			
		EndIf
		
	Next
	
	Return num
	
End Function

Function ParseFunction (def:String)

	' Copy/paste template:
	If def = "" Then Return

	Local indent:String = "~t"
	
'	If SGDModule Then indent = "~t"
	
'	If def = "SGD_TYPES_H_INCLUDED" Then Return

'	Print Tabbed ("Parsing function:", 2)
'	Print Tabbed (Div, 2)
'	Print Tabbed (def, 2)
'	Print Tabbed (Div, 2)
'	Print ""
	
	' Functions...
	
		' void SGD_DECL sgd_CreateScene();
		' int SGD_DECL sgd_WindowWidth();
		' void SGD_DECL sgd_CreateWindow(int width, int height, SGD_String title, int flags);
		' void SGD_DECL sgd_SetErrorHandler(void(SGD_DECL* handler)(const char* error, void* context), void* context);

		' Function

	'Select part.length
	'	Case 1
	'		Print "One"
	'	Case 2
	'		Print "Two"
	'	Case 3
	'		Print "Three"
	'	Default
	'		Print "WTF: " + part.length + ": " + def
	'End Select
	
	
	Local parts:String [] = def.Split (" ")

	'Print "PARSING: " + def + " (" + parts.length + " parts)"

	If IsCallback (def)
		
		' Callback function! Deal later...
		
	Else
	
		' Parse that mutha!
		
'		For Local p:String = EachIn parts
'			Print p
'		Next

		' TODO: Split by ( into type/name and params
		
		' Ditch this, not needed now...
		def = Replace (def, "SGD_DECL", "")
		def = Replace (def, "  ", " ") ' Crude, but...
		
		Local type_name:String [] = def.Split ("(")
		
		' Should just be two parts!
		
		Local _type_and_name:String = ""
		Local _params:String = ""
		
		If type_name.length = 2
			_type_and_name = type_name [0]
			_params = type_name [1]
			'Print "PARTS:"
			'Print _type_and_name
			'Print _params
			'Print ""
		Else
			RuntimeError "TOO MANY THINGS!" + def
			'Print "(" + type_name.length + " parts!"
'			End
		EndIf
		
'		If _type_and_name
'			Print BlitzParams (_type_and_name)
'		EndIf
		
		Local params_output:String = ""
		
		If _params
			
			If Right (_params, 2) = ");" Then _params = Left (_params, Len (_params) - 2)
			'Print "(" + _params + ")"
			
			_params = Replace (_params, ", ", ",")
			
			Local ps:String[] = _params.Split (",")

	' WIP: String wrappers -- do this:

	' Global CreateWindow_native (_width:Int, _height:Int, title:Byte Ptr, flags:Int) "win32"
	' Function CreateWindow (_width:Int, _height:Int, title:String, flags:Int)
	' 	CreateWindow_native (_width, _height, title.ToUTF8String (), flags)
	' End Function
	
	' and:
	
	' CreateWindow_native = GetProcAddress (sgd_lib, "sgd_CreateWindow")
				
			For Local p:String = EachIn ps
				params_output = params_output + BlitzParams (p) + ", "
			Next
			
			If Right (params_output, 2) = ", "
				params_output = Left (params_output, Len (params_output) - 2)
			EndIf
			
		EndIf

		Local byte_ptr:Int = False
		Local add_native:String = ""
		
		Local func:String = BlitzParams (_type_and_name)
		
		' NOTE: func may be FunctionName or FunctionName:Type!
		
		Local params:String = " (" + params_output + ") ~qwin32~q"

		' FUTURE: Might in future need to tag ":Byte Ptr" as string in BlitzParams! eg. ":Byte Ptr_string"
		
		' All strings for now...
		
		' Also assuming SGD will never return a UTF8 string! Update func:String if needed...
		
		If CountString (params, ":Byte Ptr")
			add_native = "_native"
		EndIf

		If Not PREFIXES
			func = RemovePrefix (func)
		EndIf
		
		If add_native
			If Not Instr (func, ":") ' No defined type
				func = func + add_native ' add "_native"
			Else
				func = Replace (func, ":", "_native:") ' Ooh, nasty!
			EndIf
		EndIf
		
		Local decl:String = func + params
		
'		If PREFIXES
			decl = "Global " + decl
'		Else
'			decl = indent + "Global " + RemovePrefix (decl)
'		EndIf
		
'		If PREFIXES
'			decl = indent + "Global " + decl
'		Else
'			decl = indent + "Global " + RemovePrefix (decl)
'		EndIf
		
		Output indent + decl
		
' 	CreateWindow_native (_width:Int, _height:Int, title:Byte Ptr, flags:Int) "win32"
' 	CreateWindow (_width, _height, title.ToUTF8String (), flags)

		'If Right (func, 7) = "_native"' Or 
		If Instr (func, "_native")
		
			func = Replace (func, "_native", "")

			If Right (params, 8) = " ~qwin32~q"
				params = Left (params, Len (params) - 8)
			EndIf

'			Output indent + "Function " + Left (func, Len (func) - 7) + Replace (params, ":Byte Ptr", ":String")
			Output indent + "Function " + func + Replace (params, ":Byte Ptr", ":String")
			
			Local decl_copy:String = decl

			decl_copy = Mid (decl_copy, 7) ' Strip "Global "
			
			decl_copy = Replace (decl_copy, ":Int", "")
			decl_copy = Replace (decl_copy, ":Float", "")
			decl_copy = Replace (decl_copy, ":Byte Ptr", ".ToUTF8String ()")

			If Right (decl_copy, 8) = " ~qwin32~q"
				decl_copy = Left (decl_copy, Len (decl_copy) - 8)
			EndIf
			
'			If add_native
'				Print decl_copy
'				Print func
'				Print ""
'			EndIf

			Local ret:String = ""
			
			decl_copy = Trim (decl_copy) ' Ends up with leading whitespace... f' it!
			
			If Instr (func, ":") Then ret = "Return "
			
			Output indent + indent + ret + decl_copy

			Output indent + "End Function"

		EndIf
		
'		If PREFIXES
'			Output indent + "Global " + BlitzParams (_type_and_name) + " (" + params_output + ") ~qwin32~q"
'		Else
'			Output indent + "Global " + RemovePrefix (BlitzParams (_type_and_name)) + " (" + params_output + ") ~qwin32~q"
'		EndIf
		
		Local name_split:String [] = _type_and_name.Split (" ")
		
		If name_split.length = 2
		
			Local name:String = name_split [1] + add_native

			Functions = Functions + name + Chr (10)
			
			'Print func + "/" + name
'		Else
'			Print "NOT 2"
'			Print "_type_and_name"
'			End
		EndIf
		
		'If Instr (BlitzParams (_type_and_name), "sgd_CreateBoxMesh")
		'	End
		'EndIf
		
	EndIf
	
End Function

Function BlitzParams:String (params:String)

	If Trim (params) = "" Then Return ""
	
	Local p:String [] = params.Split (" ")
	
	Local blitz:String = ""
	
	If p.length = 2
		
		Select p[0]
			Case "float"
				p[0] = ":Float"
			Case "SGD_Real"
				If OUTPUT_64BIT
					p[0] = ":Double"
				Else
					p[0] = ":Float"
				EndIf
			Case "int"
				p[0] = ":Int"
			Case "void"
				p[0] = "" ' Or poss Int for bmx-ng strict mode?
			Case "void*"
				p[0] = ":Byte Ptr" ' Or poss Int for bmx-ng strict mode?
			
			Case	"SGD_Bool", ..
					"SGD_Handle",..
					"SGD_Texture",..
					"SGD_Material",..
					"SGD_Surface",..
					"SGD_Mesh",..
					"SGD_Entity",..
					"SGD_Camera",..
					"SGD_Light",..
					"SGD_Model",..
					"SGD_Skybox",..
					"SGD_Sprite",..
					"SGD_Font",..
					"SGD_Sound"
				
				p[0] = ":Int"

			Case "SGD_String"
				p[0] = ":Byte Ptr" ' I think... TODO: Test

			Default
				Print "Unhandled param type:"
				Print p[0]
				Print params
				End
		End Select
		
		Select p[1]
			' Some bmx keywords
			Case "red", "green", "blue", "width", "height"
				p[1] = "_" + p[1]
		End Select
		
		blitz = p[1] + p[0]
		
		Return blitz
		
	Else
		RuntimeError "params <> 2! " + params; End
	EndIf
	
End Function

' Reminder to self: submit to gcc maintainers, seems robust...

Function IsCallback (def:String)

	If (CountString (def, "(") > 1) Or (CountString (def, ")") > 1)
		Return True
	EndIf
	
End Function

Function ParseInclude (include_path:String)

	Local inc:TStream = ReadStream (include_path)

	'Print ""
	
	If Not inc
		RuntimeError "Couldn't read " + include_path
	Else
		Print "PARSING INCLUDE '" + include_path + "'..."
	EndIf

	Local parsing:String	= ""

	While Not Eof (inc)

		parsing = Trim (ReadLine (inc))
		
		' Deal with multi-line declarations, eg.
		
			Rem
		
		//! Create a new mesh.
SGD_API SGD_Mesh SGD_DECL sgd_CreateBoxMesh(float minX, float minY, float minZ, float maxX, float maxY, float maxZ,
											SGD_Material material);
											
			End Rem
		
		If Right (parsing, 1) = ","
			Repeat
				If Not Eof (inc)
					parsing = parsing + Trim (ReadLine (inc))
				EndIf
			Until Right (parsing, 1) <> ","
		EndIf
		
		If Left (parsing, 2) = "//"
			parsing = ""
		EndIf
		
		If parsing

			'Print Tabbed (Div)
			'Print Tabbed ("Parsing line:")
			'Print Tabbed (Div)
			
			Local comment_found:Int = False
			
			' Strip any inline //-style comments...
			
			Local comment:Int = Instr (parsing, "//")
			
			If comment
				'Print "COMMENT:" + parsing
				parsing = Left (parsing, comment - 1)
				'Print parsing
				parsing = Trim (parsing)
				'Print parsing
				comment_found = True
			EndIf
			
			' Strip */ until */ -- not robust but just has to get through this!

			Local eof_found:Int = False
			
			Local open_comment:Int = Instr (parsing, "/*")
			
			If open_comment

				'Print "Open comment..." + parsing
				
				Local close_comment:Int = Instr (parsing, "*/")
				
				If Not close_comment
				
					Local grab_line:String
					Local comment_in_line:Int = 0

					Repeat
						grab_line = ReadLine (inc)
						comment_in_line = Instr (grab_line, "*/")
						eof_found = Eof (inc)
					Until comment_in_line Or eof_found
					
					If comment_in_line
						parsing = Trim (Mid (grab_line, comment_in_line + 2))
					EndIf
					
				EndIf
			
				comment_found = True
			
			EndIf
			
			Local comment_type:String = ""
			
			If comment_found
				If open_comment
					comment_type = Tabbed ("[SKIPPED: /* multi-line */ comment!]")
				Else
					comment_type = Tabbed ("[SKIPPED: // comment!]")
				EndIf
			EndIf

' DEBUG LINE			
'			Print Tabbed (parsing) + comment_type

			'If comment_found
			'	Print ""
'				Print comment_type
			'EndIf
			
			If comment_found And open_comment Then parsing = ""
			'If Not eof_found Then Print "" ' I care.
		
			' Grab keywords that are followed by a space...
			
			Local space:Int = Instr (parsing, " ")
			
			If space
				
				Local keyword:String = Left (parsing, space - 1)
				
				Select keyword
					
					Case "error"
					Case "typedef"
					
						'ParseTypeDef Mid (parsing, space + 1)
						' Not needed -- everything int!
						
					Case "#define"

						ParseDefine Mid (parsing, space + 1)

					
					Case "SGD_EXTERN"

						'ParseDefine Mid (parsing, space + 1)
						ParseDefine parsing
					
					Case "SGD_API"
					
						ParseFunction Mid (parsing, space + 1)
						
					Default
						
						If keyword <> "#if"..
							And keyword <> "#ifndef"..
							
								'Print Tabbed ("*** Unhandled keyword:", 2)
								'Print Tabbed (keyword, 2)
								'Print ""
								'End

						EndIf
						
				End Select
				
			EndIf
			
		EndIf
		
	Wend

	CloseStream (inc)

End Function

Function Tabbed:String (txt:String, indents:Int = 1)
	
	If Not indents Then Return txt
	
	If indents = 1
		Return "~t" + txt
	EndIf
	
	For Local loop:Int = 1 To indents
		txt = "~t" + txt
	Next
	
	Return txt
	
End Function

Function TrimWhitespace:String (strip:String)
	
	strip = Replace (strip, "~t", " ")
	
	Local rebuilt:String = ""
	
	For Local pos:Int = 1 To Len (strip) - 1
	
		Local this_char:String = Mid (strip, pos, 1)
		
		If this_char = " "
		
			Local next_char:String = Mid (strip, pos + 1, 1)
			
			If next_char = " "
				Continue
			Else
				rebuilt = rebuilt + this_char
			EndIf

		Else
			rebuilt = rebuilt + this_char
		EndIf
		
	Next
	
	rebuilt = rebuilt + Right (strip, 1)
	rebuilt = Trim (rebuilt)
	
	Return rebuilt
	
End Function
