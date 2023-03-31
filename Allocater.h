#pragma once
#include "pch.h"
#include "Macro.h"

namespace dsl {
	template<typename _Ty>
	class Allocater {
	public:
		// 资源申请函数
		// 申请失败报异常但不进行初始化操作
		// cnt:		元素个数
		// 返回资源指针
		virtual inline void* New(size_t cnt) {
			if (!cnt) return nullptr;
			void* p = malloc(sizeof(_Ty) * cnt);
			if (!p) throw std::bad_alloc();
			return p;
		}

		// 释放资源
		virtual inline void Free(void* src, size_t size) {
			if (src) {
				_Ty* p = (_Ty*)src;
				if (std::is_class_v<_Ty>) {
#ifdef MULTITHREADING_OPTIMIZATION
					if (size > BIG) {
						// 多线程优化
						std::thread t([p](size_t n) { for (size_t i = 0; i < n >> 1; i++) p[i].~_Ty(); }, size);
						for (size_t i = size >> 1; i < size; i++) p[i].~_Ty();
						t.join();
					}
					else
#endif // MULTITHREADING_OPTIMIZATION
						for (size_t i = 0; i < size; i++) p[i].~_Ty();
				}
				free(src);
			}
		}

	};
}