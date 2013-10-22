#include "CryptoRequest.h"

using namespace std;

future<ContextReply> CryptoRequest::get_future()
{
	return result.get_future();
}

void CryptoRequest::init(CryptoEngineSlot *) const
{
}

void CryptoRequest::load(CryptoEngineSlot *) const
{
}

void CryptoRequest::save(CryptoEngineSlot *)
{
}

void CryptoRequest::submit()
{
}
