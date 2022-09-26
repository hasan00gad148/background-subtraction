#include <iostream>
#include <math.h>
#include <stdlib.h>
#include<string.h>
#include<msclr\marshal_cppstd.h>
#include <ctime>// include this header 
#include <iomanip>
#include <sstream>
#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>
using namespace std;
using namespace msclr::interop;

struct pixel
{
	int red;
	int green;
	int blue;
};
int* inputImage(int* w, int* h, System::String^ imagePath, pixel* imageArray) //put the size of image in w & h
{
	int* input=new int;

	
	int OriginalImageWidth, OriginalImageHeight;

	//*********************************************************Read Image and save it to local arrayss*************************	
	//Read Image and save it to local arrayss

	System::Drawing::Bitmap BM(imagePath);

	OriginalImageWidth = BM.Width;
	OriginalImageHeight = BM.Height;
	*w = BM.Width;
	*h = BM.Height;
	/*
	int *Red = new int[BM.Height * BM.Width];
	int *Green = new int[BM.Height * BM.Width];
	int *Blue = new int[BM.Height * BM.Width];
	*/
	//input = new int[BM.Height*BM.Width];
	imageArray = new pixel[BM.Height * BM.Width];
	for (int i = 0; i < BM.Height; i++)
	{
		for (int j = 0; j < BM.Width; j++)
		{
			System::Drawing::Color c = BM.GetPixel(j, i);
			/*
			Red[i * BM.Width + j] = c.R;
			Blue[i * BM.Width + j] = c.B;
			Green[i * BM.Width + j] = c.G;
			*/
			imageArray[i * BM.Width + j].red = c.R;
			imageArray[i * BM.Width + j].green = c.G;
			imageArray[i * BM.Width + j].blue = c.B;
			//input[i*BM.Width + j] = ((c.R + c.B + c.G) / 3); //gray scale value equals the average of RGB values

		}

	}
	return input;
}


void createImage(int* image, int width, int height, int index, pixel* pixelArray)
{
	System::Drawing::Bitmap MyNewImage(width, height);


	for (int i = 0; i < MyNewImage.Height; i++)
	{
		for (int j = 0; j < MyNewImage.Width; j++)
		{
			//i * OriginalImageWidth + j
			/*
			if (image[i*width + j] < 0)
			{
				image[i*width + j] = 0;
			}
			if (image[i*width + j] > 255)
			{
				image[i*width + j] = 255;
			}
			*/
			//System::Drawing::Color c = System::Drawing::Color::FromArgb(image[i*MyNewImage.Width + j], image[i*MyNewImage.Width + j], image[i*MyNewImage.Width + j]);
			System::Drawing::Color c = System::Drawing::Color::FromArgb(
					pixelArray[i * MyNewImage.Width + j].red, 
					pixelArray[i * MyNewImage.Width + j].green, 
					pixelArray[i * MyNewImage.Width + j].blue);

			MyNewImage.SetPixel(j, i, c);
		}
	}
	MyNewImage.Save("..//Data//BackGroundOutput//outputRes" +index+ ".png");
	cout << "result Image Saved " << index << endl;
}


int main()
{
	int ImageWidth = 4, ImageHeight = 4;

	int start_s, stop_s, TotalTime = 0;
	/*
	System::String^ imagePath;
	std::string img;
	img = "..//Data//Input//test.png";

	imagePath = marshal_as<System::String^>(img);
	int* imageData = inputImage(&ImageWidth, &ImageHeight, imagePath);
	cout << "heigt " << ImageHeight << "  width" << ImageWidth;

	start_s = clock();
	createImage(imageData, ImageWidth, ImageHeight, 0);
	stop_s = clock();
	TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
	cout << "time: " << TotalTime << endl;

	free(imageData);
	*/
	//read 
	//(r,g,b) for each pic 
	//1-load each pic => 2d[w,h] = > [3]rgb
	//2-transform each pic from nxn => n^2 x1 and add to data array (n^2)xm (pics)
	//3-estimation for each m get avrg for each row pixel
	System::String^ imagePath;
	std::string img;
	img = "D:\\Study\\High Performance Computing\\Project\\BackGround\\in000";
	string ext = ".jpg";
	//495 picture
	//loading images
	const int M = 495;
	//create 2d array of pointers n^2*m
	pixel** totalImageArray = new pixel * [ImageWidth * ImageHeight];
	cout << "init memory\n";
	for (size_t i = 0; i < M; i++) { totalImageArray[i] = new pixel[M]; }

	cout << "start loading the images to the memory\n";
	start_s = clock();

	int n = 3;
	//std::stringstream ss;

	for (int i = 1; i <= M; i++)
	{
		//fill index to be 3 digit (1 => 001 , 99 => 099)
		//get image path = orignal+index+extention
		/*ss << std::setw(n) << std::setfill('0') << i;
		string s = ss.str();*/
		string s;
		if (i < 9) { s = "00" + to_string(i); }
		else if (i < 99) { s = "0" + to_string(i); }
		else { s = to_string(i); }

		string NewPath = img + s + ext;
		cout << NewPath << endl;
		//load it
		imagePath = marshal_as<System::String^>(NewPath);
		int* imageData = inputImage(&ImageWidth, &ImageHeight, imagePath, totalImageArray[i]);
	}
	stop_s = clock();
	TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
	cout << "time: " << TotalTime << endl;

	pixel* imageArrayAverage = new pixel[ImageWidth * ImageHeight];
	cout << "Start averaging step\n";
	start_s = clock();
	for (int i = 0; i < ImageHeight; i++)
	{
		for (int j = 0; j < ImageWidth; j++)
		{
			imageArrayAverage[i * ImageWidth + j].red = 0;
			imageArrayAverage[i * ImageWidth + j].green = 0;
			imageArrayAverage[i * ImageWidth + j].blue = 0;
			for (size_t pix = 0; pix < M; pix++)
			{
				imageArrayAverage[i * ImageWidth + j].red += totalImageArray[i * ImageWidth + j][pix].red;
				imageArrayAverage[i * ImageWidth + j].green += totalImageArray[i * ImageWidth + j][pix].green;
				imageArrayAverage[i * ImageWidth + j].blue += totalImageArray[i * ImageWidth + j][pix].blue;
			}
			imageArrayAverage[i * ImageWidth + j].red /= M;
			imageArrayAverage[i * ImageWidth + j].green /= M;
			imageArrayAverage[i * ImageWidth + j].blue /= M;
		}
	}
	stop_s = clock();
	TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
	cout << "time: " << TotalTime << endl;
	//delete total array
	  //Delete the array created
	int size = ImageWidth * ImageHeight;
	//delete array
	for (int i = 0; i < size; i++)    //To delete the inner arrays
		delete[] totalImageArray[i];
	delete[] totalImageArray;

	//save extimated b
	cout << "save estimated image\n ";
	start_s = clock();
	createImage(NULL, ImageWidth, ImageHeight, 011, imageArrayAverage);
	stop_s = clock();
	TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
	cout << "time: " << TotalTime << endl;

	cout << "foreground mask\n";
	int threshHold = 50;
	string chosenImagePath = "in000158";
	pixel* chosenImage = new pixel[ImageWidth * ImageHeight];
	//load it
	imagePath = marshal_as<System::String^>(img + chosenImagePath);
	int* imageData = inputImage(&ImageWidth, &ImageHeight, imagePath, chosenImage);
	
	int size = ImageWidth * ImageHeight;
	for (int i = 0; i < size; i++)
	{
		//check value
	}
	return 0;

}



