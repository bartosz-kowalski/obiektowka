#pragma once
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <string>
#include <filesystem>
#include <iostream>
#include <vector>
#include <fstream>
#include "czesci.hpp"

Model ImportSTLModel(const char* filename);
std::vector<std::string> listFilesInDirectory(const std::string& folderPath, std::vector<std::string>& txtFiles);
std::ifstream openFile(const std::string& filePath);
bool movableLine(const std::string& line);
Vector3 positionLine(const std::string& line);
void deleteModels(std::vector <czesc> czesci);
float normalize(float min, float value, float max);
