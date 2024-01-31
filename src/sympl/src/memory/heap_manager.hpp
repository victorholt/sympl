//
// GameSencha, LLC 1/23/24.
//
#ifndef HEAP_MANAGER_HPP
#define HEAP_MANAGER_HPP

#include <vector>
#include <memory>

class HeapManager {
public:
	HeapManager();
	~HeapManager();

	// Allocate memory for an object and track it
	template<typename T, typename... Args>
	T* allocate(Args&&... args);

	// Free memory for an object
	void free_memory(void* ptr);

	// Traverse all reachable objects from root references and mark them.
	mark_all();

	// Iterate through all objects managed by the HeapManager. Any unmarked object is not reachable and can be freed.
	sweep();

	// Perform garbage collection
	void collect_garbage();

private:
	// A list or a more complex data structure to track allocated objects
	std::vector<void*> allocations;
};

template<typename T, typename... Args>
T* HeapManager::allocate(Args&&... args) {
	T* obj = new T(std::forward<Args>(args)...);
	allocations.push_back(obj);
	return obj;
}

void HeapManager::free_memory(void* ptr) {
	// Find the object in the allocations list
	auto it = std::find(allocations.begin(), allocations.end(), ptr);

	if (it != allocations.end()) {
		// Free the memory
		delete *it;

		// Remove the pointer from the list
		allocations.erase(it);
	}
}

void HeapManager::collect_garbage() {
	// Mark Phase
	mark_all();

	// Sweep Phase
	sweep();
}

void HeapManager::mark_all() {
	// Implement logic to mark all reachable objects
}

void HeapManager::sweep() {
	for (auto it = allocations.begin(); it != allocations.end(); /* no increment */) {
		if (/* check if the object pointed to by it is unmarked */) {
			delete *it;  // Free memory
			it = allocations.erase(it);  // Remove from the list
		} else {
			++it;
		}
	}
}

#endif // HEAP_MANAGER_HPP
