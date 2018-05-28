#pragma once


#include <string>
#include <vector>
#include <fstream>
#include<sstream>

// FileReader.h ���F�j
//
//�@��ɋ�؂蕶���ŕ\�`���ɕۑ����ꂽ�e�L�X�g�t�@�C����ǂݍ��ނ��߂̃N���X�B
//	�N���X�̒m���͂Ȃ��Ƃ��g���܂�
//	
//	�y�g�����z
//	�@CFileRead�^�̕ϐ���錾�@
//	�A�����o�֐���FireRead���Ăяo���i��1�����̓t�@�C���p�X�A��2�����͋�؂蕶���j
//	�B�����o�֐���GetData���g���ēǂݍ��񂾃f�[�^���擾����i�����ŉ��s�ډ��Ԗڂ̃f�[�^�Ȃ̂����w�肷��j
//	
//�@�t���̃T���v�������ĂȂ�ƂȂ��g������c�����Ă��炦���
namespace CSVReader{
	class CFileRead{
		std::vector<std::vector<std::string>> result;
	public:
		std::vector<std::string> split(std::string str, std::string delim)
		{
			std::vector<std::string> result;
			int cutAt;
			while ((cutAt = str.find_first_of(delim)) != str.npos)
			{
				if (cutAt > 0)
				{
					result.push_back(str.substr(0, cutAt));
				}
				str = str.substr(cutAt + 1);
			}
			if (str.length() > 0)
			{
				result.push_back(str);
			}
			return result;
		}
		int FileRead(const char *filename, const char *delim){
			std::ifstream fs(filename);
			if (!fs)
				return -1;

			char line[1000];
			std::string line_s;
			int count = 0;
			while (fs){

				fs.getline(line, 1000);
				line_s = line;
				if (line_s == "")continue;
				result.resize(result.size() + 1);
				result[count] = split(line_s, delim);
				if (count > 90){
					int a;
					a = 0;
				}

				count++;
			}
			return 0;
		}
		int GetData(int row, int column){

			return atoi(result[row][column].c_str());
		}
		float GetDataf(int row, int column){
			return (float)atof(result[row][column].c_str());
		}
		const char* GetDatac(int row, int column){
			return result[row][column].c_str();
		}

		template<class Ty>
		Ty GetDatat(int row, int column)
		{
			if (row <= result.size() - 1 && column <= result[row].size() - 1){
				return result[row][column];
			}
			return Ty{};
		}

		int GetSizeRow(){ return result.size(); }
		int GetSizeColumn(int row){ return result[row].size(); }

	};
}


