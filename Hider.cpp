#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

// Function to hide the message
void hideMessageInImage(const string& inputImage, const string& outputImage, const string& message) {
    ifstream inFile(inputImage, ios::binary);
    ofstream outFile(outputImage, ios::binary);

    if (!inFile.is_open() || !outFile.is_open()) {
        cerr << "Error: Could not open the image file." << endl;
        return;
    }

    // Read the BMP header (54 bytes)
    vector<char> header(54);
    inFile.read(header.data(), 54);
    outFile.write(header.data(), 54);

    // Prepare the message with a delimiter (null character '\0')
    string secretMessage = message + char(0);

    // Embed the message in the pixel data
    char pixel;
    size_t messageIndex = 0, bitIndex = 0;

    while (inFile.get(pixel)) {
        if (messageIndex < secretMessage.size()) {
            // Modify the least significant bit of the pixel
            pixel = (pixel & 0xFE) | ((secretMessage[messageIndex] >> bitIndex) & 1);
            bitIndex++;
            if (bitIndex == 8) {
                bitIndex = 0;
                messageIndex++;
            }
        }
        outFile.put(pixel);
    }

    // Close files
    inFile.close();
    outFile.close();

    cout << "Message hidden successfully in " << outputImage << endl;
}

// Main function
int main() {
    string inputImage = "input.bmp"; // Input BMP file
    string outputImage = "output.bmp"; // Output BMP file with hidden message
    string message;

    cout << "Enter the secret message to hide: ";
    getline(cin, message);

    hideMessageInImage(inputImage, outputImage, message);
    getchar();
    return 0;
}
