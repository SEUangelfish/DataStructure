#pragma once
#include "pch.h"
#include "Allocater.h"
#include "Algorithm.h"
#include "Iterator.h"

namespace dsl {
	// ��չ�����
	// _Key��������(����ֵ��ͬ)
	template<typename _Key>
	class SplaySetNode {
		template<typename _Node, typename _Cmpr, typename _Alloc>
		friend class SplayTree;

		template<typename _DSTy>
		friend class SplayTreeIterator;

	public:
		// ������
		using _KTy = _Key;
		// �ڵ�����
		using _Node = SplaySetNode<_KTy>;

	public:
		virtual ~SplaySetNode() = default;

		// ��ȡ��
		// β�ڵ��޼���ֵ
		_KTy& Key() {
#ifdef EXCEPTION_DETECTION
			if (this->end) throw exception("object of SplaySetNode: end iterator by Key()");
#endif // EXCEPTION_DETECTION
			return this->key;
		}

		// �ж��Ƿ���β�ڵ�
		bool End() {
			return this->end;
		}

	protected:
		// Ĭ�Ϲ���
		SplaySetNode() = default;
		// ��ʼ����
		SplaySetNode(const _KTy& _key) :key(_key) {}
		// ��ʼ����
		SplaySetNode(_KTy&& _key) :key(std::move(_key)) {}
		// ��ʼ����
		template<typename... _TpArgs>
		SplaySetNode(_TpArgs&&... keyInits) : key(std::forward<_TpArgs>(keyInits)...) {}

		// ��������
		SplaySetNode(const _Node& cp) :fa(cp.fa), ch{ cp.ch[0], cp.ch[1] }, key(cp.key), end(cp.end) {}
		// �ƶ�����
		SplaySetNode(_Node&& cp) noexcept :fa(cp.fa), ch{ cp.ch[0], cp.ch[1] }, key(std::move(cp.key)), end(cp.end) {
			memset(&cp, sizeof(_Node), 0);
		}

		// ��ֵ
		_Node& operator=(const _Node& cp) {
			if (!this->end) this->~SplaySetNode();
			new (this) SplaySetNode(cp);
			return *this;
		}
		// �ƶ���ֵ
		_Node& operator=(_Node&& cp) noexcept {
			if (!this->end) this->~SplaySetNode();
			new (this) SplaySetNode(std::move(cp));
			return *this;
		}

	protected:
		// ���ڵ�
		_Node* fa = nullptr;
		// 0������
		// 1���Һ���
		_Node* ch[2]{ nullptr, nullptr };
		// ��
		_KTy key;
		// �ڱ��ڵ���
		bool end = false;
	};

	// ��չ������	
	// _Node		�ڵ�����
	// _Cmpr		�Ƚ���
	// _Alloc		������
	template<typename _Node, typename _Cmpr, typename _Alloc>
	class SplayTree {
	public:
		// ����������
		using Iterator = SplayTreeIterator<SplayTree<_Node, _Cmpr, _Alloc>>;
		// ��������ʹ�õ�ͨ������
		using _ElemType = _Node;
		// ������
		using _KTy = _Node::_KTy;

	protected:
		// ����ά��
		// x��ά���Ľ��
		virtual void PushUp(_Node* x) {
			// ���幦��ͨ����̬ʵ��
			// ����ʱע��β���
		}

		// ����ά��
		// x��ά���Ľ��
		virtual void PushDown(_Node* x) {
			// ���幦��ͨ����̬ʵ��
			// ����ʱע��β���
		}

		// ��ת������������תһ�Σ�����/������
		// x����ת�Ľ��
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

		// �ϲ���������
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
		// Ĭ�Ϲ��죺���β�ڵ�
		SplayTree() : root(this->alloc.New(1)) {
			// ��ֱ�ӵ��ù��캯������ֵ�Կ���û��Ĭ�Ϲ��캯��
			this->root->fa = this->root->ch[0] = this->root->ch[1] = nullptr;
			this->root->end = true;
		};

		// ����������
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

		// ���ĺ���
		// x����ת���
		// fa������λ�õĸ��ڵ㣬Ĭ����ת�����ڵ�
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

		// �ڵ�Ƚ������
		// n1 < n2 Ϊ��
		bool operator ()(const _Node& n1, const _Node& n2) {
			return !n1.end && (n2.end || this->cpr(n1.key, n2.key));
		}
		// �ڵ�Ƚ������
		// node < key Ϊ��
		bool operator ()(const _Node& node, const _KTy& key) {
			return !node.end && this->cpr(node.key, key);
		}
		// �ڵ�Ƚ������
		// key < node Ϊ��
		bool operator ()(const _KTy& key, const _Node& node) {
			return node.end || this->cpr(key, node.key);
		}

		// ���Ԫ��
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

		// Ԫ�ظ���
		size_t Size() { return this->size; }

		// �ж��Ƿ�Ϊ��
		bool Empty() { return !this->size; }

		// ���ظ��ڵ�
		_Node* Root() { return this->root; }

		// �����׵�����
		Iterator Begin() {
			_Node* u = this->root;
			while (u->ch[0]) u = u->ch[0];
			this->Splay(u);
			return u;
		}
		// ������Begin��ͬ
		// ��ӦC++�Ļ�����Χforѭ��
		Iterator begin() {
			return this->Begin();
		}

		// ����β������
		Iterator End() {
			_Node* u = this->root;
			while (u->ch[1]) u = u->ch[1];
			this->Splay(u);
			return u;
		}
		// ������End��ͬ
		// ��ӦC++�Ļ�����Χforѭ��
		Iterator end() {
			return this->End();
		}

		// ͨ������ѯֵ
		// key���Ƚϼ�
		// �������򷵻�β������
		Iterator Find(const _KTy& key) {
			_Node* u = this->root;
			while (u && (this->operator()(*u, key) != this->operator()(key, *u))) u = u->ch[this->operator()(key, *u)];
			if (!u) return this->End();
			this->Splay(u);
			return u;
		}

		// ��ѯǰ���ڵ�
		// key����
		// �������򷵻�β������
		Iterator Precursor(const _KTy& key) {
			_Node pre = nullptr, u = this->root;
			while (u) {
				if (u < key) {
					// ��ǰ�ڵ������ǰ���ڵ㣬������������
					pre = u;
					u = u->ch[1];
				}
				// ǰ���ڵ�ֻ��������������
				else u = u->ch[0];
			}
			if (!pre) return this->End();
			this->Splay(pre);
			return pre;
		}

		// ��ѯ��̽ڵ�
		// key����
		// �������򷵻�β������
		Iterator Successor(const _KTy& key) {
			_Node suc = nullptr, u = this->root;
			while (u) {
				if (key < u) {
					// ��ǰ�ڵ�����Ǻ�̽ڵ㣬������������
					suc = u;
					u = u->ch[0];
				}
				// ��̽ڵ�ֻ��������������
				else u = u->ch[1];
			}
			this->Splay(suc);
			return suc;
		}

		// ���ص�һ�����ڵ���key�Ľڵ�
		// key����
		// �������򷵻�β������
		Iterator LowerBound(const _KTy& key) {
			_Node suc = nullptr, u = this->root;
			while (u) {
				if (u == key) {
					this->Splay(u);
					return Iterator(u);
				}
				if (key < u) {
					// ��ǰ�ڵ�����Ǻ�̽ڵ㣬������������
					suc = u;
					u = u->ch[0];
				}
				// ��̽ڵ�ֻ��������������
				else u = u->ch[1];
			}
			this->Splay(suc);
			return suc;
		}

		// ���ص�һ������key�Ľڵ㣬�൱�ڲ�ѯ��̽ڵ�
		// key����
		// �������򷵻�β������
		Iterator UpperBound(const _KTy& key) {
			return this->Successor(key);
		}

		// ���뺯��
		// key����
		// ����ֵͬEmplace
		std::pair<Iterator, bool> Insert(const _KTy& key) {
			return this->Emplace(key);
		}
		// ���뺯��
		// key����
		// ����ֵͬEmplace
		std::pair<Iterator, bool> Insert(_KTy&& key) {
			return this->Emplace(std::move(key));
		}
		// ��������
		// begin���׵�����/��ָ��
		// end��β������/βָ��
		template <typename _Iter>
		void Insert(_Iter begin, _Iter end) {
			while (begin != end) {
				this->Emplace(*begin);
				++begin;
			}
		}
		// �����б���������
		void Insert(std::initializer_list<_KTy> vlist) {
			this->Insert(vlist.begin(), vlist.end());
		}

		// ����ڵ㲢����
		// args������ڵ�Ĳ���
		// ����ֵ�ĵڶ���������ʾ�����Ƿ�ɹ�
		// ���ɹ�����һ������Ϊ����ڵ������
		// ��ʧ�ܣ���һ������Ϊ��֮��ͻ�Ľڵ�ĵ�����
		template<typename... _Args>
		std::pair<Iterator, bool> Emplace(_Args&&... args) {
			_Node* pre = nullptr, * u = this->root, * v = this->alloc.New(1);
			new (v) _Node(std::forward<_Args>(args)...);

			while (u) {
				if (this->operator()(*u, *v) == this->operator()(*v, *u)) {
					this->alloc.Free(v, 1);
					this->Splay(u);
					++this->size;
					return std::make_pair(u, false);
				}
				pre = u;
				u = u->ch[this->operator()(*u, *v)];
			}

			pre->ch[this->operator()(*pre, *v)] = v;
			v->fa = pre;
			this->Splay(v);
			++this->size;
			return std::make_pair(v, true);
		}

		// ɾ���ڵ�
		// itr��ɾ���ڵ������
		void Erase(Iterator itr) {
#ifdef EXCEPTION_DETECTION
			if (itr->end || this->Find(itr->key) != itr) throw std::exception("object of SplayTree��invalid iterator by Erase()");
#endif // EXCEPTION_DETECTION
			this->Splay(*itr);
			_Node* tmp = this->root;
			this->root = this->Combine(this->root->ch[0], this->root->ch[1]);
			--this->size;
			this->alloc.Free(tmp, 1);
		}

		// ɾ���ڵ�
		// key��Ŀ��ڵ��
		// �ɹ�����true
		bool Erase(const _KTy& key) {
			if (this->Find(key)->end) return false;
			_Node* tmp = this->root;
			this->root = this->Combine(this->root->ch[0], this->root->ch[1]);
			--this->size;
			this->alloc.Free(tmp, 1);
		}

		// �Ƿ������key
		bool Contains(const _KTy& key) {
			return !this->Find(key)->end;
		}

	protected:
		// ������
		_Alloc alloc;
		// �Ƚ���
		_Cmpr cpr;
		// �����
		_Node* root;
		// Ԫ�ظ���
		size_t size = 0;
	};


}