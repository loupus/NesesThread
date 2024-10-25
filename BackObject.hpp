#pragma once
#include <string>
#include <iostream>
#include <typeinfo>
#include <any>
namespace NesesUtils
{
	struct BackObject
	{
		bool Success = true;
		int ErrCode = 0;
		std::string ErrDesc;
		std::string Warning;
		std::any AnyValue;

		friend std::ostream& operator << (std::ostream& out, const BackObject& c)
		{
			out << "Success: " << c.Success << '\n'
				<< "ErrCode: " << c.ErrCode << '\n'
				<< "ErrDesc: " << c.ErrDesc << '\n'
				<< "Warning: " << c.ErrDesc << '\n'
				<< "Value: " << (c.AnyValue.type() == typeid(std::string) ? std::any_cast<std::string>(c.AnyValue) : "") << std::endl;
			return out;
		}

		void Reset()
		{
			Success = true;
			ErrCode = 0;
			ErrDesc.clear();
			Warning.clear();
			AnyValue.reset();
		}
	};
}