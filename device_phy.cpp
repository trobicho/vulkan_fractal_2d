/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   device_phy.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/17 18:34:44 by trobicho          #+#    #+#             */
/*   Updated: 2021/10/20 10:46:32 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Basic_vulk.hpp"
#include <iostream>
#include <stdexcept>
#include <set>

static bool		check_device_extension_support(VkPhysicalDevice phy_dev)
{
    uint32_t	extension_count;
		const std::vector<const char*>	device_exts =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

    vkEnumerateDeviceExtensionProperties(
			phy_dev, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available_exts(extension_count);
    vkEnumerateDeviceExtensionProperties(phy_dev, nullptr
			, &extension_count, available_exts.data());

    std::set<std::string> required_exts(device_exts.begin(), device_exts.end());

    for (const auto& extension : available_exts) {
        required_exts.erase(extension.extensionName);
    }

    return (required_exts.empty());
}


static bool	is_device_suitable(const VkPhysicalDevice &phy_dev
	, VkSurfaceKHR surface)
{
	uint32_t	queue_family_count;
	//SWAP_CHAIN_DETAIL vkGetPhysicalDeviceFeatures(device, &dev_feat);
	s_swapchain_details	swapchain_details
		= query_swapchain_details(phy_dev, surface);

	vkGetPhysicalDeviceQueueFamilyProperties(phy_dev, &queue_family_count, nullptr); //void
	if (queue_family_count == 0)
		return false;
	std::vector<VkQueueFamilyProperties>	queue_family_props(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(phy_dev
			, &queue_family_count, queue_family_props.data());
	if (!check_device_extension_support
		|| swapchain_details.formats.empty()
		|| swapchain_details.present_modes.empty())
		return (false);
	for (const auto & queue : queue_family_props)
	{
		if (queue.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			return (true);
	}
	return (false);
}

void	Basic_vulk::choose_physical_device()
{
	uint32_t	phy_dev_count;

	if (VK_RESULT_INFO(
		vkEnumeratePhysicalDevices(m_instance, &phy_dev_count, nullptr)) != VK_SUCCESS)
    throw std::runtime_error("failed to get physical devices properties");
	std::vector<VkPhysicalDevice>	phy_devices(phy_dev_count);
	if (VK_RESULT_INFO(
		vkEnumeratePhysicalDevices(m_instance, &phy_dev_count, phy_devices.data()))
		!= VK_SUCCESS)
    throw std::runtime_error("failed to get physical devices properties");
	if (m_debug_mode & DEBUG_PRINT_INFO_DEVICE)
	{
		std::cout << "DEVICES INFO (" << phy_dev_count << ")" << ":" << std::endl;
		for(const auto &dev : phy_devices)
		{
			info_physical_device(dev);
			info_queue_family_properties(dev, 2);
		}
	}
	for(const auto &dev : phy_devices)
	{
		VkPhysicalDeviceProperties	dev_props;
		vkGetPhysicalDeviceProperties(dev, &dev_props);
		if (is_device_suitable(dev, m_surface))
		{
			std::cout << "Choosed device: " << dev_props.deviceName << std::endl;
			m_physical_device = dev;
		}
	}
}
