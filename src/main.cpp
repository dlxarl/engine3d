#include "Engine.h"
#include "scenes/include/DemoScene.h"
#include "scenes/include/DemoPhysics.h"
#include <iostream>
#include <memory>

int main() {
    Engine engine;

    if (engine.init(1920, 1080, "3D Engine") != 0) {
        std::cout << "Failed to initialize engine" << std::endl;
        return -1;
    }

    auto physicsScene = std::make_shared<DemoPhysics>();

    engine.setScene(physicsScene);

    engine.run();

    return 0;
}