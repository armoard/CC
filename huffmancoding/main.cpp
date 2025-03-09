#include "compressor.h"
#include "decompressor.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file_to_compress>" << std::endl;
        return 1;
    }

    std::string fileName = argv[1];
    std::string compressedFile = "compressed.huff";
    std::string decompressedFile = "decompressed.txt";

    Compressor compressor(fileName);
    compressor.compress(compressedFile);


    Decompressor decompressor(compressedFile);
    decompressor.decompress();  // 🔹 Se eliminó el argumento incorrecto

    
    return 0;
}