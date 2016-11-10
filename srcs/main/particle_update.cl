/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   particle_update.cl                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/10/10 16:57:32 by jaguillo          #+#    #+#             */
/*   Updated: 2016/11/10 14:57:45 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "particle.clh"

#define G				10.f
#define CENTER_MASS		3.f

static float4		f_gravity(__global p_vertex const *vertex,
						__global p_info const *info, float4 center)
{
	float4 const		r = center - vertex->pos;

	return ((G * P_MASS * CENTER_MASS / (dot(r, r) + 0.5f)) * r);
}

static float4		f_spring(__global p_vertex const *vertex,
						__global p_info const *info, float4 center)
{
	return ((vertex->pos - (info->spring_pos + center)) * -info->spring_k);
}

// Stokes' law
static float4		f_drag(__global p_vertex const *vertex,
						__global p_info const *info)
{
	float const			viscosity = 0.06f;

	return ((6.f * (float)M_PI * viscosity * P_RADIUS) * info->velocity);
}

__kernel void		update(__global p_vertex *vertex,
						__global p_info *info, float4 center, float delta_t)
{
	uint const			id = get_global_id(0);

	float4 const		F = 0.f
								+ f_spring(vertex + id, info + id, center)
								+ f_gravity(vertex + id, info + id, center)
								- f_drag(vertex + id, info + id)
							;

	float4 const		vel_1 = info[id].velocity + (F / P_MASS) * delta_t;
	float4 const		pos_1 = vertex[id].pos + vel_1 * delta_t;

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
