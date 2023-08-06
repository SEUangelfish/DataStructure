#pragma once
#include "pch.h"

namespace dsl {
	template<typename _Ty>
	class Allocater {
	public:
		// 资源申请函数
		// 不进行任何初始化操作
		// cnt:		元素个数
		// 返回资源指针
		virtual inline _Ty* New(size_t cnt) {
			_Ty* res = (_Ty*)operator new(cnt * sizeof(_Ty));
#ifdef EXCEPTION_DETECTION
			if (!res) throw std::bad_alloc();
#endif // EXCEPTION_DETECTION
			return res;
		}

		// 释放资源
		virtual inline void Free(_Ty*& src, size_t size) {
			if (src) {
				if (std::is_class_v<_Ty>) for (_Ty* st = src, *ed = src + size; st != ed; ++st) st->~_Ty();
				operator delete(src);
				src = nullptr;
			}
		}

	};
}