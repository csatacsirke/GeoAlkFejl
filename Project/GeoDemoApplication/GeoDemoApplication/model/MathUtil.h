#pragma once


static Mat ToGray(Mat rgbImage) {

	const int width = rgbImage.cols;
	const int height = rgbImage.rows;

	Mat grayImage(height, width, CV_8U);


	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			int depth = rgbImage.depth();
			cv::Vec3b rgb = rgbImage.at<cv::Vec3b>(y, x);


			uint8_t gray = uint8_t(int((rgb[0]) + int(rgb[1]) + int(rgb[2])) / 3);
			grayImage.at<uint8_t>(y, x) = gray;
		}
	}

	return grayImage;
}


static void ForEachPixel(Mat image, function<void(int, int)> fn) {
	const int width = image.cols;
	const int height = image.rows;


	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			fn(x, y);
		}

	}
}

static void ForEachPixel(Mat image, function<void(Point)> fn) {
	ForEachPixel(image, [&](int x, int y) {
		fn(Point(x, y));
	});
}




static void ForEachNeightbour(Mat image, Point p, function<void(int, int)> fn) {
	const int width = image.cols;
	const int height = image.rows;

	const Point points[] = { p + Point(1,0), p + Point(-1,0), p + Point(0,1), p + Point(0, -1) };

	for(const Point& point : points) {
		if(point.x >= width) continue;
		if(point.y >= height) continue;
		if(point.x < 0) continue;
		if(point.y < 0) continue;

		fn(point.x, point.y);

	}

}

static void ForEachNeightbour(Mat image, Point p, function<void(Point)> fn) {
	ForEachNeightbour(image, p, [&](int x, int y) {
		fn(Point(x, y));
	});
}



static Mat InvertGrayscale(Mat image) {
	Mat result = image.clone();
	ForEachPixel(image, [&](int x, int y) {
		uint8_t val = image.at<uint8_t>(x, y);
		result.at<uint8_t>(x, y) = uint8_t(255) - val;
	});
	return result;
}



static Mat ScaleImage(Mat& input) {
	double min, max;
	Mat result;
	cv::minMaxLoc(input, &min, &max);

	input.convertTo(result, CV_8U, 255 / (max - min), min);
	return result;
}


static Mat RandomColorIndexedImage(Mat image) {
	Mat hsvImage = Mat(image.rows, image.cols, CV_32FC3);

	ForEachPixel(hsvImage, [&](Point p) {
		int32_t index = image.at<int32_t>(p);
		Vec3f hsv = (index != 0) ? Vec3f(83.7f * float(index-1), 1, 1) : Vec3f(0, 0, 0.5f);
		hsvImage.at<Vec3f>(p) = hsv;
	});


	Mat result;
	cv::cvtColor(hsvImage, result, cv::COLOR_HSV2BGR);
	return result;
}

static void ExtractPoints(vector<KeyPoint>& keypoints, vector<Point>& points) {
	points.reserve(keypoints.size());
	for(KeyPoint& keypoint : keypoints) {
		points.push_back(keypoint.pt);
	}
}

// debughoz
static vector<Point> ReadSeeds(const char* fileName) {
	vector<Point> points;

	Mat image_rgb = cv::imread(fileName);
	Vec3b red(0, 0, 255);
	ForEachPixel(image_rgb, [&](Point p) {
		Vec3b rgb = image_rgb.at<Vec3b>(p);
		if(rgb == red) {
			points.push_back(p);
		}
	});

	return points;
}


static Mat LoadImageAsGrayscale(LPCSTR fileName) {
	Mat rgbImage = cv::imread(fileName);
	Mat image = ToGray(rgbImage);
	return image;
}


static Mat ToRgb(Mat grayscale) {
	// grayscale.convertTo(rgbImage, CV_8UC3); // ez valamiért szar

	Mat rgbImage(grayscale.rows, grayscale.cols, CV_8UC3);
	ForEachPixel(rgbImage, [&](Point p) {
		uint8_t grayValue = grayscale.at<uint8_t>(p);
		rgbImage.at<Vec3b>(p) = Vec3b(grayValue, grayValue, grayValue);
	});

	return rgbImage;
}


static void DrawIndices(Mat background, Mat indexed) {
	Vec3b maskColor = Vec3b(128, 255, 0);
	const float alpha = 0.5f;

	ForEachPixel(background, [&](Point p) {
		Vec3b rgb = background.at<Vec3b>(p);
		int32_t index = indexed.at<int32_t>(p);
		if(index != 0) {
			background.at<Vec3b>(p) = maskColor*alpha + rgb*(1 - alpha);
		}
	});
}

static int32_t MaxElement(Mat a) {
	int32_t maxIndex = 0;
	ForEachPixel(a, [&](Point p) {
		int32_t index = a.at<int32_t>(p);
		maxIndex = std::max(index, maxIndex);
	});
	return maxIndex;
}

static Mat Difference(Mat a, Mat b) {
	int32_t b_indexOffset = MaxElement(a) + 1;
	Mat result = a.clone();

	ForEachPixel(a, [&](Point p) {
		int a_index = a.at<int32_t>(p);
		int b_index = b.at<int32_t>(p);
		if(a_index != 0) {
			result.at<int32_t>(p) = 1;
			if(b_index != 0) {
				result.at<int32_t>(p) = 2;
			}
		} else {
			if(b_index != 0) {
				result.at<int32_t>(p) = 3;
			}
		}



	});

	return result;
}

