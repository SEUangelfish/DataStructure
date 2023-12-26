#pragma once
#include "SplayTree.h"

namespace dsl {
	template<typename _Key, typename _Val>
	class SplayMapNode : public SplaySetNode<_Key> {

	};

	//template<typename _Key>
	//class SplaySetNode {
	//	template<typename _Node, typename _Cmpr, typename _Alloc>
	//	friend class SplayTree;

	//	template<typename _DSTy>
	//	friend class SplayTreeIterator;

	//public:
	//	using _KTy = _Key;
	//	using _Node = SplaySetNode<_KTy>;

	//public:
	//	~SplaySetNode() = default;

	//	SplaySetNode() = default;
	//	SplaySetNode(const _KTy& _key) :key(_key) {}
	//	SplaySetNode(const _KTy& _key, _Node* _fa) :fa(_fa), key(_key) {}
	//	SplaySetNode(const _KTy& _key, _Node* _fa, _Node* leftChild, _Node* rightChild) :fa(_fa), ch{ leftChild, rightChild }, key(_key) {}
	//	SplaySetNode(_KTy&& _key) :key(std::move(_key)) {}
	//	SplaySetNode(_KTy&& _key, _Node* _fa) :fa(_fa), key(std::move(_key)) {}
	//	SplaySetNode(_KTy&& _key, _Node* _fa, _Node* leftChild, _Node* rightChild) :fa(_fa), ch{ leftChild, rightChild }, key(std::move(_key)) {}
	//	SplaySetNode(SplaySetNode&& mv) noexcept :fa(mv.fa), ch{ mv.ch[0], mv.ch[1] }, key(std::move(mv.key)) {
	//		memset(&mv, 0, sizeof(SplaySetNode));
	//	}

	//	SplaySetNode(const _Node& cp) = delete;
	//	_Node& operator=(const _Node& cp) = delete;

	//	_KTy& Key() {
	//		return this->key;
	//	}

	//protected:
	//	_Node* fa = nullptr;
	//	_Node* ch[2]{ nullptr, nullptr };
	//	_KTy key;
	//};
}