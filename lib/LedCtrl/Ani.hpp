#pragma once
#include <Arduino.h>

class Ani
{
	public:
		Ani(String name)  		{_name = name;  reset();	};
		~Ani() = default;
        String getName()		{return _name;};
		virtual void reset()  	{};
        virtual void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData) {};


	private:
		String _name;
};
