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
				if (std::is_class_v<_Ty>) for (size_t i = 0; i < size; i++) p[i].~_Ty();
				free(src);
			}
		}

	};
}