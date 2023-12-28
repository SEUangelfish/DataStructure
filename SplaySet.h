#pragma once
#include "SplayTree.h"

namespace dsl {
	template<typename _Ty>
	class SplaySetNode {
		template<typename _Node, typename _Cmpr, typename _Alloc>
		friend class SplayTree;

		template<typename _DSTy>
		friend class SplayTreeIterator;

	public:
		using _KTy = _Ty;
		using _VTy = _Ty;
		using _Node = SplaySetNode<_Ty>;
		using _ElemType = _Ty;

	public:
		~SplaySetNode() = default;

		SplaySetNode() = default;
		SplaySetNode(const _KTy& _key) :data(_key) {}
		SplaySetNode(const _KTy& _key, _Node* _fa) :fa(_fa), data(_key) {}
		SplaySetNode(const _KTy& _key, _Node* _fa, _Node* leftChild, _Node* rightChild) :fa(_fa), ch{ leftChild, rightChild }, data(_key) {}
		SplaySetNode(_KTy&& _key) :data(std::move(_key)) {}
		SplaySetNode(_KTy&& _key, _Node* _fa) :fa(_fa), data(std::move(_key)) {}
		SplaySetNode(_KTy&& _key, _Node* _fa, _Node* leftChild, _Node* rightChild) :fa(_fa), ch{ leftChild, rightChild }, data(std::move(_key)) {}
		SplaySetNode(SplaySetNode&& mv) noexcept :fa(mv.fa), ch{ mv.ch[0], mv.ch[1] }, data(std::move(mv.key)) {
			mv.fa = mv.ch[0] = mv.ch[1] = nullptr;
		}

		_KTy& Key() {
			return this->data;
		}

		_VTy& Value() {
			return this->data;
		}

		_ElemType& Data() {
			return this->data;
		}

	protected:
		_Node* fa = nullptr;
		_Node* ch[2]{ nullptr, nullptr };
		_KTy data;
	};

	// SplayTree with the key as value
	// _Ty			key type
	// _Cmpr		comparator type of key
	// _Alloc		allocator template
	template<typename _Ty, typename _Cmpr = std::less<_Ty>, template<typename> typename _Alloc = RecycleAllocator>
	class SplaySet
		:public SplayTree<SplaySetNode<_Ty>, _Cmpr, _Alloc<SplaySetNode<_Ty>>>
	{
	public:
		using _Node = SplaySetNode<_Ty>;
		using _KTy = _Ty;
		using _ElemType = _Node;
		using _ElemAlloc = _Alloc<_ElemType>;
		using _SplayTree = SplayTree<_Node, _Cmpr, _ElemAlloc>;
		using Iterator = SplayTreeIterator<_SplayTree>;

	public:
		SplaySet() = default;
		explicit SplaySet(const _Cmpr& _cpr) : _SplayTree(_cpr) {};
		explicit SplaySet(const _ElemAlloc& _alloc) :_SplayTree(_alloc) {};
		explicit SplaySet(_ElemAlloc&& _alloc) :_SplayTree(std::move(_alloc)) {};
		SplaySet(const _ElemAlloc& _alloc, const _Cmpr& _cpr) : _SplayTree(_alloc, _cpr) {};
		SplaySet(_ElemAlloc&& _alloc, const _Cmpr& _cpr) :_SplayTree(std::move(_alloc), _cpr) {};

		SplaySet(const SplaySet& cp) : _SplayTree(cp) {}
		SplaySet(SplaySet&& mv) noexcept :_SplayTree(std::move(mv)) {}

		SplaySet& operator=(const SplaySet& cp) {
			this->_SplayTree::operator=(cp);
			return *this;
		}

		SplaySet& operator=(SplaySet&& mv) noexcept {
			this->_SplayTree::operator=(std::move(mv));
			return *this;
		}

		virtual ~SplaySet() = default;

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