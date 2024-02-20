#ifndef SGD_TYPES_H_INCLUDED
#define SGD_TYPES_H_INCLUDED

#define SGD_API extern "C"

static_assert(sizeof(int)==4);

typedef int SGD_Bool;
typedef int SGD_Handle;
typedef const char* SGD_String;

typedef SGD_Handle SGD_Material;
typedef SGD_Handle SGD_Mesh;
typedef SGD_Handle SGD_Entity;
typedef SGD_Handle SGD_Camera;
typedef SGD_Handle SGD_Light;
typedef SGD_Handle SGD_Model;

#endif
