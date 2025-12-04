#include "Engine.h"
#include "scenes/include/DemoScene.h"
#include <iostream>
#include <memory>

int main() {
    Engine engine;

    if (engine.init(1980, 1080, "3D Engine") != 0) {
        std::cout << "Failed to initialize engine" << std::endl;
        return -1;
    }

    auto demoScene = std::make_shared<DemoScene>();

    engine.setScene(demoScene);

    engine.run();

    return 0;
}