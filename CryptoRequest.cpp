#include "CryptoRequest.h"
#include "async-gost.hpp"

using namespace std;

CryptoRequest::CryptoRequest(const vector<uint8_t> &data, const vector<uint8_t> &key,
	      const vector<uint8_t> &iv)
	: data(data), key(key), iv(iv), index(0)
{
}

future<ContextReply> CryptoRequest::get_future()
{
	return result.get_future();
}

vector<uint8_t> CryptoRequest::getKey() const
{
	return key;
}

vector<uint8_t> CryptoRequest::getData() const
{
	return iv;
}

void CryptoRequest::setData(const vector<uint8_t> &block)
{
	iv.assign(block.begin(), block.end());
	const auto limit = min(data.size() - index, size_t(8));
	for (size_t i = 0; i < limit; i++) {
		iv[i] ^= data[index + i];
		data[index + i] = iv[i];
	}
	index += limit;
}

bool CryptoRequest::isDone() const
{
	return index == data.size();
}

void CryptoRequest::submit()
{
	ContextReply reply;
	reply.data = data;
	result.set_value(reply);
}
