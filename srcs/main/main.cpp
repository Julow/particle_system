// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   main.cpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/04 13:50:05 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/05 15:43:36 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "ft/cl.h"
#include "ft/gl.h"

#include "f.hpp"

#include <functional>
#include <map>
#include <stdexcept>
#include <string>

/*
** ========================================================================== **
** CL utils
*/

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

#define CL_CALL(FUNC, ...)		cl_error(FUNC(__VA_ARGS__), #FUNC)

void					cl_error(cl_int err, char const *str)
{
	if (err != CL_SUCCESS)
		throw std::runtime_error("%: %"_f (str, g_cl_errors.at(err)));
}

/*
** ========================================================================== **
** Get context
*/

typedef std::function<void(char const*, void const*, size_t)>	t_err_callback;

static void		pfn_notify(char const *err, void const *info,
							size_t cb, void *user_data)
{
	(*static_cast<t_err_callback*>(user_data))(err, info, cb);
}

std::pair<cl_context, cl_command_queue>
				get_context(cl_platform_id platform, t_err_callback err_callback)
{
	cl_device_id			device;
	cl_int					err;
	cl_context				context;
	cl_command_queue		queue;
	t_err_callback *const	notify_data = new t_err_callback(err_callback);

	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	if ((context = clCreateContext(NULL, 1, &device,
				&pfn_notify, (void*)notify_data, &err)) == NULL
		|| (queue = clCreateCommandQueue(context, device, 0, &err)) == NULL)
	{
		delete notify_data;
		cl_error(err, "clCreateContext");
	}
	return {context, queue};
}

/*
** ========================================================================== **
** Get program
*/

static uint32_t	get_log_size(cl_program program, cl_device_id device)
{
	size_t			size;

	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
	return (size);
}

__attribute__ ((noreturn))
static void		program_build_error(cl_program program, cl_device_id device)
{
	uint32_t const	log_size = get_log_size(program, device);
	char			log[log_size];

	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
			log_size, log, NULL);
	clReleaseProgram(program);
	throw std::runtime_error("Failed to build program: %"_f
					(std::string(log, log_size)));
}

cl_program		get_program(cl_context context, char const *str)
{
	cl_program		program;
	cl_int			err;
	cl_device_id	device;

	CL_CALL(clGetContextInfo, context, CL_CONTEXT_DEVICES, sizeof(device), &device, NULL);
	program = clCreateProgramWithSource(context, 1, &str, NULL, &err);
	cl_error(err, "clCreateProgramWithSource");
	err = clBuildProgram(program, 1, &device, "", NULL, NULL);
	if (err == CL_BUILD_PROGRAM_FAILURE)
		program_build_error(program, device);
	else if (err != CL_SUCCESS)
	{
		clReleaseProgram(program);
		cl_error(err, "clBuildProgram");
	}
	return (program);
}

cl_kernel		get_kernel(cl_program program, char const *kernel_name)
{
	cl_int			err;
	cl_kernel		kernel;

	kernel = clCreateKernel(program, kernel_name, &err);
	cl_error(err, "clCreateKernel");
	return (kernel);
}

cl_mem			get_buffer(cl_context context, cl_mem_flags flags, uint32_t size)
{
	cl_int			err;
	cl_mem			buff;

	buff = clCreateBuffer(context, flags, size, NULL, &err);
	cl_error(err, "clCreateBuffer");
	return (buff);
}

/*
** ========================================================================== **
** Get Platform
*/

cl_platform_id	get_platform(void)
{
	cl_platform_id	p;

	CL_CALL(clGetPlatformIDs, 1, &p, NULL);
	return (p);
}

/*
** ========================================================================== **
** Kernel run
*/

template<class T>
void			kernel_arg(cl_kernel kernel, uint32_t index, T arg)
{
	CL_CALL(clSetKernelArg, kernel, index, sizeof(T), &arg);
}

template<class ...Args>
void			kernel_args(cl_kernel kernel, Args&& ...args)
{
	uint32_t		i;

	i = 0;
	(void)(int[]){(kernel_arg(kernel, i++, args), 0) ...};
}

class	kernel_run_work
{
public:
	kernel_run_work(size_t global_work_size,
				size_t local_work_size = -1,
				size_t global_work_offset = -1) :
		_g_size(global_work_size),
		_g_offset(global_work_offset),
		_l_size(local_work_size)
	{}

	size_t const	*g_size(void) const { return (&_g_size); }
	size_t const	*g_offset(void) const
		{ return ((_g_offset < 0) ? nullptr : (size_t const*)&_g_offset); }
	size_t const	*l_size(void) const
		{ return ((_l_size < 0) ? nullptr : (size_t const*)&_l_size); }

private:
	size_t		_g_size;
	ssize_t		_g_offset;
	ssize_t		_l_size;
};

template<class ...Args>
void			kernel_run(cl_command_queue queue, cl_kernel kernel,
					kernel_run_work const &work, Args&& ...args)
{
	kernel_args(kernel, std::forward<Args>(args)...);
	CL_CALL(clEnqueueNDRangeKernel, queue, kernel, 1, work.g_offset(),
			work.g_size(), work.l_size(), 0, NULL, NULL);
}

/*
** ========================================================================== **
*/

#include "GlfwWindowProxy.hpp"
#include <iostream>

class	Main final : public GlfwWindowProxy
{
public:
	Main() :
		GlfwWindowProxy(500, 500, "lol")
	{}
	virtual ~Main() {}

	void			loop()
	{
		while (!glfwWindowShouldClose(get_window()))
			glfwPollEvents();
	}

private:

private:
	Main(Main &&src) = delete;
	Main(Main const &src) = delete;
	Main			&operator=(Main &&rhs) = delete;
	Main			&operator=(Main const &rhs) = delete;
};

int				main()
{
	Main			*m;

	try
	{
		m = new Main();
	}
	catch (std::runtime_error const &e)
	{
		std::cout << "Init error: " << e.what() << std::endl;
		return (1);
	}
	m->loop();
	return (0);
}
