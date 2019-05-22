Shader "KinectToHololens/Color"
{
    Properties
    {
        _YTex("Y Texture", 2D) = "white" {}
        _UTex("U Texture", 2D) = "white" {}
        _VTex("V Texture", 2D) = "white" {}
    }
        SubShader
    {
        Tags { "RenderType" = "Opaque" }
        LOD 100

        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag

            #include "UnityCG.cginc"

            struct appdata
            {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };

            struct v2f
            {
                float2 uv : TEXCOORD0;
                float4 vertex : SV_POSITION;
            };

            Texture2D _YTex;
            Texture2D _UTex;
            Texture2D _VTex;
            SamplerState sampler_YTex;

            v2f vert(appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv = v.uv;
                return o;
            }

            fixed4 frag(v2f i) : SV_Target
            {
                // Formula came from https://docs.microsoft.com/en-us/windows/desktop/medfound/recommended-8-bit-yuv-formats-for-video-rendering.
                fixed c = mad(_YTex.Sample(sampler_YTex, i.uv).r, 1.164383, -0.072774);
                fixed d = _UTex.Sample(sampler_YTex, i.uv).r - 0.5;
                fixed e = _VTex.Sample(sampler_YTex, i.uv).r - 0.5;

                return fixed4(mad(1.596027, e, c), mad(-0.812968, e, mad(-0.391762, d, c)), mad(2.017232, d, c), 1.0);
            }
            ENDCG
        }
    }
}
