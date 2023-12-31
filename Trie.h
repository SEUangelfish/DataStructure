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
			this->Clear();
			this->alloc.Free(this->root, 1);
		}

		// remove all the words from the trie
		void Clear() {
			_Node* u = this->root, * head = nullptr;
			for (size_t i = 0; i < range; ++i) {
				if ((*u)[i]) {
					(*u)[i]->Words() = (size_t)head;
					head = (*u)[i];
					(*u)[i] = nullptr;
				}
			}
			u->Words() = u->Prefix() = 0;
			while (head) {
				u = head;
				head = (_Node*)head->Words();
				for (size_t i = 0; i < range; ++i) {
					if ((*u)[i]) {
						(*u)[i]->Words() = (size_t)head;
						head = (*u)[i];
						(*u)[i] = nullptr;
					}
				}
				this->alloc.Free(u, 1);
			}
		}

		// insert a word into the trie including null string 
		void Insert(const _Ty* word, size_t length) {
			_Node* u = this->root;
			size_t idx;
			for (size_t i = 0; i < length; ++i) {
				++u->Prefix();
				idx = this->map(word[i]);
				if ((*u)[idx] == nullptr) {
					(*u)[idx] = this->alloc.New(1);
					u = (*u)[idx];
					new (u) _Node();
				}
				else u = (*u)[idx];
			}
			++u->Prefix();
			++u->Words();
		}

		bool Contains(const _Ty* word, size_t length) {
			if (!length) return this->root->Words();
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
			if (!ensure && !this->Contains(word, length)) return;
			--this->root->Prefix();
			if (!length) {
				--this->root->Words();
				return;
			}
			_Node* pre, * cur = this->root;
			for (size_t i = 0, idx; i < length;) {
				pre = cur;
				idx = this->map(word[i++]);
				cur = (*cur)[idx];
				if (!--cur->Prefix()) {
					(*pre)[idx] = nullptr;
					while (i < length) {
						pre = cur;
						cur = (*cur)[this->map(word[i++])];
						this->alloc.Free(pre, 1);
					}
					this->alloc.Free(cur, 1);
					return;
				}
			}
			--cur->Words();
		}

		// remove all words with prefix of parameter 1
		//void Remove(const _Ty* prefix, size_t length) {
		//	_Node* u = this->root;
		//	size_t idx;
		//	for (size_t i = 0; i < length; ++i) {

		//	}
		//}

		// return number of words
		size_t Size() {
			return this->root->Prefix();
		}

		size_t CountWordsWithPrefix(const _Ty* prefix, size_t length) {
			_Node* u = this->root;
			for (size_t i = 0; i < length && u; ++i) u = (*u)[this->map(prefix[i])];
			return u ? u->Prefix() : 0;
		}

	protected:
		_ElemAlloc alloc;
		_Map map = [](const _Ty& x)->size_t { return x % 26; };
		_Node* root = this->alloc.New0(1);

		//bool startsWithPrefix(string prefix);
		//void traverse();
	};
}