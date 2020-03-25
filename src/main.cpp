/*
CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/

#include <iostream>
#include <glad/glad.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#define _USE_MATH_DEFINES
#define SHADOW_DIM 4096
#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "Program2.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;
shared_ptr<Shape> shape_tree, shape_sky, shape_sun;


double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}
class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d, i, k;
	camera()
	{
		w = a = s = d = i = k = 0;
		pos = rot = glm::vec3(0, 0, 0);
		pos = glm::vec3(0, -5, 0);
	}
	glm::mat4 process(double ftime)
	{
		float speed = 0;
		if (w == 1)
		{
			speed = 10*ftime;
		}
		else if (s == 1)
		{
			speed = -10*ftime;
		}
		float yangle=0, xangle = 0;
		if (a == 1)
			yangle = -3*ftime;
		else if(d==1)
			yangle = 3*ftime;
		if (i == 1)
			xangle = -1.5* ftime;
		else if (k == 1)
			xangle = 1.5 * ftime;
		rot.y += yangle;
		rot.x += xangle;
		glm::mat4 Ry = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::mat4 Rx = glm::rotate(glm::mat4(1), rot.x, glm::vec3(1, 0, 0));
		glm::vec4 dir = glm::vec4(0, 0, speed,1);
		dir = dir*Rx*Ry;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return Rx*Ry*T;
	}
};

camera mycam;

// Simple structure to represent a light in the scene.
struct Light
{
	vec3 position;
	vec3 direction;
	vec3 color;
};

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> heightshader;
	std::shared_ptr<Program2> treeprog, skyprog, prog_framebuffer, sunprog, shadowProg, shadowTerrProg;

	// Contains vertex information for OpenGL
	GLuint VAO_MESH_ID, VAO_TREE_ID, VAO_SUN_ID, VBO_SPOS_ID, VBO_SNOR_ID, VBO_STEX_ID, VBO_SELE_ID;

	// Data necessary to give our box to OpenGL
	GLuint MeshPosID, MeshTexID, MeshIndexBufferIDBox, TREE_POS_ID, TREE_NOR_ID, TREE_TEX_ID, TREE_ELE_ID;
	GLuint FrameBufferObj, depth_rb, fb_shadowMap;
	GLuint VAO_RECT_ID, VBO_RPOS_ID, VBO_RTEX_ID;

	//texture data
	GLuint GrassTex;
	GLuint SandTex,TreeTex, SkyTex, FBO_Tex, FBO_PosTex, FBO_ShadowTex, SunTex;

	int tree_vert_count = 0;
	void* indicies;

	int sun_rot_inc = 0, sun_rot_dec = 0;
	float sun_rot = -1.2f;
	//vec4 sun_pos;

	int hf_inc_press = 0, hf_dec_press = 0, bp_inc_press = 0, bp_dec_press = 0, a_inc_press = 0, a_dec_press = 0;
	int ho_inc_press = 0, ho_dec_press = 0;
	float h_freq = 0.02, h_pers = 0.75f;
	float b_freq = 0.08f, b_pow = 3.0f;
	float amplitude = 40;
	int h_oct = 8, b_oct = 4;

	Light primaryLight;
	bool show_shadowmap = false;
	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			mycam.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			mycam.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			mycam.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			mycam.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			mycam.d = 0;
		}
		if (key == GLFW_KEY_I && action == GLFW_PRESS)
		{
			mycam.i = 1;
		}
		if (key == GLFW_KEY_I && action == GLFW_RELEASE)
		{
			mycam.i = 0;
		}
		if (key == GLFW_KEY_K && action == GLFW_PRESS)
		{
			mycam.k = 1;
		}
		if (key == GLFW_KEY_K && action == GLFW_RELEASE)
		{
			mycam.k = 0;
		}
		if (key == GLFW_KEY_F && action == GLFW_PRESS)
		{
			hf_inc_press = 1;
		}
		if (key == GLFW_KEY_F && action == GLFW_RELEASE)
		{
			hf_inc_press = 0;
		}
		if (key == GLFW_KEY_V && action == GLFW_PRESS)
		{
			hf_dec_press = 1;
		}
		if (key == GLFW_KEY_V && action == GLFW_RELEASE)
		{
			hf_dec_press = 0;
		}
		if (key == GLFW_KEY_G && action == GLFW_PRESS)
		{
			a_inc_press = 1;
		}
		if (key == GLFW_KEY_G && action == GLFW_RELEASE)
		{
			a_inc_press = 0;
		}
		if (key == GLFW_KEY_B && action == GLFW_PRESS)
		{
			a_dec_press = 1;
		}
		if (key == GLFW_KEY_B && action == GLFW_RELEASE)
		{
			a_dec_press = 0;
		}
		if (key == GLFW_KEY_H && action == GLFW_PRESS)
		{
			h_oct += 1;
		}
		if (key == GLFW_KEY_N && action == GLFW_PRESS)
		{
			h_oct -= 1;
		}
		if (key == GLFW_KEY_E && action == GLFW_PRESS)
		{
			sun_rot_inc = 1;
		}
		if (key == GLFW_KEY_E && action == GLFW_RELEASE)
		{
			sun_rot_inc = 0;
		}
		if (key == GLFW_KEY_R && action == GLFW_PRESS)
		{
			sun_rot_dec = 1;
		}
		if (key == GLFW_KEY_R && action == GLFW_RELEASE)
		{
			sun_rot_dec = 0;
		}
		if (key == GLFW_KEY_Y && action == GLFW_RELEASE)
			{
			show_shadowmap = !show_shadowmap;
			}
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		//double posX, posY;
		//float newPt[2];
		//if (action == GLFW_PRESS)
		//{
		//	glfwGetCursorPos(window, &posX, &posY);
		//	std::cout << "Pos X " << posX <<  " Pos Y " << posY << std::endl;

		//	//change this to be the points converted to WORLD
		//	//THIS IS BROKEN< YOU GET TO FIX IT - yay!
		//	newPt[0] = 0;
		//	newPt[1] = 0;

		//	std::cout << "converted:" << newPt[0] << " " << newPt[1] << std::endl;
		//	glBindBuffer(GL_ARRAY_BUFFER, MeshPosID);
		//	//update the vertex array with the updated points
		//	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*6, sizeof(float)*2, newPt);
		//	glBindBuffer(GL_ARRAY_BUFFER, 0);
		//}
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}
#define MESHSIZE 150
	void init_mesh()
	{
		//generate the VAO
		glGenVertexArrays(1, &VAO_MESH_ID);
		glBindVertexArray(VAO_MESH_ID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &MeshPosID);
		glBindBuffer(GL_ARRAY_BUFFER, MeshPosID);
		vec3 *vertices =  new vec3[MESHSIZE * MESHSIZE * 4];
		for(int x=0;x<MESHSIZE;x++)
			for (int z = 0; z < MESHSIZE; z++)
				{
				vertices[x * 4 + z*MESHSIZE * 4 + 0] = vec3(0.0, 0.0, 0.0) + vec3(x, 0, z);
				vertices[x * 4 + z*MESHSIZE * 4 + 1] = vec3(1.0, 0.0, 0.0) + vec3(x, 0, z);
				vertices[x * 4 + z*MESHSIZE * 4 + 2] = vec3(1.0, 0.0, 1.0) + vec3(x, 0, z);
				vertices[x * 4 + z*MESHSIZE * 4 + 3] = vec3(0.0, 0.0, 1.0) + vec3(x, 0, z);
				}
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * MESHSIZE * MESHSIZE * 4, vertices, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//tex coords
		float t = 1. / 100;
		vec2 *tex = new vec2[MESHSIZE * MESHSIZE * 4];
		for (int x = 0; x<MESHSIZE; x++)
			for (int y = 0; y < MESHSIZE; y++)
			{
				tex[x * 4 + y*MESHSIZE * 4 + 0] = vec2(0.0, 0.0)+ vec2(x, y)*t;
				tex[x * 4 + y*MESHSIZE * 4 + 1] = vec2(t, 0.0)+ vec2(x, y)*t;
				tex[x * 4 + y*MESHSIZE * 4 + 2] = vec2(t, t)+ vec2(x, y)*t;
				tex[x * 4 + y*MESHSIZE * 4 + 3] = vec2(0.0, t)+ vec2(x, y)*t;
			}
		glGenBuffers(1, &MeshTexID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, MeshTexID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * MESHSIZE * MESHSIZE * 4, tex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &MeshIndexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MeshIndexBufferIDBox);
		int *elements = new int[MESHSIZE * MESHSIZE * 6];
		int ind = 0;
		for (int i = 0; i<MESHSIZE * MESHSIZE * 6; i+=6, ind+=4)
			{
			elements[i + 0] = ind + 0;
			elements[i + 1] = ind + 1;
			elements[i + 2] = ind + 2;
			elements[i + 3] = ind + 0;
			elements[i + 4] = ind + 2;
			elements[i + 5] = ind + 3;
			}			
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*MESHSIZE * MESHSIZE * 6, elements, GL_STATIC_DRAW);
		glBindVertexArray(0);
		delete[](vertices);
		delete[](tex);
		delete[](elements);
	}

	void init_trees()
	{
		string resourceDirectory = "../resources";
		shape_tree = make_shared<Shape>();
		shape_tree->loadMesh(resourceDirectory + "/tree/tree.obj");
		shape_tree->resize();
		shape_tree->init();
	}

	void init_sky()
	{
		string resourceDirectory = "../resources";
		shape_sky = make_shared<Shape>();
		shape_sky->loadMesh(resourceDirectory + "/sphere.obj");
		shape_sky->resize();
		shape_sky->init();
	}
	void init_sun()
	{
		/*string resourceDirectory = "../resources";
		shape_sun = make_shared<Shape>();
		shape_sun->loadMesh(resourceDirectory + "/sphere.obj");
		shape_sun->resize();
		shape_sun->init();*/

		glGenVertexArrays(1, &VAO_SUN_ID);
		glBindVertexArray(VAO_SUN_ID);

		glGenBuffers(1, &VBO_SPOS_ID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_SPOS_ID);

		GLfloat cube_vertices[] = {
			// front
			-1.0, -1.0,  1.0,//LD
			1.0, -1.0,  1.0,//RD
			1.0,  1.0,  1.0,//RU
			-1.0,  1.0,  1.0,//LU
		};
		/*for (int i = 0; i < 12; i++)
			cube_vertices[i] *= 0.5;*/
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		GLfloat cube_norm[] = {
			// front colors
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,

		};
		glGenBuffers(1, &VBO_SNOR_ID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_SNOR_ID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_norm), cube_norm, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glm::vec2 cube_tex[] = {
			// front colors
			glm::vec2(0.0, 1.0),
			glm::vec2(1.0, 1.0),
			glm::vec2(1.0, 0.0),
			glm::vec2(0.0, 0.0),

		};
		glGenBuffers(1, &VBO_STEX_ID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_STEX_ID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_tex), cube_tex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &VBO_SELE_ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_SELE_ID);
		GLushort cube_elements[] = {
			// front
			0, 1, 2,
			2, 3, 0,
		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);
	}

	void init_rectangle()
	{
		glGenVertexArrays(1, &VAO_RECT_ID);
		glBindVertexArray(VAO_RECT_ID);

		glGenBuffers(1, &VBO_RPOS_ID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_RPOS_ID);

		GLfloat* ver = new GLfloat[18];
		// front
		int verc = 0;

		ver[verc++] = -1.0, ver[verc++] = -1.0, ver[verc++] = 0.0;
		ver[verc++] = 1.0, ver[verc++] = -1.0, ver[verc++] = 0.0;
		ver[verc++] = -1.0, ver[verc++] = 1.0, ver[verc++] = 0.0;
		ver[verc++] = 1.0, ver[verc++] = -1.0, ver[verc++] = 0.0;
		ver[verc++] = 1.0, ver[verc++] = 1.0, ver[verc++] = 0.0;
		ver[verc++] = -1.0, ver[verc++] = 1.0, ver[verc++] = 0.0;


		glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), ver, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &VBO_RTEX_ID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_RTEX_ID);

		float t = 1. / 100.;
		GLfloat* cube_tex = new GLfloat[12];
		int texc = 0;

		cube_tex[texc++] = 0, cube_tex[texc++] = 0;
		cube_tex[texc++] = 1, cube_tex[texc++] = 0;
		cube_tex[texc++] = 0, cube_tex[texc++] = 1;
		cube_tex[texc++] = 1, cube_tex[texc++] = 0;
		cube_tex[texc++] = 1, cube_tex[texc++] = 1;
		cube_tex[texc++] = 0, cube_tex[texc++] = 1;

		glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), cube_tex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
		//initialize the net mesh
		init_mesh();
		init_trees();
		init_sky();
		init_sun();
		init_rectangle();

		primaryLight.position = vec3(0.0f, 100.0f, 1000.0f);
		primaryLight.direction = normalize(vec3(0,0,primaryLight.position.z) - primaryLight.position);
		primaryLight.color = vec3(1.0f, 1.0f, 1.0f);

		string resourceDirectory = "../resources" ;
		int width, height, channels;
		char filepath[1000];

		//texture 1
		string str = resourceDirectory + "/grass.jpg";
		strcpy(filepath, str.c_str());
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &GrassTex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GrassTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		//texture 2
		str = resourceDirectory + "/sand2.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &SandTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, SandTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//texture 3
		str = resourceDirectory + "/Leaves2.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &TreeTex);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, TreeTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//texture 4
		str = resourceDirectory + "/sky.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &SkyTex);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, SkyTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//texture 5
		str = resourceDirectory + "/sun_tex.png";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &SunTex);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, SunTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//[TWOTEXTURES]
		//set the 2 textures to the correct samplers in the fragment shader
		// Then bind the uniform samplers to texture units:
		glUseProgram(heightshader->pid);
		glUniform1i(glGetUniformLocation(heightshader->pid, "tex"), 0);
		glUniform1i(glGetUniformLocation(heightshader->pid, "tex2"), 1);
		glUniform1i(glGetUniformLocation(heightshader->pid, "shadowMapTex"), 2);

		glUseProgram(treeprog->pid);
		glUniform1i(glGetUniformLocation(treeprog->pid, "tex"), 0);
		glUniform1i(glGetUniformLocation(treeprog->pid, "shadow_tex"), 1);

		glUseProgram(skyprog->pid);
		glUniform1i(glGetUniformLocation(skyprog->pid, "tex"), 3);

		glUseProgram(sunprog->pid);
		glUniform1i(glGetUniformLocation(sunprog->pid, "tex"), 5);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		////FRAME BUFFER GEN
		
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		//RGBA8 2D texture, 24 bit depth texture, 256x256
		glGenTextures(1, &FBO_Tex);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, FBO_Tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
		glGenerateMipmap(GL_TEXTURE_2D);

		//Position texture
		glGenTextures(1, &FBO_PosTex);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, FBO_PosTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_BGRA, GL_FLOAT, NULL);
		glGenerateMipmap(GL_TEXTURE_2D);


		//-------------------------
		glGenFramebuffers(1, &FrameBufferObj);
		glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferObj);
		//Attach 2D texture to this FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBO_Tex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, FBO_PosTex, 0);
		//-------------------------
		glGenRenderbuffers(1, &depth_rb);
		glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
		//-------------------------
		//Attach depth buffer to FBO
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);
		//-------------------------
		//Does the GPU support current FBO configuration?
		GLenum status;
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		switch (status)
		{
		case GL_FRAMEBUFFER_COMPLETE:
			cout << "status framebuffer: good";
			break;
		default:
			cout << "status framebuffer: bad!!!!!!!!!!!!!!!!!!!!!!!!!";
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glUseProgram(prog_framebuffer->pid);
		glUniform1i(glGetUniformLocation(prog_framebuffer->pid, "tex"), 0);
		glUniform1i(glGetUniformLocation(prog_framebuffer->pid, "tex2"), 1);
		glUniform1i(glGetUniformLocation(prog_framebuffer->pid, "shadowMapTex"), 2);

		init_shadow_fb();
	}

	void init_shadow_fb()
	{
		glGenFramebuffers(1, &fb_shadowMap);
		glBindFramebuffer(GL_FRAMEBUFFER, fb_shadowMap);

		glGenTextures(1, &FBO_ShadowTex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, FBO_ShadowTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_DIM, SHADOW_DIM, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(vec3(1.0)));

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, FBO_ShadowTex, 0);

		// We don't want the draw result for a shadow map!
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		GLenum status;
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		switch (status)
		{
		case GL_FRAMEBUFFER_COMPLETE:
			cout << "status framebuffer: good" << std::endl;
			break;
		default:
			cout << "status framebuffer: bad!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		heightshader = std::make_shared<Program>();
		heightshader->setVerbose(true);
		heightshader->setShaderNames(resourceDirectory + "/height_vertex.glsl", resourceDirectory + "/height_frag.glsl", resourceDirectory + "/tesscontrol.glsl", resourceDirectory + "/tesseval.glsl");
		if (!heightshader->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		heightshader->addUniform("P");
		heightshader->addUniform("V");
		heightshader->addUniform("M");
		heightshader->addUniform("camoff");
		heightshader->addUniform("campos");
		heightshader->addUniform("sun_pos");
		heightshader->addUniform("h_freq");
		heightshader->addUniform("b_freq");
		heightshader->addUniform("b_pow");
		heightshader->addUniform("amplitude");
		heightshader->addUniform("lightSpace");
		heightshader->addUniform("lightpos");
		heightshader->addUniform("lightdir");
		heightshader->addUniform("h_oct");
		heightshader->addUniform("lightpos");
		heightshader->addUniform("lightdir");
		heightshader->addAttribute("vertPos");
		heightshader->addAttribute("vertTex");

		treeprog = std::make_shared<Program2>();
		treeprog->setVerbose(true);
		treeprog->setShaderNames(resourceDirectory + "/tree_vertex.glsl", resourceDirectory + "/tree_fragment.glsl");
		if (!treeprog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		treeprog->addUniform("P");
		treeprog->addUniform("V");
		treeprog->addUniform("M");
		treeprog->addUniform("campos");
		treeprog->addUniform("centerpos");
		treeprog->addUniform("int_campos");
		treeprog->addUniform("h_freq");
		treeprog->addUniform("b_freq");
		treeprog->addUniform("b_pow");
		treeprog->addUniform("amplitude");
		treeprog->addUniform("h_oct");
		treeprog->addUniform("lightSpace");
		treeprog->addAttribute("vertPos");
		treeprog->addAttribute("vertNor");
		treeprog->addAttribute("vertTex");
		//treeprog->addAttribute("InstancePos");

		skyprog = std::make_shared<Program2>();
		skyprog->setVerbose(true);
		skyprog->setShaderNames(resourceDirectory + "/sky_vertex.glsl", resourceDirectory + "/sky_fragment.glsl");
		if (!skyprog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		skyprog->addUniform("P");
		skyprog->addUniform("V");
		skyprog->addUniform("M");
		skyprog->addUniform("campos");
		skyprog->addAttribute("vertPos");
		skyprog->addAttribute("vertNor");
		skyprog->addAttribute("vertTex");

		sunprog = std::make_shared<Program2>();
		sunprog->setVerbose(true);
		sunprog->setShaderNames(resourceDirectory + "/sun_vertex.glsl", resourceDirectory + "/sun_fragment.glsl");
		if (!sunprog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		sunprog->addUniform("P");
		sunprog->addUniform("V");
		sunprog->addUniform("M");
		sunprog->addUniform("centerpos");
		sunprog->addAttribute("vertPos");
		sunprog->addAttribute("vertNor");
		sunprog->addAttribute("vertTex");

		// Initialize the Shadow Map shader program.
		shadowProg = make_shared<Program2>();
		shadowProg->setVerbose(true);
		shadowProg->setShaderNames(resourceDirectory + "/shadow_vert.glsl", resourceDirectory + "/shadow_frag.glsl");
		if (!shadowProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		shadowProg->init();
		shadowProg->addUniform("P");
		shadowProg->addUniform("V");
		shadowProg->addUniform("M");
		shadowProg->addUniform("h_freq");
		shadowProg->addUniform("b_freq");
		shadowProg->addUniform("b_pow");
		shadowProg->addUniform("amplitude");
		shadowProg->addUniform("h_oct");
		shadowProg->addUniform("centerpos");
		shadowProg->addUniform("render_terrain");
		shadowProg->addUniform("camoff");
		shadowProg->addAttribute("vertPos");
		shadowProg->addAttribute("vertNor");
		shadowProg->addAttribute("vertTex");

		prog_framebuffer = std::make_shared<Program2>();
		prog_framebuffer->setVerbose(true);
		prog_framebuffer->setShaderNames(resourceDirectory + "/FB_vert.glsl", resourceDirectory + "/FB_frag.glsl");
		if (!prog_framebuffer->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog_framebuffer->addUniform("sun_factor");
		prog_framebuffer->addAttribute("vertPos");
		prog_framebuffer->addAttribute("vertTex");
	}

	void get_light_proj_matrix(glm::mat4& lightP)
	{
		// If your scene goes outside these "bounds" (e.g. shadows stop working near boundary),
		// feel free to increase these numbers (or decrease if scene shrinks/light gets closer to
		// scene objects).
		const float left = -50.0f;
		const float right = 50.0f;
		const float bottom = -50.0f;
		const float top = 50.0f;
		const float zNear = .05f;
		const float zFar = 500.0f;

		lightP = glm::ortho(left, right, bottom, top, zNear, zFar);
	}

	void get_light_view_matrix(glm::mat4& lightV)
	{
		lightV = glm::lookAt(primaryLight.position, primaryLight.position + primaryLight.direction, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void updateLight()
	{
		primaryLight.position = vec3(-mycam.pos.x, 80 * clamp(abs(cos(sun_rot)), 0.1f, 1.f) , -mycam.pos.z + 120 * sin(sun_rot));
		primaryLight.direction = normalize(-vec3(0,80 * clamp(abs(cos(sun_rot)), 0.1f, 1.f), primaryLight.position.z + mycam.pos.z));
	}

	void check_procedural_factors()
	{
		if(hf_inc_press == 1){
			h_freq += 0.002;
			b_freq += 0.002;
		}
		else if (hf_dec_press == 1) {
			h_freq += -0.002;
			b_freq += -0.002;
		}
		if (a_inc_press == 1) {
			amplitude += 2;
		}
		else if (a_dec_press == 1) {
			amplitude -= 2;
		}
		if (sun_rot_inc == 1) {
			sun_rot += 0.03;
		}
		else if (sun_rot_dec == 1) {
			sun_rot -= 0.03;
		}
	}

	glm::vec3 getSunPosition()
	{
		glm::vec3 camp = -mycam.pos;
		glm::mat4 T_por = glm::translate(glm::mat4(1), glm::vec3(0, 0, -2));
		glm::mat4 T = glm::translate(glm::mat4(1), camp);
		glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.28f, 0.28f, 0.28f));
		float angle = (float)(((sin(sun_rot) + 1.f) / 2.f * 10.f * M_PI / 8.f) - M_PI / 8.f);
		glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 M = T * R * T_por * S;
		vec4 pos = M * vec4(0, 0, 0, 1);
		return vec3(0, 10, 10*pos.z);
	}

	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	mat4 VVV = mat4(1);
	void render_to_framebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferObj);

		GLuint attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, attachments);
		
		double frametime = get_last_elapsed_time();

		check_procedural_factors();
		
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		//glClearColor(0.8f, 0.8f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now		
		glm::mat4 V, M, P; //View, Model and Perspective matrix
		M = glm::mat4(1);
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones
		VVV = V = mycam.process(frametime);

		glDisable(GL_DEPTH_TEST);
		skyprog->bind();
		float sangle = 3.1415926 / 2.;
		glm::mat4 RotateXSky = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 camp = -mycam.pos;
		glm::mat4 TransSky = glm::translate(glm::mat4(1.0f), camp);
		glm::mat4 SSky = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));
		M = TransSky * RotateXSky * SSky;
		glUniformMatrix4fv(skyprog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(skyprog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(skyprog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape_sky->draw(skyprog, FALSE);
		skyprog->unbind();

		sunprog->bind();
		glBindVertexArray(VAO_SUN_ID);
		//sun_rot += 0.03;
		float sun_angle;
		glm::mat4 T_por = glm::translate(glm::mat4(1), glm::vec3(0, 0, -2));
		glm::mat4 T = glm::translate(glm::mat4(1), camp);
		glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.28f, 0.28f, 0.28f));
		sun_angle = (float)(((sin(sun_rot) + 1.f) / 2.f * M_PI));// 10.f * M_PI / 8.f) - M_PI / 8.f);
		glm::mat4 R = glm::rotate(glm::mat4(1.0f), sun_angle, glm::vec3(1.0f, 0.0f, 0.0f));
		M = T * R * T_por * S;
		glUniformMatrix4fv(sunprog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(sunprog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(sunprog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glm::vec4 sun_center = M * vec4(0, 0, 0, 1);
		//sun_pos = M * vec4(0.0f, 0.0f, 1.0f, 1.0f);
		glUniform3f(sunprog->getUniform("centerpos"),sun_center.x, sun_center.y, sun_center.z);
		//shape_sun->draw(sunprog, FALSE);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
		sunprog->unbind();
		glEnable(GL_DEPTH_TEST);

		
		heightshader->bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GrassTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, SandTex);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, FBO_ShadowTex);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glm::mat4 TransY = glm::translate(glm::mat4(1.0f), glm::vec3(-MESHSIZE/2.f, -3.0f, -MESHSIZE / 2.f));
		M = TransY;
		glUniformMatrix4fv(heightshader->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniformMatrix4fv(heightshader->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(heightshader->getUniform("V"), 1, GL_FALSE, &V[0][0]);

		glm::mat4 lightP, lightV, lightSpace;
		get_light_proj_matrix(lightP);
		get_light_view_matrix(lightV);
		lightSpace = lightP * lightV;
		glUniformMatrix4fv(heightshader->getUniform("lightSpace"), 1, GL_FALSE, &lightSpace[0][0]);

		glUniform3f(heightshader->getUniform("sun_pos"), sun_center.x, sun_center.y, -sin(sun_rot));
		
		vec3 offset = mycam.pos;
		offset.y = 0;
		offset.x = (int)offset.x;
		offset.z = (int)offset.z;
		glUniform3fv(heightshader->getUniform("camoff"), 1, &offset[0]);
		glUniform3fv(heightshader->getUniform("campos"), 1, &mycam.pos[0]);
		glUniform1f(heightshader->getUniform("h_freq"), h_freq);
		glUniform1f(heightshader->getUniform("b_freq"), b_freq);
		glUniform1f(heightshader->getUniform("b_pow"), b_pow);
		glUniform1f(heightshader->getUniform("amplitude"), amplitude);
		glUniform1i(heightshader->getUniform("h_oct"), h_oct);
		glUniform3fv(heightshader->getUniform("lightpos"), 1, &primaryLight.position[0]);
		glUniform3fv(heightshader->getUniform("lightdir"), 1, &primaryLight.direction[0]);
		glBindVertexArray(VAO_MESH_ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MeshIndexBufferIDBox);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GrassTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, SandTex);
		
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glDrawElements(GL_PATCHES, MESHSIZE*MESHSIZE*6, GL_UNSIGNED_INT, (void*)0);	
		heightshader->unbind();

		treeprog->bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TreeTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, FBO_ShadowTex);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindVertexArray(VAO_TREE_ID);
		glUniformMatrix4fv(treeprog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(treeprog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniform1f(treeprog->getUniform("h_freq"), h_freq);
		glUniform1f(treeprog->getUniform("b_freq"), b_freq);
		glUniform1f(treeprog->getUniform("b_pow"), b_pow);
		glUniform1f(treeprog->getUniform("amplitude"), amplitude);
		glUniform1i(treeprog->getUniform("h_oct"), h_oct);
		glUniformMatrix4fv(treeprog->getUniform("lightSpace"), 1, GL_FALSE, &lightSpace[0][0]);

		int cam_int_x = ((int)(-mycam.pos.x / 10.f)) * 10;
		int cam_int_z = ((int)(-mycam.pos.z / 10.f)) * 10;

		S = glm::scale(glm::mat4(1), glm::vec3(1.0f, 1.0f, 1.0f));
		glUniform3fv(treeprog->getUniform("campos"), 1, &mycam.pos[0]);

		for (int x = -5; x <= 5; x++){
			for (int z = -5; z <= 5; z++){

				glm::vec3 centerpos = vec3(cam_int_x + 10 * x, -2.3f, cam_int_z + 10 * z);

				glm::mat4 T = glm::translate(glm::mat4(1.0f), centerpos);
				M = T * S;

				glUniform3f(treeprog->getUniform("centerpos"), centerpos.x, -3.0f, centerpos.z);
				glUniformMatrix4fv(treeprog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
				shape_tree->draw(treeprog, FALSE);
			}
		}
		treeprog->unbind();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void render_to_screen()
	{
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width / (float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		show_shadowmap ? glBindTexture(GL_TEXTURE_2D, FBO_ShadowTex) : glBindTexture(GL_TEXTURE_2D, FBO_Tex);
		glGenerateMipmap(GL_TEXTURE_2D);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		prog_framebuffer->bind();
		glBindVertexArray(VAO_RECT_ID);
		glUniform1f(prog_framebuffer->getUniform("sun_factor"), sin(sun_rot));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		prog_framebuffer->unbind();
	}
	
	void render_to_shadowmap()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fb_shadowMap);
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, SHADOW_DIM, SHADOW_DIM);

		glDisable(GL_BLEND);

		glm::mat4 M, V, S, T, P;

		updateLight();

		// Orthographic frustum in light space; encloses the scene, adjust if larger or smaller scene used.
		get_light_proj_matrix(P);

		// "Camera" for rendering shadow map is at light source, looking at the scene.
		get_light_view_matrix(V);

		// Bind shadow map shader program and matrix uniforms.
		shadowProg->bind();
		glUniformMatrix4fv(shadowProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(shadowProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniform1f(shadowProg->getUniform("h_freq"), h_freq);
		glUniform1f(shadowProg->getUniform("b_freq"), b_freq);
		glUniform1f(shadowProg->getUniform("b_pow"), b_pow);
		glUniform1f(shadowProg->getUniform("amplitude"), amplitude);
		glUniform1i(shadowProg->getUniform("h_oct"), h_oct);
		glUniform1i(shadowProg->getUniform("render_terrain"), 0);
		//glUniformMatrix4fv(shadowProg->getUniform("M"), 1, GL_FALSE, &M_Earth[0][0]);

		int cam_int_x = ((int)(-mycam.pos.x / 10.f)) * 10;
		int cam_int_z = ((int)(-mycam.pos.z / 10.f)) * 10;
		S = glm::scale(glm::mat4(1), glm::vec3(1.0f, 1.0f, 1.0f));

		for (int x = -5; x <= 5; x++) {
			for (int z = -5; z <= 5; z++) {

				glm::vec3 centerpos = vec3(cam_int_x + 10 * x, -2.3f, cam_int_z + 10 * z);

				glm::mat4 T = glm::translate(glm::mat4(1.0f), centerpos);
				M = T * S;

				glUniform3f(shadowProg->getUniform("centerpos"), centerpos.x, -3.0f, centerpos.z);
				glUniformMatrix4fv(shadowProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);

				assert(glGetError() == GL_NO_ERROR);

				shape_tree->draw(shadowProg, FALSE);
				//glDrawElements(GL_TRIANGLES, tree_vert_count, GL_UNSIGNED_INT, (void*)0);
			}
		}


		glBindVertexArray(VAO_MESH_ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MeshIndexBufferIDBox);
		glUniform1i(shadowProg->getUniform("render_terrain"), 1);
		glm::mat4 TransY = glm::translate(glm::mat4(1.0f), glm::vec3(-MESHSIZE / 2.f, -3.0f, -MESHSIZE / 2.f));
		M = TransY;
		glUniformMatrix4fv(shadowProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		vec3 offset = mycam.pos;
		offset.y = 0;
		offset.x = (int)offset.x;
		offset.z = (int)offset.z;
		glUniform3fv(shadowProg->getUniform("camoff"), 1, &offset[0]);
		//glPatchParameteri(GL_PATCH_VERTICES, 3);
		glDrawElements(GL_TRIANGLES, MESHSIZE * MESHSIZE * 6, GL_UNSIGNED_INT, (void*)0);

		//done, unbind stuff
		shadowProg->unbind();
		glEnable(GL_BLEND);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, FBO_ShadowTex);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

};
//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	srand(0);

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render_to_shadowmap();
		application->render_to_framebuffer();
		application->render_to_screen();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
