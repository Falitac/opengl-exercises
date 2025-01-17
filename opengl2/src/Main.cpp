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

#include "LoadShader.hpp"
#include "LoadOBJ.hpp"

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
	std::string objectPath{"resources/cube.obj"};
	if(argc == 2)
	{
		objectPath = argv[1];
	}

	sf::RenderWindow window(
			sf::VideoMode(WIDTH, HEIGHT),
		  	"OpenGL",
		  	sf::Style::Default,
			sf::ContextSettings(24,8,4,3,0));

	globalWindow = &window;
	window.setFramerateLimit(60.f);
	window.setActive(true);

	sf::Texture face_texture;
	face_texture.loadFromFile("resources/bunnychick.jpg");
	sf::Sprite sprite(face_texture);

	glewExperimental = true;
	if(glewInit() != GLEW_OK)
	{
		std::cerr << "ERROR LOADING GLEW\n";
		return 1;
	}

	glEnable(GL_DEPTH_TEST);

	sf::Font ubuntu;
	ubuntu.loadFromFile("resources/ubuntu.ttf");

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint programID = LoadShaders(
		"shaders/SimpleVertexShader.vertexshader",
		"shaders/SimpleFragmentShader.fragmentshader"
	);

	GLuint matrixID = glGetUniformLocation(programID, "MVP");

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	if(!loadOBJ(objectPath.c_str(), vertices, uvs, normals))
	{
		std::cerr << "could not load \"" << objectPath << "\" ):\n";
		return 1;
	}
	std::cout << vertices.size() << std::endl;

	std::vector<GLfloat> g_color_buffer_data(vertices.size()*3);
	for(int i = 0; i < g_color_buffer_data.size()/3; i++)
	{
		float p = 0;
		p = std::abs(1)*1000.f;

		g_color_buffer_data[i*3 + 0] = float(random(0, int(p))) / p;
		g_color_buffer_data[i*3 + 1] = float(random(0, int(p))) / p;
		g_color_buffer_data[i*3 + 2] = float(random(0, int(p))) / p;
	}

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, g_color_buffer_data.size()*sizeof(GLfloat), &g_color_buffer_data[0], GL_STATIC_DRAW);

	glm::vec3 camPos = glm::vec3(0, 3, 10);

	float horizontalAngle = 3.14f;
	float verticalAngle = 0.f;
	float FOV = 50.f;

	float speed = 303.f;
	float mouseSpeed = 0.048f;


	sf::Clock clock;
	clock.restart();
	bool running = true;
	window.setMouseCursorVisible(false);
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

		int mposx = sf::Mouse::getPosition(window).x;
		int mposy = sf::Mouse::getPosition(window).y;

		horizontalAngle += mouseSpeed * (1.f/60.f) * float(int(window.getSize().x)/2 - mposx);
		verticalAngle   += mouseSpeed * (1.f/60.f) * float(int(window.getSize().y)/2 - mposy);

		glm::vec3 direction(
				std::cos(verticalAngle) * std::sin(horizontalAngle),
				std::sin(verticalAngle),
				std::cos(verticalAngle) * std::cos(horizontalAngle)
		);
		glm::vec3 right = glm::vec3(
				sin(horizontalAngle - 3.14f/2.0f),
				0,
				cos(horizontalAngle - 3.14f/2.0f)
		);
		glm::vec3 up = glm::cross(right, direction);
		
		sf::Mouse::setPosition(sf::Vector2i(window.getSize()) / 2, window);
		if(keys[sf::Keyboard::S])
			camPos -= direction * (1.f/60.f) * speed;
		if(keys[sf::Keyboard::W])
			camPos += direction * (1.f/60.f) * speed;
		if(keys[sf::Keyboard::D])
			camPos += right * (1.f/60.f) * speed;
		if(keys[sf::Keyboard::A])
			camPos -= right * (1.f/60.f) * speed;
		if(keys[sf::Keyboard::E])
			FOV += 0.5f;
		if(keys[sf::Keyboard::Q])
			FOV -= 0.5f;

		
		window.setActive(true);
		
		// MVP matrix 
		glm::mat4 projection = glm::perspective(glm::radians(FOV), (float)window.getSize().x / (float)window.getSize().y, 0.1f, 3000.0f);

		glm::mat4 view = glm::lookAt(
			camPos, 
			camPos + direction,
			up
		);

		glm::mat4 model1 =  glm::mat4(1.f);

		glm::mat4 mvp1 = projection * view * model1;


		/////////////////////////////////////////////////////////////////////////////
		// Drawing things
		/////////////////////////////////////////////////////////////////////////////

		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(programID);

		// giving MVP to GLSL
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp1[0][0]);

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
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		window.display();
	}
	window.setMouseCursorVisible(true);
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);
}
