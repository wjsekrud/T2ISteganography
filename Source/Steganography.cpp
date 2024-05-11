#include "Steganography.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace cv;

Steganography::Steganography() {}

std::string Steganography::makestr(wstring&& value)
{
    string tmp;
    tmp.assign(value.begin(), value.end());
    return tmp;
}


string Steganography::selectFile(const LPCWSTR& title, const LPCWSTR& filter) {
    OPENFILENAME ofn;
    TCHAR szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    ofn.lpstrTitle = title;

    if (GetOpenFileName(&ofn) == TRUE) {
        return this->makestr(&*ofn.lpstrFile);
    }
    return string();
}

std::string Steganography::saveFile(const LPCWSTR& title, const LPCWSTR& filter)
{
    OPENFILENAME ofn;
    TCHAR szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_OVERWRITEPROMPT;
    ofn.lpstrTitle = title;

    if (GetSaveFileName(&ofn) == TRUE) {
        string t = this->makestr(ofn.lpstrFile);
        return t;
    }
    return string();
}

void Steganography::hideLSB(const std::string& textFile, const std::string& imageFile, const std::string& outputFile)
{
    string objFilename = textFile.empty() ? selectFile(L"텍스트 파일 선택", L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0") : textFile;
    string srcFilename = imageFile.empty() ? selectFile(L"이미지 파일 선택", L"Image Files (*.png;*.jpg)\0*.png;*.jpg\0All Files (*.*)\0*.*\0") : imageFile;
    string outFilename = outputFile.empty() ? saveFile(L"출력 파일 저장", L"PNG Files (*.png)\0*.png\0All Files (*.*)\0*.*\0") : outputFile;

    if (objFilename.empty() || srcFilename.empty() || outFilename.empty()) {
        cout << "파일 선택이 취소되었습니다." << endl;
        return;
    }

    vector<bool> data = readTextFile(objFilename);

    Mat img = imread(srcFilename, IMREAD_COLOR);
    if (img.empty()) {
        cout << "이미지를 열 수 없습니다." << endl;
        return;
    }

    int dataSize = data.size();
    if (img.rows * img.cols < dataSize) {
        cout << "이미지 크기가 데이터를 숨기기에 충분하지 않습니다." << endl;
        return;
    }

    int dataIndex = 0;
    for (int row = 0; row < img.rows; ++row) {
        for (int col = 0; col < img.cols; ++col) {
            if (dataIndex < dataSize) {
                Vec3b& pixel = img.at<Vec3b>(row, col);
                for (int ch = 0; ch < 3; ++ch) {
                    if (dataIndex < dataSize) {
                        pixel[ch] = (pixel[ch] & 0xFE) | data[dataIndex];
                        ++dataIndex;
                    }
                    else {
                        break;
                    }
                }
            }
            else {
                break;
            }
        }
    }

    imwrite(outFilename, img);
    cout << "데이터가 이미지에 숨겨졌습니다." << endl;
}

void Steganography::extractLSB(const std::string& stegoImage, const std::string& outputFile)
{

    string stegoImgName = stegoImage.empty() ? selectFile(L"은닉 파일 선택", L"Image Files (*.png;*.jpg)\0*.png;*.jpg\0All Files (*.*)\0*.*\0") : stegoImage;
    string outFilename = outputFile.empty() ? saveFile(L"출력 파일 저장", L"TXT Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0") : outputFile;

    if (stegoImgName.empty() ||  outFilename.empty()) {
        cout << "파일 선택이 취소되었습니다." << endl;
        return;
    }

    Mat img = imread(stegoImgName, IMREAD_COLOR);
    if (img.empty()) {
        cout << "이미지를 열 수 없습니다." << endl;
        return;
    }

    vector<bool> extractedBits;
    for (int row = 0; row < img.rows; ++row) {
        for (int col = 0; col < img.cols; ++col) {
            Vec3b pixel = img.at<Vec3b>(row, col);
            for (int ch = 0; ch < 3; ++ch) {
                extractedBits.push_back(pixel[ch] & 1);
            }
        }
    }

    vector<char> extractedBytes;
    for (int i = 0; i < extractedBits.size(); i += 8) {
        char byte = 0;
        for (int j = 0; j < 8 && i + j < extractedBits.size(); ++j) {
            byte |= extractedBits[i + j] << (7 - j);
        }
        extractedBytes.push_back(byte);
        if (byte == '\\') {
            cout << "found endpoint" << endl;
            break;
        }
    }

    string extractedText(extractedBytes.begin(), extractedBytes.end());

    ofstream file(outFilename);
    file << extractedText;
    file.close();

    cout << "숨겨진 데이터가 추출되었습니다." << endl;
}




vector<bool> Steganography::readTextFile(const string& filename) {
    ifstream file(filename, ios::binary);
    file.seekg(0, ios::end);
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);
  
    cout << "file size is : " << size << endl;

    vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
        vector<bool> bits;
        for (char c : buffer) {
            for (int i = 7; i >= 0; --i) {
                bits.push_back((c >> i) & 1);
            }
        }
        return bits;
    }
    return vector<bool>();
}

void Steganography::hideData(Mat& target, const vector<bool>& data) {
    int bitIdx = 0;
    for (int i = 0; i < target.rows; i++) {
        for (int j = 0; j < target.cols; j++) {
            if (!((i > target.rows / 2.0 + target.rows * 0.05 || i < target.rows / 2.0 + target.rows * 0.05) &&
                (j > target.cols / 2.0 + target.cols * 0.05 || j < target.cols / 2.0 - target.cols * 0.05))) {
                continue;
            }

            if (bitIdx < data.size()) {
                if (data[bitIdx]) {
                    //target.at<uchar>(i, j) = (target.at<uchar>(i, j) + 0.1f);
                    //cout << (int)(target.at<uchar>(i, j)) << endl;
                }
                bitIdx++;
            }
            else {
                bitIdx = 0;
                return;
            }
        }
    }
}





void Steganography::hide(const string& textFile, const string& imageFile, const string& outputFile) {
    
    string objFilename = textFile.empty() ? selectFile(L"텍스트 파일 선택", L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0") : textFile;
    string srcFilename = imageFile.empty() ? selectFile(L"이미지 파일 선택", L"Image Files (*.png;*.jpg)\0*.png;*.jpg\0All Files (*.*)\0*.*\0") : imageFile;
    string outFilename = outputFile.empty() ? saveFile(L"출력 파일 저장", L"PNG Files (*.png)\0*.png\0All Files (*.*)\0*.*\0") : outputFile;

    cout << objFilename << endl;
    if (objFilename.empty() || srcFilename.empty() || outFilename.empty()) {
        cout << "파일 선택이 취소되었습니다." << endl;
        return;
    }

    vector<bool> data = readTextFile(objFilename);
   
    cout << "image name: " << srcFilename << endl;
    Mat srcImg = imread(srcFilename, IMREAD_COLOR);
    if (srcImg.empty()) {
        cout << "이미지를 열 수 없습니다." << endl;
        return;
    }

    Mat hsvImg;
    cvtColor(srcImg, hsvImg, COLOR_BGR2HSV);

    vector<Mat> hsvPlanes;
    split(hsvImg, hsvPlanes);
    Mat vImg = hsvPlanes[2];

    int m = min(getOptimalDFTSize(vImg.rows), srcImg.rows);
    int n = min(getOptimalDFTSize(vImg.cols), srcImg.cols);

    Mat padded;
    copyMakeBorder(vImg, padded, 0, m - vImg.rows, 0, n - vImg.cols, BORDER_CONSTANT, Scalar::all(0));

    Mat croppedSrc;
    Rect roi(0, 0, n, m);
    srcImg(roi).copyTo(croppedSrc);

    Mat croppedHsv;
    //croppedSrc.copyTo(croppedHsv);
    cvtColor(croppedSrc, croppedHsv, COLOR_BGR2HSV);

    vector<Mat> croppedHsvPlanes;
    split(croppedHsv, croppedHsvPlanes);
    croppedHsvPlanes[2] = padded;
    merge(croppedHsvPlanes, croppedHsv);

    Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
    Mat complexI;
    merge(planes, 2, complexI);

    Mat dftcomplex;
    dft(complexI, dftcomplex, DFT_COMPLEX_OUTPUT);

    Mat target;
    //dftcomplex.convertTo(target, CV_8U);

    hideData(dftcomplex, data);

    //target.convertTo(dftcomplex, CV_32F);
    idft(dftcomplex, complexI, DFT_REAL_OUTPUT);
    split(complexI, planes);
    //normalize(planes[0], vImg, 0, 255, NORM_MINMAX);

    Mat processedV;
    planes[0](Rect(0, 0, croppedHsvPlanes[2].cols, croppedHsvPlanes[2].rows)).convertTo(processedV, croppedHsvPlanes[2].type());
    processedV.copyTo(croppedHsvPlanes[2]);

    processedV.copyTo(croppedHsvPlanes[2]);

    merge(croppedHsvPlanes, croppedHsv);

    //croppedHsv.copyTo(croppedSrc);
    cvtColor(croppedHsv, croppedSrc, COLOR_HSV2BGR);

    croppedSrc.copyTo(srcImg(roi));

    imwrite(outFilename, srcImg);

}

void Steganography::extract(const std::string& stegoImage, const std::string& origImage, const std::string& outputFile)
{

    string stegoImgName = stegoImage.empty() ? selectFile(L"은닉 파일 선택", L"Image Files (*.png;*.jpg)\0*.png;*.jpg\0All Files (*.*)\0*.*\0") : stegoImage;
    string OrigImgName = origImage.empty() ? selectFile(L"원본 파일 선택", L"Image Files (*.png;*.jpg)\0*.png;*.jpg\0All Files (*.*)\0*.*\0") : origImage;
    string outFilename = outputFile.empty() ? saveFile(L"출력 파일 저장", L"TXT Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0") : outputFile;

    if (stegoImgName.empty() || OrigImgName.empty() || outFilename.empty()) {
        cout << "파일 선택이 취소되었습니다." << endl;
        return;
    }

    Mat origImg = imread(OrigImgName, IMREAD_COLOR);
    if (origImg.empty()) {
        cout << "이미지를 열 수 없습니다." << endl;
        return;
    }

    Mat stegoImg = imread(stegoImgName, IMREAD_COLOR);
    if (stegoImg.empty()) {
        cout << "이미지를 열 수 없습니다." << endl;
        return;
    }

    if (stegoImg.empty() || origImg.empty()) {
        cout << "이미지를 열 수 없습니다." << endl;
        return;
    }

    Mat stegoHsv, origHsv;
    cvtColor(stegoImg, stegoHsv, COLOR_BGR2HSV);
    cvtColor(origImg, origHsv, COLOR_BGR2HSV);

    vector<Mat> stegoPlanes, origPlanes;
    split(stegoHsv, stegoPlanes);
    split(origHsv, origPlanes);

    Mat stegoV = stegoPlanes[2];
    Mat origV = origPlanes[2];

    int m = min(getOptimalDFTSize(stegoV.rows), origImg.rows);
    int n = min(getOptimalDFTSize(stegoV.cols), origImg.cols);

    Mat paddedStego, paddedOrig;
    cout << m << n << endl;
    cout << stegoV.size() << origV.size() << endl;
    copyMakeBorder(stegoV, paddedStego, 0, m - stegoV.rows, 0, n - stegoV.cols, BORDER_CONSTANT, Scalar::all(0));
    copyMakeBorder(origV, paddedOrig, 0, m - origV.rows, 0, n - origV.cols, BORDER_CONSTANT, Scalar::all(0));

    Mat stegoPaddedPlanes[] = { Mat_<float>(paddedStego), Mat::zeros(paddedStego.size(), CV_32F) };
    Mat origPaddedPlanes[] = { Mat_<float>(paddedOrig), Mat::zeros(paddedOrig.size(), CV_32F) };

    Mat stegoComplex;
    Mat origComplex;
    merge(stegoPaddedPlanes, 2, stegoComplex);
    merge(origPaddedPlanes, 2, origComplex);

    dft(stegoComplex, stegoComplex);
    dft(origComplex, origComplex);

    Mat stegoTarget, origTarget;
    stegoComplex.convertTo(stegoTarget, CV_8U);
    origComplex.convertTo(origTarget, CV_8U);

    vector<bool> extractedBits;
    for (int i = 0; i < stegoTarget.rows; i++) {
        for (int j = 0; j < stegoTarget.cols; j++) {
            if ((i > stegoTarget.rows / 2 + stegoTarget.rows * 0.05 || i < stegoTarget.rows / 2 + stegoTarget.rows * 0.05) &&
                (j > stegoTarget.cols / 2 + stegoTarget.cols * 0.05 || j < stegoTarget.cols / 2 - stegoTarget.cols * 0.05)) {
                extractedBits.push_back(stegoTarget.at<uchar>(i, j) != origTarget.at<uchar>(i, j));
                //cout << (stegoTarget.at<uchar>(i, j) != origTarget.at<uchar>(i, j)) << i << j << endl;
            }
        }
    }

    vector<char> extractedBytes;
    for (int i = 0; i < extractedBits.size(); i += 8) {
        char byte = 0;
        for (int j = 0; j < 8 && i + j < extractedBits.size(); j++) {
            byte |= extractedBits[i + j] << (7 - j);
        }
        extractedBytes.push_back(byte);
    }

    string extractedText(extractedBytes.begin(), extractedBytes.end());

    ofstream file(outputFile);
    file << extractedText;
    file.close();

    cout << "숨겨진 정보가 추출되었습니다: " << outFilename << endl;
}
