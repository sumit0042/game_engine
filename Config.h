#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct Conf
{
	vector<float> cameraPosition;
	float cameraYaw;
	float cameraPitch;
	float cameraZoom;

	vector<float> lightPos;
	vector<vector<float>> lightAmbDiffSpec;

	int no_objects;
	vector<float> cubeShape;
	vector<vector<float>> modelMatrix;
	vector<vector<float>> matAmbDiffSpec;
	float matShininess;

	void save()
	{
		ofstream file;
		file.open("config.txt");

		for (float c : cameraPosition)
		{
			file << c << ' ';
		}
		file << '\n';

		file << cameraYaw << ' ';
		file << cameraPitch << ' ';
		file << cameraZoom << ' ' << '\n';

		for (float lp : lightPos)
		{
			file << lp << ' ';
		}
		file << '\n';

		for (vector<float> lads : lightAmbDiffSpec)
		{
			for (float l : lads)
			{
				file << l << ' ';
			}
		}
		file << '\n';

		file << no_objects << ' ' << '\n';

		for (float f : cubeShape)
		{
			file << f << ' ';
		}
		file << '\n';

		for (vector<float> m : modelMatrix)
		{
			for (float f : m)
			{
				file << f << ' ';
			}
		}
		file << '\n';

		for (vector<float> l : matAmbDiffSpec)
		{
			for (float f : l)
			{
				file << f << ' ';
			}
		}
		file << '\n';

		file << matShininess << ' ';

		file.close();
	}

	void load()
	{
		ifstream file("config.txt");
		vector<string> linesFromFile;
		if (file.is_open())
		{
			string line;
			while (getline(file, line))
			{
				linesFromFile.push_back(line.c_str());
			}
		}
		for (size_t i = 0; i < linesFromFile.size(); i++)
		{
			vector<float> floats = stringToFloats(linesFromFile[i]);
			switch (i)
			{
			case 0:
			{
				cameraPosition = { floats[0], floats[1], floats[2] };
				break;
			}
			case 1:
			{
				cameraYaw = floats[0];
				cameraPitch = floats[1];
				cameraZoom = floats[2];
				break;
			}
			case 2:
			{
				lightPos = { floats[0], floats[1], floats[2] };
				break;
			}
			case 3:
			{
				for (size_t i = 0; i < 3; i++)
				{
					vector<float> row(3);
					for (size_t j = 0; j < 3; j++)
					{
						row[j] = floats[(3 * i) + j];
					}
					lightAmbDiffSpec.push_back(row);
				}
				break;
			}
			case 4:
			{
				no_objects = (int)floats[0];
				break;
			}
			case 5:
			{
				cubeShape = { floats[0], floats[1], floats[2] };
				break;
			}
			case 6:
			{
				for (size_t i = 0; i < 4; i++)
				{
					vector<float> row(4);
					for (size_t j = 0; j < 4; j++)
					{
						row[j] = floats[(4 * i) + j];
					}
					modelMatrix.push_back(row);
				}
				break;
			}
			case 7:
			{
				for (size_t i = 0; i < 3; i++)
				{
					vector<float> row(3);
					for (size_t j = 0; j < 3; j++)
					{
						row[j] = floats[(3 * i) + j];
					}
					matAmbDiffSpec.push_back(row);
				}
				break;
			}
			case 8:
			{
				matShininess = stof(linesFromFile[i]);
				break;
			}
			default:
				break;
			}
		}
	}

	vector<float> stringToFloats(string s)
	{
		vector<float> res;
		vector<char> cs;
		for (char c : s)
		{
			if (c == ' ' && cs.size() > 0)
			{
				cs.push_back('\0');
				res.push_back(stof(&cs[0]));
				cs.clear();
			}
			else
			{
				cs.push_back(c);
			}
		}
		return res;
	}

};
