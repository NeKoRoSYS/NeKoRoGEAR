#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <iostream>

#include "Components/Application.h"

int main(int argc, char* argv[]) {
    try {
        Application app;
        app.Run();
    } catch (const std::exception& e) {
        std::cerr << "Engine Fatal Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}