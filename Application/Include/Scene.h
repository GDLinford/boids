#ifndef SCENE_H
#define SCENE_H

//forward declarations
struct GLFWwindow;
class Camera;
class Shader;
class Model;
class Entity;

class Scene
{
public:
	static Scene* GetInstance();
	bool Initialise();
	bool Update();
	bool CreateBoid();
	void DestroyBoid();
	void Render();
	void Deinitialise();

private:
	Scene();
	//overload the copy constructor, so we cant create/copy a new instance of this class
	Scene(const Scene&);
	//same thing as above, so we dont accidentally create a new instance
	Scene& operator=(const Scene&);

	int NUM_OF_BOIDS;
	int MAX_BOIDS;
	int CURRENT_BOIDS;

	void showFrameData(bool a_showFrameData);

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	int RandomIntBetweenRange(int iLowerRange, int iUpperRange);
	float RandomFloatBetweenRange(float fLowerRange, float fUpperRange);

	GLFWwindow* m_Window;
	Camera* m_camera;
	Shader* ourShader;
	Model* m_pModel;

	float m_lastX;
	float m_lastY;
	// timing
	float m_deltaTime;
	float m_lastFrame;

	int sliderInt;

	bool m_firstMouse;

	static Scene* s_pSceneInstance;
};


#endif // !SCENE_H


