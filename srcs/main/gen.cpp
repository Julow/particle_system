#include "gen.h"

char const		prog_particle_init[] = R"(
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   particle_init.cl                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/10/14 14:59:47 by jaguillo          #+#    #+#             */
/*   Updated: 2016/11/10 13:27:46 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   particle.clh                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juloo <juloo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/10/23 20:14:44 by juloo             #+#    #+#             */
/*   Updated: 2016/11/10 13:23:11 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARTICLE_CLH
# define PARTICLE_CLH

typedef struct p_vertex		p_vertex;
typedef struct p_info		p_info;

struct	p_vertex
{
	float4		pos;
	float4		color;
};

struct	p_info
{
	float4		velocity;
	float4		spring_pos;
	float		spring_k;
	float		pad[3];
};

# define P_MASS			1.f
# define P_DRAG			0.47f

# define P_RADIUS		5.f

# define P_AREA			((float)M_PI * P_RADIUS * P_RADIUS)

# define CENTER_MASS	3.f

#endif

static void			_init_particle(__global p_vertex *dst_vertex,
						__global p_info *dst_info)
{
	float const		w = (get_global_id(0) + 1) / (float)get_global_size(0);

	dst_vertex->pos = (float4){0, 0, 0, 0};
	dst_vertex->color = mix((float4){0.1f, 0.8f, 0.8f, 1.f},
			(float4){0.9f, 0.8f, 0.2f, 1.f}, w);
	dst_info->velocity = (float4){0.f, 0.f, 0.f, 0.f};
	// dst_info->mass = 1.f;
}

static float		frand(uint *seed)
{
	uint const			salt = 209863451;
	uint const			t = *seed ^ (*seed << 11);

	*seed = salt ^ (salt >> 19) ^ (t ^ (t >> 8));
	return (*seed / (float)0xFFFFFFFF);
}

static uint			init_rand(uint param)
{
	uint const			id = get_global_id(0);

	return (param + (id | (id << 16)) * id);
}

__kernel void		init_springs(__global p_vertex *vertex,
						__global p_info *info, uint initial_rand)
{
	uint			seed;
	uint const		id = get_global_id(0);

	seed = init_rand(initial_rand);
	info[id].spring_pos = vertex[id].pos;
	info[id].spring_k = mix(20.f, 80.f, frand(&seed));
}

__kernel void		init_square(__global p_vertex *vertex,
						__global p_info *info)
{
	uint const		id = get_global_id(0);
	uint const		per_line = sqrt((float)get_global_size(0));

	_init_particle(vertex + id, info + id);
	vertex[id].pos = (float4){
		(id % per_line) / (float)per_line - 0.5f,
		(id / per_line) / (float)per_line - 0.5f,
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

#define CUBE_RADIUS		0.5f

__kernel void		init_cube(__global p_vertex *vertex,
						__global p_info *info)
{
	uint const		id = get_global_id(0);
	uint const		per_line = sqrt((float)get_global_size(0));
	uint const		side = id % 3;
	float3			tmp;

	_init_particle(vertex + id, info + id);
	tmp.x = (id % per_line) / (float)per_line * (CUBE_RADIUS * 2.f) - CUBE_RADIUS;
	tmp.y = (id / per_line) / (float)per_line * (CUBE_RADIUS * 2.f) - CUBE_RADIUS;
	tmp.z = ((id % 6) >= 3) ? -CUBE_RADIUS : CUBE_RADIUS;
	if (side == 0)
		vertex[id].pos = (float4){tmp.x, tmp.y, tmp.z, 1.f};
	else if (side == 1)
		vertex[id].pos = (float4){tmp.x, tmp.z, tmp.y, 1.f};
	else
		vertex[id].pos = (float4){tmp.z, tmp.y, tmp.x, 1.f};
}

__kernel void		init_rand_sphere(__global p_vertex *vertex,
						__global p_info *info, uint initial_rand)
{
	uint			seed;
	uint const		id = get_global_id(0);
	float2			tmp;
	float			len;

	seed = init_rand(initial_rand);
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

	seed = init_rand(initial_rand);
	_init_particle(vertex + id, info + id);
	vertex[id].pos = (float4){
		frand(&seed) * 2.f - 1.f,
		frand(&seed) * 2.f - 1.f,
		frand(&seed) * 2.f - 1.f,
		1.f
	};
}
)";

char const		prog_particle_update[] = R"(
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   particle_update.cl                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/10/10 16:57:32 by jaguillo          #+#    #+#             */
/*   Updated: 2016/11/10 13:30:37 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   particle.clh                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juloo <juloo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/10/23 20:14:44 by juloo             #+#    #+#             */
/*   Updated: 2016/11/10 13:23:11 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARTICLE_CLH
# define PARTICLE_CLH

typedef struct p_vertex		p_vertex;
typedef struct p_info		p_info;

struct	p_vertex
{
	float4		pos;
	float4		color;
};

struct	p_info
{
	float4		velocity;
	float4		spring_pos;
	float		spring_k;
	float		pad[3];
};

# define P_MASS			1.f
# define P_DRAG			0.47f

# define P_RADIUS		5.f

# define P_AREA			((float)M_PI * P_RADIUS * P_RADIUS)

# define CENTER_MASS	3.f

#endif

#define G				10.f

__kernel void		update_gravity(__global p_vertex *vertex, __global p_info *info,
						float4 center, float delta_t)
{
	uint const			id = get_global_id(0);

	float4 const		vel_0 = info[id].velocity;
	float4 const		pos_0 = vertex[id].pos;

	float const			mid_delta_t = delta_t / 2.f;
	float4 const		pos_mid = pos_0 + vel_0 * mid_delta_t;

	float4 const		r_mid = center - pos_mid;
	float const			r_mid_sq = dot(r_mid, r_mid);
	float4 const		acc_mid = G * CENTER_MASS / (r_mid_sq + 0.5f) * r_mid;

	float4 const		vel_1 = vel_0 + acc_mid * delta_t;
	float4				pos_1 = pos_0 + (vel_0 + vel_1) * mid_delta_t;

	pos_1.w = 1.f;
	vertex[id].pos = pos_1;
	info[id].velocity = vel_1;
}

// Stokes' law
#define ATM_DENSITY			0.06f
#define ATM_RESISTANCE(V)	(6.f * (float)M_PI * ATM_DENSITY * P_RADIUS * (V))

__kernel void		update_spring(__global p_vertex *vertex,
						__global p_info *info, float4 center, float delta_t)
{
	uint const			id = get_global_id(0);

	float4 const		vel_0 = info[id].velocity;
	float4 const		pos_0 = vertex[id].pos;

	float4 const		r = pos_0 - (info[id].spring_pos + center);
	float4 const		acc_spring = r * (-info[id].spring_k / P_MASS);

	float4 const		acc = acc_spring - ATM_RESISTANCE(vel_0);

	float4 const		vel_1 = vel_0 + acc * delta_t;
	float4 const		pos_1 = pos_0 + vel_1 * delta_t;

	vertex[id].pos = (float4){pos_1.x, pos_1.y, pos_1.z, 1.f};
	info[id].velocity = vel_1;
}

__kernel void		explode(__global p_vertex *vertex, __global p_info *info,
						float4 center, float force)
{
	uint const			id = get_global_id(0);
	float4 const		d = center - vertex[id].pos;
	float const			f = force / max(1.f, dot(d, d));

	info[id].velocity -= normalize(d) * f;
}
)";

char const		prog_particle_render_vert[] = R"(
#version 150 core
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   particle_render.vert                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/10/14 12:55:11 by jaguillo          #+#    #+#             */
/*   Updated: 2016/10/23 20:37:11 by juloo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

in vec4			buff_pos;
in vec4			buff_color;

uniform mat4	u_matrix;

out vec4		p_color;

#define PARTICLE_SIZE		1.f

void		main()
{
	vec4		p_pos = u_matrix * buff_pos;
	float		p_size = PARTICLE_SIZE / p_pos.w;

	p_color = buff_color * min(1.f, p_size);
	gl_Position = p_pos;
	gl_PointSize = max(1.f, p_size);
}
)";

char const		prog_particle_render_frag[] = R"(
#version 150 core
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   particle_render.frag                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/10/20 15:52:06 by jaguillo          #+#    #+#             */
/*   Updated: 2016/10/23 20:31:32 by juloo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

in vec4			p_color;

out vec4		color;

// void		main()
// {
// 	vec2		r = gl_PointCoord - vec2(0.5f, 0.5f);

// 	color = p_color;
// 	gl_FragDepth = (dot(r, r) > 0.5f*0.5f) ? 255.f : gl_FragCoord.z;
// }

void		main() { color = p_color; }
)";
