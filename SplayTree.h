#pragma once
#include "pch.h"
#include "Allocater.h"
#include "Algorithm.h"
#include "Iterator.h"

namespace dsl {
	// 伸展树结点
// _Key			键类型(键、值相同)
	template<typename _Key>
	class SplaySetNode {
		template<typename _Node, typename _Cmpr, typename _Alloc>
		friend class SplayTree;
	public:
		// 键类型
		using _KTy = _Key;
		// 节点类型
		using _Node = SplaySetNode<_KTy>;

	public:
		virtual ~SplaySetNode() = default;

		// 获取键
		// 尾节点无键、值
		_KTy& Key() {
#ifdef EXCEPTION_DETECTION
			if (this->end) throw exception("object of SplaySetNode: end iterator by Key()");
#endif // EXCEPTION_DETECTION
			return this->key;
		}

		// 判断是否是尾节点
		bool End() {
			return this->end;
		}

	protected:
		// 默认构造
		SplaySetNode() = default;
		// 初始化键
		SplaySetNode(const _KTy& _key) :key(_key) {}
		// 初始化键
		SplaySetNode(_KTy&& _key) :key(std::move(_key)) {}
		// 初始化键
		template<typename... _TpArgs>
		SplaySetNode(_TpArgs&&... keyInits) : key(std::forward<_TpArgs>(keyInits)...) {}

		// 拷贝构造
		SplaySetNode(const _Node& cp) :fa(cp.fa), ch{ cp.ch[0], cp.ch[1] }, key(cp.key), end(cp.end) {}
		// 移动构造
		SplaySetNode(_Node&& cp) noexcept :fa(cp.fa), ch{ cp.ch[0], cp.ch[1] }, key(std::move(cp.key)), end(cp.end) {
			memset(&cp, sizeof(_Node), 0);
		}

		// 赋值
		_Node& operator=(const _Node& cp) {
			if (!this->end) this->~SplaySetNode();
			new (this) SplaySetNode(cp);
			return *this;
		}
		// 移动赋值
		_Node& operator=(_Node&& cp) noexcept {
			if (!this->end) this->~SplaySetNode();
			new (this) SplaySetNode(std::move(cp));
			return *this;
		}

	protected:
		// 父节点
		_Node* fa = nullptr;
		// 0：左孩子
		// 1：右孩子
		_Node* ch[2]{ nullptr, nullptr };
		// 键
		_KTy key;
		// 哨兵节点标记
		bool end = false;
	};

	// 伸展树	
	// _Node		节点类型
	// _Cmpr		比较器
	// _Alloc		分配器模板
	template<typename _Node, typename _Cmpr = std::less<typename _Node::_KTy>, typename _Alloc = dsl::RecycleAllocater<_Node>>
	class SplayTree {
	public:
		// 迭代器类型
		using Iterator = SplayTreeIterator<SplayTree<_Node, _Cmpr, _Alloc>>;
		// 供迭代器使用的通用类型
		using _ElemType = _Node;
		// 键类型
		using _KTy = _Node::_KTy;

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

	public:
		// 默认构造：添加尾节点
		SplayTree() : root(this->alloc.New(1)), size(1) {
			// 不直接调用构造函数，键值对可能没有默认构造函数
			this->root->fa = this->root->ch[0] = this->root->ch[1] = nullptr;
			this->root->end = true;
		};

		// 虚析构函数
		virtual ~SplayTree() {
			std::queue<_Node*> qu;
			qu.push(this->root);
			while (qu.size()) {
				this->root = qu.front();
				qu.pop();
				if (this->root->ch[0]) qu.push(this->root->ch[0]);
				if (this->root->ch[1]) qu.push(this->root->ch[1]);
				this->alloc.Free(this->root, !this->root->end);
			}
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
		bool operator ()(const _Node& n1, const _Node& n2) {
			return !n1.end && (n2.end || this->cpr(n1.key, n2.key));
		}
		// 节点比较运算符
		// node < key 为真
		bool operator ()(const _Node& node, const _KTy& key) {
			return !node.end && this->cpr(node.key, key);
		}
		// 节点比较运算符
		// key < node 为真
		bool operator ()(const _KTy& key, const _Node& node) {
			return node.end || this->cpr(key, node.key);
		}

		// 清空元素
		void Clear() {
			std::queue<_Node*> qu;
			qu.push(this->root);
			_Node* tmp;
			while (qu.size()) {
				tmp = qu.front();
				qu.pop();
				if (tmp->ch[0]) qu.push(tmp->ch[0]);
				if (tmp->ch[1]) qu.push(tmp->ch[1]);
				if (!tmp->end) this->alloc.Free(tmp, 1);
				else {
					this->root = tmp;
					this->root->fa = this->root->ch[0] = nullptr;
				}
			}
		}

		// 元素个数
		size_t Size() { return this->size; }

		// 判断是否为空
		bool Empty() { return this->size == 1; }

		// 返回根节点
		_Node* Root() { return this->size == 1; }

		// 返回首迭代器
		Iterator Begin() {
			_Node* u = this->root;
			while (u->ch[0]) u = u->ch[0];
			this->Splay(u);
			return u;
		}
		// 功能与Begin相同
		// 适应C++的基本范围for循环
		Iterator begin() {
			return this->Begin();
		}

		// 返回尾迭代器
		Iterator End() {
			_Node* u = this->root;
			while (u->ch[1]) u = u->ch[1];
			this->Splay(u);
			return u;
		}
		// 功能与End相同
		// 适应C++的基本范围for循环
		Iterator end() {
			return this->End();
		}

		// 通过键查询值
		// key：比较键
		// 不存在则返回尾迭代器
		Iterator Find(const _KTy& key) {
			_Node* u = this->root;
			while (u && *u != key) u = u->ch[!(u < key)];
			if (!u) return this->End();
			this->Splay(u);
			return u;
		}

		// 查询前驱节点
		// key：键
		// 不存在则返回尾迭代器
		Iterator Precursor(const _KTy& key) {
			_Node pre = nullptr, u = this->root;
			while (u) {
				if (u < key) {
					// 当前节点可能是前驱节点，向右子树搜索
					pre = u;
					u = u->ch[1];
				}
				// 前驱节点只可能在左子树中
				else u = u->ch[0];
			}
			if (!pre) return this->End();
			this->Splay(pre);
			return pre;
		}

		// 查询后继节点
		// key：键
		// 不存在则返回尾迭代器
		Iterator Successor(const _KTy& key) {
			_Node suc = nullptr, u = this->root;
			while (u) {
				if (key < u) {
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
			_Node suc = nullptr, u = this->root;
			while (u) {
				if (u == key) {
					this->Splay(u);
					return Iterator(u);
				}
				if (key < u) {
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

		// 返回第一个大于key的节点，相当于查询后继节点
		// key：键
		// 不存在则返回尾迭代器
		Iterator UpperBound(const _KTy& key) {
			return this->Successor(key);
		}

		// 插入函数
		//void Insert() {

		//}

		template<typename... _Args>
		void Emplace(_Args&&... args) {
			_Node* pre = nullptr, * u = this->root, * v = this->alloc.New(1);
			new (v) _Node(std::forward<_Args>(args)...);

			while (u) {
				if (this->operator()(*u, *v) == this->operator()(*v, *u)) {
					*u = std::move(*v);
					this->alloc.Free(v, 1);
					this->Splay(u);
					return;
				}
				pre = u;
				u = u->ch[this->operator()(*u, *v)];
			}

			pre->ch[this->operator()(*pre, *v)] = v;
			v->fa = pre;
			this->Splay(v);
		}

		//void Erase() {

		//}



	protected:
		// 分配器
		_Alloc alloc;
		// 比较器
		_Cmpr cpr;
		// 根结点
		_Node* root;
		// 元素个数
		size_t size;
	};


}