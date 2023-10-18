#pragma once
#include <Arduino.h>

class Ani
{
	public:
		Ani(const char * pName)  		{_pName = pName;  reset();	};
		~Ani() = default;
        const char * getName()		{return _pName;};
		virtual void reset()  	{};
        virtual void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData) {};
		virtual void trigger() 	{};

	private:
		const char * _pName;
};
