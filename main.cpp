/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/11 16:28:16 by trobicho          #+#    #+#             */
/*   Updated: 2021/10/26 20:35:46 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Basic_vulk.hpp"
#include <iostream>
#include <chrono>
#include <glm/gtx/transform.hpp>

int width = 1920;
int height = 1080;

s_frag_shader_constant_double constants;

static void	constants_init()
{
	constants.min = glm::dvec2(-4.055f, -2.0f);
	constants.max = glm::dvec2(3.055f, 2.0f);
	constants.max_iter = 500;
}

static s_frag_shader_constant_float convert_float()
{
	s_frag_shader_constant_float constants_float;
	constants_float.min = glm::vec2((float)constants.min.x, (float)constants.min.y);
	constants_float.max = glm::vec2((float)constants.max.x, (float)constants.max.y);
	constants_float.max_iter = constants.max_iter;
	constants_float.time = constants.time;
	return (constants_float);
}

void scroll_callback(GLFWwindow* win, double xoffset, double yoffset)
{
	double	xpos, ypos;
	glfwGetCursorPos(win, &xpos, &ypos);
	double zoom = 1.0f + (double)(-yoffset) / 20.0;
	glm::dvec2	pos = glm::dvec2(xpos / (double)width
		, ypos / (double)height);
	pos = pos * (constants.max - constants.min) + constants.min;
	constants.min = (constants.min - pos) * (zoom) + pos;
	constants.max = (constants.max - pos) * (zoom) + pos;
}

void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods)
{
	auto	vulk = reinterpret_cast<Basic_vulk*>(glfwGetWindowUserPointer(win));
	if (key == GLFW_KEY_EQUAL && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		constants.max_iter += 100;
		std::cout << "Max Iter: " << constants.max_iter << std::endl;
	}
	if (key == GLFW_KEY_MINUS && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		constants.max_iter -= 100;
		std::cout << "Max Iter: " << constants.max_iter << std::endl;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		vulk->set_pipeline(true, 0);
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
		vulk->set_pipeline(false, 0);
	if (key == GLFW_KEY_COMMA && action == GLFW_PRESS)
		vulk->set_pipeline(vulk->get_pipeline_double(), -1);
	if (key == GLFW_KEY_PERIOD && action == GLFW_PRESS)
		vulk->set_pipeline(vulk->get_pipeline_double(), 1);
		
}

static void	main_loop(Basic_vulk &vulk, GLFWwindow *win)
{
	auto	first_time = std::chrono::high_resolution_clock::now();
	while(!glfwWindowShouldClose(win))
	{
		auto	time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> time_span = time - first_time;
		constants.time = static_cast<float>(time_span.count());
		if (vulk.get_pipeline_double())
			vulk.update_constant_double(constants);
		else
			vulk.update_constant_float(convert_float());
		glfwPollEvents();
		vulk.draw_frame();
	}
	vulk.wait_idle();
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow *win = glfwCreateWindow(width, height, "Vulkan", NULL, NULL);
	Basic_vulk  vulk(win, width, height,
			DEBUG_PRINT_INFO_ALL | DEBUG_VALIDATION_LAYER | DEBUG_EXTENSION);
	glfwSetWindowUserPointer(win, &vulk);

	vulk.init();
	constants_init();
	glfwSetScrollCallback(win, scroll_callback);
	glfwSetKeyCallback(win, key_callback);
	main_loop(vulk, win);

	return (0);
}
