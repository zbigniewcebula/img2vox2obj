#if 0
#!/bin/bash
g++ main.cpp -g --std=c++11 -L/usr/X11R6/lib -lm -lpthread -lX11 -o main_exe
exit
#endif

#pragma comment(lib, "gdi32.lib")
//cls && g++ main.cpp --std=c++11 -lgdi32 -o main.exe && main.exe

#include <iostream>
#include <map>
#include <cmath>
#include <cstdlib>
#ifdef _WIN32
	#include <windows.h>
#endif

typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned int	uint;

#include "CImg.h"
#include "MagicaVoxel.h"
#include "MagicaVoxelSave.h"

using namespace std;
using namespace cimg_library;

int main(int argc, char** argv) {
	if (argc < 3 || argc > 4) {
		cerr	<< "ERROR! Usage: img2vox IMG.* OUT.vox [Z canvas height]" << endl; 
		return 1;
	}

	string	name = argv[2];

	cout	<< name << ": [IMG] Loading... ";
	CImg<unsigned char> image(argv[1]);
	cout	<< "DONE! ";

	map<uint, int>	pal;
	map<uint, int>	palPos;
	int				palCount	= 1;

	cout	<< " Palette loading and mapping";
	for(int y = 0; y < image.height(); ++y) {
		for(int x = 0; x < image.width(); ++x) {
			v4	pos(x, image.height() - 1 - y, 0, 0);
			v4	pix(
				image(x, y, 0, 0),
				image(x, y, 0, 1),
				image(x, y, 0, 2),
				0xFF
			);

			auto it	= pal.find(pix.val);
			if (it == pal.end()) {
				pal[pix.val]	= palCount;
				it				= pal.find(pix.val);
				++palCount;

				cout << ".";
			}

			if (!(pix.r == 0 && pix.g == 0 && pix.b == 0)
			&&	!(pix.r == 0xFF && pix.g == 0xFF && pix.b == 0xFF)
			) {
				palPos[pos.val]	= it->second;
				//cout << "X";
			} else {
				palPos[pos.val]	= 0;
				//cout << " ";
			}
		}
		//cout << endl;
	}
	cout << " DONE!" << endl;
	if (palCount > 255) {
		cerr	<< "ERROR! Too many colors in image (limited palette to 256)!" << endl;
		return 1;
	}

	int	zHeight	= 1;
	if (argc == 4) {
		zHeight	= max(atoi(argv[3]), 1);
	}


	cout	<< name << ": [VOX] Generation, ";
	VOX		model;
	model.setSize(image.width(), image.height(), zHeight);

	for(int z = 0; z < model.getSizeZ(); ++z) {
		for(int y = 0; y < model.getSizeY(); ++y) {
			for(int x = 0; x < model.getSizeX(); ++x) {
				v4	pos(x, model.getSizeY() - 1 - y, z, 0);
				model.setVoxel(
					pos.x, pos.y, pos.z,
					pos.z == 0? palPos[pos.val]: 0
				);
			}
		}
	}

	cout	<< "Palette remaping and palette file generation, ";

	CImg<unsigned char> palette(256, 1, 1, 4, 0);

	for(auto it = pal.begin(); it != pal.end(); ++it) {
		uchar idx	= it->second - 1;
		v4	clr(it->first);
		model.palette[idx].val	= clr.val;
		palette(idx, 0, 0, 0)	= clr.r;
		palette(idx, 0, 0, 1)	= clr.g;
		palette(idx, 0, 0, 2)	= clr.b;
		palette(idx, 0, 0, 3)	= clr.a;
	}
	palette.save((name + ".png").c_str());

	cout	<< "File save... ";
	model.save((name + ".vox").c_str());
	cout	<< "DONE!" << endl;

	return 0;
}

/*



#ifndef _WIN32
	cout	<< "===========<PYTHON>===========" << endl;
	
	system("python3 vox2obj.py");

	cout	<< "^^^^^^^^^^^<PYTHON>^^^^^^^^^^^" << endl;

	
#endif

*/

/*
	MV_Model	model;
	if (model.LoadModel("./untitled.vox")) {
		cout << "===Yay" << endl;
	} else {
		cout << "=== :(" << endl;
	}
	*/
