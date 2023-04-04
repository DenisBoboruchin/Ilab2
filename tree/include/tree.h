#ifndef _TREE_
#define _TREE_

namespace my_containers
{
	template <typename KeyT>
	class tree
	{
	public:
		tree() : key_{KeyT{}}, y_{}, size_{}, left_{nullptr}, right_{nullptr} {}
		tree(const KeyT& key) : key_{key} {}

		[[nodiscard]] int size() const noexcept
		{
			return size_;
		}

	private:
		void update()
		{
			size_ = left_.size() + right_.size() + 1;
		}

		KeyT key_ = KeyT{};
		int y_ = rand(); // need to find better way 
		int size_ = 0;
		tree *left_ = nullptr, *right_ = nullptr;
	}
}

#endif