
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
class Cut {
    public:
        Cut(std::vector<int> fields, char delimiter) : fields(fields), delimiter(delimiter) {}
    
        void step1(std::istream& input) {
            std::string buffer;
            while (std::getline(input, buffer)) {
                std::istringstream iss(buffer);
                std::string field;
                int column = 1;
                bool allColumns = fields.empty();  
                while (std::getline(iss, field, delimiter)) {
                    if (allColumns || std::find(fields.begin(), fields.end(), column) != fields.end()) {
                        std::cout << field << '\t';
                    }
                    column++;
                }
                std::cout << std::endl;
            }
        }

    private:
        std::vector<int> fields;
        char delimiter;
};
