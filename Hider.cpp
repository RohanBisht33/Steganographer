#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

using namespace std;

// Function to hide message in BMP image
void hideMessageInBMP(const string& inputFile, const string& outputFile, const string& message) {
    ifstream inFile(inputFile, ios::binary);
    ofstream outFile(outputFile, ios::binary);

    if (!inFile.is_open() || !outFile.is_open()) {
        cerr << "Error: Could not open the BMP file." << endl;
        return;
    }

    // Read the BMP header (54 bytes)
    vector<char> header(54);
    inFile.read(header.data(), 54);
    outFile.write(header.data(), 54);

    // Prepare the message with a delimiter and type identifier
    string secretMessage = "BMP:" + message + char(0);

    // Embed the message in the pixel data
    char pixel;
    size_t messageIndex = 0, bitIndex = 0;

    while (inFile.get(pixel)) {
        if (messageIndex < secretMessage.size()) {
            pixel = (pixel & 0xFE) | ((secretMessage[messageIndex] >> bitIndex) & 1);
            bitIndex++;
            if (bitIndex == 8) {
                bitIndex = 0;
                messageIndex++;
            }
        }
        outFile.put(pixel);
    }

    inFile.close();
    outFile.close();
    cout << "Message hidden successfully in BMP image: " << outputFile << endl;
}

// Function to hide message in WAV audio file
void hideMessageInWAV(const string& inputFile, const string& outputFile, const string& message) {
    ifstream inFile(inputFile, ios::binary);
    ofstream outFile(outputFile, ios::binary);

    if (!inFile.is_open() || !outFile.is_open()) {
        cerr << "Error: Could not open the WAV file." << endl;
        return;
    }

    // Read WAV header (44 bytes)
    vector<char> header(44);
    inFile.read(header.data(), 44);
    outFile.write(header.data(), 44);

    // Prepare the message with type identifier
    string secretMessage = "WAV:" + message + char(0);

    // Embed the message in the audio data
    char sample;
    size_t messageIndex = 0, bitIndex = 0;

    while (inFile.get(sample)) {
        if (messageIndex < secretMessage.size()) {
            sample = (sample & 0xFE) | ((secretMessage[messageIndex] >> bitIndex) & 1);
            bitIndex++;
            if (bitIndex == 8) {
                bitIndex = 0;
                messageIndex++;
            }
        }
        outFile.put(sample);
    }

    inFile.close();
    outFile.close();
    cout << "Message hidden successfully in WAV audio: " << outputFile << endl;
}

// Function to hide message in text file using whitespace
void hideMessageInText(const string& inputFile, const string& outputFile, const string& message) {
    ifstream inFile(inputFile);
    ofstream outFile(outputFile);

    if (!inFile.is_open() || !outFile.is_open()) {
        cerr << "Error: Could not open the text file." << endl;
        return;
    }

    // Prepare the message with type identifier
    string secretMessage = "TXT:" + message + char(0);
    
    string line;
    size_t messageIndex = 0, bitIndex = 0;
    bool messageComplete = false;

    while (getline(inFile, line) && !messageComplete) {
        outFile << line;
        
        // Add spaces/tabs based on message bits - continue until message is complete
        while (messageIndex < secretMessage.size()) {
            int bit = (secretMessage[messageIndex] >> bitIndex) & 1;
            if (bit == 1) {
                outFile << " "; // Space for bit 1
            } else {
                outFile << "\t"; // Tab for bit 0
            }
            bitIndex++;
            if (bitIndex == 8) {
                bitIndex = 0;
                messageIndex++;
                if (messageIndex >= secretMessage.size()) {
                    messageComplete = true;
                    break;
                }
            }
        }
        outFile << "\n";
    }

    // Copy remaining lines if any
    while (getline(inFile, line)) {
        outFile << line << "\n";
    }

    inFile.close();
    outFile.close();
    cout << "Message hidden successfully in text file: " << outputFile << endl;
}

// Function to create a dummy carrier file if input doesn't exist
void createDummyCarrier(int choice) {
    switch(choice) {
        case 1: {
            // Create a simple BMP file (minimal header + some pixel data)
            ofstream bmpFile("input.bmp", ios::binary);
            if (bmpFile.is_open()) {
                // Simple BMP header for a 2x2 image
                char bmpHeader[54] = {
                    'B', 'M', 66, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0,
                    40, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 24, 0,
                    0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                bmpFile.write(bmpHeader, 54);
                // Add some pixel data
                for (int i = 0; i < 100; i++) {
                    bmpFile.put(char(128)); // Gray pixels
                }
                bmpFile.close();
                cout << "Created dummy BMP file: input.bmp" << endl;
            }
            break;
        }
        case 2: {
            // Create a simple WAV file
            ofstream wavFile("input.wav", ios::binary);
            if (wavFile.is_open()) {
                // Simple WAV header - using unsigned char to avoid narrowing
                unsigned char wavHeader[44] = {
                    'R', 'I', 'F', 'F', 36, 1, 0, 0, 'W', 'A', 'V', 'E',
                    'f', 'm', 't', ' ', 16, 0, 0, 0, 1, 0, 1, 0,
                    68, 172, 0, 0, 68, 172, 0, 0, 1, 0, 8, 0,
                    'd', 'a', 't', 'a', 0, 1, 0, 0
                };
                wavFile.write((char*)wavHeader, 44);
                // Add some audio data
                for (int i = 0; i < 256; i++) {
                    wavFile.put(char(128 + (i % 50))); // Simple waveform
                }
                wavFile.close();
                cout << "Created dummy WAV file: input.wav" << endl;
            }
            break;
        }
        case 3: {
            // Create a simple text file
            ofstream txtFile("input.txt");
            if (txtFile.is_open()) {
                txtFile << "This is a sample text file for steganography.\n";
                txtFile << "You can hide secret messages in the whitespace.\n";
                txtFile << "Each line can contain hidden information.\n";
                txtFile << "The message is encoded using spaces and tabs.\n";
                txtFile << "This provides a covert communication channel.\n";
                txtFile.close();
                cout << "Created dummy text file: input.txt" << endl;
            }
            break;
        }
    }
}

// Main function
int main() {
    cout << "\t\t\t\tWELCOME TO THE ENHANCED STEGANOGRAPHER HIDER APPLICATION\n\n" << endl;
    
    cout << "Choose the method to hide your message:" << endl;
    cout << "1. Hide in BMP Image (.bmp)" << endl;
    cout << "2. Hide in WAV Audio (.wav)" << endl;
    cout << "3. Hide in Text File (.txt)" << endl;
    cout << "Enter your choice (1-3): ";
    
    int choice;
    cin >> choice;
    cin.ignore(); // Clear the input buffer
    
    if (choice < 1 || choice > 3) {
        cout << "Invalid choice! Please select 1, 2, or 3." << endl;
        return 1;
    }
    
    string message;
    cout << "Enter the secret message to hide: ";
    getline(cin, message);
    
    string inputFile, outputFile;
    
    switch(choice) {
        case 1:
            inputFile = "input.bmp";
            outputFile = "output.bmp";
            // Check if input file exists, create dummy if not
            {
                ifstream check(inputFile);
                if (!check.is_open()) {
                    cout << "Input BMP file not found. Creating a dummy file..." << endl;
                    createDummyCarrier(1);
                }
            }
            hideMessageInBMP(inputFile, outputFile, message);
            break;
            
        case 2:
            inputFile = "input.wav";
            outputFile = "output.wav";
            // Check if input file exists, create dummy if not
            {
                ifstream check(inputFile);
                if (!check.is_open()) {
                    cout << "Input WAV file not found. Creating a dummy file..." << endl;
                    createDummyCarrier(2);
                }
            }
            hideMessageInWAV(inputFile, outputFile, message);
            break;
            
        case 3:
            inputFile = "input.txt";
            outputFile = "output.txt";
            // Check if input file exists, create dummy if not
            {
                ifstream check(inputFile);
                if (!check.is_open()) {
                    cout << "Input text file not found. Creating a dummy file..." << endl;
                    createDummyCarrier(3);
                }
            }
            hideMessageInText(inputFile, outputFile, message);
            break;
    }
    
    cout << "\nHiding process completed!" << endl;
    cout << "Press Enter to exit...";
    cin.get();
    return 0;
}