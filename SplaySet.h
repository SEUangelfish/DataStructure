#pragma once
#include "SplayTree.h"
#include "Allocator.h"




namespace dsl {
	// ��ֵ��ͬ����չ��	
	// _Ty			��ֵ����
	// _Cmpr		�Ƚ���
	// _Alloc		������ģ��
	template<typename _Ty, typename _Cmpr = std::less<_Ty>, template<typename> typename _Alloc = RecycleAllocator>
	class SplaySet :public SplayTree<SplaySetNode<_Ty>, _Cmpr, _Alloc<SplaySetNode<_Ty>>> {
	public:
		// �������
		using _Node = SplaySetNode<_Ty>;
		// ������
		using _KTy = _Ty;
		// Ԫ������
		using _ElemType = _Node;
		// ����������
		using _ElemAlloc = _Alloc<_ElemType>;
		// ����������
		using Iterator = SplayTreeIterator<SplayTree<_ElemType, _Cmpr, _ElemAlloc>>;

	public:
		// ������Begin��ͬ
		// ��ӦC++�Ļ�����Χforѭ��
		Iterator begin() {
			return this->Begin();
		}
		// ������End��ͬ
		// ��ӦC++�Ļ�����Χforѭ��
		Iterator end() {
			return this->End();
		}
		// ���ص�һ������key�Ľڵ�
		// key����
		// �������򷵻�β������
		Iterator UpperBound(const _KTy& key) {
			return this->Successor(key);
		}

	};
}