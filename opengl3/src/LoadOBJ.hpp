#pragma once
#include<GL/glew.h>
#include<string>
#include<cstring>
#include<fstream>
#include<vector>
#include<sstream>
#include<cstdio>

bool loadOBJ(
	const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
)
{
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	FILE* file = std::fopen(path, "r");
	if(file == NULL) {
		std::printf("Impossible to open the file %s\n", path);
		return false;
	}

	while(true)
	{
		char lineHeader[128];

		int res = std::fscanf(file, "%s", lineHeader);
		if(res == EOF)
		{
			break;
		}

		if     (std::strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			std::fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if(std::strcmp(lineHeader, "vt") == 0)
		{
			glm::vec2 uv;
			std::fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = 1.f - uv.y;
			temp_uvs.push_back(uv);
		}
		else if(strcmp(lineHeader, "vn") == 0)
		{
			glm::vec3 normal;
			std::fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if(strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = std::fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vertexIndex[0],
					&uvIndex    [0],
					&normalIndex[0],
					&vertexIndex[1],
					&uvIndex    [1],
					&normalIndex[1],
					&vertexIndex[2],
					&uvIndex    [2],
					&normalIndex[2]
			);
			if(matches != 9)
			{
				std::printf("File can't be read by this piece of ***** *** ):\n");
				std::fclose(file);
				return false;
			}
			for(int i = 0; i < 3; i++)
			{
				vertexIndices.push_back(vertexIndex[i]);
				uvIndices    .push_back(uvIndex    [i]);
				normalIndices.push_back(normalIndex[i]);
			}
		}
		else
		{
			char junkBuffer[1000];
			std::fgets(junkBuffer, 1000, file);
		}
	}

	for(unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);
	}
	std::fclose(file);

	return true;
}
