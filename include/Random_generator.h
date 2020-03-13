#pragma once

#include <random>

class Random_generator {
	
public:

	Random_generator() : 
	_random_eng {_device()}
	{}

	template<typename T> 
	T generate_integer(T low, T high)
	{
		std::uniform_int_distribution<T> uni(low, high);
		return uni(_random_eng);
	}

	template<typename T> 
	T generate_real(T low, T high)
	{
		std::uniform_real_distribution<T> uni(low, high);
		return uni(_random_eng);
	}

	std::string generate_string(int length)
	{
		const std::string alphabet {
			"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789"};
		std::string result(length, 'a');
		std::for_each(result.begin(), result.end(), 
			[&](auto& elem) 
			{
				elem = alphabet[generate_integer(0, 60)]; 
			});
		return result;
	}

	bool generate_bool()
	{
		return static_cast<bool>(generate_integer(0 , 1));
	}

private:

	std::random_device _device;
	std::default_random_engine _random_eng;

};