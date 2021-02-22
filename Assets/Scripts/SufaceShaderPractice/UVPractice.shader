Shader "Custom/UVPractice"
{
    Properties
    {
        _MainTex ("Albedo (RGB)", 2D) = "white" {}
        _UVMover("UVMover",Range(0,1))=0
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        LOD 200

        CGPROGRAM
        #pragma surface surf Standard fullforwardshadows
        sampler2D _MainTex;
        float _UVMover;

        struct Input
        {
            float2 uv_MainTex;
        };


        void surf (Input IN, inout SurfaceOutputStandard o)
        {
            // UV에 숫자를 더하거나 곱하면 어떻게 될까?
            // UV는 좌표이기 떄문에, 그냥 이동한다.
            fixed4 c = tex2D(_MainTex, IN.uv_MainTex+_UVMover);
            o.Albedo = c.rgb;

            // x가 u이고, z가 v임을 직접 확인하려면 아래처럼 해보자
            //o.Emission = float3(IN.uv_MainTex.x,IN.uv_MainTex.y,0);
            // 점차 증가하는 x의 값을 Emission으로 지정한 것이다.
            o.Alpha = c.a;
        }
        ENDCG
    }
    FallBack "Diffuse"
}
