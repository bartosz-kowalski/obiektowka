#include "rozne.hpp"

Model ImportSTLModel(const char* filename) {
	if (!std::filesystem::exists(filename)) {
		std::cerr << "Plik STL nie istnieje: " << filename << std::endl;
		return Model();
	}

	return LoadModel(filename);;
}

std::vector<std::string> listFilesInDirectory(const std::string& folderPath, std::vector<std::string>& txtFiles) {
	std::vector<std::string> objFiles;

	if (!std::filesystem::exists(folderPath)) {
		std::cerr << "Folder nie istnieje: " << folderPath << std::endl;
		return {};
	}

	for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
		if (entry.is_regular_file() && entry.path().extension() == ".obj") {
			objFiles.push_back(entry.path().filename().string()); // tylko nazwa pliku 
		}

		if (entry.is_regular_file() && entry.path().extension() == ".txt") {
			txtFiles.push_back(entry.path().filename().string()); // tylko nazwa pliku 
		}
	}

	return objFiles;
}

std::ifstream openFile(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Nie mo¿na otworzyæ pliku: " << filePath << std::endl;
	}
	return file;
}

bool movableLine(const std::string& line) {
	return line.find("true") != std::string::npos;
}

Vector3 positionLine(const std::string& line) {
	Vector3 result = { 0 };

	// ZnajdŸ pocz¹tek nawiasu
	size_t start = line.find('{');
	size_t end = line.find('}');
	if (start == std::string::npos || end == std::string::npos) return result;

	std::string vecString = line.substr(start + 1, end - start - 1); // "-10, -10, 0"

	std::istringstream iss(vecString);
	std::string val;
	std::vector<float> values;

	while (std::getline(iss, val, ',')) {
		values.push_back(std::stof(val));
	}

	if (values.size() == 3) {
		result.x = values[0];
		result.z = values[1];
		result.y = values[2];
	}

	return result;
}

void deleteModels(std::vector <czesc> czesci)
{
	for (czesc& part : czesci)
	{
		part.deleteCzesc();
	}
}

float normalize(float min, float value, float max) {
	float min_x = 0;
	float max_x = 1000;
	if (max_x - min_x == 0) return min;
	return min + ((value - min_x) / (max_x - min_x)) * (max - min);
}
