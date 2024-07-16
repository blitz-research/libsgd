#pragma once

#include "std.h"

namespace sgd {

//! Replace all occurances of a substring.
String replace(CString string, CString find, CString replace);

//! Split a string into an array of substrings.
Vector<String> split(CString string, CString separator);

//! Join an array of substrings into a string.
String join(CVector<String> fields, CString separator);

//! Convert string to uppercase.
String toUpper(CString string);

//! Convert string to uppercase.
String toLower(CString string);

//! Return true if string starts with substr.
bool startsWith(CString string, CString substr);

//! Return true if string ends with substr.
bool endsWith(CString string, CString substr);

//! Add quotes to string. Noop if already quoted.
String enquote(CString str);

//! Remove quotes from string. Noop if not quoted.
String dequote(CString str);

//! "1", "true", "on"
bool truthy(CString str);

}
