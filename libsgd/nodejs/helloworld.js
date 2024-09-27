const sgd = require("./sgd");

sgd.init();

sgd.createWindow(640, 480, "Hello World!", sgd.WINDOW_FLAGS_RESIZABLE|sgd.WINDOW_FLAGS_CENTERED);

sgd.setClearColor(1,.5,0,1);

while(!sgd.pollEvents()) {

    if(sgd.isKeyHit(sgd.KEY_ESCAPE)) break;

    sgd.renderScene();

    sgd.present();
}

sgd.terminate();
