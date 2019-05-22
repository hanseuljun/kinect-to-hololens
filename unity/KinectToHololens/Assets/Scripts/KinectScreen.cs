using UnityEngine;

// reference: https://github.com/OpenKinect/libfreenect2/blob/master/src/registration.cpp
// Converts the parameters into a set of vertices with texture coordinates.
public class KinectScreen
{
    private const float depthQ = 0.01f;
    private const float colorQ = 0.002199f;
    public Vector3[] Vertices { get; private set; }
    // For the texture.
    public Vector2[] Uv { get; private set; }
    // For the depth map.
    public Vector2[] Uv2 { get; private set; }
    public KinectColorIntrinsics ColorIntrinsics { get; private set; }
    public KinectIrIntrinsics IrIntrinsics { get; private set; }

    public KinectScreen(KinectColorIntrinsics colorIntrinsics,
                        KinectIrIntrinsics irIntrinsics)
    {
        const int KINECT_DEPTH_WIDTH = 512;
        const int KINECT_DEPTH_HEIGHT = 424;

        Vertices = new Vector3[KINECT_DEPTH_WIDTH * KINECT_DEPTH_HEIGHT];
        Uv = new Vector2[KINECT_DEPTH_WIDTH * KINECT_DEPTH_HEIGHT];
        Uv2 = new Vector2[KINECT_DEPTH_WIDTH * KINECT_DEPTH_HEIGHT];
        for (int y = 0; y < KINECT_DEPTH_HEIGHT; ++y)
        {
            for (int x = 0; x < KINECT_DEPTH_WIDTH; ++x)
            {
                var distort = Distort(x, y, irIntrinsics);
                var depthToColor = DepthToColor(x, y, colorIntrinsics, irIntrinsics);
                // In line 356~357 of RegistrationImpl::getPointXYZ, r (row) and c (column) are directly used in calculating x and y.
                // The negative sign in front of the x value comes from the difference between Kinect's cooridnate system and Unity's.
                // The negative sign in front of the y value is to convert the rows and columns into x's and y's where the origin is
                // not top-left, but bottom-left.
                Vertices[x + y * KINECT_DEPTH_WIDTH] = new Vector3(-(x + 0.5f - irIntrinsics.Cx) / irIntrinsics.Fx,
                                                            -(y + 0.5f - irIntrinsics.Cy) / irIntrinsics.Fy,
                                                            1.0f);
                // This can be inferred from how the values of Frame* registered are obtained.
                Uv[x + y * KINECT_DEPTH_WIDTH] = depthToColor;
                // This can be inferred from how the values of Frame* undistorted are obtained.
                Uv2[x + y * KINECT_DEPTH_WIDTH] = new Vector2(distort.x / (KINECT_DEPTH_WIDTH - 1), distort.y / (KINECT_DEPTH_HEIGHT - 1));
            }
        }

        ColorIntrinsics = colorIntrinsics;
        IrIntrinsics = irIntrinsics;
    }

    // Equivalent to RegistrationImpl::distort().
    private static Vector2 Distort(int mx, int my, KinectIrIntrinsics irCameraParams)
    {
        // see http://en.wikipedia.org/wiki/Distortion_(optics) for description
        float dx = (mx - irCameraParams.Cx) / irCameraParams.Fx;
        float dy = (my - irCameraParams.Cy) / irCameraParams.Fy;
        float dx2 = dx * dx;
        float dy2 = dy * dy;
        float r2 = dx2 + dy2;
        float dxdy2 = 2.0f * dx * dy;
        float kr = 1.0f + ((irCameraParams.K3 * r2 + irCameraParams.K2) * r2 + irCameraParams.K1) * r2;

        float x = irCameraParams.Fx * (dx * kr + irCameraParams.P2 * (r2 + 2.0f * dx2) + irCameraParams.P1 * dxdy2) + irCameraParams.Cx;
        float y = irCameraParams.Fy * (dy * kr + irCameraParams.P1 * (r2 + 2.0f * dy2) + irCameraParams.P2 * dxdy2) + irCameraParams.Cy;

        return new Vector2(x, y);
    }

    // Equivalent to RegistrationImpl::depth_to_color().
    private static Vector2 DepthToColor(float mx,
                                        float my,
                                        KinectColorIntrinsics colorCameraParams,
                                        KinectIrIntrinsics irCameraParams)
    {
        mx = (mx - irCameraParams.Cx) * depthQ;
        my = (my - irCameraParams.Cy) * depthQ;

        float wx = mx * mx * mx * colorCameraParams.MxX3y0
                    + my * my * my * colorCameraParams.MxX0y3
                    + mx * mx * my * colorCameraParams.MxX2y1
                    + mx * my * my * colorCameraParams.MxX1y2
                    + mx * mx * colorCameraParams.MxX2y0
                    + my * my * colorCameraParams.MxX0y2
                    + mx * my * colorCameraParams.MxX1y1
                    + mx * colorCameraParams.MxX1y0
                    + my * colorCameraParams.MxX0y1
                    + colorCameraParams.MxX0y0;

        float wy = mx * mx * mx * colorCameraParams.MyX3y0
                    + my * my * my * colorCameraParams.MyX0y3
                    + mx * mx * my * colorCameraParams.MyX2y1
                    + mx * my * my * colorCameraParams.MyX1y2
                    + mx * mx * colorCameraParams.MyX2y0
                    + my * my * colorCameraParams.MyX0y2
                    + mx * my * colorCameraParams.MyX1y1
                    + mx * colorCameraParams.MyX1y0
                    + my * colorCameraParams.MyX0y1
                    + colorCameraParams.MyX0y0;

        float rx = (wx / (colorCameraParams.Fx * colorQ)) - (colorCameraParams.ShiftM / colorCameraParams.ShiftD);
        float ry = (wy / colorQ) + colorCameraParams.Cy;

        return new Vector2(rx, ry);
    }
}
