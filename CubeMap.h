#pragma once
#include<GL/glew.h>     // use this before GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>
#include "Shader.h"

using namespace std;
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

unsigned int load_texture()
{
	vector<std::string> faces
	{
		"C:\\Users\\Skumar3\\source\\repos\\gameengineoreon\\skybox\\assets\\right.jpg",
		"C:\\Users\\Skumar3\\source\\repos\\gameengineoreon\\skybox\\assets\\left.jpg",
		"C:\\Users\\Skumar3\\source\\repos\\gameengineoreon\\skybox\\assets\\top.jpg",
		"C:\\Users\\Skumar3\\source\\repos\\gameengineoreon\\skybox\\assets\\bottom.jpg",
		"C:\\Users\\Skumar3\\source\\repos\\gameengineoreon\\skybox\\assets\\front.jpg",
		"C:\\Users\\Skumar3\\source\\repos\\gameengineoreon\\skybox\\assets\\back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);
	return cubemapTexture;
}

GLuint create_program_CM()
{
	GLuint program = glCreateProgram();
	shaderAttach(program, GL_VERTEX_SHADER, vertexShaderSrcSB);
	shaderAttach(program, GL_FRAGMENT_SHADER, fragmentShaderSrcSB);
	glLinkProgram(program);
	glValidateProgram(program);
	return program;
}


void bind_vert_n_texel_CM(GLuint program)
{
	GLuint posAttrib = glGetAttribLocation(program, "pos");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
}

struct CubeMap
{
public:
	GLuint m_g;
	GLuint program;
	std::vector<glm::mat4x3> m_faces;
	float m_c[108];
	GLuint m_texId;
	glm::mat4 m_ModelMatrix;


	CubeMap(float l)
	{
		m_ModelMatrix = glm::mat4(1.0f);
		setData(l);
		m_bind_buffer();
		program = create_program_CM();
		m_texId = load_texture();
	}

	~CubeMap()
	{
		glDeleteProgram(program);
		glDeleteVertexArrays(1, &m_g);
	}

	void bind_static_attribs()
	{
		glUseProgram(program);
		bind_vert_n_texel_CM(program);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_texId);
	}

	void m_bind_buffer()
	{
		glGenBuffers(1, &m_g);
		glBindBuffer(GL_ARRAY_BUFFER, this->m_g);
		glBufferData(GL_ARRAY_BUFFER, sizeof(this->m_c), this->m_c, GL_STATIC_DRAW);
	}


	void draw(Camera camera)
	{
		glBindBuffer(GL_ARRAY_BUFFER, this->m_g);
		bind_static_attribs();

		glUseProgram(program);

		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));  
		GLint viewLoc = glGetUniformLocation(program, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 pro = glm::mat4(1.0f);
		pro = glm::perspective(glm::radians(camera.Zoom), 1280.0f / 720.0f, 0.01f, 100.0f);
		GLint projectionLoc = glGetUniformLocation(program, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(pro));

		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		glUseProgram(0);

	}

	void setData(float l)
	{
		float cv[108] = {
			// positions          
			-l,  l, -l,
			-l, -l, -l,
			 l, -l, -l,
			 l, -l, -l,
			 l,  l, -l,
			-l,  l, -l,

			-l, -l,  l,
			-l, -l, -l,
			-l,  l, -l,
			-l,  l, -l,
			-l,  l,  l,
			-l, -l,  l,

			 l, -l, -l,
			 l, -l,  l,
			 l,  l,  l,
			 l,  l,  l,
			 l,  l, -l,
			 l, -l, -l,

			-l, -l,  l,
			-l,  l,  l,
			 l,  l,  l,
			 l,  l,  l,
			 l, -l,  l,
			-l, -l,  l,

			-l,  l, -l,
			 l,  l, -l,
			 l,  l,  l,
			 l,  l,  l,
			-l,  l,  l,
			-l,  l, -l,

			-l, -l, -l,
			-l, -l,  l,
			 l, -l, -l,
			 l, -l, -l,
			-l, -l,  l,
			 l, -l,  l
		};
		for (size_t i = 0; i < 108; i++)
		{
			*(m_c+i) = *(cv+i);
		}
	}

	std::vector<glm::mat4x3> cube_faces(float x, float y, float z)
	{
		std::vector<glm::mat4x3> faces(6);
		glm::mat4x3 frontFace = {
			{-x,-y, z},
			{-x, y, z},
			{ x, y, z},
			{ x,-y, z}
		};

		glm::mat4x3 backFace = {
			{-x,-y,-z},
			{-x, y,-z},
			{ x, y,-z},
			{ x,-y,-z},
		};

		glm::mat4x3 topFace = {
			{ x, y,-z},
			{-x, y,-z},
			{-x, y, z},
			{ x, y, z}
		};

		glm::mat4x3 bottomFace = {
			{-x,-y, z},
			{-x,-y,-z},
			{ x,-y,-z},
			{ x,-y, z},
		};

		glm::mat4x3 rightFace = {
			{ x, y, z},
			{ x, y,-z},
			{ x,-y,-z},
			{ x,-y, z},
		};

		glm::mat4x3 leftFace = {
			{-x, y, z},
			{-x,-y, z},
			{-x,-y,-z},
			{-x, y,-z},
		};

		faces[0] = frontFace;
		faces[1] = backFace;
		faces[2] = leftFace;
		faces[3] = rightFace;
		faces[4] = topFace;
		faces[5] = bottomFace;

		return faces;
	}

	/*void create_faces_data()
	{
		for (size_t i = 0; i < no_rects; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				m_c[i][j].xyz[0] = m_faces[i][j].x;
				m_c[i][j].xyz[1] = m_faces[i][j].y;
				m_c[i][j].xyz[2] = m_faces[i][j].z;

				switch (j)
				{
				case 1:
				{
					m_c[i][j].tx = 0.0f;
					m_c[i][j].ty = 0.0f;
					break;
				}
				case 2:
				{
					m_c[i][j].tx = 0.0f;
					m_c[i][j].ty = 1.0f;
					break;
				}
				case 3:
				{
					m_c[i][j].tx = 1.0f;
					m_c[i][j].ty = 1.0f;
					break;
				}
				case 0:
				{
					m_c[i][j].tx = 1.0f;
					m_c[i][j].ty = 0.0f;
					break;
				}
				default:
					break;
				}
			}

		}
	}*/
};