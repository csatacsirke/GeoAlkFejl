#include <stdafx.h>

#include "LidarChangeDetector.h"
#include "MathUtil.h"




class LidarChangeDetectorImpl {
	//const float maxSteepness = 4;
	const float maxSteepness = 8;
	static const size_t minimumBuildingArea = 25;
	static const int maxHeightDifference = 80;


	Result result; 


public:

	Result DoEverything(LPCSTR fileName1, LPCSTR fileName2) {
		//Mat TEST = cv::imread("segmentation_example.png");
		//ForEachPixel(TEST, [&](Point p) {
		//	TEST.at<Vec3b>(p) = Vec3b(0, 123, 255);
		//	ForEachNeightbour(TEST, p, [&] (Point p2) {
		//		TEST.at<Vec3b>(p) = Vec3b(0, 123, 0);
		//	});
		//});

		Mat image1;
		Mat image2;


		result = Result();

		image1 = LoadImageAsGrayscale(fileName1);
		image2 = LoadImageAsGrayscale(fileName2);
		


		Mat segmentedImage2 = ProcessGrayscaleImage(image2);
		Mat segmentedImage1 = ProcessGrayscaleImage(image1);
	


		Mat difference = Difference(segmentedImage1, segmentedImage2);
		Mat newBuildings = FindNewBuildings(difference);
		//CString fileName = CString("segmentation_example.png");
		//const char* fileName = "segmentation_example.png";
		//const char* fileName = "b2.png";
		
		Mat debug1 = ToRgb(image1);
		Mat debug2 = ToRgb(image2);

		DrawIndices(debug1, segmentedImage1);
		DrawIndices(debug2, segmentedImage2);

		result.im1 = debug1;
		result.im2 = debug2;
		result.im3 = RandomColorIndexedImage(newBuildings);


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

		//Mat debug_keys;
		//cv::drawKeypoints(filteredImage, keypoints, debug_keys);
		//result.im2 = debug_keys;
		

		//vector<Point> seeds = ReadSeeds("b2_modded.png");
		//SegmentImage(filteredImage, seeds);
		vector<Point> seeds;
		ExtractPoints(keypoints, seeds);
		Mat segments;
		SegmentImage(filteredImage, seeds, segments);



		RemoveTooSmallSegments(segments); 

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


#if 0

		// a sima gaussos simítás kevés, valami advancedebb kéne, 
		// http://www.sciencedirect.com/science/article/pii/092427169598236S
		// erre a cikkre hivatkoznak az eredeti cikkben de nem sikerült elérnem :( 
		// tudom... konzultálnom kellett volna... sajnálom :(
		// viszont enélkül is sikerült egész jó eredményeket elérni
		const int kernelSize = 75;
		double sigma = kernelSize /6.0;



		Mat blurred;
		cv::GaussianBlur(image, blurred, Size(kernelSize, kernelSize), sigma);

		Mat normalizedImage = image - blurred;

		Mat median;		
		cv::medianBlur(normalizedImage, median, 7);
		//cv::medianBlur(image, median, 7);


		

		result.im1 = image;
		result.im2 = blurred;
		result.im3 = normalizedImage;
#else 

		Mat median;
		cv::medianBlur(image, median, 7);
#endif
		filteredImage = median;
		//Abort();

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



	struct SegmentInfo {
		
		uint8_t min;
		uint8_t max;

		SegmentInfo() {
			min = 255;
			max = 0;
		}

		SegmentInfo(uint8_t initialValue) {
			min = initialValue;
			max = initialValue;
		}

		void Update(uint8_t newValue) {
			min = std::min(min, newValue);
			max = std::max(max, newValue);
		}

		void Update(const SegmentInfo& other) {
			min = std::min(min, other.min);
			max = std::max(max, other.max);
		}

		int DifferenceFrom(uint8_t value) {
			if(value < min) return value - min;
			if(value > max) return max - value;
			return 0;
		}

		int GetLevelDifference() {
			return (max - min);
		}

		int IsProbablyTerrainSegment() {
			return GetLevelDifference() > maxHeightDifference;
		}
	};

	// visszaadja a legkisebb és legnagyobb pontot is
	SegmentInfo FloodFillEx(Mat indexedImage, Point start, int32_t newIndex, Mat elevationGrayscale) {
		queue<Point> pointsToProcess;
		pointsToProcess.push(start);

		int32_t oldIndex = indexedImage.at<int32_t>(start);
		
		SegmentInfo segmentInfo = SegmentInfo(elevationGrayscale.at<uint8_t>(start));

		// hülye vagy
		assert(oldIndex != newIndex);
		if(oldIndex == newIndex) return segmentInfo;

		while(!pointsToProcess.empty()) {
			Point p = pointsToProcess.front();
			pointsToProcess.pop();

			segmentInfo.Update(elevationGrayscale.at<uint8_t>(p));

			ForEachNeightbour(indexedImage, p, [&](Point neighbour) {
				int neighbourSegmentIndex = indexedImage.at<int32_t>(neighbour);
				if(oldIndex == neighbourSegmentIndex) {
					pointsToProcess.push(neighbour);
					indexedImage.at<int32_t>(neighbour) = newIndex;
				}
			});
		}

		return segmentInfo;
	}

	static void EleminateIndex(Mat segments, int32_t indexToDelete) {
		ForEachPixel(segments, [&](Point p) {
			int currentSegmentIndex = segments.at<int32_t>(p);
			if(currentSegmentIndex == indexToDelete) {
				segments.at<int32_t>(p) = -1;
			}
		});
	}

	void SegmentImage(const Mat image, vector<Point>& seeds, Mat& segments) {
		segments = Mat(image.rows, image.cols, CV_32S);
		segments.setTo(0);

		map<int, SegmentInfo> segmentInfo;
		set<int> terrainSegmentIndices;
		terrainSegmentIndices.insert(-1);
		

		queue<Point> pointsToProcess;
		int currentSegmentIndex = 1;
#ifndef _DEBUG
		for(Point& seed : seeds) {
			segments.at<int32_t>(seed) = currentSegmentIndex;
			++currentSegmentIndex;
			pointsToProcess.push(seed);
		}
#else
		{ // debug
			
			Point p(40, 196);
			//Point p(196, 40);
			pointsToProcess.push(p);
			segments.at<int32_t>(p) = currentSegmentIndex;
			uint8_t currentHeightValue = image.at<uint8_t>(p);
			segmentInfo[currentSegmentIndex] = SegmentInfo(currentHeightValue);
		}
#endif

		while(!pointsToProcess.empty()) {
			Point p = pointsToProcess.front();
			pointsToProcess.pop();

			int centerValue = image.at<uint8_t>(p);
			int currentSegmentIndex = segments.at<int32_t>(p);

			if(0 == currentSegmentIndex) continue;
			if(-1 == currentSegmentIndex) continue;

			if(segmentInfo[currentSegmentIndex].IsProbablyTerrainSegment()) {
				EleminateIndex(segments, currentSegmentIndex);
				continue;
			}

			segmentInfo[currentSegmentIndex].Update(centerValue);

			ForEachNeightbour(segments, p, [&](Point neighbour) {
				int neighbourSegmentIndex = segments.at<int32_t>(neighbour);
				if(neighbourSegmentIndex == currentSegmentIndex) return;
				

				uint8_t neighbourValue = image.at<uint8_t>(neighbour);
				int levelDifference = centerValue - neighbourValue;
				if(abs(levelDifference) < maxSteepness) {

					// ha valaki már felhasználta egyesitunk
					if(neighbourSegmentIndex != 0 ) {
						//FloodFill(segments, neighbour, currentSegmentIndex);
						SegmentInfo attachedSegmentInfo = FloodFillEx(segments, neighbour, currentSegmentIndex, image);
						segmentInfo[currentSegmentIndex].Update(attachedSegmentInfo);
					} else {
						segments.at<int32_t>(neighbour) = currentSegmentIndex;
						pointsToProcess.push(neighbour);
					}

				} else {
					// ha a angy szintkülönség miatt a mellettünk levö szegmens
					// felettünk van akkor mi nyilván nem épület vagyunk
					int levelDifferenceFromSegmentExtremes = segmentInfo[currentSegmentIndex].DifferenceFrom(neighbourValue);
					if(levelDifferenceFromSegmentExtremes > 0) {
						terrainSegmentIndices.insert(currentSegmentIndex);
					}
				}
				
			});
		}


		FilterLowGrounds(image, segments);


		// ami terrain azt kidobjuk
		ForEachPixel(segments, [&](Point p) {
			int currentSegmentIndex = segments.at<int32_t>(p);
			if(terrainSegmentIndices.find(currentSegmentIndex) != terrainSegmentIndices.end()) {
				segments.at<int32_t>(p) = 0;
			}

			if(segmentInfo[currentSegmentIndex].IsProbablyTerrainSegment()) {
				segments.at<int32_t>(p) = 0;
			}
		});

	}


	// a 'segments' et tönkreteszi!!
	bool IsContourHigherThanSegment(Mat elevations, Mat segments, Point start) {
		int32_t newIndex = 0;
		queue<Point> pointsToProcess;
		pointsToProcess.push(start);

		int32_t oldIndex = segments.at<int32_t>(start);
		uint8_t segmentMinimumElevation = 255;
		uint8_t contourMaximumElevation = 0;

		// hülye vagy
		assert(oldIndex != newIndex);
		if(oldIndex == newIndex) return false;

		while(!pointsToProcess.empty()) {
			Point p = pointsToProcess.front();
			pointsToProcess.pop();

			ForEachNeightbour(segments, p, [&](Point neighbour) {
				int neighbourSegmentIndex = segments.at<int32_t>(neighbour);
				if(oldIndex == neighbourSegmentIndex) {
					pointsToProcess.push(neighbour);
					segments.at<int32_t>(neighbour) = newIndex;
					uint8_t elevation = elevations.at<uint8_t>(p);
					segmentMinimumElevation = std::min(segmentMinimumElevation, elevation);
				} else {
					// ilyenkor vagyunk kontur pontban
					uint8_t elevation = elevations.at<uint8_t>(p);
					contourMaximumElevation = std::max(contourMaximumElevation, elevation);
				}
			});
		}
		return (contourMaximumElevation > segmentMinimumElevation + maxHeightDifference || segmentMinimumElevation < 128);
	}

	void FilterLowGrounds(Mat elevations, Mat segments) {
		set<int> checkedIndices;
		// a IsContourHigherThanSegment-nak bele kell irnia
		Mat segements_clone = segments.clone();

		ForEachPixel(segments, [&](Point p) {
			int currentSegmentIndex = segments.at<int32_t>(p);
			if(checkedIndices.find(currentSegmentIndex) == checkedIndices.end()) {
				checkedIndices.insert(currentSegmentIndex);

				if(currentSegmentIndex != 0) {
					if(IsContourHigherThanSegment(elevations, segements_clone, p)) {
						FloodFill(segments, p, 0);
					}
				}
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

	enum Relation {
		None, FirstOnly, SecondOnly, Both
	};

	static Mat Difference(Mat a, Mat b) {
		int32_t b_indexOffset = MaxElement(a) + 1;
		Mat result = a.clone();

		ForEachPixel(a, [&](Point p) {
			int a_index = a.at<int32_t>(p);
			int b_index = b.at<int32_t>(p);
			if(a_index != 0) {
				if(b_index != 0) {
					result.at<int32_t>(p) = Relation::Both;
				} else {
					result.at<int32_t>(p) = Relation::FirstOnly;
				}
			} else {
				if(b_index != 0) {
					result.at<int32_t>(p) = Relation::SecondOnly;
				} else {
					result.at<int32_t>(p) = Relation::Both;
				}
			}



		});

		return result;
	}

	Mat FindNewBuildings(Mat differences) {
		Mat result = differences.clone();


		ForEachPixel(result, [&](Point p) {
			int32_t index = differences.at<int32_t>(p);
			if(index != Relation::SecondOnly) {
				result.at<int32_t>(p) = 0;
			} else {
				result.at<int32_t>(p) = 1;
			}
		});

		int32_t nextIndex = 2;
		ForEachPixel(result, [&](Point p) {
			int32_t index = result.at<int32_t>(p);
			if(index == 1) {
				FloodFill(result, p, nextIndex);
				++nextIndex;
			}
		});

		RemoveTooSmallSegments(result);

		return result;
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

