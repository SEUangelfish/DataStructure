#pragma once
#include "pch.h"

namespace dsl {
	template<typename _Ty>
	class Allocater {
	public:
		// ��Դ���뺯��
		// �������κγ�ʼ������
		// cnt:		Ԫ�ظ���
		// ������Դָ��
		virtual inline _Ty* New(size_t cnt) {
			_Ty* res = (_Ty*)operator new(cnt * sizeof(_Ty));
#ifdef EXCEPTION_DETECTION
			if (!res) throw std::bad_alloc();
#endif // EXCEPTION_DETECTION
			return res;
		}

		// �ͷ���Դ
		virtual inline void Free(_Ty*& src, size_t size) {
			if (src) {
				if (std::is_class_v<_Ty>) for (_Ty* st = src, *ed = src + size; st != ed; ++st) st->~_Ty();
				operator delete(src);
				src = nullptr;
			}
		}

	};
}