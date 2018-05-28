#pragma once


#include <string>
#include <vector>
#include <fstream>
#include<sstream>

// FileReader.h ＠色男
//
//　主に区切り文字で表形式に保存されたテキストファイルを読み込むためのクラス。
//	クラスの知識はなくとも使えます
//	
//	【使い方】
//	①CFileRead型の変数を宣言　
//	②メンバ関数のFireReadを呼び出す（第1引数はファイルパス、第2引数は区切り文字）
//	③メンバ関数のGetDataを使って読み込んだデータを取得する（引数で何行目何番目のデータなのかを指定する）
//	
//　付属のサンプルを見てなんとなく使い方を把握してもらえれば
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


