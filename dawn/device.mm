#include "webgpu/webgpu_glfw.h"
#import <QuartzCore/CAMetalLayer.h>
#include "GLFW/glfw3.h"
#include <cstdlib>
#define GLFW_EXPOSE_NATIVE_COCOA
#include "GLFW/glfw3native.h"

namespace sgd {

// Based on this code:
//
// https://dawn.googlesource.com/dawn/+/refs/heads/main/src/dawn/glfw/utils_metal.mm

void* createMetalLayer(GLFWwindow* window) {
	@autoreleasepool {

		NSWindow* nsWindow = glfwGetCocoaWindow(window);
		NSView* view = [nsWindow contentView];

		// Create a CAMetalLayer that covers the whole window that will be passed to
		// CreateSurface.
		[view setWantsLayer:YES];
		[view setLayer:[CAMetalLayer layer]];

		// Use retina if the window was created with retina support.
		[[view layer] setContentsScale:[nsWindow backingScaleFactor]];

		return [view layer];
	}
}

}
