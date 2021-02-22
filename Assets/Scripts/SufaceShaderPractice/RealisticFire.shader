Shader "Custom/RealisticFire"
{
    Properties
    {
        _MainTex ("Albedo (RGB)", 2D) = "black" {}
        _MainTex2("Albedo (RGB)", 2D) = "black" {}
        _Tilt("Tilt",Range(3,10))=3
    }
    SubShader
    {
        Tags { "RenderType"="TransParent" "Queue"="Transparent" }
        LOD 200

        CGPROGRAM
        #pragma surface surf Standard alpha:fade
        sampler2D _MainTex;
        sampler2D _MainTex2;
        float _Tilt;

        struct Input
        {
            float2 uv_MainTex;
            float2 uv_MainTex2;
        };

        void surf (Input IN, inout SurfaceOutputStandard o)
        {
            fixed4 d = tex2D(_MainTex2, IN.uv_MainTex2 + float2(0, -_Time.y));
            // 만약 d에 들어가는 텍스쳐가
            // 좌표마다 색깔이 다른, 텍스쳐라면?
            fixed4 c = tex2D (_MainTex, IN.uv_MainTex + d.r* _Tilt-float2(0,0.1));
            // 위 코드는 좌표마다 다르게 움직일 것이다.
            // 그러니 저 구기는 이미지를 움직이면 될지도?
            o.Albedo = c.rgb;
            o.Alpha = c.a;
        }
        ENDCG
    }
    FallBack "Diffuse"
}
