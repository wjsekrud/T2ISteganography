## Image Steganography

This project implements image steganography using the Least Significant Bit (LSB) technique. 

Steganography is the practice of concealing a message or information within another medium, such as an image, in a way that only the intended recipient knows about its existence.
Features

Hide text data within an image file

Extract hidden text data from a steganographic image

Utilize the LSB technique for data hiding

Support for various image formats (PNG, JPEG, etc.)

Command-line interface for ease of use

## How It Works

The LSB technique involves modifying the least significant bit of each pixel in an image to store the binary data of the hidden message. The changes made to the image are imperceptible to the human eye, making it difficult to detect the presence of hidden information.

To hide data, the program takes a text file and an image file as input. It reads the text file and converts its contents into a binary representation. Then, it iterates over each pixel of the image and replaces the least significant bit of each color channel (RGB) with a bit from the binary data. The modified image is then saved to disk.

To extract hidden data, the program takes a steganographic image as input. It retrieves the least significant bit from each color channel of each pixel and reconstructs the binary data. The binary data is then converted back into text format and saved to an output file.

## Usage

Clone the repository:
```shell
git clone https://github.com/your-username/image-steganography.git
```
Build the project using your preferred C++ compiler.
Run the program with the desired mode and arguments:

To hide data:
```shell
./steg --hide --text path/to/text/file --image path/to/image/file --output path/to/output/file
```
- or type "H" in the CLI of the built executable and select text file to hide and target image in interactive windows which show up right next.

To extract data:
```shell
Copy code./steg --extract --stego path/to/stego/image --output path/to/output/file
```
- or type "E" in the CLI of the built executable and select target image to extract and extract location in interactive windows which show up right next.


The program will perform the requested operation and display a success message upon completion.

## Limitations

The size of the text data that can be hidden is limited by the size of the image. The program checks if the image has enough pixels to accommodate the data and displays an error message if the data is too large.
The LSB technique is susceptible to image manipulations, such as compression or editing. The hidden data may be lost or corrupted if the steganographic image undergoes such operations.

## Contributing
Contributions to this project are welcome. If you find any bugs, have suggestions for improvements, or want to add new features, please open an issue or submit a pull request.
##License
This project is licensed under the MIT License.
Feel free to customize the readme file based on your specific project details and requirements. Include any additional sections or information that you think would be helpful for users and contributors.
