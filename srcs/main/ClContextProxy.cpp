// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ClContextProxy.cpp                                 :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/05 15:57:31 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/10 23:30:22 by juloo            ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "ClContextProxy.hpp"
#include "f.hpp"

#include <map>

#ifdef __APPLE__
# include <OpenGL/OpenGL.h>

#else
# include <GL/gl.h>

#endif

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

#ifdef __APPLE__

static std::tuple<cl_context, cl_device_id>
				make_context()
{
	auto const		gl_context = CGLGetCurrentContext();

	_context = clCreateContext((cl_context_properties const[]){
			CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
				(cl_context_properties)CGLGetShareGroup(gl_context),
			0
		}, 0, nullptr, nullptr, nullptr, &err);
	if (_context == nullptr)
		cl_error(err, "clCreateContext");
	err = clGetGLContextInfoAPPLE(_context, gl_context,
			CL_CGL_DEVICE_FOR_CURRENT_VIRTUAL_SCREEN_APPLE,
			sizeof(cl_device_id), &device, nullptr);
	if (device == nullptr)
		cl_error(err, "clGetGLContextInfoAPPLE");
}

#else

# include <CL/cl_gl.h>
# include <GL/glx.h>

static std::tuple<cl_context, cl_device_id>
				make_context()
{
	unsigned const	platform_count = ({
			cl_uint		count;
			clGetPlatformIDs(0, nullptr, &count); count;
		});
	cl_platform_id	platforms[platform_count];
	cl_context		context;
	cl_device_id	device;
	cl_int			err;

	clGetPlatformIDs(platform_count, platforms, NULL);
	for (cl_platform_id platform : platforms)
	{
		cl_context_properties const	props[] = {
			CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
			CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
			CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
			0
		};

		err = clGetGLContextInfoKHR(props, CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR,
			sizeof(device), &device, nullptr);
		if (err != CL_SUCCESS)
			continue ;
		context = clCreateContext(props, 1, &device, nullptr, nullptr, nullptr);
		if (context == nullptr)
			continue ;
		return (std::make_tuple(context, device));
	}
	if (err == CL_SUCCESS)
		throw std::runtime_error("No platform");
	cl_error(err, "make_context");
}

#endif

ClContextProxy::ClContextProxy()
{
	cl_device_id	device;
	cl_int			err;

	std::tie(_context, device) = make_context();
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
