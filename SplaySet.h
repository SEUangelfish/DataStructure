#pragma once
#include "SplayTree.h"
#include "Allocator.h"




namespace dsl {
	// 键值相同的伸展树	
	// _Ty			键值类型
	// _Cmpr		比较器
	// _Alloc		分配器模板
	template<typename _Ty, typename _Cmpr = std::less<_Ty>, template<typename> typename _Alloc = RecycleAllocator>
	class SplaySet :public SplayTree<SplaySetNode<_Ty>, _Cmpr, _Alloc<SplaySetNode<_Ty>>> {
	public:
		// 结点类型
		using _Node = SplaySetNode<_Ty>;
		// 键类型
		using _KTy = _Ty;
		// 元素类型
		using _ElemType = _Node;
		// 分配器类型
		using _ElemAlloc = _Alloc<_ElemType>;
		// 迭代器类型
		using Iterator = SplayTreeIterator<SplayTree<_ElemType, _Cmpr, _ElemAlloc>>;

	public:
		// 功能与Begin相同
		// 适应C++的基本范围for循环
		Iterator begin() {
			return this->Begin();
		}
		// 功能与End相同
		// 适应C++的基本范围for循环
		Iterator end() {
			return this->End();
		}
		// 返回第一个大于key的节点
		// key：键
		// 不存在则返回尾迭代器
		Iterator UpperBound(const _KTy& key) {
			return this->Successor(key);
		}

	};
}