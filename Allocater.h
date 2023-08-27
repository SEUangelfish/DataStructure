#pragma once
#include "pch.h"

// DSLʹ�õķ�����������New��Free����
// �����ö�̬��ԭ�������Ч��

namespace dsl {

	template<typename _Ty>
	class Allocater {
	public:
		// ��Դ���뺯��
		// �������κγ�ʼ������
		// cnt��Ԫ�ظ���
		// ������Դָ��
		_Ty* New(size_t cnt) {
			_Ty* res = (_Ty*)operator new(cnt * sizeof(_Ty));
#ifdef EXCEPTION_DETECTION
			if (!res) throw std::bad_alloc();
#endif // EXCEPTION_DETECTION
			return res;
		}

		// �ͷ���Դ
		// �����ͻ������������
		// size��������Ԫ�ظ���
		void Free(_Ty* src, size_t size) {
			if constexpr (std::is_class<_Ty>::value) for (_Ty* st = src, *ed = src + size; st != ed; ++st) st->~_Ty();
			operator delete(src);
		}
	};

	// ��Դ���շ�����
	// �÷������Ὣ���õ��ڴ汣��������������Դʱ����ʹ�û��յ���Դ
	// ����������ʱ���ͷ�������Դ
	// ע��1���÷�������������ʽ������ÿ�η����������Ԫ�ظ�������Ϊ1�����������ڴ��˷�
	// ע��2������Ԫ�ش�С������ڵ���ָ��Ĵ�С���������ʧ��
	template<typename _Ty, typename std::enable_if<sizeof(_Ty) >= sizeof(_Ty*)>::type* = nullptr>
	class RecycleAllocater {
		union Block {
			_Ty null;
			Block* next;
		};

	public:
		RecycleAllocater() = default;

		~RecycleAllocater() {
			void* tmp;
			while (this->head) {
				tmp = this->head;
				this->head = this->head->next;
				operator delete(tmp);
			}
		}

		// ��Դ���뺯��
		// �������κγ�ʼ������
		// cnt��Ԫ�ظ���������Ϊ1��
		// ������Դָ��
		_Ty* New(size_t cnt) {
			if (cnt == 1 && this->head) {
				_Ty* res = (_Ty*)this->head;
				this->head = this->head->next;
				return res;
			}
			_Ty* res = (_Ty*)operator new(cnt * sizeof(_Ty));
#ifdef EXCEPTION_DETECTION
			if (!res) throw std::bad_alloc();
#endif // EXCEPTION_DETECTION
			return res;
		}

		// �ͷ���Դ
		// �����ͻ������������
		// size��������Ԫ�ظ���
		void Free(_Ty* src, size_t size) {
			if constexpr (std::is_class<_Ty>::value)  for (_Ty* st = src, *ed = src + size; st != ed; ++st) st->~_Ty();
			Block* head = (Block*)src;
			head->next = this->head;
			this->head = head;
		}

	protected:
		Block* head = nullptr;
	};


}