#ifndef SGD_RUNTIME_H_INCLUDED
#define SGD_RUNTIME_H_INCLUDED

#include "types.h"

#include "beginextern.h"

#define SGD_WINDOW_FLAGS_FULLSCREEN 1
#define SGD_WINDOW_FLAGS_RESIZABLE 2

#define SGD_WINDOW_EVENT_MASK_CLOSE_CLICKED 1
#define SGD_WINDOW_EVENT_MASK_SIZE_CHANGED 2

SGD_API void sgd_CreateWindow(int width, int height, SGD_String title, int flags);

SGD_API void sgd_Run(void(*main)());

SGD_API int sgd_WindowWidth();

SGD_API int sgd_WindowHeight();

SGD_API int sgd_PollEvents();

SGD_API void sgd_Render();

#include "endextern.h"

#endif
