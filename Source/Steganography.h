#pragma once
#ifndef STEGANOGRAPHY_H
#define STEGANOGRAPHY_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <commdlg.h>

class Steganography {
public:
    Steganography();
    void hide(const std::string& textFile, const std::string& imageFile, const std::string& outputFile);
    void extract(const std::string& stegoImage, const std::string& origImage, const std::string& outputFile);
    std::string selectFile(const LPCWSTR& title, const LPCWSTR& filter);
    std::string saveFile(const LPCWSTR& title, const LPCWSTR& filter);
    void hideLSB(const std::string& textFile, const std::string& imageFile, const std::string& outputFile);
    void extractLSB(const std::string& stegoImage, const std::string& outputFile);

private:
    std::vector<bool> readTextFile(const std::string& filename);
    void hideData(cv::Mat& target, const std::vector<bool>& data);
    std::string makestr(std::wstring&& value);

};

#endif