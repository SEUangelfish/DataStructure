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

		SplayMapNode(const _ElemType& _data) :data(_data) {}
		SplayMapNode(const _KTy& _key) {
			this->data.first = _key;
		}
		SplayMapNode(_KTy&& _key) {
			this->data.first = std::move(_key);
		}
		SplayMapNode(const _KTy& _key, const _VTy& _val) :data(_key, _val) {}
		SplayMapNode(const _KTy& _key, _VTy&& _val) :data(_key, std::move(_val)) {}
		SplayMapNode(_KTy&& _key, const _VTy& _val) :data(std::move(_key), _val) {}
		SplayMapNode(_KTy&& _key, _VTy&& _val) :data(std::move(_key), std::move(_val)) {}
		SplayMapNode(_ElemType&& _data) :data(std::move(_data)) {}
		SplayMapNode(SplayMapNode&& mv) noexcept :fa(mv.fa), ch{ mv.ch[0], mv.ch[1] }, data(std::move(mv.data)) {
			mv.fa = mv.ch[0] = mv.ch[1] = nullptr;
		}

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

	// _Key			key type
	// _Value		value type
	// _Cmpr		comparator type of key
	// _Alloc		allocator template
	template<typename _Key, typename _Value, typename _Cmpr = std::less<_Key>, template<typename> typename _Alloc = Allocator>
	class SplayMap
		: public SplayTree<SplayMapNode<_Key, _Value>, _Cmpr, _Alloc<SplayMapNode<_Key, _Value>>>
	{
	public:
		using _Node = SplayMapNode<_Key, _Value>;
		using _KTy = _Key;
		using _VTy = _Value;
		using _ElemType = _Node;
		using _ElemAlloc = _Alloc<_ElemType>;
		using _SplayTree = SplayTree<_Node, _Cmpr, _ElemAlloc>;
		using Iterator = SplayTreeIterator<_SplayTree>;

	public:
		SplayMap() = default;
		explicit SplayMap(const _Cmpr& _cpr) : _SplayTree(_cpr) {};
		explicit SplayMap(const _ElemAlloc& _alloc) :_SplayTree(_alloc) {};
		explicit SplayMap(_ElemAlloc&& _alloc) :_SplayTree(std::move(_alloc)) {};
		SplayMap(const _ElemAlloc& _alloc, const _Cmpr& _cpr) : _SplayTree(_alloc, _cpr) {};
		SplayMap(_ElemAlloc&& _alloc, const _Cmpr& _cpr) :_SplayTree(std::move(_alloc), _cpr) {};

		SplayMap(const SplayMap& cp) : _SplayTree(cp) {}
		SplayMap(SplayMap&& mv) noexcept :_SplayTree(std::move(mv)) {}

		SplayMap& operator=(const SplayMap& cp) {
			this->_SplayTree::operator=(cp);
			return *this;
		}

		SplayMap& operator=(SplayMap&& mv) noexcept {
			this->_SplayTree::operator=(std::move(mv));
			return *this;
		}

		~SplayMap() = default;

		_VTy& operator[](const _KTy& key) {
			return this->Emplace(key).first->second;
		}

	};
}