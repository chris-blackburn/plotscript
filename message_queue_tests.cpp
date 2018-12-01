#include "catch.hpp"

#include "message_queue.hpp"
#include <thread>
#include <chrono>

TEST_CASE ("Simple try and pop", "[MessageQueue]") {
	MessageQueue<int> queue;

	// should be empty to begin with
	REQUIRE(queue.empty());

	// The first thread addes a number to the queue
	std::thread th1([](MessageQueue<int>& q) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		while (q.empty()) {
			q.push(7);
			break;
		}
	}, std::ref(queue));

	// the second thread enters in an event loop to pop that number
	std::thread th2([](MessageQueue<int>& q) {
		int expected;

		// wait for something to be in the queue
		while (!q.try_pop(expected)) {}

		// at this point the expected value should be what was pushed in the other thread
		REQUIRE(expected == 7);
		REQUIRE(q.empty());

		// should return false since nothing is in the queue
		REQUIRE(!q.try_pop(expected));
	}, std::ref(queue));

	th1.join();
	th2.join();

	// After both threads have completed their tasks, the queue should be empty
	REQUIRE(queue.empty());
}

TEST_CASE ("Simple wait and pop", "[MessageQueue]") {
	MessageQueue<int> queue;

	// should be empty to begin with
	REQUIRE(queue.empty());

	// The first thread addes a number to the queue (wait to add it)
	std::thread th1([](MessageQueue<int>& q) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		while (q.empty()) {
			q.push(7);
			break;
		}
	}, std::ref(queue));

	// the second thread enters in an event loop to pop that number
	std::thread th2([](MessageQueue<int>& q) {
		int expected;

		// wait for something to be in the queue
		q.wait_pop(expected);

		// at this point the expected value should be what was pushed in the other thread
		REQUIRE(expected == 7);
		REQUIRE(q.empty());
	}, std::ref(queue));

	th1.join();
	th2.join();

	// After both threads have completed their tasks, the queue should be empty
	REQUIRE(queue.empty());
}
