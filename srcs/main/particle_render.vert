#version 410 core
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   particle_render.vert                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/10/14 12:55:11 by jaguillo          #+#    #+#             */
/*   Updated: 2016/10/14 13:53:32 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

layout (location = 0) in vec4		buff_pos;
layout (location = 1) in vec4		buff_color;

uniform mat4	u_matrix;

out vec4		p_color;

#define PARTICLE_SIZE		1.f

void		main()
{
	vec4		p_pos = u_matrix * buff_pos;
	float		p_size = PARTICLE_SIZE / p_pos.w;

	p_color = buff_color * min(1.f, p_size); // ^2 ? sqrt ?
	gl_Position = p_pos;
	gl_PointSize = max(1.f, p_size);
}
