/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   particule.cl.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/10/07 16:22:34 by jaguillo          #+#    #+#             */
/*   Updated: 2016/10/07 16:36:28 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARTICULE_CL_H
# define PARTICULE_CL_H

# ifdef __cplusplus

#  include "ft/cl.h"

namespace particule
{

typedef cl_float			_float;

static_assert(std::is_same<cl_float, GLfloat>::value, "");

# else

typedef float				_float;
typedef struct vec3			vec3;
typedef struct particule	particule;

# endif

struct	vec3
{
	_float		x;
	_float		y;
	_float		z;
};

struct	particule
{
	vec3		pos;
	vec3		color;
};

# ifdef __cplusplus
};
# endif

#endif
