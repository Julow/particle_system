// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ClContextProxy.cpp                                 :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/05 15:57:31 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/05 19:30:03 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "ClContextProxy.hpp"
#include "f.hpp"

#include <map>
#include <OpenGL/OpenGL.h>

static std::map<cl_int, char const*> const	g_cl_errors = {
	{CL_COMPILER_NOT_AVAILABLE, "CL_COMPILER_NOT_AVAILABLE"},
	{CL_BUILD_PROGRAM_FAILURE, "CL_BUILD_PROGRAM_FAILURE"},
	{CL_INVALID_OPERATION, "CL_INVALID_OPERATION"},
	{CL_OUT_OF_RESOURCES, "CL_OUT_OF_RESOURCES"},
	{CL_SUCCESS, "CL_SUCCESS"},
	{CL_DEVICE_NOT_FOUND, "CL_DEVICE_NOT_FOUND"},
	{CL_DEVICE_NOT_AVAILABLE, "CL_DEVICE_NOT_AVAILABLE"},
	// {CL_COMPILER_NOT, "CL_COMPILER_NOT"},
	// {CL_MEM_OBJECT, "CL_MEM_OBJECT"},
	{CL_OUT_OF_RESOURCES, "CL_OUT_OF_RESOURCES"},
	{CL_OUT_OF_HOST_MEMORY, "CL_OUT_OF_HOST_MEMORY"},
	// {CL_PROFILING_INFO_NOT, "CL_PROFILING_INFO_NOT"},
	{CL_MEM_COPY_OVERLAP, "CL_MEM_COPY_OVERLAP"},
	{CL_IMAGE_FORMAT, "CL_IMAGE_FORMAT"},
	// {CL_IMAGE_FORMAT_NOT, "CL_IMAGE_FORMAT_NOT"},
	// {CL_BUILD_PROGRAM, "CL_BUILD_PROGRAM"},
	{CL_MAP_FAILURE, "CL_MAP_FAILURE"},
	// {CL_MISALIGNED_SUB, "CL_MISALIGNED_SUB"},
	// {CL_EXEC_STATUS_ERROR_, "CL_EXEC_STATUS_ERROR_"},
	// {CL_COMPILE_PROGRAM, "CL_COMPILE_PROGRAM"},
	{CL_LINKER_NOT_AVAILABLE, "CL_LINKER_NOT_AVAILABLE"},
	{CL_LINK_PROGRAM_FAILURE, "CL_LINK_PROGRAM_FAILURE"},
	// {CL_DEVICE_PARTITION, "CL_DEVICE_PARTITION"},
	// {CL_KERNEL_ARG_INFO, "CL_KERNEL_ARG_INFO"},
	{CL_INVALID_VALUE, "CL_INVALID_VALUE"},
	{CL_INVALID_DEVICE_TYPE, "CL_INVALID_DEVICE_TYPE"},
	{CL_INVALID_PLATFORM, "CL_INVALID_PLATFORM"},
	{CL_INVALID_DEVICE, "CL_INVALID_DEVICE"},
	{CL_INVALID_CONTEXT, "CL_INVALID_CONTEXT"},
	{CL_INVALID_QUEUE_PROPERTIES, "CL_INVALID_QUEUE_PROPERTIES"},
	{CL_INVALID_COMMAND_QUEUE, "CL_INVALID_COMMAND_QUEUE"},
	{CL_INVALID_HOST_PTR, "CL_INVALID_HOST_PTR"},
	{CL_INVALID_MEM_OBJECT, "CL_INVALID_MEM_OBJECT"},
	{CL_INVALID_IMAGE_FORMAT_DESCRIPTOR, "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR"},
	{CL_INVALID_IMAGE_SIZE, "CL_INVALID_IMAGE_SIZE"},
	{CL_INVALID_SAMPLER, "CL_INVALID_SAMPLER"},
	{CL_INVALID_BINARY, "CL_INVALID_BINARY"},
	{CL_INVALID_BUILD_OPTIONS, "CL_INVALID_BUILD_OPTIONS"},
	{CL_INVALID_PROGRAM, "CL_INVALID_PROGRAM"},
	{CL_INVALID_PROGRAM_EXECUTABLE, "CL_INVALID_PROGRAM_EXECUTABLE"},
	{CL_INVALID_KERNEL_NAME, "CL_INVALID_KERNEL_NAME"},
	{CL_INVALID_KERNEL_DEFINITION, "CL_INVALID_KERNEL_DEFINITION"},
	{CL_INVALID_KERNEL, "CL_INVALID_KERNEL"},
	{CL_INVALID_ARG_INDEX, "CL_INVALID_ARG_INDEX"},
	{CL_INVALID_ARG_VALUE, "CL_INVALID_ARG_VALUE"},
	{CL_INVALID_ARG_SIZE, "CL_INVALID_ARG_SIZE"},
	{CL_INVALID_KERNEL_ARGS, "CL_INVALID_KERNEL_ARGS"},
	{CL_INVALID_WORK_DIMENSION, "CL_INVALID_WORK_DIMENSION"},
	{CL_INVALID_WORK_GROUP_SIZE, "CL_INVALID_WORK_GROUP_SIZE"},
	{CL_INVALID_WORK_ITEM_SIZE, "CL_INVALID_WORK_ITEM_SIZE"},
	{CL_INVALID_GLOBAL_OFFSET, "CL_INVALID_GLOBAL_OFFSET"},
	{CL_INVALID_EVENT_WAIT_LIST, "CL_INVALID_EVENT_WAIT_LIST"},
	{CL_INVALID_EVENT, "CL_INVALID_EVENT"},
	{CL_INVALID_OPERATION, "CL_INVALID_OPERATION"},
	{CL_INVALID_GL_OBJECT, "CL_INVALID_GL_OBJECT"},
	{CL_INVALID_BUFFER_SIZE, "CL_INVALID_BUFFER_SIZE"},
	{CL_INVALID_MIP_LEVEL, "CL_INVALID_MIP_LEVEL"},
	{CL_INVALID_GLOBAL_WORK_SIZE, "CL_INVALID_GLOBAL_WORK_SIZE"},
	{CL_INVALID_PROPERTY, "CL_INVALID_PROPERTY"},
	{CL_INVALID_IMAGE_DESCRIPTOR, "CL_INVALID_IMAGE_DESCRIPTOR"},
	{CL_INVALID_COMPILER_OPTIONS, "CL_INVALID_COMPILER_OPTIONS"},
	{CL_INVALID_LINKER_OPTIONS, "CL_INVALID_LINKER_OPTIONS"},
	{CL_INVALID_DEVICE_PARTITION_COUNT, "CL_INVALID_DEVICE_PARTITION_COUNT"},
	// {CL_INVALID_PIPE_SIZE, "CL_INVALID_PIPE_SIZE"},
	// {CL_INVALID_DEVICE_QUEUE, "CL_INVALID_DEVICE_QUEUE"},
	// {CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR, "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR"},
	// {CL_INVALID_D3D10_DEVICE_KHR, "CL_INVALID_D3D10_DEVICE_KHR"},
	// {CL_INVALID_D3D10_RESOURCE_KHR, "CL_INVALID_D3D10_RESOURCE_KHR"},
	// {CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR, "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR"},
	// {CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR, "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR"},
};

__attribute__ ((noreturn))
static void		cl_error(cl_int err, char const *str)
{
	throw std::runtime_error("%: %"_f (str, ClContextProxy::strerror(err)));
}

#include <iostream>

// TODO: check device's opencl version and required extensions
static bool		get_device(cl_platform_id platform, cl_device_id &dst)
{
	cl_device_type const	type = CL_DEVICE_TYPE_GPU;
	unsigned const			device_count = ({
			cl_uint				count;
			(clGetDeviceIDs(platform, type, 0, nullptr, &count) == CL_SUCCESS) ?
				count : 0;
		});
	cl_device_id			devices[device_count];

	if (device_count == 0 || clGetDeviceIDs(platform, type,
					device_count, devices, nullptr) != CL_SUCCESS)
		return (false);
	dst = devices[0];
	return (true);
}

static std::tuple<cl_platform_id, cl_device_id>
				get_platform()
{
	unsigned const	platform_count = ({
			cl_uint		count;
			clGetPlatformIDs(0, nullptr, &count); count;
		});
	cl_platform_id	platforms[platform_count];
	std::tuple<cl_platform_id, cl_device_id>	res;

	clGetPlatformIDs(platform_count, platforms, nullptr);
	for (auto p : platforms)
	{
		std::get<0>(res) = p;
		if (get_device(p, std::get<1>(res)))
			return (res);
	}
	throw std::runtime_error("No compatible GPU found");
}

static cl_platform_id	get_first_platform()
{
	unsigned const	platform_count = ({
			cl_uint		count;
			clGetPlatformIDs(0, nullptr, &count); count;
		});
	cl_platform_id	platforms[platform_count];

	if (platform_count == 0)
		throw std::runtime_error("No platform");
	clGetPlatformIDs(platform_count, platforms, nullptr);
	return (platforms[0]);
}

ClContextProxy::ClContextProxy()
{
	cl_platform_id	platform;
	cl_device_id	device;
	cl_int			err;

	// std::tie(platform, device) = get_platform();
	// _context = clCreateContext((cl_context_properties[]){
	// 		CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
	// 		CL_GL_CONTEXT_KHR, (cl_context_properties)gl_context,
	// 		CL_CGL_SHAREGROUP_KHR, (cl_context_properties)CGLGetShareGroup(gl_context),

	// 		0
	// 	}, 1, &device, nullptr, nullptr, &err);

	// {
		auto const		gl_context = CGLGetCurrentContext();

	// 	// clGetGLContextInfoKHR
	// 	err = clGetGLContextInfoAPPLE((cl_context_properties const[]){
	// 			CL_GL_CONTEXT_KHR, (cl_context_properties)gl_context,
	// 			0
	// 		}, 
	// 		// CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR,
	// 		CL_CGL_DEVICE_FOR_CURRENT_VIRTUAL_SCREEN_APPLE,
	// 		sizeof(cl_device_id), &device, NULL);
	// 	if (err != CL_SUCCESS)
	// 		cl_error(err, "clGetGLContextInfoKHR");

	// 	err = clGetDeviceInfo(device, CL_DEVICE_PLATFORM,
	// 			sizeof(cl_platform_id), &platform, NULL);
	// 	if (err != CL_SUCCESS)
	// 		cl_error(err, "clGetGLContextInfoKHR");
	// }

	// APPLE

	_context = clCreateContext((cl_context_properties const[]){
			CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
				(cl_context_properties)CGLGetShareGroup(gl_context),
			CL_CONTEXT_PLATFORM, (cl_context_properties)get_first_platform(),
			0
		}, 0, NULL, NULL, NULL, &err);
	if (_context == nullptr)
		cl_error(err, "clCreateContext");

	err = clGetGLContextInfoAPPLE(_context, gl_context,
			CL_CGL_DEVICE_FOR_CURRENT_VIRTUAL_SCREEN_APPLE,
			sizeof(cl_device_id), &device, NULL);
	if (device == nullptr)
		cl_error(err, "clGetGLContextInfoAPPLE");

	// -

	if ((_queue = clCreateCommandQueue(_context, device, 0, &err)) == nullptr)
	{
		clReleaseContext(_context);
		cl_error(err, "clCreateCommandQueue");
	}
}

ClContextProxy::~ClContextProxy(void)
{
	clReleaseCommandQueue(_queue);
	clReleaseContext(_context);
}

cl_context			ClContextProxy::get_context()
{
	return (_context);
}

cl_command_queue	ClContextProxy::get_queue()
{
	return (_queue);
}

char const			*ClContextProxy::strerror(cl_int err)
{
	auto const			it = g_cl_errors.find(err);

	if (it == g_cl_errors.end())
		return ("Unknown error");
	return (it->second);
}
