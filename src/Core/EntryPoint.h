#pragma once
#include <iostream>
#include "Core/Components/Application.h"

extern Application* CreateAppliction();

int main(int argc, char* argv[]) {
    try {
        auto app = CreateApplication();
        app->Run();
        delete app;
    } catch (const std::exception& e) {
        std::cerr << "Engine Fatal Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}