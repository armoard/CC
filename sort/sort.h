#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <algorithm>
#include <random> 
#include <unordered_map>
#include <functional>


class sorter {
    public:
        explicit sorter(std::istream& inputStream, bool uniqueFlag = false, const std::string& method)
        : input(inputStream), uflag(uniqueFlag), method(method) {}

        void start(){
            if (method == "n"){
                stdsortoption();
            }
            else if (method == "m"){
                mergesortoption();
            }
            else if (method == "q"){
                quicksortoption();
            }
            else if (method == "h"){
                heapsortoption();
            }
            else {
                randomsortoption();
            }
        }
        void stdsortoption() {
            std::vector<std::string> allWords = processistream();

            std::sort(allWords.begin(), allWords.end());  

            eraseandprint(allWords);
        }
        void mergesortoption(){
            std::vector<std::string> allWords = processistream();

            int right = allWords.size() - 1 ;
            int left = 0;

            mergeSort(allWords,left,right);

            eraseandprint(allWords);
        }
        void quicksortoption(){
            std::vector<std::string> allWords = processistream();

            int right = allWords.size() - 1;
            int left = 0;
            
            quicksort(allWords,left,right);

            eraseandprint(allWords);
        }

        void heapsortoption(){
            std::vector<std::string> allWords = processistream();
            heapsort(allWords);
            eraseandprint(allWords);
        }
        
        void randomsortoption() {
            std::vector<std::string> allWords = processistream();
            std::ifstream randStream("/dev/random", std::ios::in | std::ios::binary);
            unsigned int seed;
            randStream.read(reinterpret_cast<char*>(&seed), sizeof(seed));
            randStream.close();
    
            std::mt19937 rng(seed);
        
            std::uniform_int_distribution<uint64_t> dist;
            std::unordered_map<std::string, uint64_t> randomHashes;
        
            for (const auto& word : allWords) {
                if (randomHashes.find(word) == randomHashes.end()) {
                    randomHashes[word] = dist(rng);
                }
            }
        
            std::sort(allWords.begin(), allWords.end(), [&](const std::string& a, const std::string& b) {
                return randomHashes[a] < randomHashes[b];
            });
        
            eraseandprint(allWords);
        }
    private:
        std::istream& input;
        bool uflag;
        const std::string& method;

        void eraseandprint(std::vector<std::string>& allWords){
            if (uflag) {
                allWords.erase(std::unique(allWords.begin(), allWords.end()), allWords.end());
            }

            for (const auto& w : allWords) {
                std::cout << w << std::endl;
            }
        }
        std::vector<std::string> processistream(){
            std::vector<std::string> allWords;
            std::string word;
    
            while (input >> word) { 
                allWords.push_back(word);
            }
            input.clear();

            return allWords;
        }
        void merge(std::vector<std::string>& arr, int left, int mid, int right) {
            int n1 = mid - left + 1;  
            int n2 = right - mid;      
        
            std::vector<std::string> leftArr(n1), rightArr(n2);
        
            
            for (int i = 0; i < n1; i++)
                leftArr[i] = arr[left + i];
            for (int j = 0; j < n2; j++)
                rightArr[j] = arr[mid + 1 + j];
        
            int i = 0, j = 0, k = left;
            while (i < n1 && j < n2) {
                if (leftArr[i] <= rightArr[j]) {
                    arr[k] = leftArr[i];
                    i++;
                } else {
                    arr[k] = rightArr[j];
                    j++;
                }
                k++;
            }
        
            while (i < n1) {
                arr[k] = leftArr[i];
                i++;
                k++;
            }
            while (j < n2) {
                arr[k] = rightArr[j];
                j++;
                k++;
            }
        }

        void mergeSort(std::vector<std::string>& arr, int left, int right) {
            if (left < right) {
                int mid = left + (right - left) / 2;  
        
                mergeSort(arr, left, mid);    
                mergeSort(arr, mid + 1, right); 
        
                merge(arr, left, mid, right);  
            }
        }
        void quicksort(std::vector<std::string>& arr, int left, int right) {
            if (left >= right) return;
        
            int mid = left + (right - left) / 2;
            std::swap(arr[mid], arr[right]);  
            std::string pivot = arr[right];   
        
            int i = left - 1;
            for (int j = left; j < right; ++j) {
                if (arr[j] < pivot) {
                    ++i;
                    std::swap(arr[i], arr[j]);
                }
            }
        
            std::swap(arr[i + 1], arr[right]); 
            int pivotIndex = i + 1;
        
            quicksort(arr, left, pivotIndex - 1);
            quicksort(arr, pivotIndex + 1, right);
        }

        void heapify(std::vector<std::string>& arr, int n, int i) {
            int largest = i; 
            int left = 2 * i + 1;
            int right = 2 * i + 2;
        
        
            if (left < n && arr[left] > arr[largest])
                largest = left;
        
            
            if (right < n && arr[right] > arr[largest])
                largest = right;
        
            
            if (largest != i) {
                std::swap(arr[i], arr[largest]);
                heapify(arr, n, largest);
            }
        }
        void heapsort(std::vector<std::string>& arr) {
            int n = arr.size();
        
            for (int i = n / 2 - 1; i >= 0; i--)
                heapify(arr, n, i);
        
            for (int i = n - 1; i > 0; i--) {
                std::swap(arr[0], arr[i]);
                heapify(arr, i, 0);
            }
        }
};