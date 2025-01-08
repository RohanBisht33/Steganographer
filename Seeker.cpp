#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Function to extract the hidden message
string extractMessageFromImage(const string& imageFile) {
    ifstream inFile(imageFile, ios::binary);
    if (!inFile.is_open()) {
        cerr << "Error: Could not open the image file." << endl;
        return "";
    }

    // Skip the BMP header (54 bytes)
    inFile.seekg(54);

    string message;
    char pixel;
    char currentChar = 0;
    int bitIndex = 0;

    while (inFile.get(pixel)) {
        // Extract the LSB of the pixel and add it to the current character
        currentChar |= (pixel & 1) << bitIndex;
        bitIndex++;

        if (bitIndex == 8) {
            // If we have read 8 bits, add the character to the message
            if (currentChar == '\0') break; // End of message
            message += currentChar;
            currentChar = 0;
            bitIndex = 0;
        }
    }

    inFile.close();
    return message;
}


// Main function
int main() {
    string imageFile = "output.bmp"; // Image file containing hidden message
    string extractedMessage = extractMessageFromImage(imageFile);
    if (!extractedMessage.empty()) {
        cout << "Extracted Message: " << extractedMessage << endl;
    } else {
        cout << "No hidden message found or error reading the file." << endl;
    }
    cout << "Press Enter to exit..." << endl;
    cin.get(); // Wait for user to press Enter
    return 0;
}
