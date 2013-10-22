
#include "CryptoThread.h"
#include <boost/test/unit_test.hpp>
#include "async-gost.h"
#include "CpuSupport.h"
#include "CryptoEngineGeneric.h"
#include "CryptoEngineSSSE3.h"
#include "CryptoRequestNull.h"

using namespace std;

class CryptoRequestTerminator : public CryptoRequestNull
{
public:
	CryptoRequestTerminator(const std::thread::id id)
		: id(id)
	{
	}

	const std::thread::id id;
};

CryptoThread::CryptoThread(void (CryptoEngine::* action)(), queue_type *queue)
	: action(action), queue(queue), crypto_thread(&CryptoThread::thread_function, this)
{
}

CryptoThread::~CryptoThread()
{
	queue->push(make_shared<CryptoRequestTerminator>(crypto_thread.get_id()));
	crypto_thread.join();
}

shared_ptr<CryptoEngine> CryptoThread::createEngine() const
{
	if (cpu_support_ssse3()) {
		return make_shared<CryptoEngineSSSE3>();
	}

	return make_shared<CryptoEngineGeneric>();
}

bool CryptoThread::isTerminator(const shared_ptr<CryptoRequest> &request) const
{
	if (auto terminator = dynamic_pointer_cast<CryptoRequestTerminator>(request)) {
		if (terminator->id == this_thread::get_id()) {
			return true;
		}
	}
	return false;
}

void CryptoThread::thread_function()
{
	auto engine = createEngine();
	const auto slots = engine->getSlots();
	vector<shared_ptr<CryptoRequest>> request(slots.size());
	bool terminated = false;
	while(true) {
		int active_slots = slots.size();
		for (unsigned s = 0; s < slots.size(); s++) {
			if (!request[s]) {
				if (!terminated && queue->try_pop(request[s])) {
					request[s]->init(&slots[s]);
					if (isTerminator(request[s])) {
						terminated = true;
					}
				} else {
					request[s] = make_shared<CryptoRequestNull>();
					active_slots--;
				}
			}

			request[s]->load(&slots[s]);
		}

		if (active_slots == 0) {
			if (terminated) {
				break;
			}

			// Заданий нет - ждем до первого задания
			queue->pop(request[0]);

			if (isTerminator(request[0])) {
				break;
			}

			request[0]->init(&slots[0]);
			request[0]->load(&slots[0]);
		}

		engine->encrypt();

		for (unsigned s = 0; s < slots.size(); s++) {
			request[s]->save(&slots[s]);

			if (request[s]->isDone()) {
				request[s]->submit();
				request[s].reset();
			}
		}
	}
}

BOOST_AUTO_TEST_SUITE(suiteCryptoThread)

BOOST_AUTO_TEST_CASE(CryptoThreadShouldJoin)
{
	tbb::concurrent_bounded_queue<std::shared_ptr<CryptoRequest>> queue;
	CryptoThread joined_thread(&CryptoEngine::encrypt, &queue);
	// Все, тест на этом закончен
}

BOOST_AUTO_TEST_SUITE_END()
