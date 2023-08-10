#pragma once
#include "pch.h"
#include "Allocater.h"
#include "Algorithm.h"

namespace dsl {

	template<class _KTy, class _VTy>
	class SplayNode;

	// ��չ��	
	// _KTy			������
	// _VTy			ֵ����
	// _Cmpr		�Ƚ���
	// _Alloc		������ģ��
	template<typename _KTy, typename _VTy, typename _Cmpr = std::less<_KTy>, template<typename _NTy> typename _Alloc = dsl::Allocater>
	class SplayTree {
		using SNode = SplayNode<_KTy, _VTy>;

	protected:
		// ����ά��
		// x��ά���Ľ��
		virtual void PushUp(SNode* x) {
			x->sum = x->cnt;
			if (x->ch[0]) x->sum += x->ch[0]->sum;
			if (x->ch[1]) x->sum += x->ch[1]->sum;
		}

		// ����ά��
		// x��ά���Ľ��
		virtual void PushDown(SNode* x) {
			// ���幦��ͨ����̬ʵ��
		}

		// ��ת������������תһ�Σ�����/������
		// x����ת�Ľ��
		void Rotate(SNode* x) {
			SNode* y = x->fa, * z = y->fa;
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
		void Splay(SNode* x, SNode* fa = nullptr) {
			while (x->fa != fa) {
				SNode* y = x->fa, * z = y->fa;
				if (z != fa) {
					if ((x == y->ch[1]) == (y == z->ch[1])) this->Rotate(y);
					else this->Rotate(x);
				}
				this->Rotate(x);
			}
			if (!fa) this->root = x;
		}

	public:
		// Ĭ�Ϲ���
		SplayTree() = default;

		// ����������
		virtual ~SplayTree() {

		}

		// ���뺯��
		void Insert() {

		}

		template<typename... _Args>
		void Emplace(_Args&&... args) {
			// �½��
			SNode* newNode = this->alloc.New(1);
			new (newNode) SNode(std::forward<_Args>(args)...);

			// ����
			if (!this->root) {
				this->root = newNode;
				return;
			}

			// Ѱ�Ҳ���λ��
			SNode* u = this->root, * fa = nullptr;
			while (u && (this->cpr(u->key, newNode->key) != this->cpr(newNode->key, u->key))) {
				fa = u;
				u = u->ch[this->cpr(newNode->key, u->key)];
			}


		}


	protected:
		// �����
		SNode* root = nullptr;
		// �Ƚ���
		_Cmpr cpr;
		// ������
		_Alloc<SNode> alloc;
	};

	// ��չ�����
	// _KTy			������
	// _VTy			ֵ����
	template<class _KTy, class _VTy>
	class SplayNode {
		template<typename _KTy, typename _VTy, typename _Cmpr, template<typename _NTy> typename _Alloc>
		friend class SplayTree;
	public:
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
		// ��ͬԪ�ظ���
		size_t cnt = 1;
		// ����Ԫ������
		size_t sum = 1;
		// ��
		_KTy key;
		// ֵ
		_KTy val;
	};
}