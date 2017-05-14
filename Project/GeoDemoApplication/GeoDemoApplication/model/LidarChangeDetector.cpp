#include <stdafx.h>

#include "LidarChangeDetector.h"
#include "MathUtil.h"




struct LidarChangeDetectorImpl {
	const float maxSteepness = 8;
	const size_t minimumBuildingArea = 25;

	// nem szép de a debug miatt hasznos, hogy bárki gyorsan bele tudjon irni
	Result result; 

	Result DoEverything(LPCSTR fileName) {
		//Mat TEST = cv::imread("segmentation_example.png");
		//ForEachPixel(TEST, [&](Point p) {
		//	TEST.at<Vec3b>(p) = Vec3b(0, 123, 255);
		//	ForEachNeightbour(TEST, p, [&] (Point p2) {
		//		TEST.at<Vec3b>(p) = Vec3b(0, 123, 0);
		//	});
		//});


		result = Result();

		//CString fileName = CString("segmentation_example.png");
		//const char* fileName = "segmentation_example.png";
		//const char* fileName = "b2.png";
		Mat image_rgb = cv::imread(fileName);
		const Mat image = ToGray(image_rgb);

		result.im1 = image;

		
		Mat filteredImage;


		FilterImage(image, filteredImage);

		vector<KeyPoint> keypoints;
		FindObjects(filteredImage, keypoints);
		//return result;
		//filteredImage = 

		//list<Segment> segments;
		//seeds
		//Mat markers;

		//vector<Point> seeds = ReadSeeds("b2_modded.png");
		//SegmentImage(filteredImage, seeds);
		vector<Point> seeds;
		ExtractPoints(keypoints, seeds);
		Mat segments;
		SegmentImage(filteredImage, seeds, segments);

		

		
		
		RemoveTooSmallSegments(segments);

		Mat debug = RandomColorIndexedImage(segments);
		result.im2 = filteredImage;
		result.im3 = debug;


		//imshow(image);
		//imshow("im1", image);


		
		//result.im2 = filteredImage;
		return result;
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
		

		cv::medianBlur(image, filteredImage, 5);

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


	void SegmentImage(const Mat image, vector<Point>& seeds, Mat& segments) {
		segments = Mat(image.rows, image.cols, CV_32S);
		segments.setTo(0);

		set<int> terrainSegmentIndices;

		queue<Point> pointsToProcess;
		int currentSegmentIndex = 1;
		for(Point& seed : seeds) {
			segments.at<int32_t>(seed) = currentSegmentIndex;
			++currentSegmentIndex;
			pointsToProcess.push(seed);
		}

		while(!pointsToProcess.empty()) {
			Point p = pointsToProcess.front();
			pointsToProcess.pop();

			int centerValue = image.at<uint8_t>(p);
			int currentSegmentIndex = segments.at<int32_t>(p);

			ForEachNeightbour(segments, p, [&](Point neighbour) {
				int neighbourSegmentIndex = segments.at<int32_t>(neighbour);
				if(neighbourSegmentIndex != 0) return; // ha valaki már felhasználta

				uint8_t neighbourValue = image.at<uint8_t>(neighbour);
				int levelDifference = centerValue - neighbourValue;
				if(abs(levelDifference) < maxSteepness) {
					segments.at<int32_t>(neighbour) = currentSegmentIndex;
					pointsToProcess.push(neighbour);
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


	//void SegmentImage(Mat difference, list<Segment>& segments) {

	//	cv::SimpleBlobDetector::Params params;
	//	params.minThreshold = 10;
	//	params.maxThreshold = 180;
	//	params.thresholdStep = 10;

	//	



	//	params.filterByArea = false;
	//	//params.filterByArea = false;
	//	params.maxArea = 1000000;
	//	params.minArea = 100;
	//	//params.maxArea = 30 * 30;
	//	//params.minArea = 3 * 3;



	//	auto detector = cv::SimpleBlobDetector::create(params);

	//	// Detect blobs.
	//	std::vector<KeyPoint> keypoints;
	//	//cv::SimpleBlobDetector detector;
	//	//detector->detect(image, keypoints);
	//	detector->detect(difference, keypoints);
	//	//detector.detect(difference, keypoints);

	//	// Draw detected blobs as red circles.
	//	// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
	//	Mat im_with_keypoints;
	//	drawKeypoints(difference, keypoints, im_with_keypoints, Scalar(255, 255, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);


	//	//im_with_keypoints.convertTo(im_with_keypoints, CV_8U);
	//	im_with_keypoints = ToGray(im_with_keypoints);

	//	//Mat markers(markerMask.size(), CV_32S);
	//	//markers = Scalar::all(0);



	//	


	//	result.im2 = difference;
	//	result.im3 = im_with_keypoints;

	//}


};

Result LidarChangeDetector::DoEverything(LPCSTR fileName) {
	LidarChangeDetectorImpl detector;
	return detector.DoEverything(fileName);
}

