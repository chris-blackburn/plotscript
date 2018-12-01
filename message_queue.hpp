#ifndef MESSAGE_QUEUE_HPP
#define MESSAGE_QUEUE_HPP
// Thread safe queue implementation for inter-thread communication

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class MessageQueue {
public:
	void push(const T& val);

	bool empty() const;

	// Successfuly pops return true, else false
	bool try_pop(T& popped);

	// Waits until something has been pushed into the queue
	void wait_pop(T& popped);
private:
	std::queue<T> m_queue;
	mutable std::mutex m_mutex;
	std::condition_variable m_cv;
};

#include "message_queue.tpp"
#endif
