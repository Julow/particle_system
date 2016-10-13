/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   particule.cl.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/10/07 16:22:34 by jaguillo          #+#    #+#             */
/*   Updated: 2016/10/13 12:52:54 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARTICULE_CL_H
# define PARTICULE_CL_H

# ifndef __OPENCL_C_VERSION__

#  include "ft/cl.h"
#  include <glm/glm.hpp>

namespace particule
{

typedef cl_float			_float;

typedef glm::vec4			vec4;

static_assert(std::is_same<cl_float, GLfloat>::value, "");

# else

typedef float4				vec4;
typedef struct p_vertex		p_vertex;
typedef struct p_info		p_info;

# endif

struct	p_vertex
{
	vec4		pos;
	vec4		color;
};

struct	p_info
{
	vec4		velocity;
	float		mass;
	float		_pad[3];
};

# ifndef __OPENCL_C_VERSION__
};
# endif

#endif
