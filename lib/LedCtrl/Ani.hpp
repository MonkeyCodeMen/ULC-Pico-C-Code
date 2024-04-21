#pragma once
#include <Arduino.h>
#include <helper.h>

#define ANI_OK								0
#define ANI_ERROR_GENERAL 					1
#define ANI_ERROR_PROGRAM_DOES_NOT_EXIST	2
#define ANI_ERROR_OUT_OF_RANGE     			3
#define ANI_ERROR_INTERNAL      			4
#define ANI_ERROR_PAR_MISSING				5
#define ANI_ERROR_FILE_NOT_FOUND			6
#define ANI_ERROR_OUT_OF_MEMORY				7


class Ani
{
	public:
		Ani(const char * pName)  																		{ _pName = pName;  reset();		}
		~Ani() = default;
        const char * getName()																			{ return _pName;				}
		virtual void reset()  																			{								}
        virtual int setup(uint32_t p1,uint32_t p2,uint32_t p3,uint32_t p4,String str,uint32_t length,uint8_t ** pData) 	{ return ANI_OK;				}
		virtual void trigger() 																			{								}
		static const char * getErrorText(int error) 													{
			switch (error){
				case ANI_OK:							return "OK";
				case ANI_ERROR_GENERAL:					return "general error";
				case ANI_ERROR_PROGRAM_DOES_NOT_EXIST:	return "program does not exist";
				case ANI_ERROR_OUT_OF_RANGE:			return "parameter out of range";
				case ANI_ERROR_INTERNAL:				return "internal error";
				case ANI_ERROR_PAR_MISSING:				return "parameter missing";
				case ANI_ERROR_FILE_NOT_FOUND:			return "file not found";
				case ANI_ERROR_OUT_OF_MEMORY:			return "out of memory";
			}
			return "unknow error code";
		}

	private:
		const char * _pName;
};
