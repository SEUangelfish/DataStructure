#include "pch.h"
#include "Allocator.h"

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

	// range:	character range
	// _Ty:		character type
	// _Map:	type of mapping function which maps characters to different indexes
	//			eg: to all lowercase letters, the function should be "mod 26" 
	// _Alloc:	allocator template
	template <size_t range = 26, typename _Ty = char, typename _Map = std::function<size_t(const _Ty&)>, template<typename> typename _Alloc = Allocator>
	class Trie {
	public:
		using _Elemtype = TrieNode<range>;
		using _ElemAlloc = _Alloc<TrieNode<range>>;
		using _Node = _Elemtype;

	public:
		Trie() = default;
		Trie(_Map _map) :map(_map) {}

		~Trie() {

		}

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
			++this->size;
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

		// remove the word from trie
		// ensure: true if it must contains the word
		void Remove(const _Ty* word, size_t length, bool ensure) {
			if (!length || !ensure && !this->Contains(word, length)) return;
			size_t idx1 = this->map(word[0]), idx2, i = 1;
			_Node* pre = this->root, * cur = (*pre)[idx1], * nxt;
			do {
				if (!--cur->Prefix()) {
					(*pre)[idx1] = nullptr;
					while (cur) {
						pre = cur;
						cur = (*cur)[this->map(word[i++])];
						this->alloc.Free(pre, 1);
					}
					--this->size;
					return;
				}
				idx2 = this->map(word[i]);
				nxt = (*cur)[idx2];
				pre = cur;
				cur = nxt;
				idx1 = idx2;
			} while (++i < length);
			--cur->Words();
			--this->size;
		}

		void Remove() {

		}

		// return number of words
		size_t Size() {
			return this->size;
		}

	protected:
		_ElemAlloc alloc;
		_Map map = [](const _Ty& x)->size_t { return x % 26; };
		size_t size = 0;
		_Node* root = this->alloc.New0(1);

		//bool startsWithPrefix(string prefix);
		//int countWordsWithPrefix(string prefix);
		//void traverse();
		//void clear();
	};
}