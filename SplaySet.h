#pragma once
#include "SplayTree.h"

namespace dsl {
	// ��ֵ��ͬ����չ��	
	// _Ty			��ֵ����
	// _Cmpr		�Ƚ���
	// _Alloc		������ģ��
	template<typename _Ty, typename _Cmpr = std::less<_Ty>, template<class> typename _Alloc = dsl::RecycleAllocater>
	class SplaySet
		: public SplayTree<SplaySetNode<_Ty>, _Cmpr, _Alloc<SplaySetNode<_Ty>>> {

	};
}