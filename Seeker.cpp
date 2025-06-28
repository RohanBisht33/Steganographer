#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

// Function to detect file type by extension
string getFileType(const string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != string::npos) {
        string extension = filename.substr(dotPos + 1);
        // Convert to lowercase
        for (auto& c : extension) {
            c = tolower(c);
        }
        return extension;
    }
    return "";
}

// Function to extract message from BMP file
string extractFromBMP(const string& filename) {
    ifstream inFile(filename, ios::binary);
    if (!inFile.is_open()) {
        return "";
    }

    // Skip the BMP header (54 bytes)
    inFile.seekg(54);

    string message;
    char pixel;
    char currentChar = 0;
    int bitIndex = 0;

    while (inFile.get(pixel)) {
        currentChar |= (pixel & 1) << bitIndex;
        bitIndex++;

        if (bitIndex == 8) {
            if (currentChar == '\0') break;
            message += currentChar;
            currentChar = 0;
            bitIndex = 0;
        }
    }

    inFile.close();
    
    // Check if message starts with BMP identifier
    if (message.substr(0, 4) == "BMP:") {
        return message.substr(4); // Return message without identifier
    }
    return message;
}

// Function to extract message from WAV file
string extractFromWAV(const string& filename) {
    ifstream inFile(filename, ios::binary);
    if (!inFile.is_open()) {
        return "";
    }

    // Skip the WAV header (44 bytes)
    inFile.seekg(44);

    string message;
    char sample;
    char currentChar = 0;
    int bitIndex = 0;

    while (inFile.get(sample)) {
        currentChar |= (sample & 1) << bitIndex;
        bitIndex++;

        if (bitIndex == 8) {
            if (currentChar == '\0') break;
            message += currentChar;
            currentChar = 0;
            bitIndex = 0;
        }
    }

    inFile.close();
    
    // Check if message starts with WAV identifier
    if (message.substr(0, 4) == "WAV:") {
        return message.substr(4); // Return message without identifier
    }
    return message;
}

// Function to extract message from text file using whitespace
string extractFromText(const string& filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        return "";
    }

    string message;
    string line;
    char currentChar = 0;
    int bitIndex = 0;

    while (getline(inFile, line)) {
        // Look for trailing whitespace pattern
        size_t contentEnd = line.find_last_not_of(" \t");
        if (contentEnd != string::npos && contentEnd + 1 < line.length()) {
            string whitespace = line.substr(contentEnd + 1);
            
            for (char c : whitespace) {
                int bit = (c == ' ') ? 1 : 0; // Space = 1, Tab = 0
                currentChar |= bit << bitIndex;
                bitIndex++;
                
                if (bitIndex == 8) {
                    if (currentChar == '\0') {
                        inFile.close();
                        // Check if message starts with TXT identifier
                        if (message.length() >= 4 && message.substr(0, 4) == "TXT:") {
                            return message.substr(4);
                        }
                        return message;
                    }
                    message += currentChar;
                    currentChar = 0;
                    bitIndex = 0;
                }
            }
        }
        
        // If we have a partial character and hit end of whitespace, continue to next line
        // This allows the message to span multiple lines
    }

    inFile.close();
    
    // Check if message starts with TXT identifier
    if (message.length() >= 4 && message.substr(0, 4) == "TXT:") {
        return message.substr(4);
    }
    return message;
}

// Function to try all extraction methods
string extractMessage(const string& filename) {
    string fileType = getFileType(filename);
    string message;

    cout << "Analyzing file: " << filename << " (Type: " << fileType << ")" << endl;

    // Try extraction based on file extension first
    if (fileType == "bmp") {
        message = extractFromBMP(filename);
        if (!message.empty()) {
            cout << "Successfully extracted message from BMP image!" << endl;
            return message;
        }
    }
    else if (fileType == "wav") {
        message = extractFromWAV(filename);
        if (!message.empty()) {
            cout << "Successfully extracted message from WAV audio!" << endl;
            return message;
        }
    }
    else if (fileType == "txt") {
        message = extractFromText(filename);
        if (!message.empty()) {
            cout << "Successfully extracted message from text file!" << endl;
            return message;
        }
    }

    // If extension-based extraction fails, try all methods
    cout << "Trying BMP extraction method..." << endl;
    message = extractFromBMP(filename);
    if (!message.empty()) {
        cout << "Successfully extracted message using BMP method!" << endl;
        return message;
    }

    cout << "Trying WAV extraction method..." << endl;
    message = extractFromWAV(filename);
    if (!message.empty()) {
        cout << "Successfully extracted message using WAV method!" << endl;
        return message;
    }

    cout << "Trying text extraction method..." << endl;
    message = extractFromText(filename);
    if (!message.empty()) {
        cout << "Successfully extracted message using text method!" << endl;
        return message;
    }

    return "";
}

// Function to auto-detect and process output files only
vector<string> findPossibleFiles() {
    vector<string> files;
    vector<string> outputFiles = {"output.bmp", "output.wav", "output.txt"};
    
    try {
        for (const auto& entry : fs::directory_iterator(".")) {
            if (entry.is_regular_file()) {
                string filename = entry.path().filename().string();
                
                // Check if it's an output file or starts with "output"
                for (const string& outputFile : outputFiles) {
                    if (filename == outputFile) {
                        files.push_back(filename);
                        break;
                    }
                }
                
                // Also check for files that start with "output" and have supported extensions
                if (filename.substr(0, 6) == "output" && filename.length() > 6) {
                    string fileType = getFileType(filename);
                    if (fileType == "bmp" || fileType == "wav" || fileType == "txt") {
                        // Make sure it's not already in the list
                        bool alreadyAdded = false;
                        for (const string& file : files) {
                            if (file == filename) {
                                alreadyAdded = true;
                                break;
                            }
                        }
                        if (!alreadyAdded) {
                            files.push_back(filename);
                        }
                    }
                }
            }
        }
    } catch (const exception& e) {
        // If filesystem operations fail, try common output files
        for (const string& file : outputFiles) {
            ifstream check(file);
            if (check.is_open()) {
                files.push_back(file);
                check.close();
            }
        }
    }
    
    return files;
}

// Main function
int main() {
    cout << "\t\t\t\tWELCOME TO THE ENHANCED STEGANOGRAPHER SEEKER APPLICATION\n\n" << endl;
    
    // Auto-detect possible files
    vector<string> possibleFiles = findPossibleFiles();
    
    string filename;
    
    if (!possibleFiles.empty()) {
        cout << "Found the following output files that might contain hidden messages:" << endl;
        for (size_t i = 0; i < possibleFiles.size(); i++) {
            cout << (i + 1) << ". " << possibleFiles[i] << endl;
        }
        cout << (possibleFiles.size() + 1) << ". Enter custom filename" << endl;
        cout << "Choose an option: ";
        
        int choice;
        cin >> choice;
        cin.ignore();
        
        if (choice >= 1 && choice <= (int)possibleFiles.size()) {
            filename = possibleFiles[choice - 1];
        } else {
            cout << "Enter the filename: ";
            getline(cin, filename);
        }
    } else {
        cout << "No output files found. Enter the filename containing the hidden message: ";
        getline(cin, filename);
    }
    
    // Check if file exists
    ifstream checkFile(filename);
    if (!checkFile.is_open()) {
        cout << "Error: File '" << filename << "' not found!" << endl;
        cout << "Press Enter to exit...";
        cin.get();
        return 1;
    }
    checkFile.close();
    
    cout << "\nProcessing file: " << filename << endl;
    cout << "Attempting to extract hidden message...\n" << endl;
    
    string extractedMessage = extractMessage(filename);
    
    if (!extractedMessage.empty()) {
        cout << "\n" << string(50, '=') << endl;
        cout << "EXTRACTED MESSAGE: " << extractedMessage << endl;
        cout << string(50, '=') << endl;
        
        // Ask if user wants to delete the file
        cout << "\nDo you want to delete the file '" << filename << "' after extraction? (y/n): ";
        char deleteChoice;
        cin >> deleteChoice;
        
        if (deleteChoice == 'y' || deleteChoice == 'Y') {
            if (remove(filename.c_str()) == 0) {
                cout << "File '" << filename << "' deleted successfully." << endl;
            } else {
                cerr << "Failed to delete the file '" << filename << "'." << endl;
            }
        }
    } else {
        cout << "\nNo hidden message found in the file." << endl;
        cout << "The file might not contain a steganographically hidden message," << endl;
        cout << "or it might use a different encoding method." << endl;
    }
    
    cout << "\nPress Enter to exit...";
    cin.ignore();
    cin.get();
    return 0;
}