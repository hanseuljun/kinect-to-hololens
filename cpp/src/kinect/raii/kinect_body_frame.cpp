#include "kinect_raii.h"	

#include <iostream>

namespace rgbd_streamer
{
namespace kinect
{
namespace raii
{
KinectBodyFrame::KinectBodyFrame(IBodyFrame* body_frame)
    : body_frame_(body_frame)
{
}

KinectBodyFrame::~KinectBodyFrame()
{
    body_frame_->Release();
}

std::vector<Joint> KinectBodyFrame::getHeadJoints()
{
    std::vector<std::shared_ptr<KinectBody>> bodies;

    IBody* ppBodies[BODY_COUNT] = { 0 };
    if (FAILED(body_frame_->GetAndRefreshBodyData(_countof(ppBodies), ppBodies)))
        throw std::exception("Failed GetAndRefreshBodyData in KinectBodyFrame::getBodies...");

    for (int i = 0; i < _countof(ppBodies); ++i)
        bodies.push_back(std::make_shared<KinectBody>(ppBodies[i]));

    std::vector<Joint> head_joints;

    for (auto body : bodies) {
        if (!body->isTracked())
            continue;

        Joint joint = body->getHeadJoint();
        if (joint.TrackingState != TrackingState_Tracked)
            continue;

        head_joints.push_back(joint);
    }

    return head_joints;
}

Vector4 KinectBodyFrame::getFloorClipPlane()
{
    Vector4 floor_clip_plane;
    if (FAILED(body_frame_->get_FloorClipPlane(&floor_clip_plane)))
        throw std::exception("Failed get_FloorClipPlane in KinectBodyFrame::getFloor...");

    return floor_clip_plane;
}
}
}
}