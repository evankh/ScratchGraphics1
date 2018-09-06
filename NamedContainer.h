#ifndef __EKH_SCRATCH_GRAPHICS_1_NAMED_CONTAINER__
#define __EKH_SCRATCH_GRAPHICS_1_NAMED_CONTAINER__

#include <map>
#include <string>

template<class Type>
class NamedContainer {
private:
	std::map<std::string, Type> mItems;
	Type mDefault;
public:
	NamedContainer(Type default);
	void add(std::string key, Type item);
	Type get(std::string key);
	void remove(std::string key);
	void clear();
	void clear_delete();
};

template<class Type>
inline NamedContainer<Type>::NamedContainer(Type default) {
	mDefault = default;
}

template<class Type>
void NamedContainer<Type>::add(std::string key, Type item) {
	if (mItems.count(key))
		return;
	mItems[key] = item;
}

template<class Type>
inline Type NamedContainer<Type>::get(std::string key) {
	if (mItems.count(key))
		return mItems[key];
	else
		return mDefault;
}

template<class Type>
inline void NamedContainer<Type>::remove(std::string key) {
	if (mItems.count(key))
		mItems.erase(key);
}

template<class Type>
inline void NamedContainer<Type>::clear() {
	mItems.clear();
}

template<class Type>
inline void NamedContainer<Type>::clear_delete() {
	for (auto item : mItems)
		delete item.second;
	mItems.clear();
}

#endif//__EKH_SCRATCH_GRAPHICS_1_NAMED_CONTAINER__