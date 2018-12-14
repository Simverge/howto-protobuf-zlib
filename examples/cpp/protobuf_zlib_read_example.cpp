/*
 * MIT License
 *
 * Copyright (c) 2018 Simverge Software LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "examples/cpp/blob.pb.h"

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/io/gzip_stream.h>

#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	if (argc < 3)
	{
		std::cerr << "Usage: " << argv[0] << " input-protobuf-file output-data-file" << std::endl;
		return -1;
	}

	char *protobufPath = argv[1];
	char *dataPath = argv[2];

	// Read the blob and write its contents to the data file.
	simverge::Blob blob;

	std::ifstream in(protobufPath, std::ios::binary);

	if (in)
	{
		in.unsetf(std::ios::skipws);
		in.seekg(0, std::ios::end);
		std::cout << "Read file with " << in.tellg() << " bytes: " << protobufPath << std::endl;
		in.seekg(0, std::ios::beg);

		google::protobuf::io::IstreamInputStream iss(&in);
		google::protobuf::io::GzipInputStream gis(&iss);

		if (blob.ParseFromZeroCopyStream(&gis))
		{
			if (gis.ZlibErrorCode() > 0)
			{
				std::cout << "Decompressed and parsed Protobuf message: " << blob.ByteSize() << " bytes" << std::endl;
				std::cout << "Message contains " << blob.data().size() << " bytes from " << blob.source() << std::endl;

				std::ofstream out(dataPath, std::ios::binary);
				out.write(blob.data().c_str(), blob.data().size());
				std::cout << "Wrote Protobuf message data to " << dataPath << std::endl;
			}
			else
			{
				std::cerr << "Could not decompress Protobuf message (zlib error "
					<< gis.ZlibErrorCode() << ")";
				auto message = gis.ZlibErrorMessage();

				if (message)
				{
					std::cerr << ": " << message;
				}

				std::cerr << std::endl;
			}
		}
		else
		{
			std::cerr << "Could not parse input Protobuf file: " << protobufPath << std::endl;
		}
	}
	else
	{
		std::cerr << "Could not read input Protobuf file: " << protobufPath << std::endl;
	}

	return 0;
}