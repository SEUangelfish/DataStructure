#pragma once
#include "SplayTree.h"
#include "Allocator.h"

namespace dsl {
	// SplayTree with the key as value
	// _Ty			key type
	// _Cmpr		comparator type of key
	// _Alloc		allocator template
	template<typename _Ty, typename _Cmpr = std::less<_Ty>, template<typename> typename _Alloc = RecycleAllocator>
	class SplaySet :public SplayTree<SplaySetNode<_Ty>, _Cmpr, _Alloc<SplaySetNode<_Ty>>> {
	public:
		using _Node = SplaySetNode<_Ty>;
		using _KTy = _Ty;
		
		using _ElemType = _Node;
		using _ElemAlloc = _Alloc<_ElemType>;
		using Iterator = SplayTreeIterator<SplayTree<_ElemType, _Cmpr, _ElemAlloc>>;

	public:
		// the same as Begin()
		// adapts to C++'s range-based for loops
		Iterator begin() {
			return this->Begin();
		}
		// the same as End()
		// adapts to C++'s range-based for loops
		Iterator end() {
			return this->End();
		}
		// the same as Successor()
		Iterator UpperBound(const _KTy& key) {
			return this->Successor(key);
		}

	};
}