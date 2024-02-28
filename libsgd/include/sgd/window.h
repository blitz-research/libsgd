#ifndef SGD_WINDOW_H_INCLUDED
#define SGD_WINDOW_H_INCLUDED

#include "types.h"

// Used by sgd_CreateWindow.
#define SGD_WINDOW_FLAGS_FULLSCREEN 1
#define SGD_WINDOW_FLAGS_RESIZABLE 2

// Returned by sgd_PollEvents.
#define SGD_EVENT_MASK_CLOSE_CLICKED 1
#define SGD_EVENT_MASK_SIZE_CHANGED 2

SGD_EXTERN void SGD_DECL sgd_CreateWindow(int width, int height, SGD_String title, int flags);

SGD_EXTERN int SGD_DECL sgd_WindowWidth();

SGD_EXTERN int SGD_DECL sgd_WindowHeight();

SGD_EXTERN int SGD_DECL sgd_PollEvents();

#endif
