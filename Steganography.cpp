//This project requires an image of 24 bit in BMP format, the image that is created after the prorcessing is also in bmp format that can be later transformed into jpg or png
//the message should contain "####" at the ending to indentify the ending of the text.

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
using namespace std;

const int HEADER_SIZE = 54;
const string DELIMITER = "####"; // End-of-message marker

// Convert a string message to a binary string
string toBinary(const string& message) {
    string binary;
    for (char c : message) {
        binary += bitset<8>(c).to_string();
    }
    return binary;
}

// Convert a binary string to original message
string fromBinary(const string& binary) {
    string message;
    for (size_t i = 0; i + 7 < binary.size(); i += 8) {
        char c = bitset<8>(binary.substr(i, 8)).to_ulong();
        message += c;
        if (message.size() >= 4 && message.substr(message.size() - 4) == DELIMITER) {
            message.erase(message.end() - 4, message.end());
            break;
        }
    }
    return message;
}

// Embed binary data into image pixels
void hideMessage(const string& inputImage, const string& outputImage, const string& message) {
    ifstream in(inputImage, ios::binary);
    if (!in) {
        cerr << "Error: Cannot open input image.\n";
        return;
    }

    // Read header and pixel data
    char header[HEADER_SIZE];
    in.read(header, HEADER_SIZE);
    vector<unsigned char> pixels((istreambuf_iterator<char>(in)), {});
    in.close();

    // Convert message to binary with delimiter
    string binary = toBinary(message + DELIMITER);

    if (binary.size() > pixels.size()) {
        cerr << "Error: Message too large to fit in image.\n";
        return;
    }

    // Modify LSBs to embed message
    for (size_t i = 0; i < binary.size(); ++i) {
        pixels[i] &= 0xFE;                      // Clear LSB
        pixels[i] |= (binary[i] - '0');         // Set LSB to message bit
    }

    // Write to output image
    ofstream out(outputImage, ios::binary);
    out.write(header, HEADER_SIZE);
    out.write(reinterpret_cast<char*>(pixels.data()), pixels.size());
    out.close();

    cout << "Message successfully hidden in " << outputImage << "\n";
}

// Extract hidden message from stego image
void extractMessage(const string& stegoImage) {
    ifstream in(stegoImage, ios::binary);
    if (!in) {
        cerr << "Error: Cannot open stego image.\n";
        return;
    }

    char header[HEADER_SIZE];
    in.read(header, HEADER_SIZE);
    vector<unsigned char> pixels((istreambuf_iterator<char>(in)), {});
    in.close();

    string binary;
    for (unsigned char byte : pixels) {
        binary += (byte & 1) ? '1' : '0';
    }

    string message = fromBinary(binary);
    cout << "Extracted Message: " << message << "\n";
}

// Main menu
int main() {
    int choice;
    cout << "===== LSB Steganography in C++ (BMP) =====\n";
    cout << "1. Hide a message\n";
    cout << "2. Extract a message\n";
    cout << "Enter choice: ";
    cin >> choice;
    cin.ignore(); // Flush newline

    if (choice == 1) {
        string inputImage, outputImage, message;
        cout << "Enter input BMP image name (24-bit): ";
        getline(cin, inputImage);
        cout << "Enter output image name: ";
        getline(cin, outputImage);
        cout << "Enter message to hide: ";
        getline(cin, message);
        hideMessage(inputImage, outputImage, message);
    } else if (choice == 2) {
        string stegoImage;
        cout << "Enter image name with hidden message: ";
        getline(cin, stegoImage);
        extractMessage(stegoImage);
    } else {
        cout << "Invalid choice.\n";
    }

    return 0;
}

