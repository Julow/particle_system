/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   particle_init.cl                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/10/14 14:59:47 by jaguillo          #+#    #+#             */
/*   Updated: 2016/10/14 18:15:20 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "particule.cl.h"

void				_init_particle(__global p_vertex *dst_vertex,
						__global p_info *dst_info)
{
	float const		w = (get_global_id(0) + 1) / (float)get_global_size(0);

	dst_vertex->pos = (float4){0, 0, 0, 0};
	dst_vertex->color = mix((float4){0.1f, 0.8f, 0.8f, 1.f},
			(float4){0.9f, 0.8f, 0.2f, 1.f}, w);
	dst_info->velocity = (float4){0.f, 0.f, 0.f, 0.f};
	dst_info->mass = 1.f;
}

__kernel void		init_square(__global p_vertex *vertex,
						__global p_info *info)
{
	uint const		id = get_global_id(0);
	uint const		per_line = sqrt((float)get_global_size(0));

	_init_particle(vertex + id, info + id);
	vertex[id].pos = (float4){
		(id % per_line) / (float)per_line * 2.f - 1.f,
		(id / per_line) / (float)per_line * 2.f - 1.f,
		0.f,
		1.f
	};
}

__kernel void		init_sphere(__global p_vertex *vertex,
						__global p_info *info)
{
	uint const		id = get_global_id(0);
	uint const		global_size = get_global_size(0);
	float const		offset = 2.f / global_size;
	float			y, r, phi;

	_init_particle(vertex + id, info + id);
	y = ((id * offset) - 1.f) + (offset / 2.f);
	r = sqrt(1.f - pow(y, 2.f));
	phi = (id % global_size) * M_PI * (3.f - sqrt(5.f));
	vertex[id].pos = (float4){cos(phi) * r, y, sin(phi) * r, 1.f};
	vertex[id].pos = normalize(vertex[id].pos);
}

__kernel void		init_cube(__global p_vertex *vertex,
						__global p_info *info)
{
	uint const		id = get_global_id(0);
	uint const		per_line = sqrt((float)get_global_size(0));
	uint const		side = id % 3;
	float3			tmp;

	_init_particle(vertex + id, info + id);
	tmp.x = (id % per_line) / (float)per_line * 2.f - 1.f;
	tmp.y = (id / per_line) / (float)per_line * 2.f - 1.f;
	tmp.z = ((id % 6) >= 3) ? -1.f : 1.f;
	if (side == 0)
		vertex[id].pos = (float4){tmp.x, tmp.y, tmp.z, 1.f};
	else if (side == 1)
		vertex[id].pos = (float4){tmp.x, tmp.z, tmp.y, 1.f};
	else
		vertex[id].pos = (float4){tmp.z, tmp.y, tmp.x, 1.f};
}

float				frand(uint *seed)
{
	uint const			salt = 209863451;
	uint const			t = *seed ^ (*seed << 11);

	*seed = salt ^ (salt >> 19) ^ (t ^ (t >> 8));
	return (*seed / (float)0xFFFFFFFF);
}

__kernel void		init_rand_sphere(__global p_vertex *vertex,
						__global p_info *info, uint initial_rand)
{
	uint			seed;
	uint const		id = get_global_id(0);
	float2			tmp;
	float			len;

	seed = initial_rand + (id | (id << 16)) * id;
	_init_particle(vertex + id, info + id);
	tmp = (float2){
		(frand(&seed) * 2.f) * M_PI,
		(frand(&seed) * 2.f) * M_PI
	};
	len = frand(&seed);

	vertex[id].pos = (float4){
		cos(tmp.x) * cos(tmp.y) * len,
		sin(tmp.x) * cos(tmp.y) * len,
		sin(tmp.y) * len,
		1.f
	};
}

__kernel void		init_rand_cube(__global p_vertex *vertex,
						__global p_info *info, uint initial_rand)
{
	uint			seed;
	uint const		id = get_global_id(0);

	seed = initial_rand + (id | (id << 16)) * id;
	_init_particle(vertex + id, info + id);
	vertex[id].pos = (float4){
		frand(&seed) * 2.f - 1.f,
		frand(&seed) * 2.f - 1.f,
		frand(&seed) * 2.f - 1.f,
		1.f
	};
}
