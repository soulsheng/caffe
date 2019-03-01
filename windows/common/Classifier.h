
#pragma once

#include <caffe/caffe.hpp>
#ifdef USE_OPENCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#endif  // USE_OPENCV
#include <algorithm>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>


using namespace caffe;  // NOLINT(build/namespaces)
using std::string;

/* Pair (label, confidence) representing a prediction. */
typedef std::pair<string, float> Prediction;

// Detection format: [image_id, label, score, xmin, ymin, xmax, ymax].
typedef struct tagDetection
{
	float image_id;
	float label, score;
	float xmin, ymin, xmax, ymax;
	tagDetection(float *buf)
	{
		memcpy(this, buf, sizeof(tagDetection));
	}
	tagDetection(const std::vector<float>& buf)
	{
		image_id = buf[0];
		label = buf[1];
		score = buf[2];
		xmin = buf[3];
		ymin = buf[4];
		xmax = buf[5];
		ymax = buf[6];
	}
	tagDetection()
	{
		memset(this, 0, sizeof(tagDetection));
	}
} Detection;

typedef std::multimap<float, Detection, std::greater<float>> DetectionMap;
typedef std::pair<float, Detection> DetectionPair;


enum TestType
{
	ENUM_CLASSIFICATION,
	ENUM_DETECTION
};

class Classifier {
public:
	Classifier();

	void load(const string& model_file,
		const string& trained_file,
		const string& mean_file,
		const string& label_file,
		const string& mean_value);

	std::vector<Prediction> Classify(const cv::Mat& img, int N = 5);
	std::vector<Detection> Detect(const cv::Mat& img);
	DetectionMap Detect(const cv::Mat& img, int N, float fScoreMin=0.0f);

	bool isInitialized();

	void setTestType(TestType type);
	TestType getTestType();

	std::vector<string>& getLabels();

private:
	void SetMean(const string& mean_file, const string& mean_value);

	std::vector<float> ClassifyKernel(const cv::Mat& img);
	std::vector<vector<float> > DetectKernel(const cv::Mat& img);

	void WrapInputLayer(std::vector<cv::Mat>* input_channels);

	void Preprocess(const cv::Mat& img,
		std::vector<cv::Mat>* input_channels);

private:
	shared_ptr<Net<float> > net_;
	cv::Size input_geometry_;
	int num_channels_;
	cv::Mat mean_;
	std::vector<string> labels_;

	TestType testType;
};

static bool PairCompare(const std::pair<float, int>& lhs,
	const std::pair<float, int>& rhs) {
	return lhs.first > rhs.first;
}

/* Return the indices of the top N values of vector v. */
static std::vector<int> Argmax(const std::vector<float>& v, int N) {
	std::vector<std::pair<float, int> > pairs;
	for (size_t i = 0; i < v.size(); ++i)
		pairs.push_back(std::make_pair(v[i], static_cast<int>(i)));
	std::partial_sort(pairs.begin(), pairs.begin() + N, pairs.end(), PairCompare);

	std::vector<int> result;
	for (int i = 0; i < N; ++i)
		result.push_back(pairs[i].second);
	return result;
}
