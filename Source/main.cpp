#include <iostream>
#include "Steganography.h"

using namespace std;

int main(int argc, char* argv[]) {
    string modeBit = argc > 1 ? argv[1] : "";
    string objFilename = argc > 2 ? argv[1] : "";
    string srcFilename = argc > 3 ? argv[2] : "";
    string outFilename = argc > 4 ? argv[3] : "";

    cout << "Choose Operation Mode: " << endl;
    cout << "[Hide: H || Extract: E]" << endl;
    cin >> modeBit;

    Steganography steg;
    if (modeBit[0] == 'H') {
        //steg.hide(objFilename, srcFilename, outFilename);
        steg.hideLSB(objFilename, srcFilename, outFilename);
    }
    else if (modeBit[0] == 'E') {
        //steg.extract(objFilename, srcFilename, outFilename);
        steg.extractLSB(objFilename, outFilename);
    }
    else {
        cout << "Invalid Mode Bit, Operation Closing..." << endl;
    }
    return 0;
}