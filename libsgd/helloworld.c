//
// This can be built with something like:
//
// gcc -I../../include -L. -DSGD_DYNAMIC=1 -o helloworld helloworld.c -lsgd_dynamic

#include <sgd/sgd.h>

int main() {

	sgd_Startup();
    
    sgd_CreateWindow(640, 480, "Hello World!", 0);
    sgd_CreateScene();
    sgd_SetSceneClearColor(1,.5,0,1);
    
    while(!sgd_PollEvents()) {
        sgd_RenderScene();
        sgd_Present();
    }

	sgd_Shutdown();
}
