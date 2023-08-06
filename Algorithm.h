#pragma once
#include "pch.h"

namespace dsl {

	// ���ٻ�ֵ��ֱ�ӽ�����������
	template<class _Ty>
	inline void Swap(_Ty& p, _Ty& q) {
		if (std::is_class_v<_Ty>) {
			static char buf[sizeof(_Ty)]{};
			memcpy(buf, &p, sizeof(_Ty));
			memcpy(&p, &q, sizeof(_Ty));
			memcpy(&q, buf, sizeof(_Ty));
		}
		else std::swap(p, q);
	}
}