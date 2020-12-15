#include <Utils/Encoding.hpp>
#include <Utils/Unique_ptr.hpp>
#include <iostream>

bool Encoding::Deflate(const std::string &input,
					   std::vector<uint8_t> &output,
					   DeflateMode mode)
{
	z_stream deflateStream;
	deflateStream.zalloc = Z_NULL;
	deflateStream.zfree = Z_NULL;
	deflateStream.opaque = Z_NULL;
	if (mode == DEFLATE &&
		deflateInit(&deflateStream,
					Z_DEFAULT_COMPRESSION) != Z_OK)
	{
		std::cerr << "deflateInit failed\n";
		return FAILURE;
	}
	if (mode == GZIP &&
		deflateInit2(&deflateStream,
					 Z_DEFAULT_COMPRESSION,
					 Z_DEFLATED,
					 16 + MAX_WBITS,
					 8,
					 Z_DEFAULT_STRATEGY) != Z_OK)
	{
		std::cerr << "deflateInit2 failed\n";
		return FAILURE;
	}
	// used to clean up the stream
	Unique_ptr<z_stream, endft> deflateStreamReference(
		&deflateStream,
		deflateEnd);

	// Deflate the data
	output.clear();
	deflateStream.next_in = (Bytef *)input.c_str();
	deflateStream.avail_in = (uInt)input.length();
	deflateStream.total_in = 0;
	int result = Z_OK;
	while (result != Z_STREAM_END)
	{
		size_t deflatedPreviously = output.size();
		output.resize(deflatedPreviously + DEFLATE_BUFFER_INCREMENT);
		deflateStream.next_out = (Bytef *)output.data() + deflatedPreviously;
		deflateStream.avail_out = DEFLATE_BUFFER_INCREMENT;
		deflateStream.total_out = 0;
		result = deflate(&deflateStream, Z_FINISH);
		output.resize(deflatedPreviously + deflateStream.total_out);
		if (result != Z_OK && result != Z_STREAM_END)
		{
			std::cerr << "deflate failed\n";
			return FAILURE;
		}
	}
	return SUCCESS;
}

bool Encoding::Deflate(const std::vector<uint8_t> &input,
					   std::vector<uint8_t> &output,
					   DeflateMode mode)
{
	return Deflate(std::string(input.begin(), input.end()),
				   output,
				   mode);
}

bool Encoding::Inflate(const std::vector<uint8_t> &input,
					   std::string &output,
					   DeflateMode mode)
{
	std::vector<uint8_t> inflatedBuffer(INFLATE_BUFFER_SIZE);
	z_stream inflateStream;
	inflateStream.zalloc = Z_NULL;
	inflateStream.zfree = Z_NULL;
	inflateStream.opaque = Z_NULL;
	if (mode == DEFLATE && inflateInit(&inflateStream) != Z_OK)
	{
		std::cerr << "inflateInit failed\n";
		return FAILURE;
	}
	if (mode == GZIP && inflateInit2(&inflateStream,
									 16 + MAX_WBITS) == Z_OK)
	{
		std::cerr << "inflateInit2 failed\n";
		return FAILURE;
	}
	// used to clean up the stream
	Unique_ptr<z_stream, endft> deflateStreamReference(
		&inflateStream,
		inflateEnd);
	// inflate the data
	output.clear();
	inflateStream.next_in = (Bytef *)input.data();
	inflateStream.avail_in = (uInt)input.size();
	inflateStream.total_in = 0;
	int result = Z_OK;
	while (result != Z_STREAM_END)
	{
		inflateStream.next_out = (Bytef *)inflatedBuffer.data();
		inflateStream.avail_out = inflatedBuffer.size();
		inflateStream.total_out = 0;
		result = inflate(&inflateStream, Z_FINISH);
		output += std::string(inflatedBuffer.begin(),
							  inflatedBuffer.begin() + inflateStream.total_out);
		if (result != Z_OK && result != Z_STREAM_END)
		{
			std::cerr << "inflate failed\n";
			return FAILURE;
		}
	}
	if (inflateEnd(&inflateStream) != Z_OK)
	{
		std::cerr << "inflateEnd failed\n";
		return FAILURE;
	}
	return SUCCESS;
}

static const std::string base64_chars =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

static inline bool is_base64(unsigned char c)
{
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string Encoding::Base64_Decode(std::string const &encoded_string)
{
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
	{
		char_array_4[i++] = encoded_string[in_];
		in_++;
		if (i == 4)
		{
			for (i = 0; i < 4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}
	if (i)
	{
		for (j = i; j < 4; j++)
			char_array_4[j] = 0;

		for (j = 0; j < 4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}
	return ret;
}
