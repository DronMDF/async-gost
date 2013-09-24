#include "CryptoRequest.h"

using namespace std;

future<ContextReply> CryptoRequest::get_future()
{
	return result.get_future();
}
