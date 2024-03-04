#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

const int nmax = 100; // Số lượng bản mã tối đa

using namespace std;

int countCipher = 0; // đếm số lượng bản mã trong mảng
int maxLen = 0; // chỉ sô lưu trữ độ dài lớn nhất của bản mã trong mảng
string cipher[nmax]; // Mảng string để lưu trữ bản mã đọc từ file
string binaryCipher[nmax]; // Mảng string để lưu trữ bản mã nhị phân
string targetCipher; // Bản mã cần giải mã
string binaryTargetCipher; // Bản mã nhị phân cần giải mã

// đọc các bản mã hóa từ file data.txt
void getData(){
    ifstream file_input("data.txt"); // Mở file để đọc
    ifstream file_target_input("target.txt"); // Mở file để đọc

    if (!file_input.is_open() || !file_target_input.is_open()) {
        cout << "Không thể mở file." << '\n';
        return;
    }

    // Đọc từng dòng từ file và gán vào mảng string
    while (countCipher < nmax && getline(file_input, cipher[countCipher])) {
        if(cipher[countCipher].size() > maxLen ) maxLen = cipher[countCipher].size();
        countCipher++;
    }

    // Đọc bản mã cần giải mã
    getline(file_target_input, targetCipher);

    file_target_input.close(); // Đóng file sau khi đọc xong
    file_input.close(); // Đóng file sau khi đọc xong
}

// Chuyển đổi các ký tự từ hex sang nhị phân
string hexToBinary(char hex) {
    switch (hex) {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'a': return "1010";
        case 'b': return "1011";
        case 'c': return "1100";
        case 'd': return "1101";
        case 'e': return "1110";
        case 'f': return "1111";
        default:
            cout << "Invalid hex digit: " << hex << '\n';
            return "";
    }
}

// Chuyển đổi chuỗi hex sang chuỗi nhị phân
string hexToBinaryString(string hex) {
    string binaryString;
    for (char c : hex) {
        binaryString += hexToBinary(c);
    }
    return binaryString;
}

// chuyển các bản mã đầu vào sang dạng nhị phân
void hexStringToBinaryString(){
    for (int i = 0; i < countCipher; i++) {
        binaryCipher[i] = hexToBinaryString(cipher[i]);
    }
    binaryTargetCipher = hexToBinaryString(targetCipher);
}

// Thêm bit 0 vào trước chuỗi nhị phân để tất cả bản mã có độ dài bằng nhau
string padBinary(string binary, int targetLength) {
    if (binary.length() < targetLength) {
        // Thêm bit 0 vào trước dãy nhị phân
        return binary + string(targetLength - binary.length(), '0');
    }
    return binary;
}

// Thực hiện phép XOR giữa hai chuỗi nhị phân
string binaryXOR(string binary1, string binary2) {
    // Đảm bảo độ dài của hai dãy nhị phân bằng nhau
    int maxLength = max(binary1.length(), binary2.length());
    string paddedBinary1 = padBinary(binary1, maxLength);
    string paddedBinary2 = padBinary(binary2, maxLength);

    // Thực hiện phép XOR
    string result;
    for (int i = 0; i < maxLength; i++) {
        result += (paddedBinary1[i] == paddedBinary2[i]) ? '0' : '1';
    }

    return result;
}

// Thực hiện phép XOR giữa bản mã cần giải mã và key
string binaryXORTarget(string binaryTargetCipher, string key, bool checkKey[]){
    int lenGood = binaryTargetCipher.size();
    string paddedBinary1 = padBinary(binaryTargetCipher, lenGood);
    string paddedBinary2 = padBinary(key, lenGood);

    // Thực hiện phép XOR
    string result;
    for (int i = 0; i < lenGood; i+=8) {
        if(checkKey[i/8]){
            for(int j = i; j < i+8; j++)
                result += (paddedBinary1[j] == paddedBinary2[j]) ? '0' : '1';
        } else {
            result += "00101010";
        }
    }
    return result;
}

// chuyển nhị phân sang thập phân
int binaryToDecimal(string n) {
    int ans = 0;
    for(int i = 0; i < 8; i++){
        int temp = (n[i] == '1') ? 1 : 0;
        ans += temp * (1 << (7-i));
    }
    return ans;
}

int main() {
    getData();
    hexStringToBinaryString();

    string key = ""; // key để giải mã dạng nhị phân ,"00101010"
    bool checkKey[1000] = {false}; // mảng kiểm tra vị trí key đã được xác định chưa
    string ans = ""; // chuỗi lưu trữ kết quả giải mã

    //Khởi tạo mảng lưu trữ khả năng xuất hiện khoảng trắng
    int** isSpace = new int*[nmax];
    for (int i = 0; i < nmax; i++) {
        isSpace[i] = new int[maxLen/2];
       memset(isSpace[i], 0, (maxLen/2)*sizeof(int));
    }

    // Thực hiện XOR giữa các bản mã
    for (int i = 0; i < countCipher; i++) {
        for (int j = i+1; j < countCipher; j++) {
            string result = binaryXOR(binaryCipher[i], binaryCipher[j]);
            int len1 = binaryCipher[i].size();
            int len2 = binaryCipher[j].size();
            int lenResult = result.size();
            for (int k = 0; k < result.size(); k += 8) {
                string sub = result.substr(k, 8);
                if(sub == "00000000" || (sub >= "01000001" && sub <= "01011010") || (sub >= "01100001" && sub <= "01111010")){
                    if(k <= len1) isSpace[i][k/8]++;
                    if(k <= len2) isSpace[j][k/8]++;
                }
            }
        }
    }

    // Sinh key từ khả năng xuất hiện khoảng trắng
    for(int i = 0; i < maxLen/2; i++){
        int max = 0, mark = 0;
        for(int j = 0; j < countCipher; j++){
            int lenCipher = binaryCipher[j].size();
            if(i*8 <= lenCipher && isSpace[j][i] > max){
                max = isSpace[j][i];
                mark = j;
            }
        }
        if(max >= 7){
            key += binaryXOR(binaryCipher[mark].substr(8*i, 8), "00100000");
            checkKey[i] = true;
        } else {
            key += "00000000";
        }
    }

    // for(int  i = 0; i < 100; i++){
    //     cout <<"vị trí" << i <<"có check = "<< checkKey[i] << "\n";
    // }

    // giải mã bản mã cuối cùng
    string result = binaryXORTarget(binaryTargetCipher, key, checkKey);
    // string result = binaryXOR(binaryTargetCipher, key);
    for(int i = 0; i < result.size(); i += 8){
        string tmp = result.substr(i, 8);
        ans += (char) binaryToDecimal(tmp);
    }
    cout << "Bản rõ chương trình cho ra là: " <<  ans << '\n';

    // Hoàn thiện bản rõ thủ công ta sẽ được
    cout << "Bản rõ sau khi chỉnh thủ công: " << "The secret message is: When using a stream cipher, never use the key more than once" << '\n';

    // giải phóng bộ nhớ
    for (int i = 0; i < nmax; i++) {
        delete[] isSpace[i];
    }
    delete[] isSpace;

    return 0;
}
