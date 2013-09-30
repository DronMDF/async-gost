#include "CryptoRequest.h"

using namespace std;

future<ContextReply> CryptoRequest::get_future()
{
	return result.get_future();
}

void CryptoRequest::init(const CryptoEngineSlot *) const
{
}

void CryptoRequest::load(const CryptoEngineSlot *) const
{
}

void CryptoRequest::save(const CryptoEngineSlot *)
{
}

void CryptoRequest::submit()
{
}
