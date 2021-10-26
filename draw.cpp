/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/22 16:17:39 by trobicho          #+#    #+#             */
/*   Updated: 2021/10/26 19:26:23 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Basic_vulk.hpp"

void	Basic_vulk::record_command_buffer_to_draw(uint32_t image_index)
{
	VkCommandBufferBeginInfo	begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	begin_info.pInheritanceInfo = nullptr;
	if (VK_RESULT_INFO(vkBeginCommandBuffer(m_command_buffers[image_index]
			, &begin_info)) != VK_SUCCESS)
		throw std::runtime_error("failed to begin recording command buffer!");
	VkRenderPassBeginInfo render_pass_info{};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.renderPass = m_render_pass;
	render_pass_info.framebuffer = m_framebuffers[image_index];
	render_pass_info.renderArea.offset = {0, 0};
	render_pass_info.renderArea.extent = m_swapchain_extent;
	VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	render_pass_info.clearValueCount = 1;
	render_pass_info.pClearValues = &clear_color;
	vkCmdBeginRenderPass(m_command_buffers[image_index], &render_pass_info
		, VK_SUBPASS_CONTENTS_INLINE);
	if (!m_cur_pipeline_double)
	{
		vkCmdBindPipeline(m_command_buffers[image_index]
			, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines_float[m_cur_pipeline_n]);
		vkCmdPushConstants(m_command_buffers[image_index], m_pipeline_layout_float
			, VK_SHADER_STAGE_FRAGMENT_BIT, 0
			, sizeof(s_frag_shader_constant_float), &m_constant_float);
	}
	else
	{
		vkCmdBindPipeline(m_command_buffers[image_index]
			, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines_double[m_cur_pipeline_n]);
		vkCmdPushConstants(m_command_buffers[image_index], m_pipeline_layout_double
			, VK_SHADER_STAGE_FRAGMENT_BIT, 0
			, sizeof(s_frag_shader_constant_double), &m_constant_double);
	}
	vkCmdDraw(m_command_buffers[image_index], 4, 1, 0, 0);
	vkCmdEndRenderPass(m_command_buffers[image_index]);
	if (VK_RESULT_INFO(vkEndCommandBuffer(m_command_buffers[image_index]))
			!=VK_SUCCESS)
		throw std::runtime_error("failed to record command buffer!");
}

void	Basic_vulk::draw_frame()
{
	uint32_t	image_index;

	vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX
		, m_semaphore_image_available, VK_NULL_HANDLE, &image_index);
	
	//-------------COMMAND BUFFER--
	record_command_buffer_to_draw(image_index);
	//-------------COMMAND BUFFER--

	VkSubmitInfo	submit_info{};
	VkSemaphore	wait_semaphores[] = {m_semaphore_image_available};
	VkPipelineStageFlags	wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &m_command_buffers[image_index];
	VkSemaphore	signal_semaphores[] = {m_semaphore_render_finish};
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;
	if (VK_RESULT_INFO(vkQueueSubmit(m_queue_graphics, 1
			, &submit_info, VK_NULL_HANDLE)) != VK_SUCCESS)
		throw std::runtime_error("failed to submit draw command buffer!");

	VkPresentInfoKHR present_info{};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphores;
	VkSwapchainKHR swapchains[] = {m_swapchain};
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapchains;
	present_info.pImageIndices = &image_index;
	present_info.pResults = nullptr; //TODO: check result
	vkQueuePresentKHR(m_queue_graphics, &present_info);
	vkQueueWaitIdle(m_queue_graphics);
}
