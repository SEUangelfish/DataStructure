#pragma once
#include "pch.h"

namespace dsl {
	template<typename _Ty>
	class Allocater {
	public:
		// ��Դ���뺯��
		// ����ʧ�ܱ��쳣�������г�ʼ������
		// cnt:		Ԫ�ظ���
		// ������Դָ��
		virtual inline _Ty* New(size_t cnt) {
			return (_Ty*)new char[cnt * sizeof(_Ty)] {};
		}

		// �ͷ���Դ
		virtual inline void Free(_Ty*& src, size_t size) {
			if (src) {
				if (std::is_class_v<_Ty>) for (size_t i = 0; i < size; i++) src[i].~_Ty();
				delete[] src;
				src = nullptr;
			}
		}

	};
}