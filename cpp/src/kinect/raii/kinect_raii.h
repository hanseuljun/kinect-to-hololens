#pragma once

#include <Kinect.h>
#include <memory>
#include <vector>

namespace rgbd_streamer
{
namespace kinect
{
namespace raii
{
class KinectColorCameraSettings
{
public:
	KinectColorCameraSettings(IColorCameraSettings* color_camera_settings);
	~KinectColorCameraSettings();
	KinectColorCameraSettings(const KinectColorCameraSettings& other) = delete;
	KinectColorCameraSettings& operator=(const KinectColorCameraSettings& other) = delete;
	int64_t getFrameInterval();

private:
	IColorCameraSettings* color_camera_settings_;
};

class KinectColorFrame
{
public:
	KinectColorFrame(IColorFrame* color_frame);
	~KinectColorFrame();
	KinectColorFrame(const KinectColorFrame& other) = delete;
	KinectColorFrame& operator=(const KinectColorFrame& other) = delete;
	std::unique_ptr<KinectColorCameraSettings> getColorCameraSettings();

private:
	IColorFrame* color_frame_;
};

class KinectDepthFrame
{
public:
	KinectDepthFrame(IDepthFrame* depth_frame);
	~KinectDepthFrame();
	KinectDepthFrame(const KinectDepthFrame& other) = delete;
	KinectDepthFrame& operator=(const KinectDepthFrame& other) = delete;
	UINT16* getBuffer();

private:
	IDepthFrame* depth_frame_;
};

class KinectBody
{
public:
	KinectBody(IBody* body);
	~KinectBody();
	KinectBody(const KinectBody& other) = delete;
	KinectBody& operator=(const KinectBody& other) = delete;
	bool isTracked();
	Joint getHeadJoint();

private:
	IBody* body_;
};

class KinectBodyFrame
{
public:
	KinectBodyFrame(IBodyFrame* body_frame);
	~KinectBodyFrame();
	KinectBodyFrame(const KinectBodyFrame& other) = delete;
	KinectBodyFrame& operator=(const KinectBodyFrame& other) = delete;
	std::vector<Joint> getHeadJoints();
	Vector4 getFloorClipPlane();

private:
	IBodyFrame* body_frame_;
};

class KinectMultiSourceFrame
{
public:
	KinectMultiSourceFrame(IMultiSourceFrame* multi_source_frame);
	~KinectMultiSourceFrame();
	KinectMultiSourceFrame(const KinectMultiSourceFrame& other) = delete;
	KinectMultiSourceFrame& operator=(const KinectMultiSourceFrame& other) = delete;
	std::unique_ptr<KinectColorFrame> getColorFrame();
	std::unique_ptr<KinectDepthFrame> getDepthFrame();
	std::unique_ptr<KinectBodyFrame> getBodyFrame();

private:
	IMultiSourceFrame* multi_source_frame_;
};

class KinectMultiSourceFrameReader
{
public:
	KinectMultiSourceFrameReader(IMultiSourceFrameReader* multi_source_frame_reader);
	~KinectMultiSourceFrameReader();
	KinectMultiSourceFrameReader(const KinectMultiSourceFrameReader& other) = delete;
	KinectMultiSourceFrameReader& operator=(const KinectMultiSourceFrameReader& other) = delete;
	std::unique_ptr<KinectMultiSourceFrame> getMultiSourceFrame();

private:
	IMultiSourceFrameReader* multi_source_frame_reader_;
};

class KinectSensor
{
public:
	KinectSensor(IKinectSensor* kinect_sensor);
	~KinectSensor();
	KinectSensor(const KinectSensor& other) = delete;
	KinectSensor& operator=(const KinectSensor& other) = delete;
	static std::unique_ptr<KinectSensor> create();
	bool isAvailable();
	std::unique_ptr<KinectMultiSourceFrameReader> getMultiSourceFrameReader();

private:
	IKinectSensor* kinect_sensor_;
};
}
}
}