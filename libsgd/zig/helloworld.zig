//
// zig run helloworld.zig -L<your_library_directory>/libsgd -lsgd_dynamic
//

const std = @import("std");
const sgd = @import("sgd.zig");

pub fn main() void {
    sgd.init();
    sgd.createWindow(640, 480, "Hello LibSGD in Zig!", 0);
    sgd.setClearColor(1, 0.5, 0, 1);

    while ((sgd.pollEvents() & sgd.EVENT_MASK_CLOSE_CLICKED) == 0) {
        sgd.renderScene();
        sgd.present();
    }
    sgd.terminate();
}
