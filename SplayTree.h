#pragma once
#include "Allocator.h"
#include "Iterator.h"

#define DEPTH_THRESHOULD		32u

namespace dsl {
	template<typename _Key>
	class SplaySetNode {
		template<typename _Node, typename _Cmpr, typename _Alloc>
		friend class SplayTree;

		template<typename _DSTy>
		friend class SplayTreeIterator;

	public:
		using _KTy = _Key;
		using _Node = SplaySetNode<_KTy>;

	public:
		~SplaySetNode() = default;

		SplaySetNode() = default;
		SplaySetNode(const _KTy& _key) :key(_key) {}
		SplaySetNode(_KTy&& _key) :key(std::move(_key)) {}

		SplaySetNode(const _Node& cp) = delete;
		_Node& operator=(const _Node& cp) = delete;

		_KTy& Key() {
			return this->key;
		}

	protected:
		_Node* fa = nullptr;
		_Node* ch[2]{ nullptr, nullptr };
		_KTy key;
	};

	// SplayTree base class	
	// _Node		node type which contains key, value and other essential information
	// _Cmpr		comparator of key
	// _Alloc		allocator type of node
	template<typename _Node, typename _Cmpr, typename _Alloc>
	class SplayTree {
	public:
		using _ElemType = _Node;
		using _ElemAlloc = _Alloc;
		using _KTy = typename _Node::_KTy;
		using Iterator = SplayTreeIterator<SplayTree<_ElemType, _Cmpr, _ElemAlloc>>;

	protected:
		// expansion interface
		virtual void PushUp(_Node* x) {}
		// expansion interface
		virtual void PushDown(_Node* x) {}

		// zigzag
		void Rotate(_Node* x) {
			_Node* y = x->fa, * z = y->fa;
			bool idx = x == y->ch[1];
			y->ch[idx] = x->ch[!idx];
			y->fa = x;
			if (x->ch[!idx]) x->ch[!idx]->fa = y;
			x->ch[!idx] = y;
			x->fa = z;
			if (z) z->ch[z->ch[1] == y] = x;
			PushUp(y);
			PushUp(x);
		}

		// merge left and right subtrees
		_Node* Combine(_Node* left, _Node* right) {
			if (!left) return right;
			if (!right) return left;
			left->fa = right->fa = nullptr;
			while (left->ch[1]) left = left->ch[1];

			while (left->fa) {
				_Node* y = left->fa, * z = y->fa;
				if (z) {
					if ((left == y->ch[1]) == (y == z->ch[1])) this->Rotate(y);
					else this->Rotate(left);
				}
				this->Rotate(left);
			}

			left->ch[1] = right;
			right->fa = left;
			this->PushUp(left);
			return left;
		}

	public:
		SplayTree() : root(this->alloc.New(1)), sentry(root) {
			// attention: sentry node do not call construction and destructor
			memset(this->root, 0, sizeof(_Node));
		};

		//SplayTree(const SplayTree& cp) :alloc(cp.alloc), cpr(cp.cpr), size(cp.size) {
		//	_Node* head = this->root = this->alloc.New(1), *tail = head;
		//	head->ch[0] = 

		//}

		SplayTree(SplayTree&& mv) noexcept :alloc(std::move(mv.alloc)), cpr(std::move(mv.cpr)), root(mv.root), sentry(mv.sentry), size(mv.size) {
			memset(&mv, 0, sizeof(SplayTree));
		}

		SplayTree& operator=(const SplayTree& mv) {
			this->~SplayTree();
			new (this) SplayTree(mv);
			return *this;
		}

		SplayTree& operator=(SplayTree&& mv)noexcept {
			this->~SplayTree();
			new (this) SplayTree(std::move(mv));
			return *this;
		}

		virtual ~SplayTree() {
			if (!this->root) return;

			_Node* head = this->root, * tail = this->root, * next;
			while (1) {
				if (head->ch[0]) tail = tail->fa = head->ch[0];
				if (head->ch[1]) tail = tail->fa = head->ch[1];
				next = head->fa;
				this->alloc.Free(head, head != this->sentry);
				if (head == tail) break;
				head = next;
			};
		}

		// rotate x to the child of fa
		// set fa to nullptr if want x to rotate to root
		void Splay(_Node* x, _Node* fa) {
#ifdef EXCEPTION_DETECTION
			if (!x) throw std::exception("subclass object of SplayTree: nullptr x by Spaly(x)");
#endif // EXCEPTION_DETECTION
			while (x->fa != fa) {
				_Node* y = x->fa, * z = y->fa;
				if (z != fa) {
					if ((x == y->ch[1]) == (y == z->ch[1])) this->Rotate(y);
					else this->Rotate(x);
				}
				this->Rotate(x);
			}
			if (!fa) this->root = x;
		}

		// rotate x to the depth DEPTH_THRESHOULD
		void Splay(_Node* x) {
#ifdef EXCEPTION_DETECTION
			if (!x) throw std::exception("subclass object of SplayTree: nullptr x by Spaly(x, ancestor)");
#endif // EXCEPTION_DETECTION

			_Node* ancestor = x;
			for (unsigned i = DEPTH_THRESHOULD; i && ancestor; --i) ancestor = ancestor->fa;
			while (ancestor) {
				_Node* y = x->fa, * z = y->fa;
				if (z) {
					if ((x == y->ch[1]) == (y == z->ch[1])) this->Rotate(y);
					else this->Rotate(x);
					ancestor = ancestor->fa;
					if (!ancestor) return;
				}
				this->Rotate(x);
				ancestor = ancestor->fa;
			}
		}

		// node comparison operator
		// ture if n1 < n2 
		bool operator ()(_Node* n1, _Node* n2) {
			return n1 != this->sentry && (n2 == this->sentry || this->cpr(n1->key, n2->key));
		}
		// node comparison operator
		// true if node < key 
		bool operator ()(_Node* node, const _KTy& key) {
			return node != this->sentry && this->cpr(node->key, key);
		}
		// node comparison operator
		// true if key < node
		bool operator ()(const _KTy& key, _Node* node) {
			return node == this->sentry || this->cpr(key, node->key);
		}

		void Clear() {
			_Node* head = this->root, * tail = this->root;

			while (1) {
				if (head->ch[0]) tail = tail->fa = head->ch[0];
				if (head->ch[1]) tail = tail->fa = head->ch[1];
				this->root = head->fa;
				if (head != this->sentry) this->alloc.Free(head, 1);
				if (head == tail) break;
				head = this->root;
			};

			this->root = this->sentry;
			this->root->fa = this->root->ch[0] = nullptr;
			this->size = 0;
		}

		size_t Size() { return this->size; }

		// true if empty
		bool Empty() { return !this->size; }

		_Node* Root() { return this->root; }

		Iterator Begin() {
			_Node* u = this->root;
			while (u->ch[0]) u = u->ch[0];
			this->Splay(u);
			return { u, this };
		}

		Iterator End() {
			return { this->sentry, this };
		}

		// find node by key
		// return End() if such node do not exist 
		Iterator Find(const _KTy& key) {
			_Node* u = this->root;
			bool idx;
			while (true) {
				if (this->operator()(u, key) == this->operator()(key, u)) {
					this->Splay(u);
					return { u, this };
				}
				idx = this->operator()(u, key);
				if (!u->ch[idx]) {
					this->Splay(u);
					return { this->sentry, this };
				}
				u = u->ch[idx];
			}
			this->Splay(u);
			return { u, this };
		}

		// precursor of key
		// return End() if such node do not exist 
		Iterator Precursor(const _KTy& key) {
			_Node* pre = nullptr, * u = this->root;
			while (u) {
				if (this->operator()(u, key)) {
					pre = u;
					u = u->ch[1];
					while (u) {
						if (this->operator()(u, key)) {
							pre = u;
							u = u->ch[1];
						}
						else u = u->ch[0];
					}
				}
				else if (!u->ch[0]) {
					this->Splay(u);
					return { this->sentry, this };
				}
				else u = u->ch[0];
			}
			this->Splay(pre);
			return { pre, this };
		}

		// successor of key
		// return End() if such node do not exist 
		Iterator Successor(const _KTy& key) {
			_Node* suc = nullptr, * u = this->root;
			while (u) {
				if (this->operator()(key, u)) {
					suc = u;
					u = u->ch[0];
				}
				else u = u->ch[1];
			}
			this->Splay(suc);
			return { suc, this };
		}

		// return the first node >= key
		// return End() if such node do not exist 
		Iterator LowerBound(const _KTy& key) {
			_Node* suc = nullptr, * u = this->root;
			while (u) {
				if (this->operator()(u, key) == this->operator()(key, u)) {
					this->Splay(u);
					return { u, this };
				}
				if (this->operator()(key, u)) {
					suc = u;
					u = u->ch[0];
				}
				else u = u->ch[1];
			}
			this->Splay(suc);
			return { suc, this };
		}

		std::pair<Iterator, bool> Insert(const _KTy& key) {
			return this->Emplace(key);
		}
		std::pair<Iterator, bool> Insert(_KTy&& key) {
			return this->Emplace(std::move(key));
		}
		template <typename _Iter>
		void Insert(_Iter begin, _Iter end) {
			while (begin != end) {
				this->Emplace(*begin);
				++begin;
			}
		}
		void Insert(std::initializer_list<_KTy> vlist) {
			this->Insert(vlist.begin(), vlist.end());
		}

		// construct the node and insert it
		// args: parameters for constructing
		// the second parameter of return value means success or not
		// if suceess, the first parameter is the iterator of new node
		// if fail, the first parameter is the iterator of conflicting node
		template<typename... _Args>
		std::pair<Iterator, bool> Emplace(_Args&&... args) {
			_Node* pre = nullptr, * u = this->root, * v = this->alloc.New(1);

			new (v) _Node(_KTy(std::forward<_Args>(args)...));

			while (u) {
				if (this->operator()(u, v) == this->operator()(v, u)) {
					this->alloc.Free(v, 1);
					this->Splay(u);
					return std::make_pair(Iterator(u, this), false);
				}
				pre = u;
				u = u->ch[this->operator()(u, v)];
			}

			pre->ch[this->operator()(pre, v)] = v;
			v->fa = pre;
			this->Splay(v);
			++this->size;
			return std::make_pair(Iterator(v, this), true);
		}

		void Erase(Iterator itr) {
#ifdef EXCEPTION_DETECTION
			if (itr.Source() == this->sentry) throw std::exception("object of SplayTree£ºinvalid iterator by Erase()");
#endif // EXCEPTION_DETECTION
			_Node* tmp = this->Combine(itr->ch[0], itr->ch[1]);
			if (tmp) tmp->fa = itr->fa;
			if (itr->fa) itr->fa->ch[this->operator()(itr->fa, itr.Source())] = tmp;
			else this->root = tmp;
			--this->size;
			this->alloc.Free(itr.Source(), 1);
		}

		bool Erase(const _KTy& key) {
			_Node* x = this->Find(key).Source();
			if (x == this->sentry) return false;
			_Node* tmp = this->Combine(x->ch[0], x->ch[1]);
			if (tmp) tmp->fa = x->fa;
			if (x->fa) x->fa->ch[this->operator()(x->fa, x)] = tmp;
			else this->root = tmp;
			--this->size;
			this->alloc.Free(x, 1);
			return true;
		}

		bool Contains(const _KTy& key) {
			return this->Find(key).Source() != this->sentry;
		}

	protected:
		_ElemAlloc alloc;
		_Cmpr cpr;
		_Node* root;
		_Node* sentry;
		size_t size = 0;
	};
}