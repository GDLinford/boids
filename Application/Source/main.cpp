//Main.cpp

#include "Scene.h"



int main()
{
    Scene* pScene = Scene::GetInstance();
    if (pScene) 
    {
        bool bIsInitiaalised = pScene->Initialise();
        if (bIsInitiaalised) 
        {
            bool bShouldQuit = false;
            while (!bShouldQuit) 
            {
                bShouldQuit = pScene->Update();
                pScene->Render();
            }

            pScene->Deinitialise();
        }
    }
    delete pScene;

    return 0;
}



