public class KinectIrIntrinsics
{
    public float Fx { get; private set; }
    public float Fy { get; private set; }
    public float Cx { get; private set; }
    public float Cy { get; private set; }
    public float K1 { get; private set; }
    public float K2 { get; private set; }
    public float K3 { get; private set; }
    public float P1 { get; private set; }
    public float P2 { get; private set; }

    public KinectIrIntrinsics(float fx,
                                float fy,
                                float cx,
                                float cy,
                                float k1,
                                float k2,
                                float k3,
                                float p1,
                                float p2)
    {
        Fx = fx;
        Fy = fy;
        Cx = cx;
        Cy = cy;
        K1 = k1;
        K2 = k2;
        K3 = k3;
        P1 = p1;
        P2 = p2;
    }
}
