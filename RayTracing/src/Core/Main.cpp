#include "Core/Application.h"


int main(int argc, char* argv[])
{
    Application* app = CreateApplication();
    app->Run();
    delete app;
}
