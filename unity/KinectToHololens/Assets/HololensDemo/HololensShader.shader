// Used fixed for all the floating point numbers.
// Reference: https://docs.microsoft.com/en-us/windows/mixed-reality/performance-recommendations-for-unity
Shader "KinectToHololens/Hololens"
{
    Properties
    {
        _YTex("Y Texture", 2D) = "white" {}
        _UTex("U Texture", 2D) = "white" {}
        _VTex("V Texture", 2D) = "white" {}
        _DepthTex("Depth Texture", 2D) = "white" {}
        //_ColorFrameWidth("Color Frame Width", Float) = 0
        //_ColorFrameHeight("Color Frame Height", Float) = 0
        _ColorFrameWidthReciprocal("Reciprocal of Color Frame Width", Float) = 0
        _ColorFrameHeightReciprocal("Reciprocal of Color Frame Height", Float) = 0
        _ColorFx("Color Camera Parameter fx", Float) = 0
        _ColorCx("Color Camera Parameter cx", Float) = 0
        //_ColorShiftM("Color Camera Parameter shift_m", Float) = 0
        _ColorShiftMForMeters("Color Camera Parameter shift_m Divided by 1000", Float) = 0
        _VertexOffsetXVector("Vertex Offset X Vector", Vector) = (0, 0, 0, 0)
        _VertexOffsetYVector("Vertex Offset Y Vector", Vector) = (0, 0, 0, 0)
    }
    SubShader
    {
        Tags { "RenderType" = "Opaque" }
        Cull Off
        LOD 100

        Pass
        {
            CGPROGRAM
            #pragma target 4.0
            #pragma vertex vert
            #pragma geometry geom
            #pragma fragment frag

            #include "UnityCG.cginc"

            struct appdata
            {
                fixed4 vertex : POSITION;
                fixed2 uv : TEXCOORD0;
                fixed2 uv2 : TEXCOORD1;
                fixed2 uv3 : TEXCOORD2;
                fixed2 uv4 : TEXCOORD3;
            };

            struct v2g
            {
                fixed4 vertex : POSITION;
                fixed2 uv : TEXCOORD0;
                fixed2 vertex_offset : TEXCOORD1;
                fixed2 uv_offset : TEXCOORD2;
            };

            struct g2f
            {
                fixed4 vertex : SV_POSITION;
                fixed2 uv : TEXCOORD0;
            };

            Texture2D _YTex;
            Texture2D _UTex;
            Texture2D _VTex;
            SamplerState sampler_YTex;
            sampler2D _DepthTex;
            //fixed _ColorFrameWidth;
            //fixed _ColorFrameHeight;
            fixed _ColorFrameWidthReciprocal;
            fixed _ColorFrameHeightReciprocal;
            fixed _ColorFx;
            fixed _ColorCx;
            //fixed _ColorShiftM;
            fixed _ColorShiftMForMeters;
            fixed4 _VertexOffsetXVector;
            fixed4 _VertexOffsetYVector;

            v2g vert(appdata v)
            {
                v2g o;
                // Equivalent to "fixed depth = tex2Dlod(_DepthTex, fixed4(v.uv2, 0, 0)).r * 8.0 * 8.191;"
                // 8.0 is for converting 13-bit data to 16-bit format of the texture.
                // 8.191 (= (2^13 - 1) / 1000) is for converting the depth encoded in the range of 0 ~ (2^13 - 1) mm to meters.
                // "depth" is in meters.
                fixed depth = tex2Dlod(_DepthTex, fixed4(v.uv2, 0, 0)).r * 65.528;

                // Rotating the vertex in the way to make it to match the direction of the camera (not directly facing it). 
                // Commented code before optimization.
                //fixed3 vertex = v.vertex + _LocalCameraRightVector * v.uv3.x + _LocalCameraUpVector * v.uv3.y;
                //fixed3 vertex = mad(_LocalCameraRightVector, v.uv3.x, mad(_LocalCameraUpVector, v.uv3.y, v.vertex));
                //o.vertex = UnityObjectToClipPos(vertex * depth);
                //o.vertex = UnityObjectToClipPos(mad(_LocalCameraRightVector, v.uv3.x, mad(_LocalCameraUpVector, v.uv3.y, v.vertex)) * depth);
                o.vertex = v.vertex * depth;

                // Optimized code for uv calculation using the below conversion of constants.
                // _ColorShiftMForMeters = _ColorShiftM / 1000
                // _ColorFrameWidthReciprocal = 1 / _ColorFrameWidth
                // _ColorFrameHeightReciprocal = 1 / _ColorFrameHeight
                // Commented code before optimization.
                //fixed rx = v.uv.x;
                //fixed cy = v.uv.y;
                //rx = rx + _ColorShiftM / (depth * 1000.0); // depth in millimeters, the default unit of Kinect2
                //fixed cx = rx * _ColorFx + _ColorCx;
                //o.uv = fixed2(cx / _ColorFrameWidth, cy / _ColorFrameHeight);
                //o.uv = fixed2(mad(v.uv.x + _ColorShiftMForMeters / depth, _ColorFx, _ColorCx) * _ColorFrameWidthReciprocal, v.uv.y * _ColorFrameHeightReciprocal);
                o.uv = fixed2(mad(mad(_ColorShiftMForMeters, rcp(depth), v.uv.x), _ColorFx, _ColorCx) * _ColorFrameWidthReciprocal, v.uv.y * _ColorFrameHeightReciprocal);

                o.vertex_offset = v.uv3 * depth;
                o.uv_offset = v.uv4;

                return o;
            }

            [maxvertexcount(4)]
            void geom(point v2g i[1], inout TriangleStream<g2f> triangles)
            {
                // Filtering out invalid depth pixels and depth pixels without a corresponding color pixel.
                // Tried mad here, but it added cost.
                if (i[0].vertex.z > 0.1 && i[0].uv.x > 0.0 && i[0].uv.x < 1.0 && i[0].uv.y > 0.0 && i[0].uv.y < 1.0)
                {
                    g2f o;
                    // Tried using mvp matrix instead of the below one using vp matrix and unity_ObjectToWorld.
                    // It turns calculating vertex, offset_x, and offset_y from 6 matrix-vector multiplications into
                    // a matrix-matrix multiplication and 3 matrix-vector multiplications.
                    // Unfortunately, it didn't improved this shader...
                    fixed4 vertex = mul(UNITY_MATRIX_VP, mul(unity_ObjectToWorld, float4(i[0].vertex.xyz, 1.0)));
                    fixed4 offset_x = mul(UNITY_MATRIX_VP, mul(unity_ObjectToWorld, _VertexOffsetXVector * i[0].vertex_offset.x));
                    fixed4 offset_y = mul(UNITY_MATRIX_VP, mul(unity_ObjectToWorld, _VertexOffsetYVector * i[0].vertex_offset.y));

                    fixed uv_right = i[0].uv_offset.x;
                    fixed uv_up = i[0].uv_offset.y;

                    o.vertex = vertex;
                    o.uv = i[0].uv;
                    triangles.Append(o);

                    o.vertex = o.vertex + offset_x;
                    o.uv = i[0].uv + fixed2(i[0].uv_offset.x, 0.0);
                    triangles.Append(o);

                    o.vertex = vertex + offset_y;
                    o.uv = i[0].uv + fixed2(0.0, i[0].uv_offset.y);
                    triangles.Append(o);

                    o.vertex = vertex + offset_x + offset_y;
                    o.uv = i[0].uv + i[0].uv_offset;
                    triangles.Append(o);
                }
            }

            fixed4 frag(g2f i) : SV_Target
            {
                // Formula came from https://docs.microsoft.com/en-us/windows/desktop/medfound/recommended-8-bit-yuv-formats-for-video-rendering.
                // Commented code before optimization.
                //fixed c = (tex2D(_YTex, i.uv).r - 0.0625) * 1.164383;
                //fixed c = tex2D(_YTex, i.uv).r * 1.164383 - 0.072774;
                //fixed c = mad(tex2D(_YTex, i.uv).r, 1.164383, -0.072774);
                //fixed d = tex2D(_UTex, i.uv).r - 0.5;
                //fixed e = tex2D(_VTex, i.uv).r - 0.5;
                fixed c = mad(_YTex.Sample(sampler_YTex, i.uv).r, 1.164383, -0.072774);
                fixed d = _UTex.Sample(sampler_YTex, i.uv).r - 0.5;
                fixed e = _VTex.Sample(sampler_YTex, i.uv).r - 0.5;

                //return fixed4(c + 1.596027 * e, c - 0.391762 * d - 0.812968 * e, c + 2.017232 * d, 1.0);
                return fixed4(mad(1.596027, e, c), mad(-0.812968, e, mad(-0.391762, d, c)), mad(2.017232, d, c), 1.0);
            }
            ENDCG
        }
    }
}