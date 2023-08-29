#pragma once
#include "pch.h"
#include "Allocator.h"
#include "Iterator.h"

namespace dsl {
	// 伸展树结点
	// _Key：键类型(键、值相同)
	template<typename _Key>
	class SplaySetNode {
		template<typename _Node, typename _Cmpr, typename _Alloc>
		friend class SplayTree;

		template<typename _DSTy>
		friend class SplayTreeIterator;

	public:
		// 键类型
		using _KTy = _Key;
		// 节点类型
		using _Node = SplaySetNode<_KTy>;

	public:
		virtual ~SplaySetNode() = default;

		// 默认构造
		SplaySetNode() = default;
		// 初始化键
		SplaySetNode(const _KTy& _key) :key(_key) {}
		// 初始化键
		SplaySetNode(_KTy&& _key) :key(std::move(_key)) {}

		// 拷贝构造
		SplaySetNode(const _Node& cp) :fa(cp.fa), ch{ cp.ch[0], cp.ch[1] }, key(cp.key){}
		// 移动构造
		SplaySetNode(_Node&& cp) noexcept :fa(cp.fa), ch{ cp.ch[0], cp.ch[1] }, key(std::move(cp.key)){
			memset(&cp, sizeof(_Node), 0);
		}

		_Node& operator=(const _Node& cp) = delete;
		_Node& operator=(_Node&& cp) = delete;

		// 获取键
		// 尾节点无键、值
		_KTy& Key() {
			return this->key;
		}

	protected:
		// 父节点
		_Node* fa = nullptr;
		// 0：左孩子
		// 1：右孩子
		_Node* ch[2]{ nullptr, nullptr };
		// 键
		_KTy key;
	};

	// 伸展树根类	
	// _Node		节点类型
	// _Cmpr		比较器类型
	// _Alloc		分配器类型
	template<typename _Node, typename _Cmpr, typename _Alloc>
	class SplayTree {
	public:
		// 元素类型
		using _ElemType = _Node;
		// 分配器类型
		using _ElemAlloc = _Alloc;
		// 键类型
		using _KTy = typename _Node::_KTy;
		// 迭代器类型
		using Iterator = SplayTreeIterator<SplayTree<_ElemType, _Cmpr, _ElemAlloc>>;

	protected:
		// 向上维护
		// x：维护的结点
		virtual void PushUp(_Node* x) {
			// 具体功能通过多态实现
			// 重载时注意尾结点
		}

		// 向下维护
		// x：维护的结点
		virtual void PushDown(_Node* x) {
			// 具体功能通过多态实现
			// 重载时注意尾结点
		}

		// 旋转函数：向上旋转一次（左旋/右旋）
		// x：旋转的结点
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

		// 合并左右子树
		_Node* Combine(_Node* left, _Node* right) {
			if (!left) return right;
			if (!right) return left;
			while (left->ch[1]) left = left->ch[1];
			this->Splay(left);
			left->ch[1] = right;
			right->fa = left;
			this->PushUp(left);
			return left;
		}

	public:
		// 默认构造：添加尾节点
		SplayTree() : root(this->alloc.New(1)), sentry(root) {
			new (this->root) _ElemType;
		};

		// 虚析构函数
		virtual ~SplayTree() {
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

		// 核心函数
		// x：旋转起点
		// fa：最终位置的父节点，默认旋转到根节点
		void Splay(_Node* x, _Node* fa = nullptr) {
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

		// 节点比较运算符
		// n1 < n2 为真
		bool operator ()(_Node* n1, _Node* n2) {
			return n1 != this->sentry && (n2 == this->sentry || this->cpr(n1->key, n2->key));
		}
		// 节点比较运算符
		// node < key 为真
		bool operator ()(_Node* node, const _KTy& key) {
			return node != this->sentry && this->cpr(node->key, key);
		}
		// 节点比较运算符
		// key < node 为真
		bool operator ()(const _KTy& key, _Node* node) {
			return node == this->sentry || this->cpr(key, node->key);
		}

		// 清空元素
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

		// 元素个数
		size_t Size() { return this->size; }

		// 判断是否为空
		bool Empty() { return !this->size; }

		// 返回根节点
		_Node* Root() { return this->root; }

		// 返回首迭代器
		Iterator Begin() {
			_Node* u = this->root;
			while (u->ch[0]) u = u->ch[0];
			this->Splay(u);
			return u;
		}

		// 返回尾迭代器
		Iterator End() {
			return this->sentry;
		}

		// 通过键查询值
		// key：比较键
		// 不存在则返回尾迭代器
		Iterator Find(const _KTy& key) {
			_Node* u = this->root;
			while (u && this->operator()(u, key) != this->operator()(key, u)) u = u->ch[this->operator()(key, u)];
			this->Splay(u ? u : this->sentry);
			return this->root;
		}

		// 查询前驱节点
		// key：键
		// 不存在则返回尾迭代器
		Iterator Precursor(const _KTy& key) {
			_Node* pre = nullptr, * u = this->root;
			while (u) {
				if (this->operator()(u, key)) {
					// 当前节点可能是前驱节点，向右子树搜索
					pre = u;
					u = u->ch[1];
				}
				// 前驱节点只可能在左子树中
				else u = u->ch[0];
			}
			this->Splay(pre ? pre : this->sentry);
			return this->root;
		}

		// 查询后继节点
		// key：键
		// 不存在则返回尾迭代器
		Iterator Successor(const _KTy& key) {
			_Node* suc = nullptr, * u = this->root;
			while (u) {
				if (this->operator()(key, u)) {
					// 当前节点可能是后继节点，向左子树搜索
					suc = u;
					u = u->ch[0];
				}
				// 后继节点只可能在左子树中
				else u = u->ch[1];
			}
			this->Splay(suc);
			return suc;
		}

		// 返回第一个大于等于key的节点
		// key：键
		// 不存在则返回尾迭代器
		Iterator LowerBound(const _KTy& key) {
			_Node* suc = nullptr, * u = this->root;
			while (u) {
				if (this->operator()(u, key) == this->operator()(key, u)) {
					this->Splay(u);
					return u;
				}
				if (this->operator()(key, u)) {
					// 当前节点可能是后继节点，向左子树搜索
					suc = u;
					u = u->ch[0];
				}
				// 后继节点只可能在左子树中
				else u = u->ch[1];
			}
			this->Splay(suc);
			return suc;
		}

		// 插入函数
		// key：键
		// 返回值同Emplace
		std::pair<Iterator, bool> Insert(const _KTy& key) {
			return this->Emplace(key);
		}
		// 插入函数
		// key：键
		// 返回值同Emplace
		std::pair<Iterator, bool> Insert(_KTy&& key) {
			return this->Emplace(std::move(key));
		}
		// 批量插入
		// begin：首迭代器/首指针
		// end：尾迭代器/尾指针
		template <typename _Iter>
		void Insert(_Iter begin, _Iter end) {
			while (begin != end) {
				this->Emplace(*begin);
				++begin;
			}
		}
		// 参数列表批量插入
		void Insert(std::initializer_list<_KTy> vlist) {
			this->Insert(vlist.begin(), vlist.end());
		}

		// 构造节点并插入
		// args：构造节点的参数
		// 返回值的第二个参数表示插入是否成功
		// 若成功，第一个参数为插入节点迭代器
		// 若失败，第一个参数为与之冲突的节点的迭代器
		template<typename... _Args>
		std::pair<Iterator, bool> Emplace(_Args&&... args) {
			_Node* pre = nullptr, * u = this->root, * v = this->alloc.New(1);

			new (v) _Node(_KTy(std::forward<_Args>(args)...));

			while (u) {
				if (this->operator()(u, v) == this->operator()(v, u)) {
					this->alloc.Free(v, 1);
					this->Splay(u);
					return std::make_pair(u, false);
				}
				pre = u;
				u = u->ch[this->operator()(u, v)];
			}

			pre->ch[this->operator()(pre, v)] = v;
			v->fa = pre;
			this->Splay(v);
			++this->size;
			return std::make_pair(v, true);
		}

		// 删除节点
		// itr：删除节点迭代器
		void Erase(Iterator itr) {
#ifdef EXCEPTION_DETECTION
			if (itr.Source() == this->sentry || this->Find(itr->key).Source() == this->sentry) throw std::exception("object of SplayTree：invalid iterator by Erase()");
#endif // EXCEPTION_DETECTION
			this->Splay(*itr);
			_Node* tmp = this->root;
			this->root = this->Combine(this->root->ch[0], this->root->ch[1]);
			--this->size;
			this->alloc.Free(tmp, 1);
		}

		// 删除节点
		// key：目标节点键
		// 成功返回true
		bool Erase(const _KTy& key) {
			if (this->Find(key).Source() == this->sentry) return false;
			_Node* tmp = this->root;
			this->root = this->Combine(this->root->ch[0], this->root->ch[1]);
			--this->size;
			this->alloc.Free(tmp, 1);
		}

		// 是否包含键key
		bool Contains(const _KTy& key) {
			return this->Find(key).Source() != this->sentry;
		}

	protected:
		// 分配器
		_ElemAlloc alloc;
		// 比较器
		_Cmpr cpr;
		// 根结点
		_Node* root;
		// 哨兵节点
		_Node* sentry;
		// 元素个数
		size_t size = 0;
	};


}