#pragma once
#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include <string>
#include <vector>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture
{
	unsigned int id;
	std::string type; //texture_diffuse map, texture_specular map etc
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		this->SetupMesh();
	}


	void Draw(Shader& shader)
	{
		unsigned int diffuseN = 0;
		unsigned int specularN = 0;

		for (int i = 0; i < this->textures.size(); i++)
		{
			//Associate the current texture to GL_TEXTURE0 + i
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, this->textures[i].id);

			std::string name = this->textures[i].type;
			std::string number;

			if (name == "texture_diffuse")
			{
				number = std::to_string(diffuseN++);
			}
			else if (name == "texture_specular")
			{
				number = std::to_string(specularN++);
			}
			
			glUniform1i(glGetUniformLocation(shader.ID, ("material." + name + number).c_str()), i); //i is the same number as (GL_TEXTURE0 + i, which is assocaited with the currect texture).
		}

	}

private:
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;

	void SetupMesh()
	{
		//Create Buffers and VAO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		//Bind them (work on them)
		glBindVertexArray(VAO); //working on this vao
		glBindBuffer(GL_ARRAY_BUFFER, VBO); //working on this VBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //working on this EBO

		//On the buffers abd VAo we're working on, set their data
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW); //&this->vertices[0] is the address of the first element of vertices. (check footnote for why)
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size(), &this->indices[0], GL_STATIC_DRAW); //again, indices is an std::vector which is contigous in memory
		
		glEnableVertexAttribArray(0); //vertex positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); //sizeof(Vertex) just goes to the next vertex in the same spot. (footnote explains why)
		glEnableVertexAttribArray(1); //vertex normals
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal))); //little cheat code offsetof() function. returns size from start of Vertex to normal
		glEnableVertexAttribArray(2); //vertex texCoords
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoords))); //same here but for texCoords

		glBindVertexArray(0); //unbind when done

		//Note: 
		/*
		Struct is just a sequence of data in memory.

		Vertex vertex;
		vertex.Position  = glm::vec3(0.2f, 0.4f, 0.6f);
		vertex.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
		vertex.TexCoords = glm::vec2(1.0f, 0.0f);
		vertex = [0.2f, 0.4f, 0.6f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f]; (glm::vec3's are contigous)
		So a Vertex looks essentially like [position.x][position.y][position.z][normal.x][normal.y][normal.z][texCoords.x][texCoords.y] 

		Even more important: A vector of structs like std::vector<Vertex> is contigous itself, since std::vector is contigous.
		eg: std::vector<Vertex> = {v1, v2, v3} === [0.2f, 0.4f, 0.6f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.2f, 0.4f, 0.6f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.2f, 0.4f, 0.6f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f]
		This is why glBufferData just takes a pointer to the first element of it, its just liek an array!
		*/

		//sizeof(Vertex) = 8floats = 32bytes
	}


};