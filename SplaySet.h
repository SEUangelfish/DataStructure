#pragma once
#include "SplayTree.h"

namespace dsl {
	// 键值相同的伸展树	
	// _Ty			键值类型
	// _Cmpr		比较器
	// _Alloc		分配器模板
	template<typename _Ty, typename _Cmpr = std::less<_Ty>, template<class> typename _Alloc = dsl::RecycleAllocater>
	class SplaySet
		: public SplayTree<SplaySetNode<_Ty>, _Cmpr, _Alloc<SplaySetNode<_Ty>>> {

	};
}