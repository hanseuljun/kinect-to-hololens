#include "kinect_raii.h"	

#include <iostream>

namespace rgbd_streamer
{
namespace kinect
{
namespace raii
{
KinectBody::KinectBody(IBody* body)
	: body_(body)
{
}

KinectBody::~KinectBody()
{
	body_->Release();
}

bool KinectBody::isTracked()
{
	BOOLEAN tracked = false;
	if (FAILED(body_->get_IsTracked(&tracked)))
		throw std::exception("Failed in KinectBody::isTracked...");

	return tracked;
}

Joint KinectBody::getHeadJoint()
{
	Joint joints[JointType_Count];
	if (FAILED(body_->GetJoints(_countof(joints), joints)))
		throw std::exception("Failed in KinectBody::getHeadJoint...");

	return joints[JointType_Head];
}
}
}
}