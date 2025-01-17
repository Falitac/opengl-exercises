#include<iostream>
#include<map>
#include<memory>
#include<random>
#include<cmath>

#include<GL/glew.h>

#include<SFML/Graphics.hpp>
#include<SFML/Network.hpp>
#include<SFML/OpenGL.hpp>
#include<SFML/OpenGL.hpp>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/transform.hpp>

#include"loadShader.hpp"

thread_local std::mt19937 gen{std::random_device{}()};

template<typename T>
T random(T min, T max)
{
	return std::uniform_int_distribution<T>{min, max}(gen);
}

// easy access
sf::RenderWindow* globalWindow;
std::map<int, bool> keys;

// default screen size
constexpr unsigned WIDTH = 1600; 
constexpr unsigned HEIGHT = 900;


int main(int argc, char** argv)
{
	sf::RenderWindow window(
			sf::VideoMode(WIDTH, HEIGHT),
		  	"OpenGL",
		  	sf::Style::Default,
			sf::ContextSettings(24,8,4,3,0));

	globalWindow = &window;
	window.setFramerateLimit(60.f);
	window.setActive(true);

	sf::Texture face_texture;
	face_texture.loadFromFile("bunnychick.jpg");

	glewExperimental = true;
	if(glewInit() != GLEW_OK)
	{
		std::cerr << "ERROR LOADING GLEW\n";
		return 1;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);


	sf::Font ubuntu;
	ubuntu.loadFromFile("ubuntu.ttf");

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	static const GLfloat g_vertex_buffer_data[] = {
		 -1.0f,-1.0f,-1.0f, // triangle 1 : begin
		 -1.0f,-1.0f, 1.0f,
		 -1.0f, 1.0f, 1.0f, // triangle 1 : end
		 1.0f, 1.0f,-1.0f, // triangle 2 : begin
		 -1.0f,-1.0f,-1.0f,
		 -1.0f, 1.0f,-1.0f, // triangle 2 : end
		 1.0f,-1.0f, 1.0f,
		 -1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 -1.0f,-1.0f,-1.0f,
		 -1.0f,-1.0f,-1.0f,
		 -1.0f, 1.0f, 1.0f,
		 -1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		 -1.0f,-1.0f, 1.0f,
		 -1.0f,-1.0f,-1.0f,
		 -1.0f, 1.0f, 1.0f,
		 -1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		 -1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 -1.0f, 1.0f,-1.0f,
		 -1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 -1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f
	};

	static GLfloat g_color_buffer_data[12*3*3];
	for(int i = 0; i < 12*3; i++)
	{
		float x = g_vertex_buffer_data[i*3 + 0];
		float y = g_vertex_buffer_data[i*3 + 1];
		float z = g_vertex_buffer_data[i*3 + 2];
		float p = x*x+y*y+z*z;
		p = std::abs(p)*1000.f;

		g_color_buffer_data[i*3 + 0] = float(random(0, int(p))) / p;
		g_color_buffer_data[i*3 + 1] = float(random(0, int(p))) / p;
		g_color_buffer_data[i*3 + 2] = float(random(0, int(p))) / p;
	}

	static GLfloat triangle_data_v[] = {
		5.f, 5.f, 7.f,
		-5.f, -5.f, 7.f,
		0.f, 5.f, 7.f
	};
	static GLfloat triangle_data_c[] = {
		1.f, 0.f, 1.f,
		1.f, 1.f, 0.f,
		0.f, 1.f, 1.f
	};


	// This will identify our vertex buffer
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// This will identify our color buffer
	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	GLuint triangle_buffer_v;
	glGenBuffers(1, &triangle_buffer_v);
	glBindBuffer(GL_ARRAY_BUFFER, triangle_buffer_v);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_data_v), triangle_data_v, GL_STATIC_DRAW);

	GLuint triangle_buffer_c;
	glGenBuffers(1, &triangle_buffer_c);
	glBindBuffer(GL_ARRAY_BUFFER, triangle_buffer_c);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_data_c), triangle_data_c, GL_STATIC_DRAW);


	float xCam = 0.0f,
			yCam = 0.0f,
			zCam = 8.0f,
			rotateAngle = 0.0f,
			distance = 3.f;

	sf::Clock clock;
	clock.restart();
	bool running = true;
	while(running)
	{
		/////////////////////////////////////////////////////////////////////////////
		// Events 
		/////////////////////////////////////////////////////////////////////////////
		for(sf::Event evnt;window.pollEvent(evnt);)
		{
			switch(evnt.type)
			{
				case sf::Event::Closed:
					running = false;
				break;


				case sf::Event::TextEntered:

				break;


				case sf::Event::KeyPressed:
					keys[evnt.key.code] = true;


				switch(evnt.key.code)
				{
					case sf::Keyboard::Escape:
						running = false;
					break;

				}
				break;


				case sf::Event::KeyReleased:
					keys[evnt.key.code] = false;
				break;


				case sf::Event::MouseButtonReleased:

				break;	

				case sf::Event::Resized:
					window.setView(
						sf::View(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y))
						);
				break;


				default: break;
			}
		}

		/////////////////////////////////////////////////////////////////////////////
		// Logic
		/////////////////////////////////////////////////////////////////////////////

		if(keys[sf::Keyboard::S])
			distance += 0.2f;
		if(keys[sf::Keyboard::W])
			distance -= 0.2f;
		if(keys[sf::Keyboard::D])
		{
			//xCam += 0.2f;
			rotateAngle += 3.2f;
		}
		if(keys[sf::Keyboard::A])
		{
			//xCam -= 0.2f;
			rotateAngle -= 3.2f;
		}
		if(keys[sf::Keyboard::E])
			yCam += 0.2f;
		if(keys[sf::Keyboard::Q])
			yCam -= 0.2f;

		xCam = distance*std::cos(glm::radians(rotateAngle));
		zCam = distance*std::sin(glm::radians(rotateAngle));
		
		for(int i = 0; i < 12*3; i++)
		{
			float dr = 0.002f;
			float dg = 0.002f;
			float db = 0.002f;
			if(g_color_buffer_data[i*3 + 0] > 1.f) g_color_buffer_data[i*3 + 0] = 0.f;
			if(g_color_buffer_data[i*3 + 1] > 1.f) g_color_buffer_data[i*3 + 1] = 0.f;
			if(g_color_buffer_data[i*3 + 2] > 1.f) g_color_buffer_data[i*3 + 2] = 0.f;
			g_color_buffer_data[i*3 + 0] += dr;
			g_color_buffer_data[i*3 + 1] += dg;
			g_color_buffer_data[i*3 + 2] += db;
		}
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
		
		window.setActive(true);
		
		// MVP matrix 
		glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float)window.getSize().x / (float)window.getSize().y, 0.1f, 100.0f);

		glm::mat4 view = glm::lookAt(
		glm::vec3(xCam, yCam, zCam), 
		glm::vec3(0.f, 0.f, 0.f), // and looks at the origin
		glm::vec3(0.f, 1.f, 0.f)  // Head is up (set to 0,-1,0 to look upside-down)
		);

		glm::mat4 model1 =  glm::mat4(1.f);
		glm::mat4 model2 =  glm::mat4(1.f);

		glm::mat4 mvp1 = projection * view * model1;
		glm::mat4 mvp2 = projection * view * model2;



		/////////////////////////////////////////////////////////////////////////////
		// Drawing things
		/////////////////////////////////////////////////////////////////////////////

		//window.clear(backgroundColor);
		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(programID);

		// giving MVP to GLSL
		GLuint matrixID = glGetUniformLocation(programID, "MVP");
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp1[0][0]);
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp2[0][0]);

		// cube part
		glEnableVertexAttribArray(0);
		//sf::Texture::bind(&face_texture);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		// Draw the cube !
		glDrawArrays(GL_TRIANGLES, 0, 12*3);

		// triangle part
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, triangle_buffer_v);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, triangle_buffer_c);
		glVertexAttribPointer(
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		window.setActive(false);
		window.display();
	}
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);
}
