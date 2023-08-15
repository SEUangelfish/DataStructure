#pragma once
#include "pch.h"
#include "Allocater.h"
#include "Algorithm.h"
#include "Iterator.h"

namespace dsl {

	template<class _KTy, class _VTy, typename _Cmpr>
	class SplayNode;

	// ��չ��	
	// _KTy			������
	// _VTy			ֵ����
	// _Cmpr		�Ƚ���
	// _Alloc		������ģ��
	template<typename _KTy, typename _VTy, typename _Cmpr = std::less<_KTy>, template<typename _NTy> typename _Alloc = dsl::Allocater>
	class SplayTree {
	public:
		// �������
		using _ElemType = SplayNode<_KTy, _VTy, _Cmpr>;
		// ����������
		using Iterator = SplayTree_Iterator<SplayTree<_KTy, _VTy, _Cmpr, _Alloc>>;

	protected:
		// ����ά��
		// x��ά���Ľ��
		virtual void PushUp(_ElemType* x) {
			// ����ʱע��β���
			x->sum = x->cnt;
			if (x->ch[0]) x->sum += x->ch[0]->sum;
			if (x->ch[1]) x->sum += x->ch[1]->sum;
		}

		// ����ά��
		// x��ά���Ľ��
		virtual void PushDown(_ElemType* x) {
			// ����ʱע��β���
			// ���幦��ͨ����̬ʵ��
		}

		// ��ת������������תһ�Σ�����/������
		// x����ת�Ľ��
		void Rotate(_ElemType* x) {
			_ElemType* y = x->fa, * z = y->fa;
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

		// ���ĺ���
		// x����ת���
		// fa������λ�õĸ��ڵ㣬Ĭ����ת�����ڵ�
		void Splay(_ElemType* x, _ElemType* fa = nullptr) {
			while (x->fa != fa) {
				_ElemType* y = x->fa, * z = y->fa;
				if (z != fa) {
					if ((x == y->ch[1]) == (y == z->ch[1])) this->Rotate(y);
					else this->Rotate(x);
				}
				this->Rotate(x);
			}
			if (!fa) this->root = x;
		}

	public:
		// Ĭ�Ϲ��죺���β�ڵ�
		SplayTree() : root(this->alloc.New(1)) {
			// ��ֱ�ӵ��ù��캯������ֵ�Կ���û��Ĭ�Ϲ��캯��
			this->root->fa = this->root->cnt[0] = this->root->cnt[1] = nullptr;
			this->root->ch = this->root->sum = 1;
			this->root->end = true;
		};

		// ����������
		virtual ~SplayTree() {
			std::queue<_ElemType*> qu;
			qu.push(this->root);
			while (qu.size()) {
				this->root = qu.front();
				qu.pop();
				if (this->root->ch[0]) qu.push(this->root->ch[0]);
				if (this->root->ch[1]) qu.push(this->root->ch[1]);
				this->alloc.Free(qu.front(), !this->root->end);
			}
		}

		// ���Ԫ��
		void Clear() {
			std::queue<_ElemType*> qu;
			qu.push(this->root);
			_ElemType* tmp;
			while (qu.size()) {
				tmp = qu.front();
				qu.pop();
				if (tmp->ch[0]) qu.push(tmp->ch[0]);
				if (tmp->ch[1]) qu.push(tmp->ch[1]);
				if (!tmp->end) this->alloc.Free(tmp, 1);
				else {
					this->root = tmp;
					this->root->fa = this->root->cnt[0] = this->root->cnt[1] = nullptr;
					this->root->ch = this->root->sum = 1;
					this->root->end = true;
				}
			}
		}

		// Ԫ�ظ���
		size_t Size() { return this->root->sum - 1; }

		// �ж��Ƿ�Ϊ��
		bool Empty() { return !(this->root->sum - 1); }

		// ���ظ��ڵ�
		_ElemType* GetRoot() { return this->root; }

		// �����׵�����
		Iterator Begin() {
			Iterator res(this->root);
			while (res->src->ch[0]) res->src = res->src->ch[0];
			this->Splay(res->src);
			return res;
		}
		// ������Begin��ͬ
		// ��ӦC++�Ļ�����Χforѭ��
		Iterator begin() {
			return this->Begin();
		}

		// ����β������
		Iterator End() {
			Iterator res(this->root);
			while (res->src->ch[1]) res->src = res->src->ch[1];
			this->Splay(res->src);
			return res;
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
			_ElemType u = this->root;
			while (u && u != key) u = u->ch[!(u < key)];
			if (!u) return this->End();
			this->Splay(u);
			return Iterator(u);
		}

		// ͨ��������ѯֵ
		// rk������(��1��ʼ)
		// �������򷵻�β������
		Iterator Find(size_t rk) {
			if (rk >= this->root->sum) return this->End();
			_ElemType* u = this->root;
			while (1) {
				if (u->ch[0]) {
					if (rk <= u->ch[0]->sum) {
						u = u->ch[0];
						continue;
					}
					rk -= u->ch[0]->sum;
				}
				if (rk <= u->cnt) break;
				rk -= u->cnt;
				u = u->ch[1];
			}
			this->Splay(u);
			return Iterator(u);
		}

		// ��ѯ����
		// ���ж��ֵ�����ص�����С����
		// key����
		// ��ֵ�������򷵻�0
		size_t GetRank(const _KTy& key) {
			if (this->Find(key)->end) return 0;
			return this->root->ch[0] ? this->root->ch[0]->sum + 1 : 1;
		}

		// ��ѯ����
		// ���ж��ֵ�����ص�����С����
		// itr��Ŀ��ڵ�ĵ�����
		// ��Ч����������0�����쳣
		size_t GetRank(const Iterator& itr) {
			this->Splay(itr->src);
			return this->root->ch[1] ? (this->root->ch[0] ? this->root->ch[0]->sum + 1 : 1) : 0;
		}

		// ��ѯǰ���ڵ�
		// key����
		// �������򷵻�β������
		//Iterator GetPrecursor(const _KTy& key) {
		//	Iterator res = this->Find(key);
		//	if (!res->src->ch[0]) return this->End();
		//	this->Splay((--res)->src);
		//	return res;
		//}

		// ��ѯ��̽ڵ�
		// key����
		// �������򷵻�β������
		//Iterator GetSuccessor(const _KTy& key) {
		//	Iterator res = this->Find(key);
		//	if (!res->src->ch[0]) return this->End();
		//	this->Splay((--res)->src);
		//	return res;
		//}

		// ���뺯��
		//void Insert() {

		//}

		//template<typename... _Args>
		//void Emplace(_Args&&... args) {
		//	//// �½��
		//	//_ElemType* newNode = this->alloc.New(1);
		//	//new (newNode) _ElemType(std::forward<_Args>(args)...);

		//	//// ����
		//	//if (!this->root) {
		//	//	this->root = newNode;
		//	//	return;
		//	//}

		//	//// Ѱ�Ҳ���λ��
		//	//_ElemType* u = this->root, * fa = nullptr;
		//	//while (u && (this->cpr(u->key, newNode->key) != this->cpr(newNode->key, u->key))) {
		//	//	fa = u;
		//	//	u = u->ch[this->cpr(newNode->key, u->key)];
		//	//}


		//}


	protected:
		// ������
		_Alloc<_ElemType> alloc;
		// �����
		_ElemType* root;
	};

	// ��չ�����
	// _KTy			������
	// _VTy			ֵ����
	template<typename _KTy, typename _VTy, typename _Cmpr>
	class SplayNode {
		template<typename _KTy, typename _VTy, typename _Cmpr, template<typename _NTy> typename _Alloc>
		friend class SplayTree;
	public:
		// �ȽϷ�����
		bool operator < (const SplayNode<_KTy, _VTy, _Cmpr>& val) {
			return !this->end && (val.end || this->cpr(this->key, val->key));
		}
		bool operator < (const _KTy& key) {
			return !this->end && this->cpr(this->key, key);
		}
		friend bool operator < (const _KTy& key, const SplayNode<_KTy, _VTy, _Cmpr>& val) {
			return val.end || this->cpr(key, val.key);
		}
		bool operator == (const SplayNode<_KTy, _VTy, _Cmpr>& val) {
			return *this < val == val < *this;
		}
		bool operator == (const _KTy& val) {
			return *this < val == val < *this;
		}
		bool operator != (const SplayNode<_KTy, _VTy, _Cmpr>& val) {
			return *this < val != val < *this;
		}
		bool operator != (const _KTy& val) {
			return *this < val != val < *this;
		}


		// ������
		virtual ~SplayNode() = default;

	protected:
		// Ĭ�Ϲ���
		SplayNode() = default;
		// ��ʼ������ֵ
		SplayNode(const _KTy& _key, const _VTy& _val) :key(_key), val(_val) {}
		// ��ʼ������ֵ
		SplayNode(_KTy&& _key, const _VTy& _val) :key(std::move(_key)), val(_val) {}
		// ��ʼ������ֵ
		SplayNode(const _KTy& _key, _VTy&& _val) :key(_key), val(std::move(_val)) {}
		// ��ʼ������ֵ
		SplayNode(_KTy&& _key, _VTy&& _val) :key(std::move(_key)), val(std::move(_val)) {}

	protected:
		// ���ڵ�
		SplayNode* fa = nullptr;
		// 0������
		// 1���Һ���
		SplayNode* ch[2]{ nullptr, nullptr };
		// ����Ԫ������
		size_t sum = 1;
		// �Ƚ���
		_Cmpr cpr;

	public:
		// ��ͬԪ�ظ���
		size_t cnt = 1;
		// ��
		_KTy key;
		// ֵ
		_KTy val;
		// �ڱ��ڵ���
		bool end = false;

	};
}