const sgd = require("./sgd");

sgd.Init();

sgd.CreateWindow(640, 480, "Hello World!", sgd.WINDOW_FLAGS_RESIZABLE|sgd.WINDOW_FLAGS_CENTERED);
sgd.SetClearColor(1,.5,0,1);

while(!sgd.PollEvents()) {
    if(sgd.IsKeyHit(sgd.KEY_ESCAPE)) break;
    sgd.RenderScene();
    sgd.Present();
}

sgd.Terminate();
