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
		std::cerr << "Usage: " << argv[0] << " input-data-file output-blob-file" << std::endl;
		return -1;
	}

	char *dataPath = argv[1];
	char *blobPath = argv[2];

	// Read the data into the blob and write the blob.
	simverge::Blob blob;
	std::ifstream in(dataPath, std::ios::binary);

	if (in)
	{
		*blob.mutable_source() = dataPath;
		auto data = blob.mutable_data();

		in.unsetf(std::ios::skipws);
		in.seekg(0, std::ios::end);
		data->reserve(in.tellg());
		in.seekg(0, std::ios::beg);
		
		data->assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
		in.close();
		std::cout << "Creating new blob with " << data->size() << " bytes read from " << dataPath << std::endl;

		auto uncompressedBytes = blob.ByteSizeLong();
		std::cout << "Uncompressed blob size (source path and data): " << uncompressedBytes << " bytes" << std::endl;

		std::unique_ptr<char[]> buffer(new char[uncompressedBytes]);
		google::protobuf::io::ArrayOutputStream aos(buffer.get(), (int) uncompressedBytes);
		google::protobuf::io::GzipOutputStream gos(&aos);

		if (blob.SerializeToZeroCopyStream(&gos))
		{
			gos.Close();
			auto compressedBytes = aos.ByteCount();

			std::ofstream out(blobPath);
			out.write(buffer.get(), compressedBytes);

			std::cout << "Wrote compressed blob of size " << compressedBytes << " bytes ("
				<< (100.0 * (uncompressedBytes - compressedBytes) / uncompressedBytes) << "% compression ratio): "
				<< blobPath << std::endl;
		}
	}

	return 0;
}