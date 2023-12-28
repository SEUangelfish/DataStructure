#pragma once
#include "SplaySet.h"

namespace dsl {
	template<typename _Key, typename _Value>
	class SplayMapNode
	{
		template<typename _Node, typename _Cmpr, typename _Alloc>
		friend class SplayTree;

		template<typename _DSTy>
		friend class SplayTreeIterator;

	public:
		using _KTy = _Key;
		using _VTy = _Value;
		using _Node = SplayMapNode<_Key, _Value>;
		using _ElemType = std::pair<_Key, _Value>;

	public:
		~SplayMapNode() = default;

		SplayMapNode() = default;
		//SplayMapNode(const _KTy& _key) :_SplaySetNode(_key) {}
		//SplayMapNode(const _KTy& _key, _Node* _fa) :_SplaySetNode(_key, _fa) {}
		//SplayMapNode(const _KTy& _key, _Node* _fa, _Node* leftChild, _Node* rightChild) : _SplaySetNode(_key, _fa, leftChild, rightChild) {}
		//SplayMapNode(_KTy&& _key) :_SplaySetNode(std::move(_key)) {}
		//SplayMapNode(_KTy&& _key, _Node* _fa) :_SplaySetNode(std::move(_key), _fa) {}
		//SplayMapNode(_KTy&& _key, _Node* _fa, _Node* leftChild, _Node* rightChild) :_SplaySetNode(std::move(_key), _fa, leftChild, rightChild) {}
		//SplayMapNode(SplayMapNode&& mv) noexcept :_SplaySetNode(std::move(mv)), value(std::move(value)) {}

		_KTy& Key() {
			return this->data.first;
		}

		_VTy& Value() {
			return this->data.second;
		}

		_ElemType& Data() {
			return this->data;
		}

	protected:
		_Node* fa = nullptr;
		_Node* ch[2]{ nullptr, nullptr };
		std::pair<_KTy, _Value> data;
	};

	template<typename _Key, typename _Value, typename _Cmpr = std::less<_Value>, template<typename> typename _Alloc = dsl::RecycleAllocator>
	class SplayMap
		: public SplayTree<SplayMapNode<_Key, _Value>, _Cmpr, _Alloc<SplayMapNode<_Key, _Value>>>
	{
	public:
		using _Node = typename SplayMapNode<_Key, _Value>::_Node;
		using _KTy = _Key;
		using _VTy = _Value;
		using _ElemType = _Node;
		using _ElemAlloc = _Alloc<_ElemType>;
		using _SplayTree = SplayTree<_Node, _Cmpr, _ElemAlloc>;
		using Iterator = SplayTreeIterator<_SplayTree>;



	};
}