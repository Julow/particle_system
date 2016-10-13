// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   FpsCounter.tpp                                     :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/12 17:38:53 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/13 12:16:58 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef FPSCOUNTER_TPP
# define FPSCOUNTER_TPP

template<typename C, unsigned R>
FpsCounter<C, R>::FpsCounter() :
	_frames{},
	_i(0),
	_frame_begin(),
	_last_update(),
	_global{}
{}

template<typename C, unsigned R>
void		FpsCounter<C, R>::begin(FpsCounter<C, R>::time_point_t const &now)
{
	_frame_begin = now;
}

template<typename C, unsigned R>
bool		FpsCounter<C, R>::end(FpsCounter<C, R>::time_point_t const &now)
{
	static const FpsCounter<C, R>::duration_t	RES_INTERVAL =
			std::chrono::duration<long long, std::ratio<1, R>>(1);
	FpsCounter<C, R>::duration_t const			delta_t = now - _frame_begin;

	if (_global.begin == time_point_t())
	{
		_global = {_frame_begin, 0, delta_t, delta_t, duration_t()};
		for (frame &f : _frames)
			f = {_frame_begin, 0, delta_t, delta_t, duration_t()};
	}
	_last_update = now;
	if ((_frames[_i].begin + RES_INTERVAL) <= now)
	{
		_global.count += _frames[_i].count;
		_global.min_t = std::min(_frames[_i].min_t, _global.min_t);
		_global.max_t = std::max(_frames[_i].max_t, _global.max_t);
		_global.total_t += _frames[_i].total_t;
		if (++_i >= BUFF_SIZE)
			_i = 0;
		_frames[_i] = {now, 1, delta_t, delta_t, delta_t};
		return (true);
	}
	_frames[_i].count++;
	_frames[_i].total_t += delta_t;
	if (_frames[_i].min_t > delta_t) _frames[_i].min_t = delta_t;
	else if (_frames[_i].max_t < delta_t) _frames[_i].max_t = delta_t;
	return (false);
}

template<typename C, unsigned R>
float		FpsCounter<C, R>::get_fps() const
{
	unsigned const		last = (_i + 1) % BUFF_SIZE;
	s_duration_t const	delta = _last_update - _frames[last].begin;
	unsigned			total_count;

	total_count = 0;
	for (frame const &f : _frames)
		total_count += f.count;
	return (total_count / delta.count());
}

template<typename C, unsigned R>
std::tuple<float, float, float>
			FpsCounter<C, R>::get_stats() const
{
	s_duration_t	min_t;
	s_duration_t	max_t;
	s_duration_t	total_t{};
	unsigned		count{};

	min_t = _frames[0].min_t;
	max_t = _frames[0].max_t;
	for (frame const &f : _frames)
	{
		if (min_t > f.min_t) min_t = f.min_t;
		else if (max_t < f.max_t) max_t = f.max_t;
		total_t += f.total_t;
		count += f.count;
	}
	return {min_t.count(), max_t.count(), total_t.count() / count};
}

template<typename C, unsigned R>
float		FpsCounter<C, R>::get_global_fps() const
{
	s_duration_t const	delta = _last_update - _global.begin;

	return (_global.count / delta.count());
}

template<typename C, unsigned R>
std::tuple<float, float, float>
			FpsCounter<C, R>::get_global_stats() const
{
	s_duration_t const	min_t = _global.min_t;
	s_duration_t const	max_t = _global.max_t;
	s_duration_t const	total_t = _global.total_t;

	return {min_t.count(), max_t.count(), total_t.count() / _global.count};
}

#endif
