#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <GL/gl.h>
#include <SFML/Window.hpp>

#include <iostream>


int main(int argc, char *argv[])
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 3;
	settings.minorVersion = 3;
	settings.attributeFlags = sf::ContextSettings::Core;
	sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default,
	                  settings);
	window.setVerticalSyncEnabled(true);
	
	bool running = true;

	while (running) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed){
				running = false;
			} else if (event.type == sf::Event::Resized) {
				glViewport(0,0, event.size.width, event.size.height);
			}
		}
			
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		window.display();

	}
	return 0;
}
