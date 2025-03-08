#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

class Decompressor{
    public: 
        void getHeaderInfo(std::string fileName){
            std::ifstream fileInput(fileName,std::ios::binary);
            if (!fileInput){
                std::cerr << "Could not open filename" << std::endl;
                return;
            }
            fileInput >> unique;
            for (int i = 0 ; i < unique ; i++){
                int charCode;
                std::string huffmanCode;
                fileInput >> charCode >> huffmanCode;  
                huffmanTable[(wchar_t)charCode] = huffmanCode;  
            }
            fileInput.read((char*)&toRead, sizeof(toRead));
            char buffer;
            while (fileInput.read(&buffer, 1)) {  
                for (int i = 7; i >= 0; i--) {    
                    compressedBits.push_back((buffer >> i) & 1);
                }
            }
            fileInput.close();
        }
    private:
        std::map<wchar_t, std::string> huffmanTable;
        int unique = 0;
        uint32_t toRead = 0;
        std::vector<bool> compressedBits;
};