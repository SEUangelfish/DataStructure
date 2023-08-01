#pragma once
#include "pch.h"

namespace dsl {

	// 快速换值，直接交换二进制流
	template<class _Ty>
	inline void Swap(_Ty& p, _Ty& q) {
		if (std::is_class_v<_Ty>) {
			static char buf[sizeof(_Ty)]{};
			std::copy_n((char*)&p, sizeof(_Ty), buf);
			std::copy_n((char*)&q, sizeof(_Ty), (char*)&p);
			std::copy_n(buf, sizeof(_Ty), (char*)&q);
		}
		else std::swap(p, q);
	}
}