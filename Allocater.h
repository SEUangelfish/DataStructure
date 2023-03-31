#pragma once
#include "pch.h"
#include "Macro.h"

namespace dsl {
	template<typename _Ty>
	class Allocater {
	public:
		// ��Դ���뺯��
		// ����ʧ�ܱ��쳣�������г�ʼ������
		// cnt:		Ԫ�ظ���
		// ������Դָ��
		virtual inline void* New(size_t cnt) {
			if (!cnt) return nullptr;
			void* p = malloc(sizeof(_Ty) * cnt);
			if (!p) throw std::bad_alloc();
			return p;
		}

		// �ͷ���Դ
		virtual inline void Free(void* src, size_t size) {
			if (src) {
				_Ty* p = (_Ty*)src;
				if (std::is_class_v<_Ty>) {
#ifdef MULTITHREADING_OPTIMIZATION
					if (size > BIG) {
						// ���߳��Ż�
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