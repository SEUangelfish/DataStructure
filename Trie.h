#include "pch.h"
#include "Allocator.h"

//#ifdef WORDS_MAX_LENGTH
//void* wordsBuffer[WORDS_MAX_LENGTH];
//#endif // WORDS_MAX_LENGTH

namespace dsl {
	template<size_t range>
	class TrieNode {
	public:
		TrieNode() = default;
		TrieNode(size_t _prefix) :prefix(_prefix) {}

		~TrieNode() = default;

		TrieNode*& operator[](size_t idx) {
			return this->nexts[idx];
		}

		size_t& Prefix() {
			return this->prefix;
		}

		size_t& Words() {
			return this->words;
		}

	protected:
		size_t words = 0;
		size_t prefix = 0;
		TrieNode* nexts[range]{};
	};

	template <size_t range = 26, typename _Ty = char, typename _Map = std::function<size_t(char)>, template<typename> typename _Allocator = Allocator >
	class Trie {
	public:
		using _Elemtype = TrieNode<range>;
		using _ElemAlloc = Allocator<TrieNode<range>>;
		using _Node = _Elemtype;

	public:

		void Insert(const _Ty* word, size_t length) {
			_Node* u = this->root;
			size_t idx;
			for (size_t i = 0; i < length; ++i) {
				idx = this->map(word[i]);
				if ((*u)[idx] == nullptr) {
					(*u)[idx] = this->alloc.New(1);
					u = (*u)[idx];
					new (u) _Node(1);
				}
				else u = (*u)[idx];
			}
			u->Words() = 1;
		}

		bool Contains(const _Ty* word, size_t length) {
			_Node* u = this->root;
			size_t idx;
			for (size_t i = 0; i < length; ++i) {
				idx = this->map(word[i]);
				if ((*u)[idx] == nullptr) return false;
				u = (*u)[idx];
			}
			return true;
		}

		void Remove(const _Ty* word, size_t length) {
			//			_Node** stack;
			//#ifdef WORDS_MAX_LENGTH
			//			stack = (_Node**)wordsBuffer;
			//#else
			//			stack = new _Node * [length];
			//#endif // WORDS_MAX_LENGTH
			//			int top = 0;
			//			_Node* u = this->root;
			//			size_t idx;
			//			for (size_t i = 0; i < length && u; ++i) {
			//				
			//				idx = this->map(word[i]);
			//				u = (*u)[idx];
			//			}
			//			if (u && u->Words()) {
			//				--u->Words();
			//				while (true) {
			//					--u->Prefix();
			//
			//				}
			//			}
		}

	protected:
		_ElemAlloc alloc;
		_Map map = [](char x)->size_t { return x % 26; };
		_Node* root = this->alloc.New0(1);

		//void remove(string word);
		//bool startsWithPrefix(string prefix);
		//int countWordsWithPrefix(string prefix);
		//void traverse();
		//int getSize();
		//void clear();
	};
}