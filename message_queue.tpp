#include "message_queue.hpp"

template<typename T>
void MessageQueue<T>::push(const T& val) {
	std::unique_lock<std::mutex> lock(m_mutex);
	m_queue.push(val);
	lock.unlock();
	m_cv.notify_one();
}

template<typename T>
bool MessageQueue<T>::empty() const {
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_queue.empty();
}

template<typename T>
bool MessageQueue<T>::try_pop(T& popped) {
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_queue.empty()) {
		return false;
	}

	popped = m_queue.front();
	m_queue.pop();
	return true;
}

template<typename T>
void MessageQueue<T>::wait_pop(T& popped) {
	std::unique_lock<std::mutex> lock(m_mutex);
	while (m_queue.empty()) {
		m_cv.wait(lock);
	}

	popped = m_queue.front();
	m_queue.pop();
}
