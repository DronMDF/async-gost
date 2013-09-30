#include "CryptoRequest.h"

using namespace std;

future<ContextReply> CryptoRequest::get_future()
{
	return result.get_future();
}

void CryptoRequest::load(const CryptoEngineSlot *) const
{
}

void CryptoRequest::save(const CryptoEngineSlot *)
{
}
