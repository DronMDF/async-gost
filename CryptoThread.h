#pragma once
#include <memory>
#include <thread>
#include <tbb/concurrent_queue.h>

class CryptoEngine;
class CryptoRequest;

class CryptoThread
{
public:
	typedef tbb::concurrent_bounded_queue<std::shared_ptr<CryptoRequest>> queue_type;

	CryptoThread(void (CryptoEngine::* action)(), queue_type *queue);
	~CryptoThread();

private:
	void (CryptoEngine::* action)();
	queue_type *queue;
	std::thread crypto_thread;

	std::shared_ptr<CryptoEngine> createEngine() const;
	bool isTerminator(const std::shared_ptr<CryptoRequest> &request) const;
	void thread_function();
};
