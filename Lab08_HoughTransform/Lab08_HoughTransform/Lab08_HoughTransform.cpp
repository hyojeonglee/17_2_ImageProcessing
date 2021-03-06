#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <math.h>
#define BYTE unsigned char 

int _tmain(int argc, _TCHAR* argv[])
{
	FILE *infile;
	infile = fopen("edge.bmp", "rb");
	if (infile == NULL) { printf("There is no file!!!\n"); exit(1); }

	BITMAPFILEHEADER hf;
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, infile);
	if (hf.bfType != 0x4D42) exit(1);

	BITMAPINFOHEADER hInfo;
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, infile);
	printf("Image Size: (%3dx%3d)\n", hInfo.biWidth, hInfo.biHeight);
	printf("Pallete Type: %dbit Colors\n", hInfo.biBitCount);

	if (hInfo.biBitCount<8) { printf("Bad File format!!"); exit(1); }
	RGBQUAD *pRGB;
	if (hInfo.biClrUsed != 0)
	{
		pRGB = new RGBQUAD[hInfo.biClrUsed];
		fread(pRGB, sizeof(RGBQUAD), hInfo.biClrUsed, infile);
	}

	BYTE lpImg2[256][256][3];
	fread(lpImg2, sizeof(char), hInfo.biSizeImage, infile);
	fclose(infile);

	BYTE lpOutImg2[256][256][3];

	int HT[256 * 4 + 1][100];
	float PI = 3.14;

	for (int i = 0; i < 256 * 4 + 1; i++) {
		for (int j = 0; j < 100; j++) {
			HT[i][j] = 0;
		}
	}

	for (int i = 0; i < hInfo.biHeight; i++) {
		for (int j = 0; j < hInfo.biWidth; j++) {
			if (lpImg2[i][j][0] == 255) {
				float theta = 0.;
				for (int m = 0; m < 100; m++) {
					float rho = j*cos(theta) + i*sin(theta);
					HT[(int)rho + 512][m]++;
					theta += PI / 100;
				}
			}
		}
	}

	int max = 0;
	int rho_sol = 0;
	float theta_sol = 0;
	for (int i = 0; i < 256 * 4 + 1; i++) {
		for (int j = 0; j < 100; j++) {
			if (max < HT[i][j]) {
				max = HT[i][j];
				rho_sol = i - 512;
				theta_sol = (PI / 100) * (float)j;
			}
		}
	}
	printf("%d\n", max);
	printf("%d\n", rho_sol);
	printf("%f\n", theta_sol);

	for (int i = 0; i < hInfo.biHeight; i++) {
		for (int j = 0; j < hInfo.biWidth; j++) {
			for (int c = 0; c < 3; c++) {
				lpOutImg2[i][j][c] = 0;
			}
		}
	}

	for (int j = 0; j < hInfo.biWidth; j++) {
		int i = (-cos(theta_sol) / sin(theta_sol)) * j + (rho_sol / sin(theta_sol));
		for (int c = 0; c < 3; c++) {
			lpOutImg2[i][j][c] = 255;
		}
	}

	///////////////////////////////////////////////////////////////////

	FILE *outfile = fopen("hough.bmp", "wb");
	fwrite(&hf, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);
	fwrite(&hInfo, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);
	fwrite(lpOutImg2, sizeof(char), 3 * 256 * 256, outfile);
	fclose(outfile);

	return 0;
}