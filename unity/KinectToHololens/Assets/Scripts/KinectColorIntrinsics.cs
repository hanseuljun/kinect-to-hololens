public class KinectColorIntrinsics
{
    public float Fx { get; private set; }
    public float Fy { get; private set; }
    public float Cx { get; private set; }
    public float Cy { get; private set; }
    public float ShiftD { get; private set; }
    public float ShiftM { get; private set; }
    public float MxX3y0 { get; private set; }
    public float MxX0y3 { get; private set; }
    public float MxX2y1 { get; private set; }
    public float MxX1y2 { get; private set; }
    public float MxX2y0 { get; private set; }
    public float MxX0y2 { get; private set; }
    public float MxX1y1 { get; private set; }
    public float MxX1y0 { get; private set; }
    public float MxX0y1 { get; private set; }
    public float MxX0y0 { get; private set; }
    public float MyX3y0 { get; private set; }
    public float MyX0y3 { get; private set; }
    public float MyX2y1 { get; private set; }
    public float MyX1y2 { get; private set; }
    public float MyX2y0 { get; private set; }
    public float MyX0y2 { get; private set; }
    public float MyX1y1 { get; private set; }
    public float MyX1y0 { get; private set; }
    public float MyX0y1 { get; private set; }
    public float MyX0y0 { get; private set; }

    public KinectColorIntrinsics(float fx,
                                    float fy,
                                    float cx,
                                    float cy,
                                    float shiftD,
                                    float shiftM,
                                    float mxX3y0,
                                    float mxX0y3,
                                    float mxX2y1,
                                    float mxX1y2,
                                    float mxX2y0,
                                    float mxX0y2,
                                    float mxX1y1,
                                    float mxX1y0,
                                    float mxX0y1,
                                    float mxX0y0,
                                    float myX3y0,
                                    float myX0y3,
                                    float myX2y1,
                                    float myX1y2,
                                    float myX2y0,
                                    float myX0y2,
                                    float myX1y1,
                                    float myX1y0,
                                    float myX0y1,
                                    float myX0y0)
    {
        Fx = fx;
        Fy = fy;
        Cx = cx;
        Cy = cy;
        ShiftD = shiftD;
        ShiftM = shiftM;
        MxX3y0 = mxX3y0;
        MxX0y3 = mxX0y3;
        MxX2y1 = mxX2y1;
        MxX1y2 = mxX1y2;
        MxX2y0 = mxX2y0;
        MxX0y2 = mxX0y2;
        MxX1y1 = mxX1y1;
        MxX1y0 = mxX1y0;
        MxX0y1 = mxX0y1;
        MxX0y0 = mxX0y0;
        MyX3y0 = myX3y0;
        MyX0y3 = myX0y3;
        MyX2y1 = myX2y1;
        MyX1y2 = myX1y2;
        MyX2y0 = myX2y0;
        MyX0y2 = myX0y2;
        MyX1y1 = myX1y1;
        MyX1y0 = myX1y0;
        MyX0y1 = myX0y1;
        MyX0y0 = myX0y0;
    }
}
