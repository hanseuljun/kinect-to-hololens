using UnityEngine;
using UnityEngine.Rendering;

// A Unity script that renderers pixels of a Kinect with a corresponding KinectScreen.
[RequireComponent(typeof(MeshFilter), typeof(MeshRenderer))]
public class ScreenRenderer : MonoBehaviour
{
    public MeshFilter meshFilter;
    public MeshRenderer meshRenderer;
    public Camera headsetCamera;

    // Updates _VertexOffsetXVector and _VertexOffsetYVector so the rendered quads can face the headsetCamera.
    // This method gets called right before this ScreenRenderer gets rendered.
    void OnWillRenderObject()
    {
        if (meshRenderer.sharedMaterial == null)
            return;

        // Ignore when this method is called while Unity rendering the Editor's "Scene" (not the "Game" part of the editor).
        if (Camera.current != headsetCamera)
            return;

        var cameraTransform = Camera.current.transform;
        var worldCameraFrontVector = cameraTransform.TransformDirection(new Vector3(0.0f, 0.0f, 1.0f));

        // Using the y direction as the up vector instead the up vector of the camera allows the user to feel more
        // comfortable as it preserves the sense of gravity.
        // Getting the right vector directly from the camera transform through zeroing its y-component does not
        // work when the y-component of the camera's up vector is negative. While it is possible to solve the problem
        // with an if statement, inverting when the y-component is negative, I decided to detour this case with
        // usage of the cross product with the front vector.
        var worldUpVector = new Vector3(0.0f, 1.0f, 0.0f);
        var worldRightVector = Vector3.Cross(worldUpVector, worldCameraFrontVector);
        worldRightVector = new Vector3(worldRightVector.x, 0.0f, worldRightVector.z);
        worldRightVector.Normalize();

        var localRightVector = transform.InverseTransformDirection(worldRightVector);
        var localUpVector = transform.InverseTransformDirection(worldUpVector);

        // The coordinate system of Kinect's textures have (1) its origin at its left-up side.
        // Also, the viewpoint of it is sort of (2) the opposite of the viewpoint of the Hololens, considering the typical use case.
        // Due to (1), vertexOffsetYVector = -localCameraUpVector.
        // Due to (2), vertexOffsetXVector = -localCameraRightVector.
        var vertexOffsetXVector = -localRightVector;
        var vertexOffsetYVector = -localUpVector;

        meshRenderer.sharedMaterial.SetVector("_VertexOffsetXVector", new Vector4(vertexOffsetXVector.x, vertexOffsetXVector.y, vertexOffsetXVector.z, 0.0f));
        meshRenderer.sharedMaterial.SetVector("_VertexOffsetYVector", new Vector4(vertexOffsetYVector.x, vertexOffsetYVector.y, vertexOffsetYVector.z, 0.0f));
    }

    public void SetKinectScreen(KinectScreen kinectScreen)
    {
        const int KINECT_COLOR_WIDTH = 1920;
        const int KINECT_COLOR_HEIGHT = 1080;
        meshFilter.mesh = CreateMesh(kinectScreen);

        meshRenderer.sharedMaterial.SetFloat("_ColorFrameWidthReciprocal", 1.0f / KINECT_COLOR_WIDTH);
        meshRenderer.sharedMaterial.SetFloat("_ColorFrameHeightReciprocal", 1.0f / KINECT_COLOR_HEIGHT);
        meshRenderer.sharedMaterial.SetFloat("_ColorFrameWidth", KINECT_COLOR_WIDTH);
        meshRenderer.sharedMaterial.SetFloat("_ColorFrameHeight", KINECT_COLOR_HEIGHT);
        meshRenderer.sharedMaterial.SetFloat("_ColorFx", kinectScreen.ColorIntrinsics.Fx);
        meshRenderer.sharedMaterial.SetFloat("_ColorCx", kinectScreen.ColorIntrinsics.Cx);
        meshRenderer.sharedMaterial.SetFloat("_ColorShiftMForMeters", kinectScreen.ColorIntrinsics.ShiftM / 1000.0f);
    }

    private static Mesh CreateMesh(KinectScreen kinect2Screen)
    {
        const int KINECT_DEPTH_WIDTH = 512;
        const int KINECT_DEPTH_HEIGHT = 424;

        // Interpolation of vertices. A [depthWidth * depthHeight] screen into a [(depthWidth - 1) * (depthHeight - 1)].
        var interploatedVertices = new Vector3[(KINECT_DEPTH_WIDTH - 1) * (KINECT_DEPTH_HEIGHT - 1)];
        var interploatedUvs = new Vector2[(KINECT_DEPTH_WIDTH - 1) * (KINECT_DEPTH_HEIGHT - 1)];
        for (int i = 0; i < KINECT_DEPTH_WIDTH - 1; ++i)
        {
            for (int j = 0; j < KINECT_DEPTH_HEIGHT - 1; ++j)
            {
                interploatedVertices[i + j * (KINECT_DEPTH_WIDTH - 1)] = (kinect2Screen.Vertices[(i + 0) + (j + 0) * KINECT_DEPTH_WIDTH]
                                                                       +  kinect2Screen.Vertices[(i + 1) + (j + 0) * KINECT_DEPTH_WIDTH]
                                                                       +  kinect2Screen.Vertices[(i + 0) + (j + 1) * KINECT_DEPTH_WIDTH]
                                                                       +  kinect2Screen.Vertices[(i + 1) + (j + 1) * KINECT_DEPTH_WIDTH]) * 0.25f;

                interploatedUvs[i + j * (KINECT_DEPTH_WIDTH - 1)] = (kinect2Screen.Uv[(i + 0) + (j + 0) * KINECT_DEPTH_WIDTH]
                                                                  +  kinect2Screen.Uv[(i + 1) + (j + 0) * KINECT_DEPTH_WIDTH]
                                                                  +  kinect2Screen.Uv[(i + 0) + (j + 1) * KINECT_DEPTH_WIDTH]
                                                                  +  kinect2Screen.Uv[(i + 1) + (j + 1) * KINECT_DEPTH_WIDTH]) * 0.25f;
            }
        }

        var vertices = new Vector3[(KINECT_DEPTH_WIDTH - 2) * (KINECT_DEPTH_HEIGHT - 2)];
        var uvs = new Vector2[(KINECT_DEPTH_WIDTH - 2) * (KINECT_DEPTH_HEIGHT - 2)];
        var uvs2 = new Vector2[(KINECT_DEPTH_WIDTH - 2) * (KINECT_DEPTH_HEIGHT - 2)];
        var uvs3 = new Vector2[(KINECT_DEPTH_WIDTH - 2) * (KINECT_DEPTH_HEIGHT - 2)];
        var uvs4 = new Vector2[(KINECT_DEPTH_WIDTH - 2) * (KINECT_DEPTH_HEIGHT - 2)];

        for (int i = 0; i < KINECT_DEPTH_WIDTH - 2; ++i)
        {
            for (int j = 0; j < KINECT_DEPTH_HEIGHT - 2; ++j)
            {
                int index = i + j * (KINECT_DEPTH_WIDTH - 2);
                var vertex = interploatedVertices[(i + 0) + (j + 0) * (KINECT_DEPTH_WIDTH - 1)];
                var vertexOffset = interploatedVertices[(i + 1) + (j + 1) * (KINECT_DEPTH_WIDTH - 1)] - vertex;

                var uv = interploatedUvs[(i + 0) + (j + 0) * (KINECT_DEPTH_WIDTH - 1)];
                var uvOffset = interploatedUvs[(i + 1) + (j + 1) * (KINECT_DEPTH_WIDTH - 1)] - uv;

                // As each quad is for a pixel in the depth map, they share a same uv (color map coordinate), uv2 (depth map coordinate) value.
                //uv[index] = kinect2Screen.Uv[(i + 1) + (j + 1) * depthWidth];
                //uv2[index] = kinect2Screen.Uv2[(i + 1) + (j + 1) * depthWidth];
                //uv3[index] = new Vector2(offset.x, offset.y);

                vertices[index] = vertex;
                uvs[index] = uv;
                uvs2[index] = kinect2Screen.Uv2[(i + 1) + (j + 1) * KINECT_DEPTH_WIDTH];
                uvs3[index] = new Vector2(vertexOffset.x, vertexOffset.y);
                uvs4[index] = uvOffset;
            }
        }

        var triangles = new int[vertices.Length];
        for (int i = 0; i < triangles.Length; ++i)
            triangles[i] = i;

        // Without the bounds, Unity decides whether to render this mesh or not based on the vertices calculated here.
        // This causes Unity not rendering the mesh transformed by the depth texture even when the transformed one
        // belongs to the viewport of the camera.
        var bounds = new Bounds(Vector3.zero, Vector3.one * 1000.0f);

        var mesh = new Mesh()
        {
            indexFormat = IndexFormat.UInt32,
            vertices = vertices,
            uv = uvs,
            uv2 = uvs2,
            uv3 = uvs3,
            triangles = triangles,
            bounds = bounds,
        };
        mesh.SetIndices(triangles, MeshTopology.Points, 0);

        return mesh;
    }
}
