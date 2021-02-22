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
            // 그리고 여기에 시간까지 더해보자, x방향만 움직이게 해보자
            fixed4 c = tex2D(_MainTex, IN.uv_MainTex+_UVMover+float2(_Time.y,0));
            // 응용을 하려면 물을 흐르게 할 수 있을 것
            
            // _Time은 씬이 열린 다음부터의 시간을 얻을 수 있는 유니티 제공기능이다.
            // x,y,z,w 순으로 1/20배, 1배, 2배, 3배로 빨리간다.
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
