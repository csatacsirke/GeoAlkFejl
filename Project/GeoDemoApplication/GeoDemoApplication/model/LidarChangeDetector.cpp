#include <stdafx.h>

#include "LidarChangeDetector.h"
#include "MathUtil.h"




class LidarChangeDetectorImpl {
	const float maxSteepness = 8;
	const size_t minimumBuildingArea = 25;


	Result result; 

	Mat image1;
	Mat image2;


public:

	Result DoEverything(LPCSTR fileName1, LPCSTR fileName2) {
		//Mat TEST = cv::imread("segmentation_example.png");
		//ForEachPixel(TEST, [&](Point p) {
		//	TEST.at<Vec3b>(p) = Vec3b(0, 123, 255);
		//	ForEachNeightbour(TEST, p, [&] (Point p2) {
		//		TEST.at<Vec3b>(p) = Vec3b(0, 123, 0);
		//	});
		//});


		result = Result();

		image1 = LoadImageAsGrayscale(fileName1);
		image2 = LoadImageAsGrayscale(fileName2);
		


		Mat segmentedImage2 = ProcessGrayscaleImage(image2);
		Mat segmentedImage1 = ProcessGrayscaleImage(image1);
	


		Mat difference = Difference(segmentedImage1, segmentedImage2);
		//CString fileName = CString("segmentation_example.png");
		//const char* fileName = "segmentation_example.png";
		//const char* fileName = "b2.png";
		
		Mat debug1 = ToRgb(image1);
		Mat debug2 = ToRgb(image2);

		DrawIndices(debug1, segmentedImage1);
		DrawIndices(debug2, segmentedImage2);

		result.im1 = debug1;
		result.im2 = debug2;
		result.im3 = RandomColorIndexedImage(difference);


		//imshow(image);
		//imshow("im1", image);


		
		//result.im2 = filteredImage;
		return result;
	}

	Result GetResult() {
		return result;
	}

	void Abort() {
		throw DebugAbort();
	}
private:


	Mat ProcessGrayscaleImage(Mat image) {

		


		result.im1 = image;


		Mat filteredImage;
		FilterImage(image, filteredImage);

		vector<KeyPoint> keypoints;
		FindObjects(filteredImage, keypoints);

		Mat debug_keys;
		cv::drawKeypoints(filteredImage, keypoints, debug_keys);
		result.im2 = debug_keys;
		

		//vector<Point> seeds = ReadSeeds("b2_modded.png");
		//SegmentImage(filteredImage, seeds);
		vector<Point> seeds;
		ExtractPoints(keypoints, seeds);
		Mat segments;
		SegmentImage(filteredImage, seeds, segments);



		//RemoveTooSmallSegments(segments); 

		Mat debug = RandomColorIndexedImage(segments);



		//Abort();

		result.im2 = filteredImage;
		result.im3 = debug;


		return segments;
	}

	

	//For each of the two original data sets DSM(t1) and DSM(t2) a
	//	segmentation of 3D object areas has to be performed.In order
	//	to get exclusively 3D objects on the surface of the Earth - i.e.to
	//	eliminate the influence of topography - a so - called normalized
	//	DSM(nDSM) was determined for each data set.Therefore, in a
	//	rough filtering process, points found to reliably lie on the
	//	ground were extracted(e.g.Weidner & Förstner, 1995; Sithole
	//	& Vosselman 2003; Vögtle & Steinle, 2003) which can be
	//	interpolated to a digital terrain model(DTM).The resulting
	//	nDSM for each date can be e calculated easily by subtracting this
	//	DTM from the original DSM.
	void FilterImage(const Mat image, Mat& filteredImage) {
		const int kernelSize = 7;
		double sigma = 1;



		//Mat blurred;
		//cv::GaussianBlur(image, blurred, Size(kernelSize, kernelSize), sigma);

		Mat median;
		cv::medianBlur(image, median, 7);


		filteredImage = median;

		//cv::blur(image, blurred, cv::Size(5, 5), );
		//const int kernelSize = 15;
		//const double sigma = 5;
		//const double maxDiff = 2;



		//Mat blurred;
		//cv::GaussianBlur(image, blurred, Size(kernelSize, kernelSize), sigma);
		//Mat difference = image - blurred;
		//Mat median;
		//cv::medianBlur(difference, median, 7);

		//filteredImage = ScaleImage(median);


		
		

		//Mat laplacian;
		//cv::Laplacian(image, laplacian, CV_64F);

		//double min, max;
		//cv::minMaxLoc(laplacian, &min, &max);

		////laplacian.convertTo(filteredImage, CV_8U, max);
		//laplacian.convertTo(filteredImage, CV_8U, (max-min)/255, min/255);

		//
		//cv::minMaxLoc(filteredImage, &min, &max);


		//ForEachPixel(filteredImage, [&](int x, int y) {
		//	double diff = abs(int(laplacian.at<double>(y, x)));
		//	if(diff > maxDiff) {
		//		filteredImage.at<uint8_t>(y, x) = uint8_t(255);
		//	}


		//});

		//

		//

		//cv::GaussianBlur(image, blurred, Size(kernelSize, kernelSize), sigma);
		//Mat sobel_x;
		//Mat sobel_y;
		//cv::Sobel(image, sobel_x, CV_64F, 0, 1, 1);
		//cv::Sobel(image, sobel_y, CV_64F, 1, 0, 1);

		//

		//sobel_x.convertTo(filteredImage, CV_8U);
		////filteredImage = Mat(sobel_x, CV_8U);
		//
		//filteredImage = image.clone();

		//Mat difference = image - blurred;

		//ForEachPixel(filteredImage, [&](int x, int y) {
		//	int diff = abs(int(difference.at<uint8_t>(y, x)));
		//	if(diff > maxDiff) {
		//		filteredImage.at<uint8_t>(y, x) = uint8_t(0);
		//	}


		//});


	}


	void FindObjects(Mat image, vector<KeyPoint>& keypoints) {
		int kernelSize = 7;
		double sigma = 2;
		Mat blurred;
		cv::GaussianBlur(image, blurred, Size(kernelSize, kernelSize), sigma);

		
		result.im3 = blurred;
		//Abort();

		ForEachPixel(blurred, [&](Point p) {
			uint8_t center = blurred.at<uint8_t>(p);
			bool allOthersAreLower = true;
			ForEachNeightbour(blurred, p, [&](Point p) {
				uint8_t neighbour = blurred.at<uint8_t>(p);
				if(neighbour > center) {
					allOthersAreLower = false;
				}
			});
			if(allOthersAreLower) {
				KeyPoint keypoint;
				keypoint.pt = p;
				keypoints.push_back(keypoint);
			}
		}); 

	}


	void FindObjects_2(Mat image, vector<KeyPoint>& keypoints) {
		cv::SimpleBlobDetector::Params params;
		params.minThreshold = 0;
		params.maxThreshold = 255;
		params.thresholdStep = 1;



		params.filterByArea = true;
		//params.filterByArea = false;
		params.maxArea = 1000000;
		params.minArea = 30;
		//params.maxArea = 30 * 30;
		//params.minArea = 3 * 3;

		auto detector = SimpleBlobDetector::create(params);


		Mat inverse = InvertGrayscale(image);
		detector->detect(inverse, keypoints);

		Mat im_with_keypoints;
		drawKeypoints(image, keypoints, im_with_keypoints, Scalar(255, 255, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

		result.im3 = im_with_keypoints;
	}

	void FloodFill(Mat indexedImage, Point start, int32_t newIndex) {
		queue<Point> pointsToProcess;
		pointsToProcess.push(start);

		int32_t oldIndex = indexedImage.at<int32_t>(start);

		// hülye vagy
		assert(oldIndex != newIndex);
		if(oldIndex == newIndex) return;

		while(!pointsToProcess.empty()) {
			Point p = pointsToProcess.front();
			pointsToProcess.pop();

			ForEachNeightbour(indexedImage, p, [&](Point neighbour) {
				int neighbourSegmentIndex = indexedImage.at<int32_t>(neighbour);
				if(oldIndex == neighbourSegmentIndex) {
					pointsToProcess.push(neighbour);
					indexedImage.at<int32_t>(neighbour) = newIndex;
				}
			});
		}
	}


	void SegmentImage(const Mat image, vector<Point>& seeds, Mat& segments) {
		segments = Mat(image.rows, image.cols, CV_32S);
		segments.setTo(0);

		set<int> terrainSegmentIndices;

		queue<Point> pointsToProcess;
		int currentSegmentIndex = 1;
		//for(Point& seed : seeds) {
		//	segments.at<int32_t>(seed) = currentSegmentIndex;
		//	++currentSegmentIndex;
		//	pointsToProcess.push(seed);
		//}
		{ // debug
			
			Point p(40, 196);
			//Point p(196, 40);
			pointsToProcess.push(p);
			segments.at<int32_t>(p) = currentSegmentIndex;
		}


		while(!pointsToProcess.empty()) {
			Point p = pointsToProcess.front();
			pointsToProcess.pop();

			int centerValue = image.at<uint8_t>(p);
			int currentSegmentIndex = segments.at<int32_t>(p);

			ForEachNeightbour(segments, p, [&](Point neighbour) {
				int neighbourSegmentIndex = segments.at<int32_t>(neighbour);

				if(neighbourSegmentIndex == currentSegmentIndex) return;

				uint8_t neighbourValue = image.at<uint8_t>(neighbour);
				int levelDifference = centerValue - neighbourValue;
				if(abs(levelDifference) < maxSteepness) {

					// ha valaki már felhasználta egyesitunk
					if(neighbourSegmentIndex != 0 ) {
						FloodFill(segments, neighbour, currentSegmentIndex);
					} else {
						segments.at<int32_t>(neighbour) = currentSegmentIndex;
						pointsToProcess.push(neighbour);
					}

				} else {
					// ha a angy szintkülönség miatt a mellettünk levö szegmens
					// felettünk van akkor mi nyilván nem épület vagyunk
					if(levelDifference < 0) {
						terrainSegmentIndices.insert(currentSegmentIndex);
					}
				}
				
			});
		}

		// ami terrain azt kidobjuk
		ForEachPixel(segments, [&](Point p) {
			int currentSegmentIndex = segments.at<int32_t>(p);
			if(terrainSegmentIndices.find(currentSegmentIndex) != terrainSegmentIndices.end()) {
				segments.at<int32_t>(p) = 0;
			}
		});

		

	}


	void RemoveTooSmallSegments(Mat segments) {
		map<int, size_t> segmentSizes;
		
		ForEachPixel(segments, [&](Point p) {
			int currentSegmentIndex = segments.at<int32_t>(p);
			segmentSizes[currentSegmentIndex]++;
		});

		ForEachPixel(segments, [&](Point p) {
			int currentSegmentIndex = segments.at<int32_t>(p);
			if(segmentSizes[currentSegmentIndex] < minimumBuildingArea) {
				segments.at<int32_t>(p) = 0;
			}
		});

	}



};

Result LidarChangeDetector::DoEverything(LPCSTR fileName1, LPCSTR fileName2) {
	LidarChangeDetectorImpl detector;
	try {
		detector.DoEverything(fileName1, fileName2);
	} catch(DebugAbort) {

	}
	
	return detector.GetResult();
}

