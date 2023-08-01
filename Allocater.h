#pragma once
#include "pch.h"

namespace dsl {
	template<typename _Ty>
	class Allocater {
	public:
		// 资源申请函数
		// 申请失败报异常但不进行初始化操作
		// cnt:		元素个数
		// 返回资源指针
		virtual inline _Ty* New(size_t cnt) {
			return (_Ty*)new char[cnt * sizeof(_Ty)] {};
		}

		// 释放资源
		virtual inline void Free(_Ty*& src, size_t size) {
			if (src) {
				if (std::is_class_v<_Ty>) for (size_t i = 0; i < size; i++) src[i].~_Ty();
				delete[] src;
				src = nullptr;
			}
		}

	};
}