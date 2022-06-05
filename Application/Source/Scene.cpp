#define STB_IMAGE_IMPLEMENTATION

#include "Scene.h"
#include <iostream>

//OpenGl includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//GLM Includes
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>

//project includes
#include "Entitty.h"
#include "TransformComponent.h"
#include "ModelComponent.h"
#include "BrainComponent.h"

//LearnOpenGL include
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

//imgui include
#include <imgui/imgui.h>
#include <imgui/impl/imgui_impl_glfw.h>
#include <imgui/impl/imgui_impl_opengl3.h>

//constants
// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;


//statics
Scene* Scene::s_pSceneInstance = nullptr;

Scene* Scene::GetInstance() 
{
    if (s_pSceneInstance == nullptr) 
    {
        s_pSceneInstance = new Scene();
    }

    return s_pSceneInstance;
}

Scene::Scene() : m_lastX(SCR_WIDTH / 2.0f),
                m_lastY(SCR_HEIGHT / 2.0f),
                m_deltaTime(0.0f),
                m_lastFrame(0.0f),
                m_firstMouse(true),
                m_Window(nullptr),
                m_camera(nullptr)
{
    MAX_BOIDS = 100;
    NUM_OF_BOIDS = 0;
    CURRENT_BOIDS = 0;
    
}

bool Scene::Initialise() 
{
    // glfw: initialize and configure
// ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    m_Window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (m_Window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(m_Window);
    glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);
    glfwSetCursorPosCallback(m_Window, mouse_callback);
    glfwSetScrollCallback(m_Window, scroll_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    int major = glfwGetWindowAttrib(m_Window, GLFW_CONTEXT_VERSION_MAJOR);
    int minor = glfwGetWindowAttrib(m_Window, GLFW_CONTEXT_VERSION_MINOR);
    int revision = glfwGetWindowAttrib(m_Window, GLFW_CONTEXT_REVISION);

    std::cout << "OpenGL Version Supported: " << major << "." << minor << "." << revision << std::endl;

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    //setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //set up dear imgui style
    ImGui::StyleColorsLight();
    const char* glsl_version = "#version 150";
    //set up platform/renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    ourShader = new Shader("shaders/model_loading.vs", "shaders/model_loading.fs");

    // load models
    m_pModel = new Model("models/Fish/Fish.obj");

    //Initialise camers
    m_camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

    //seed the random
    srand((unsigned int)(time(nullptr)));

    return true;
}

bool Scene::CreateBoid() 
{
    //Create entities
    Entity* pEntity = new Entity();

   //Transform COmponent
    TransformComponent* pTransformComponent = new TransformComponent(pEntity);
    pTransformComponent->SetEntityMatrixRow(POSITION_VECTOR, glm::vec3(RandomFloatBetweenRange(-2, 2),
        RandomFloatBetweenRange(-2, 2),
        RandomFloatBetweenRange(-2, 2)));
    pEntity->AddComponent(pTransformComponent);

    //Model Component
    modelComponent* pModelComponent = new modelComponent(pEntity);
    pModelComponent->SetModel(m_pModel);
    pModelComponent->SetScale(0.25f);
    pEntity->AddComponent(pModelComponent);

    //Brain Component
    BrainComponent* pBrainComponent = new BrainComponent(pEntity);
    pEntity->AddComponent(pBrainComponent);
   

    return (CURRENT_BOIDS);
}

void Scene::DestroyBoid()
{
    static std::map<const unsigned int, Entity*>::const_iterator xIter;
    for (xIter = Entity::GetEntityMap().begin(); xIter != Entity::GetEntityMap().end(); ++xIter)
    {
        Entity* pEntity = xIter->second;
        if (pEntity)
        {
            xIter = Entity::GetEntityMap().erase(xIter);
            delete pEntity;
            break;
        }
    }
}

bool Scene::Update() 
{
    // per-frame time logic
// --------------------
    float currentFrame = (float)(glfwGetTime());
    m_deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;

    // input
    // -----
    m_camera->processInput(m_Window, m_deltaTime);

    //iupdae our entities
    static std::map<const unsigned int, Entity*>::const_iterator xIter;
    for (xIter = Entity::GetEntityMap().begin(); xIter != Entity::GetEntityMap().end(); ++xIter) 
    {
        Entity* pEntity = xIter->second;
        if (pEntity) 
        {
            pEntity->Update(m_deltaTime);
        }
    }


    ////if (glfwGetKey(m_Window, GLFW_KEY_SPACE) == GLFW_PRESS) 
    ////{
    ////    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    ////}

    ////else if(glfwGetKey(m_Window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    ////{
    ////    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    ////}

    //return whether to close or not
    return glfwWindowShouldClose(m_Window);
}

void Scene::Render() 
{
    //start our Imgui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    showFrameData(true);
   
    // render
    glClearColor(0.6f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!ourShader) 
    {
        return;
    }

    if (!m_pModel) 
    {
        return;
    }

    // don't forget to enable shader before setting uniforms
    ourShader->use();

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(m_camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = m_camera->GetViewMatrix();
    ourShader->setMat4("projection", projection);
    ourShader->setMat4("view", view);

    //render our Entities
    static std::map<const unsigned int, Entity*>::const_iterator xIter;
    for (xIter = Entity::GetEntityMap().begin(); xIter != Entity::GetEntityMap().end(); ++xIter)
    {
        Entity* pEntity = xIter->second;
        if (pEntity)
        {
            pEntity->Draw(ourShader);
        }
    }

    if (ImGui::CollapsingHeader("Manipulation"))
    {
        if (ImGui::InputInt("Num Of Boids", &NUM_OF_BOIDS))
        {
            if (NUM_OF_BOIDS <= 0)
            {
                NUM_OF_BOIDS = 0;
            }
            //if num boids greater than current then creeat
            if (NUM_OF_BOIDS > CURRENT_BOIDS)
            {
                for (int i = CURRENT_BOIDS; i < NUM_OF_BOIDS; ++i)
                {
                    CreateBoid();
                    ++CURRENT_BOIDS;
                    std::cout << "Creating Boid: " << CURRENT_BOIDS << endl;
                }
            }
            else if(CURRENT_BOIDS > 0)
            {
                //remove boids
                for (int i = CURRENT_BOIDS; i > NUM_OF_BOIDS; --i)
                {
                    DestroyBoid();
                    --CURRENT_BOIDS;
                    std::cout << "Destroying Boid: " << CURRENT_BOIDS << endl;
                }
            }
        }

        ImGui::Spacing();
        ImGui::Text("constants");
        ImGui::SliderFloat("Speed", &BrainComponent::fSpeed, 0.f, 5.f);
        ImGui::SliderFloat("Neighbourhood Radius", &BrainComponent::fNEIGHBOURHOOD_RADIUS, 50.f, 100.f);

        ImGui::Spacing();
        ImGui::Text("Wander Constants");
        ImGui::SliderFloat("Jitter", &BrainComponent::fJITTER, 0.1f, 1.f);
        ImGui::SliderFloat("WanderRadius", &BrainComponent::fWANDER_RADIUS, 0.1f, 5.f);
        ImGui::SliderFloat("Circle Forward Multiplier", &BrainComponent::fCIRCLE_FORWARD_MULTIPLIER, 10.f, 15.f);

        ImGui::Spacing();
        ImGui::Text("WeightedForces");
        ImGui::SliderFloat("Separation Weighting", &BrainComponent::SeparationWeighting, 0.f, 10.f);
        ImGui::SliderFloat("Cohesion Weighting", &BrainComponent::CohesionWeighting, 0.f, 10.f);
        ImGui::SliderFloat("Allignment weighting", &BrainComponent::AlignmentWeighting, 0.f, 10.f);
        ImGui::SliderFloat("Wander Weighting", &BrainComponent::WanderWeighting, 0.f, 1.f);
    }

    //draw the ImGui buffer
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(m_Window);
    glfwPollEvents();
}

void Scene::Deinitialise() 
{
    delete m_camera;
    delete ourShader;
    delete m_pModel;

    static std::map<const unsigned int, Entity*>::const_iterator xIter;
    for (xIter = Entity::GetEntityMap().begin(); xIter != Entity::GetEntityMap().end(); ++xIter)
    {
        Entity* pEntity = xIter->second;
        delete pEntity;
    }

    //close down ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
}

void Scene::showFrameData(bool a_showFrameData)
{
    static bool m_showFrameData = a_showFrameData;
    const float DISTANCE = 10.0f;
    static int corner = 0;
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
    ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    ImGui::SetNextWindowBgAlpha(0.3f); //Transparent 

    if (ImGui::Begin("Frame Data", &m_showFrameData, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) 
    {
        static bool checkbox = false;
        static float sliderFloat = 0.f;
        
        ImGui::Separator();
        ImGui::Text("Application Average: %.3f ms/frame (%.1f FPS)", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        if (ImGui::IsMousePosValid())
            ImGui::Text("Mouse Position: (%.1f, %.1f)", io.MousePos.x, io.MousePos.y);
        else
            ImGui::Text("Mouse Position: <invalid>");
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
            if (ImGui::MenuItem("Top-light", NULL, corner == 1)) corner = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (&m_showFrameData && ImGui::MenuItem("Close")) m_showFrameData = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void Scene::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void Scene::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    Scene* pScene = Scene::GetInstance();

    if (!pScene) 
    {
        return; //Early out
    }
        
    if (pScene->m_firstMouse)
    {
        pScene->m_lastX = (float)xpos;
        pScene->m_lastY = (float)ypos;
        pScene->m_firstMouse = false;
    }

    float xoffset = (float)xpos - pScene->m_lastX;
    float yoffset = pScene->m_lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

    pScene->m_lastX = (float)xpos;
    pScene->m_lastY = (float)ypos;

    if (pScene->m_camera) 
    {
        pScene->m_camera->ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void Scene::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Scene* pScene = Scene::GetInstance();

    if (!pScene && !pScene->m_camera)
    {
        return; //Early out
    }

    pScene->m_camera->ProcessMouseScroll((float)yoffset);
}

int Scene::RandomIntBetweenRange(int iLowerRange, int IUpperRange) 
{
    //spawn randomly by getting value between 
    return rand() & (glm::abs(iLowerRange - IUpperRange)) + (iLowerRange);
}

float Scene::RandomFloatBetweenRange(float fLowerRange, float fUpperRange) 
{
    return fLowerRange + static_cast<float>(rand()) / (static_cast<float> (RAND_MAX / (fUpperRange - fLowerRange)));
}