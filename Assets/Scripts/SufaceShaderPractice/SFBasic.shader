Shader "SFBasic/NewSurfaceShader"
{
    Properties // Property, 툴에서 인터페이스를 제공하고, 툴에서 변경할 수 있는 부분
    {
        // _Name 이렇게 언더바를 붙이는 것은 일종의 어노테이션 같은 것.
        _Brightness("Brightness",Range(0,1)) = 0.5
        // Range로 툴에서 범위내의 float 값을 얻을 수 있다.
        _FloatValue("FloatValue",Float) = number
        // 그냥 float 값을 입력받는다
        _Color ("Color", Color) = (1,1,1,1)
        // Color 값을 입력 받는다
        _MainTex ("Albedo (RGB)", 2D) = "white" {}
        // 2D 텍스쳐를 받는다.
        _VectorValue("Vector Value", Vector)=(number,number,number,number)
        //Vector 값을 받는다.
        _Glossiness ("Smoothness", Range(0,1)) = 0.5
        _Metallic ("Metallic", Range(0,1)) = 0.0
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        LOD 200

        CGPROGRAM
        // Physically based Standard lighting model, and enable shadows on all light types
        #pragma surface surf Standard fullforwardshadows

        // Use shader model 3.0 target, to get nicer looking lighting
        #pragma target 3.0

        sampler2D _MainTex;

        struct Input
        {
            float2 uv_MainTex;
        };

        half _Glossiness;
        half _Metallic;
        fixed4 _Color;

        // Add instancing support for this shader. You need to check 'Enable Instancing' on materials that use the shader.
        // See https://docs.unity3d.com/Manual/GPUInstancing.html for more information about instancing.
        // #pragma instancing_options assumeuniformscaling
        UNITY_INSTANCING_BUFFER_START(Props)
            // put more per-instance properties here
        UNITY_INSTANCING_BUFFER_END(Props)
            // GPU Instantiating을 해주는 것이다. 없어도 무방한 코드

        void surf (Input IN, inout SurfaceOutputStandard o)
        {
            // Albedo comes from a texture tinted by color
            fixed4 c = tex2D (_MainTex, IN.uv_MainTex) * _Color;
            o.Albedo = c.rgb;
            // Metallic and smoothness come from slider variables
            o.Metallic = _Metallic;
            o.Smoothness = _Glossiness;
            o.Alpha = c.a;
        }
        ENDCG
    }
    FallBack "Diffuse"
}
