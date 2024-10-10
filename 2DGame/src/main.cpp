#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <bits/stdc++.h>

#include "../include/text.h"

#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window,unsigned int ShaderProg);

// settings
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <bits/stdc++.h>

#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

unsigned int initShaderProgram()
{
	int load_success;
	char infoLog[512];
	std::string temp; // for extracting from file
	std::string vertexShaderSource = "";
	std::string fragShaderSource = "";

	// for reading through file for vertex shader
	std::ifstream myfile;
	myfile.open("<abs_path>/src/textrender/vertex.shader");
	if (myfile.is_open())
	{
		while (myfile)
		{
			getline(myfile, temp);
			if (!myfile)
				break;
			temp = temp.c_str();
			vertexShaderSource += temp + "\n";
		}
	}
	else
	{
		std::cout << "ERROR::FILE::NOT::OPENING\n"
				  << infoLog << std::endl;
	}
	myfile.close();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const GLchar *vertsh = vertexShaderSource.c_str();
	glShaderSource(vertexShader, 1, &vertsh, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &load_success);
	if (!load_success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
		glfwTerminate();
	}

	// check for compile errors

	// for reading through file for fragment shader
	myfile.open("<abs_path>/src/textrender/fragment.shader");
	if (myfile.is_open())
	{
		while (myfile)
		{
			getline(myfile, temp);
			if (!myfile)
				break;
			temp = temp.c_str();
			fragShaderSource += temp + "\n";
		}
	}
	else
	{
		std::cout << "ERROR::FILE::NOT::OPENING\n"
				  << infoLog << std::endl;
		glfwTerminate();
	}
	myfile.close();

	unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar *fragsh = fragShaderSource.c_str();
	glShaderSource(fragShader, 1, &fragsh, NULL);
	glCompileShader(fragShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &load_success);
	if (!load_success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
		glfwTerminate();
	}
	// check for compile errors

	// link shaders
	unsigned int program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	glGetShaderiv(vertexShader, GL_LINK_STATUS, &load_success);
	if (!load_success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				  << infoLog << std::endl;
		glfwTerminate();
	}
	// checking for linking errors

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

	return program;
}

glm::mat4 trans;
glm::mat4 avatar_pos = glm::mat4(1.0f);

glm::vec3 curr_pos = glm::vec3(-0.8f, -0.75f, 0.2f);

unsigned int timer = 0;

#define obs_count 100

float coin_sets[obs_count][256];

int collision_Detection(map<int, vector<float>> &mp, int vert, float dist)
{
	for (int i = 0; i < vert - 31; i += 32)
	{
		if (mp[i / 32][0] >= curr_pos.x-0.05f && mp[i / 32][1] <= curr_pos.x+0.05f)
		{
			if (mp[i / 32][2] >= curr_pos.y-0.05f && mp[i / 32][3] <= curr_pos.y+0.05f)
			{
				return 1;
			}
		}
	}
	return 0;
}

bool check_detect[obs_count][8] = {0};

int collision_Detection_for_coins(int vert, float dist)
{
	int calc = 0;
	for (int j = 0; j < vert; j++)
	{
		for (int i = 0; i < 256 - 31; i += 32)
		{
			if (coin_sets[j][i] + dist <= curr_pos.x + 0.2f && coin_sets[j][i + 8] + dist >= curr_pos.x - 0.2f)
			{
				if (coin_sets[j][i + 1] <= curr_pos.y + 0.15f && coin_sets[j][i + 17] >= curr_pos.y - 0.15f)
				{
					if (check_detect[j][i / 32] == 0)
					{
						check_detect[j][i / 32] = 1;
						calc++;
					}
				}
			}
		}
	}

	return calc;
}

unsigned int coins_ind[] = {
	0,
	1,
	2,
	1,
	2,
	3,

	4,
	5,
	6,
	5,
	6,
	7,

	8,
	9,
	10,
	9,
	10,
	11,

	12,
	13,
	14,
	13,
	14,
	15,

	16,
	17,
	18,
	17,
	18,
	19,

	20,
	21,
	22,
	21,
	22,
	23,

	24,
	25,
	26,
	25,
	26,
	27,

	28,
	29,
	30,
	29,
	30,
	31,
};

void set_Coins(float *obstacles, int tot_vert)
{
	int itr = 0;
	for (int i = 0; i < tot_vert - 31; i += 32)
	{
		float starter = 0.6f;
		int count = 0;
		float x_mid = (obstacles[i] + obstacles[i + 8]) / 2;
		float y_mid = (obstacles[i + 1] + obstacles[i + 17]) / 2 + 0.04f;
		for (int j = 0; j < 256 - 31; j += 32)
		{
			if (count == 4)
				starter = 0.6f;
			if (count < 4)
			{
				coin_sets[itr][j] = x_mid + starter;
				coin_sets[itr][j + 8] = x_mid - 0.1f + starter;
				coin_sets[itr][j + 16] = x_mid + starter;
				coin_sets[itr][j + 24] = x_mid - 0.1f + starter;
				coin_sets[itr][j + 1] = y_mid + 0.1f;
				coin_sets[itr][j + 9] = y_mid + 0.1f;
				coin_sets[itr][j + 17] = y_mid;
				coin_sets[itr][j + 25] = y_mid;
			}
			else
			{
				coin_sets[itr][j] = x_mid + starter;
				coin_sets[itr][j + 8] = x_mid - 0.1f + starter;
				coin_sets[itr][j + 16] = x_mid + starter;
				coin_sets[itr][j + 24] = x_mid - 0.1f + starter;
				coin_sets[itr][j + 1] = y_mid - 0.1f;
				coin_sets[itr][j + 9] = y_mid - 0.1f;
				coin_sets[itr][j + 17] = y_mid - 0.2f;
				coin_sets[itr][j + 25] = y_mid - 0.2f;
			}
			coin_sets[itr][j + 2] = 0.2f;
			coin_sets[itr][j + 10] = 0.2f;
			coin_sets[itr][j + 18] = 0.2f;
			coin_sets[itr][j + 26] = 0.2f;
			coin_sets[itr][j + 3] = 0.0f;
			coin_sets[itr][j + 4] = 0.0f;
			coin_sets[itr][j + 5] = 0.0f;
			coin_sets[itr][j + 11] = 0.0f;
			coin_sets[itr][j + 12] = 0.0f;
			coin_sets[itr][j + 13] = 0.0f;
			coin_sets[itr][j + 19] = 0.0f;
			coin_sets[itr][j + 20] = 0.0f;
			coin_sets[itr][j + 21] = 0.0f;
			coin_sets[itr][j + 27] = 0.0f;
			coin_sets[itr][j + 28] = 0.0f;
			coin_sets[itr][j + 29] = 0.0f;
			coin_sets[itr][j + 6] = 0.0f;
			coin_sets[itr][j + 7] = 0.0f;
			coin_sets[itr][j + 14] = 1.0f;
			coin_sets[itr][j + 15] = 0.0f;
			coin_sets[itr][j + 22] = 0.0f;
			coin_sets[itr][j + 23] = 1.0f;
			coin_sets[itr][j + 30] = 1.0f;
			coin_sets[itr][j + 31] = 1.0f;
			count++;
			starter += 0.2f;
		}
		itr++;
	}
}

int release=0;
// for avatar smoothstep 

unsigned int texture[7];
unsigned int VBO[4], VAO[4], EBO[4];

// if(text.isError())

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GAME", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	float bg_image_1[] = {
		// vertices       //color          //texture
		-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f};

	float bg_image_2[] = {
		// vertices       //color          //texture
		3.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		3.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f};

	float avatar[] = {
		// vertices       //color          //texture
		-0.9f, -0.9f, 0.2f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.9f, -0.6f, 0.2f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.7f, -0.6f, 0.2f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.7f, -0.9f, 0.2f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f};

	int n_vertices = ((int)(obs_count)) * 32;
	float obstacles[n_vertices]; // 6 obs
	int itr = 0;
	float start_pos = -1.0f;
	float rand_num;
	int tog = 0, rang = 0;
	for (int i = 0; i < n_vertices - 7; i += 8)
	{
		itr = i;
		if (itr % 32 == 0)
		{
			rand_num = abs(abs(sin(rand() * rand()))+0.7f)/2;
			// cout<<start_pos <<" "<<rand_num<<endl;
		}
		if (i % 32 == 8 || i % 32 == 24)
		{
			obstacles[i] = start_pos + rand_num;
		}
		else
		{
			obstacles[i] = start_pos + rand_num + 0.2f;
		}

		if (tog == 0)
		{
			if (i % 32 < 16)
				obstacles[i + 1] = 1.0f;
			else
				obstacles[i + 1] = 0.3f;
			// tog = !tog;
		}
		else
		{
			if (i % 32 < 16)
				obstacles[i + 1] = -0.3f;
			else
				obstacles[i + 1] = -1.0f;
			// tog = !tog;
		}

		if (i % 32 == 24)
		{
			start_pos += 1.2f;
			tog = !tog;
		}

		obstacles[i + 2] = 0.2f;
		if (rang % 3 == 0)
		{
			obstacles[i + 3] = 0.0f;
			obstacles[i + 4] = 0.0f;
			obstacles[i + 5] = 0.0f;
		}
		if (rang % 3 == 1)
		{
			obstacles[i + 3] = 0.0f;
			obstacles[i + 4] = 0.0f;
			obstacles[i + 5] = 0.0f;
		}

		if (rang % 3 == 2)
		{
			obstacles[i + 3] = 0.0f;
			obstacles[i + 4] = 0.0f;
			obstacles[i + 5] = 0.0f;
		}

		rang++;

		if (i % 32 == 0)
		{
			obstacles[i + 6] = 0.0f;
			obstacles[i + 7] = 0.0f;
		}
		if (i % 32 == 8)
		{
			obstacles[i + 6] = 0.0f;
			obstacles[i + 7] = 1.0f;
		}
		if (i % 32 == 16)
		{
			obstacles[i + 6] = 1.0f;
			obstacles[i + 7] = 0.0f;
		}
		if (i % 32 == 24)
		{
			obstacles[i + 6] = 1.0f;
			obstacles[i + 7] = 1.0f;
		}
	}

	set_Coins(obstacles, n_vertices);

	int itr1 = 0;

	unsigned int ind_obs[(n_vertices / 32) * 6];

	for (int i = 0; i < (n_vertices / 32) * 6 - 5; i += 6)
	{
		ind_obs[i] = itr1;
		ind_obs[i + 1] = itr1 + 1;
		ind_obs[i + 2] = itr1 + 2;
		ind_obs[i + 3] = itr1 + 1;
		ind_obs[i + 4] = itr1 + 3;
		ind_obs[i + 5] = itr1 + 2;
		itr1 += 4;
	}

	unsigned int ind_bg[] = {
		0, 1, 2,
		0, 3, 2};

	glGenVertexArrays(1, &VAO[0]);
	glGenBuffers(1, &VBO[0]);
	glGenBuffers(1, &EBO[0]);

	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bg_image_1), bg_image_1, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ind_bg), ind_bg, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// for coins
	unsigned int coins[(n_vertices / 32)], EB, VB;

	for (int i = 0; i < (n_vertices / 32); i++)
	{
		glGenVertexArrays(1, &coins[i]);
		glGenBuffers(1, &VB);
		glGenBuffers(1, &EB);

		glBindVertexArray(coins[i]);

		glBindBuffer(GL_ARRAY_BUFFER, VB);
		glBufferData(GL_ARRAY_BUFFER, sizeof(coin_sets[i]), coin_sets[i], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EB);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(coins_ind), coins_ind, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	// for bg2

	glGenVertexArrays(1, &VAO[1]);
	glGenBuffers(1, &VBO[1]);
	glGenBuffers(1, &EBO[1]);

	glBindVertexArray(VAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bg_image_2), bg_image_2, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ind_bg), ind_bg, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// for avatar

	glGenVertexArrays(1, &VAO[2]);
	glGenBuffers(1, &VBO[2]);
	glGenBuffers(1, &EBO[2]);

	glBindVertexArray(VAO[2]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(avatar), avatar, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ind_bg), ind_bg, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// for obstacles
	glGenVertexArrays(1, &VAO[3]);
	glGenBuffers(1, &VBO[3]);
	glGenBuffers(1, &EBO[3]);

	glBindVertexArray(VAO[3]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(obstacles), obstacles, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ind_obs), ind_obs, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	float bg_dist1 = 0.0f, bg_dist2 = 0.0f;
	float bg_dist3 = 0.0f;

	// load and create a texture
	// -------------------------
	// for bg1
	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;

	unsigned char *bg1 = stbi_load("<abs_path>/src/images/bg1.jpg", &width, &height, &nrChannels, 0);
	if (bg1)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bg1);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	// stbi_image_free(bg1);

	// for bg2
	glGenTextures(1, &texture[1]);
	glBindTexture(GL_TEXTURE_2D, texture[1]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps

	unsigned char *bg2 = stbi_load("<abs_path>/src/images/bg2.jpg", &width, &height, &nrChannels, 0);
	if (bg2)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bg2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	// stbi_image_free(bg2);

	// for avatar
	glGenTextures(1, &texture[2]);
	glBindTexture(GL_TEXTURE_2D, texture[2]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps

	unsigned char *avat = stbi_load("<abs_path>/src/images/av.jpg", &width, &height, &nrChannels, 0);
	if (avat)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, avat);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	// stbi_image_free(avatar);

	// for obstacles
	glGenTextures(1, &texture[3]);
	glBindTexture(GL_TEXTURE_2D, texture[3]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps

	unsigned char *obs = stbi_load("<abs_path>/src/images/obs.jpeg", &width, &height, &nrChannels, 0);
	if (obs)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, obs);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	// stbi_image_free(obs);

	// for coins
	glGenTextures(1, &texture[4]);
	glBindTexture(GL_TEXTURE_2D, texture[4]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps

	unsigned char *obs1 = stbi_load("<abs_path>/src/images/coin.jpg", &width, &height, &nrChannels, 0);
	if (obs1)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, obs1);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	// stbi_image_free(obs1);

	// for l3
	glGenTextures(1, &texture[5]);
	glBindTexture(GL_TEXTURE_2D, texture[5]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps

	unsigned char *l3 = stbi_load("<abs_path>/src/images/bg3-min.jpg", &width, &height, &nrChannels, 0);
	if (l3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, l3);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	unsigned int shaderProgram = initShaderProgram();

	// render loop
	// -----------

	// for text
	// OpenGL state
	// ------------
	Text text;
	if (text.isError())
	{
		cout << "Failed to load Text" << endl;
		return 0;
	}

	// compile and setup the shader
	// ----------------------------

	float speed_movement = 0.01f;

	int final_ans = 0;
	int flag = 0;
	float ob_mov = 0.0f;

	map<int, vector<float>> mp;


	while (!glfwWindowShouldClose(window))
	{
		// glUseProgram();
		processInput(window,shaderProgram);

		glUseProgram(shaderProgram);

		// text.render("Score : ", 3, 460);
		// cout<<flag<<" "<<"THIS IS FLAG ACTIVATE"<<endl;
		if (flag == 1 || flag == 2)
		{
			glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			if (flag == 2)
				text.render("YOU LOST !!!", 250, 500);
			else
				text.render("YOU WON !!!", 250, 500);

			text.render("GAME OVER", 250, 400);
			text.render("Your Score : ", 150, 300);
			text.render(to_string(final_ans), 550, 300);
		}
		else
		{
			// render
			// ------
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// bind Texture
			glUniform1i(glGetUniformLocation(shaderProgram, "isZapper"),0);

			if (curr_pos.x + (-bg_dist3) <= 25.0f)
			{
				trans = glm::translate(glm::mat4(1.0f), glm::vec3(bg_dist1, 0.0f, 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "trans"), 1, GL_FALSE, glm::value_ptr(trans));

				glBindTexture(GL_TEXTURE_2D, texture[0]);
				glBindVertexArray(VAO[0]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

				glBindTexture(GL_TEXTURE_2D, texture[0]);

				trans = glm::translate(glm::mat4(1.0f), glm::vec3(bg_dist2, 0.0f, 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "trans"), 1, GL_FALSE, glm::value_ptr(trans));
				glBindVertexArray(VAO[1]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
			else if (curr_pos.x + (-bg_dist3) <= 50.0f)
			{
				trans = glm::translate(glm::mat4(1.0f), glm::vec3(bg_dist1, 0.0f, 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "trans"), 1, GL_FALSE, glm::value_ptr(trans));

				glBindTexture(GL_TEXTURE_2D, texture[1]);
				glBindVertexArray(VAO[0]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

				glBindTexture(GL_TEXTURE_2D, texture[1]);

				trans = glm::translate(glm::mat4(1.0f), glm::vec3(bg_dist2, 0.0f, 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "trans"), 1, GL_FALSE, glm::value_ptr(trans));
				glBindVertexArray(VAO[1]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
			else
			{
				trans = glm::translate(glm::mat4(1.0f), glm::vec3(bg_dist1, 0.0f, 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "trans"), 1, GL_FALSE, glm::value_ptr(trans));

				glBindTexture(GL_TEXTURE_2D, texture[5]);
				glBindVertexArray(VAO[0]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

				glBindTexture(GL_TEXTURE_2D, texture[5]);

				trans = glm::translate(glm::mat4(1.0f), glm::vec3(bg_dist2, 0.0f, 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "trans"), 1, GL_FALSE, glm::value_ptr(trans));
				glBindVertexArray(VAO[1]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}



			long long int obs_off = 0;
			for (int i = 0; i < (n_vertices); i += 32)
			{
			glUniform1i(glGetUniformLocation(shaderProgram, "isZapper"),1);

				glBindTexture(GL_TEXTURE_2D, texture[3]);
				trans = glm::translate(glm::mat4(1.0f), glm::vec3(bg_dist3, 0.0f, 0.0f));

				if ((i / 32) % 5 == 0)
				{
					if (obstacles[i + 1] > 0 && obstacles[i + 1] - ob_mov >= -1.2f)
					{
						trans = glm::translate(glm::mat4(1.0f), glm::vec3(bg_dist3, -ob_mov, 0.0f));
						if (mp[i / 32].size() == 0)
						{
							mp[i / 32].push_back(obstacles[i] + bg_dist3);
							mp[i / 32].push_back(obstacles[i + 8] + bg_dist3);
							mp[i / 32].push_back(obstacles[i + 1] - ob_mov);
							mp[i / 32].push_back(obstacles[i + 17] - ob_mov);
						}
						else{
							mp[i / 32][0] = obstacles[i] + bg_dist3;
							mp[i / 32][1] = obstacles[i+8] + bg_dist3;
							mp[i / 32][2] = obstacles[i+1] -ob_mov;
							mp[i / 32][3] = obstacles[i+17] -ob_mov;
						}
					}
					else if (obstacles[i + 1] > 0 && obstacles[i + 1] - ob_mov <= 1.2f)
					{

						trans = glm::translate(glm::mat4(1.0f), glm::vec3(bg_dist3, ob_mov, 0.0f));
						if (mp[i / 32].size() == 0)
						{
							mp[i / 32].push_back(obstacles[i] + bg_dist3);
							mp[i / 32].push_back(obstacles[i + 8] + bg_dist3);
							mp[i / 32].push_back(obstacles[i + 1] + ob_mov);
							mp[i / 32].push_back(obstacles[i + 17] + ob_mov);
						}
						else{
							mp[i / 32][0] = obstacles[i] + bg_dist3;
							mp[i / 32][1] = obstacles[i+8] + bg_dist3;
							mp[i / 32][2] = obstacles[i+1] +ob_mov;
							mp[i / 32][3] = obstacles[i+17] +ob_mov;
						}
					}
					else if (obstacles[i + 1] + ob_mov <= 1.2f)
					{

						trans = glm::translate(glm::mat4(1.0f), glm::vec3(bg_dist3, ob_mov, 0.0f));
						if (mp[i / 32].size() == 0)
						{
							mp[i / 32].push_back(obstacles[i] + bg_dist3);
							mp[i / 32].push_back(obstacles[i + 8] + bg_dist3);
							mp[i / 32].push_back(obstacles[i + 1] + ob_mov);
							mp[i / 32].push_back(obstacles[i + 17] + ob_mov);
						}
						else{
							mp[i / 32][0] = obstacles[i] + bg_dist3;
							mp[i / 32][1] = obstacles[i+8] + bg_dist3;
							mp[i / 32][2] = obstacles[i+1] +ob_mov;
							mp[i / 32][3] = obstacles[i+17] +ob_mov;
						}
					}
					else
					{
						trans = glm::translate(glm::mat4(1.0f), glm::vec3(bg_dist3, -ob_mov, 0.0f));
						if (mp[i / 32].size() == 0)
						{
							mp[i / 32].push_back(obstacles[i] + bg_dist3);
							mp[i / 32].push_back(obstacles[i + 8] + bg_dist3);
							mp[i / 32].push_back(obstacles[i + 1] - ob_mov);
							mp[i / 32].push_back(obstacles[i + 17] - ob_mov);
						}
						else{
							mp[i / 32][0] = obstacles[i] + bg_dist3;
							mp[i / 32][1] = obstacles[i+8] + bg_dist3;
							mp[i / 32][2] = obstacles[i+1] -ob_mov;
							mp[i / 32][3] = obstacles[i+17] -ob_mov;
						}
					}
				}
				else
				{
					if (mp[i / 32].size() == 0)
						{
							mp[i / 32].push_back(obstacles[i] + bg_dist3) ;
							mp[i / 32].push_back(obstacles[i + 8] + bg_dist3);
							mp[i / 32].push_back(obstacles[i + 1]);
							mp[i / 32].push_back(obstacles[i + 17]);
						}
						else{
							mp[i / 32][0] = obstacles[i] + bg_dist3 ;
							mp[i / 32][1] = obstacles[i+8] + bg_dist3;
							mp[i / 32][2] = obstacles[i+1] ;
							mp[i / 32][3] = obstacles[i+17] ;
						}
				}

				// trans = glm::translate(glm::mat4(1.0f), glm::vec3(bg_dist3, 0.0f, 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "trans"), 1, GL_FALSE, glm::value_ptr(trans));
				glBindVertexArray(VAO[3]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)obs_off);
				obs_off += 6 * (sizeof(unsigned int));
			}

			int out = collision_Detection(mp, n_vertices, bg_dist3);

			// mp.clear();
			if (out == 1)
			{
				cout << "GAME OVER" << endl;
				flag = 2;
			}

			ob_mov += 0.002f;
			if (ob_mov >= 2.5f)
				ob_mov = 0.0f;

			int num = collision_Detection_for_coins((n_vertices / 32), bg_dist3);

			for (int i = 0; i < (n_vertices / 32); i++)
			{
				glBindTexture(GL_TEXTURE_2D, texture[4]);
				trans = glm::translate(glm::mat4(1.0f), glm::vec3(bg_dist3, 0.0f, 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "trans"), 1, GL_FALSE, glm::value_ptr(trans));
			glUniform1i(glGetUniformLocation(shaderProgram, "isZapper"),0);

				glBindVertexArray(coins[i]);
				int off_Set = 0;
				for (int j = 0; j < 256 - 31; j += 32)
				{
					if (check_detect[i][j / 32] == 0)
					{
						glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)((long long int)off_Set));
					}

					off_Set += 6 * (sizeof(unsigned int));
				}
			}

			if(release==1)
				glUniform1i(glGetUniformLocation(shaderProgram, "isZapper"),2);
	
			glBindTexture(GL_TEXTURE_2D, texture[2]);
			trans = avatar_pos;
			if (curr_pos.y - 0.00049f * (timer) >= -0.8f)
			{
				glm::mat4 temp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.00049f * (timer), 0.0f));
				avatar_pos = temp * avatar_pos;
				curr_pos -= glm::vec3(0.0f, 0.00049f * (timer), 0.0f);
				timer++;
			}
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "trans"), 1, GL_FALSE, glm::value_ptr(trans));
			glBindVertexArray(VAO[2]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			// break;

			final_ans += num;
			bg_dist1 -= speed_movement;
			bg_dist2 -= speed_movement;
			bg_dist3 -= speed_movement;

			if (bg_dist1 <= -2.0f && bg_dist1 >= -2.00f - speed_movement)
			{
				bg_dist1 = 2.0f;
			}
			if (bg_dist2 <= -4.0f && bg_dist2 >= -4.00f - speed_movement)
			{
				bg_dist2 = 0.0f;
			}

			text.render("Desi Joyride", 3, 560);

			if (curr_pos.x + (-bg_dist3) >= 75.0f)
			{
				text.render("Score : ", 3, 460);
				flag = 1;
				continue;
			}

			if (curr_pos.x + (-bg_dist3) <= 25.0f)
			{
				text.render("Level : 1", 3, 510);
			}
			else if (curr_pos.x + (-bg_dist3) <= 50.0f)
			{
				text.render("Level : 2", 3, 510);
				speed_movement = 0.012f;
			}
			else
			{
				text.render("Level : 3", 3, 510);
				speed_movement = 0.014f;
			}
			text.render("Score : ", 3, 460);
			text.render(to_string(final_ans), 220, 460);

			text.render("Distance : ", 3, 410);
			text.render(to_string(((float)(round(-bg_dist3 * 100)) / 100)), 310, 410);
		}


		glfwSwapBuffers(window);
		glfwPollEvents();

		release=1;
	}

	glDeleteVertexArrays(1, &VAO[0]);
	glDeleteBuffers(1, &VBO[0]);
	glDeleteBuffers(1, &EBO[0]);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window,unsigned int ShaderProg)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		release=0;
		float up_speed = 0.02f; // for avatar
		timer = 0;
		if (curr_pos.y + up_speed <= 0.8f)
		{
			glm::mat4 temp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, up_speed, 0.0f));
			avatar_pos = temp * avatar_pos;
			curr_pos += glm::vec3(0.0f, up_speed, 0.0f);
		}

		// glUniform1i(glGetUniformLocation(ShaderProg,"isZapper"),2);
		glBindVertexArray(VAO[2]);
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}