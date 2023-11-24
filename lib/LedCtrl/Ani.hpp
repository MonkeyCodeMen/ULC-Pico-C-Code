#pragma once
#include <Arduino.h>
#include "helper.hpp"

#define ANI_OK								0
#define ANI_ERROR_GENERAL 					1
#define ANI_ERROR_PROGRAM_DOES_NOT_EXIST	2
#define ANI_ERROR_OUT_OF_RANGE     			3
#define ANI_ERROR_INTERNAL      			4
#define ANI_ERROR_PAR_MISSING				5
#define ANI_ERROR_FILE_NOT_FOUND			6


class Ani
{
	public:
		Ani(const char * pName)  		{_pName = pName;  reset();	};
		~Ani() = default;
        const char * getName()		{return _pName;};
		virtual void reset()  	{};
        virtual int setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,String str,u32_t length,u8_t ** pData) {return ANI_OK;};
		virtual void trigger() 	{};
		static const char * getErrorText(int error) {
			switch (error){
				case ANI_OK:							return F_CONST("OK");
				case ANI_ERROR_GENERAL:					return F_CONST("general error");
				case ANI_ERROR_PROGRAM_DOES_NOT_EXIST:	return F_CONST("program does not exist");
				case ANI_ERROR_OUT_OF_RANGE:			return F_CONST("parameter out of range");
				case ANI_ERROR_INTERNAL:				return F_CONST("internal error");
				case ANI_ERROR_PAR_MISSING:				return F_CONST("parameter missing");
				case ANI_ERROR_FILE_NOT_FOUND:			return F_CONST("file not found");
			}
			return F_CONST("unknow error code");
		}

	private:
		const char * _pName;
};
