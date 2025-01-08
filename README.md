Image Steganography - Hide Secret Messages in BMP Images
Overview

This project allows you to hide a secret message inside a 24-bit BMP image using Least Significant Bit (LSB) steganography. The program takes an input BMP image, embeds a user-provided message into the image’s pixel data, and saves the modified image as a new output file. The hidden message remains invisible to the naked eye but can be extracted using a decoding method.

Features
Hides text messages inside an image file.
Uses LSB steganography technique to modify only the least significant bit of the image's pixels, keeping changes imperceptible.
Supports 24-bit BMP files as input.
Outputs a modified BMP file with the secret message encoded.
Prerequisites
C++ Compiler: You need a C++ compiler (such as g++ or Visual Studio) to compile and run the code.
24-bit BMP Image: The input image should be in the 24-bit BMP format.
How to Use
1. Prepare the Input Image
Make sure the image you want to use is a 24-bit BMP image. You can create one using any image editing software like Paint or GIMP.

Place the BMP file in the same directory as the program executable. For example, name it input.bmp.

2. Run the Program
Compile the code and run the executable. The program will prompt you to enter a secret message that you want to hide inside the image.

bash
Copy code
Enter the secret message to hide: <Your secret message here>
3. Output Image
After you enter the secret message, the program will create a new image file with the hidden message, named output.bmp. You can open this image file in any image viewer, but the hidden message will not be visible to the human eye.
