const sgd = require("./sgd");

sgd.Init();

sgd.CreateWindow(640, 480, "Hello World!", 0);
sgd.CreateScene();
sgd.SetSceneClearColor(1,.5,0,1);

while(!sgd.PollEvents()) {
    sgd.RenderScene();
    sgd.Present();
}

sgd.Terminate();
