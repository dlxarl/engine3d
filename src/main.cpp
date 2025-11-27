#include "Engine.h"

int main() {
    Engine engine;

    if (engine.init(800, 600, "3D Engine") != 0) {
        return -1;
    }

    engine.run();
    return 0;
}