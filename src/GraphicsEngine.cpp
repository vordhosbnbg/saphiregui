#include "GraphicsEngine.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"



GraphicsEngine::GraphicsEngine() : animationManager(std::make_shared<AnimationManager>())
{
    SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int initstatus = IMG_Init(flags);
    TTF_Init();
}

GraphicsEngine::~GraphicsEngine()
{
    Stop();
    mainThread->join();
}

std::shared_ptr<RootWindow> GraphicsEngine::CreateRootWindow(const char * title, int posX, int posY, int width, int height)
{
    return std::make_shared<RootWindow>(title, posX, posY, width, height);
}

void GraphicsEngine::AddWindow(std::shared_ptr<RootWindow> wnd)
{
    windowListToInit.emplace_back(wnd);
}

void GraphicsEngine::AddAnimation(std::shared_ptr<IAnimation> anim)
{
    if(!animationManager)
        animationManager = std::make_shared<AnimationManager>();

    animationManager->AddAnimation(anim);
}


void GraphicsEngine::Start()
{
    isRunning.test_and_set();
    animationManager->Start();
    mainThread = std::make_shared<std::thread>(&GraphicsEngine::MainLoop, this);
}

void GraphicsEngine::MainLoop()
{
    SDL_Event ev;
    while (isRunning.test_and_set())
    {
        InitIfNecessary();
        animationManager->Tick();
        for (auto iterWnd = windowListActive.begin(); iterWnd != windowListActive.end(); ++iterWnd)
        {
            while(SDL_PollEvent(&ev))
            {
                switch (ev.type)
                {
                case SDL_QUIT:
                    Stop();
                    break;
                case SDL_WINDOWEVENT:
                    if (ev.window.windowID == (*iterWnd)->GetWindowID())
                    {
                        (*iterWnd)->ProcessEvent(ev);
                    }
                    break;
                default:
                    (*iterWnd)->ProcessEvent(ev);
                    break;
                }
            }
            (*iterWnd)->Render();
        }
    }
}


void GraphicsEngine::Stop()
{
    animationManager->Stop();
    isRunning.clear();
}

void GraphicsEngine::InitIfNecessary()
{
    if (windowListToInit.size() > 0)
    {
        auto iterWnd = windowListToInit.begin();
        while (iterWnd != windowListToInit.end())
        {
            (*iterWnd)->Init();
            windowListActive.emplace_back(*iterWnd);
            iterWnd = windowListToInit.erase(iterWnd);
        }
    }
}
