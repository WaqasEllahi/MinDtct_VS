#include <iostream>
#include <fstream>
#include <vld.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
extern "C" {
#include "inc/lfs.h" //a C header, so wrap it in extern "C" 
}

using namespace cv;
using namespace std;

unsigned char* MatToByte(Mat image)
{
	int size = image.total() * image.elemSize();
	unsigned char* bytes = new unsigned char[size];  // you will have to delete[] that later
	std::memcpy(bytes, image.data, size * sizeof(char));

	return bytes;
}

Mat ByteToMat(unsigned char* bytes, int height, int width)
{
	Mat image = Mat(height, width, CV_8UC1, bytes).clone(); // make a copy
	return image;
}

void ShowImage(Mat image, string title)
{
	/// Create windows
	namedWindow(title, WINDOW_AUTOSIZE);
	imshow(title, image);
	waitKey(0);
}

void writeMinutiaeList(MINUTIAE *minutiae, string path)
{
	ofstream outputFile;
	outputFile.open(path);
	for (int i = 0; i < minutiae->num; i++)
		outputFile << minutiae->list[i]->x <<" " << minutiae->list[i]->y << " " <<
		minutiae->list[i]->direction << " " << minutiae->list[i]->type << endl;
}

int main(int argc, char *argv[]) {
	string inputFileName, outputFileName;
	if (argc == 3) {
		inputFileName = argv[1];
		outputFileName = argv[2];
	}
	else{
		cout << "Input Argument Problems" << endl;
		return - 1;
	}

	Mat image = imread(inputFileName, IMREAD_GRAYSCALE);
	if (image.empty())
	{
		return -1;
	}

	//ShowImage(image, "Orignal Image");

	unsigned char* idata = MatToByte(image);

	int ret;
	MINUTIAE *minutiae;
	int *direction_map, *low_contrast_map, *low_flow_map;
	int *high_curve_map;
	int map_w, map_h;
	unsigned char *bdata;
	int bw, bh;
	const int ih = image.rows, iw = image.cols;

	/* Detect minutiae in grayscale fingerpeint image. */
	if ((ret = lfs_detect_minutiae_V2(&minutiae,
		&direction_map, &low_contrast_map,
		&low_flow_map, &high_curve_map,
		&map_w, &map_h,
		&bdata, &bw, &bh,
		idata, iw, ih, &lfsparms_V2))) {
		return(ret);
	}

	writeMinutiaeList(minutiae, outputFileName);

	free(idata);
	free(direction_map);
	free(low_contrast_map);
	free(low_flow_map);
	free(high_curve_map);
	free(bdata);
	free_minutiae(minutiae);
	image.release();

	
	
}
