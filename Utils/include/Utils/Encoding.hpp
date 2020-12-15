#ifndef __ENCODING_H__
#define __ENCODING_H__

#include <stdint.h>
#ifdef BONUS
#include <zlib.h>
#endif
#include <string>
#include <vector>

class Encoding
{
#ifdef BONUS
	static const size_t DEFLATE_BUFFER_INCREMENT = 256;
	static const size_t INFLATE_BUFFER_SIZE = 256;
	typedef int (*endft)(z_stream *);
#endif
public:
	static const bool FAILURE = true;
	static const bool SUCCESS = false;
#ifdef BONUS
	/**
	 * This is used to pick a compression mode for the Deflate method.
	 */
	enum DeflateMode
	{
		/**
		 * This selects a "zlib" data format containing "deflate" compressed
		 * data.
		 */
		DEFLATE,
		/**
		 * This selects a "zlib" data format containing "gzip" compressed
		 * data.
		 */
		GZIP,
	};
	/**
	 * This method will deflate the input and put the deflated content into
	 * the given output.
	 * @param[in] input 
	 * 		This is the content to deflate.
	 * @param[out] output
	 * 		This is where the deflated content will be stored.
	 * @param[in] mode 
	 * 		This is the chosen compression mode to use.
	 * 
	 * @return Deflate::FAILURE or Deflate::SUCCESS is returned.
	 */
	static bool Deflate(const std::string &input,
						std::vector<uint8_t> &output,
						DeflateMode mode = DEFLATE);
	/**
	 * This method will deflate the input and put the deflated content into
	 * the given output.
	 * @param[in] input 
	 * 		These are the bytes to deflate.
	 * @param[out] output
	 * 		This is where the deflated content will be stored.
	 * @param[in] mode 
	 * 		This is the chosen compression mode to use.
	 * 
	 * @return Deflate::FAILURE or Deflate::SUCCESS is returned.
	 */
	static bool Deflate(const std::vector<uint8_t> &input,
						std::vector<uint8_t> &output,
						DeflateMode mode = DEFLATE);
	/**
	 * This method will inflate the input and put the inflated content into
	 * the given output.
	 * @param[in] input 
	 * 		This is the content to inflate.
	 * @param[out] output
	 * 		This is where the inflated content will be stored.
	 * @param[in] mode
	 * 		This is the compression mode of the input.
	 * 
	 * @return Deflate::FAILURE or Deflate::SUCCESS is returned.
	 */
	static bool Inflate(const std::vector<uint8_t> &input,
						std::string &output,
						DeflateMode mode = DEFLATE);
	// static bool Inflate(const std::vector<uint8_t> &input,
	// 					std::string &output,
	// 					DeflateMode mode = DEFLATE);
#endif
	/**
	 * This method decodes the given string from base 64.
	 */
	static std::string Base64_Decode(std::string const &encodedString);
};
#endif	// __ENCODING_H__
