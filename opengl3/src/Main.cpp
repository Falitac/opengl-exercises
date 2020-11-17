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
	std::string objectPath{"resources/cube3.obj"};
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
	face_texture.loadFromFile("resources/ramenTexture.jpg");
	sf::Sprite sprite(face_texture);
	face_texture.generateMipmap();

	glewExperimental = true;
	if(glewInit() != GLEW_OK)
	{
		std::cerr << "ERROR LOADING GLEW\n";
		return 1;
	}

	glEnable(GL_DEPTH_TEST);

	sf::Font ubuntu;
	ubuntu.loadFromFile("resources/ubuntu.ttf");
	sf::Text pauseText("Pauza", ubuntu, 50);

	GLuint programID = LoadShaders(
		"shaders/SimpleVertexShader.vertexshader",
		"shaders/SimpleFragmentShader.fragmentshader"
	);

	GLuint matrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

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
	glBufferData(
		GL_ARRAY_BUFFER,
		vertices.size()*sizeof(glm::vec3),
		&vertices[0],
		GL_STATIC_DRAW
	);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER,
		uvs.size()*sizeof(glm::vec2),
		&uvs[0],
		GL_STATIC_DRAW
	);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER,
		normals.size()*sizeof(glm::vec3),
		&normals[0],
		GL_STATIC_DRAW
	);

	glUseProgram(programID);

	sf::Texture::bind(&face_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

	glm::vec3 camPos = glm::vec3(0, 3, 10);
	float horizontalAngle = 3.14f;
	float verticalAngle = 0.f;
	float FOV = 50.f;
	glm::vec3 direction(
			std::cos(verticalAngle) * std::sin(horizontalAngle),
			std::sin(verticalAngle),
			std::cos(verticalAngle) * std::cos(horizontalAngle)
	);
	glm::vec3 right(
			sin(horizontalAngle - 3.14f/2.0f),
			0,
			cos(horizontalAngle - 3.14f/2.0f)
	);
	glm::vec3 up = glm::cross(right, direction);

	float speed = 10.f;
	float mouseSpeed = 0.048f;

	window.setMouseCursorVisible(false);
	sf::Mouse::setPosition(sf::Vector2i(window.getSize()) / 2, window);
	sf::Clock clock;
	clock.restart();
	bool running = true;
	bool pause = false;
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
						if(pause)
						{
							running = false;
							break;
						}
						pause = true;
						window.setMouseCursorVisible(true);
					break;

					case sf::Keyboard::Space:
						if(pause)
						{
							pause = false;
							window.setMouseCursorVisible(false);
							sf::Mouse::setPosition(sf::Vector2i(window.getSize()) / 2, window);
						}
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
						sf::View(sf::FloatRect(
							0,
							0,
							window.getSize().x,
							window.getSize().y
						))
						);
				break;

				default: break;
			}
		}

/////////////////////////////////////////////////////////////////////////////
// Logic
/////////////////////////////////////////////////////////////////////////////


		if(!pause)
		{
			int mposx = sf::Mouse::getPosition(window).x;
			int mposy = sf::Mouse::getPosition(window).y;

			sf::Mouse::setPosition(sf::Vector2i(window.getSize()) / 2, window);

			horizontalAngle +=
				mouseSpeed * (1.f/60.f) * float(int(window.getSize().x)/2 - mposx);
			verticalAngle   +=
				mouseSpeed * (1.f/60.f) * float(int(window.getSize().y)/2 - mposy);

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
			if(keys[sf::Keyboard::R])
				speed += 1.5f;
			if(keys[sf::Keyboard::F])
				speed -= 1.5f;

			if(speed < 0.f)
				speed = 0.f;

			direction = glm::vec3(
					std::cos(verticalAngle) * std::sin(horizontalAngle),
					std::sin(verticalAngle),
					std::cos(verticalAngle) * std::cos(horizontalAngle)
			);
			right = glm::vec3(
					sin(horizontalAngle - 3.14f/2.0f),
					0,
					cos(horizontalAngle - 3.14f/2.0f)
			);
			up = glm::cross(right, direction);
		}

		// MVP matrix
		glm::mat4 projection = glm::perspective(
			glm::radians(FOV),
			(float)window.getSize().x / (float)window.getSize().y,
			0.1f,
			3000.0f
		);

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
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &model1[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &view[0][0]);

		glm::vec3 lightPos = camPos;
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		sf::Texture::bind(&face_texture);
		glUniform1i(TextureID, 0);

		// cube part
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			nullptr             // array buffer offset
		);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,            // normalized?
			0,                  // stride
			nullptr             // array buffer offset
		);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,            // normalized?
			0,                  // stride
			nullptr             // array buffer offset
		);
		// Draw the cube !
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// SFML things
		window.pushGLStates();

		if(pause)
			window.draw(pauseText);

		window.popGLStates();
		// SFML things end

		window.display();
	}
	window.setMouseCursorVisible(true);
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(programID);
}
