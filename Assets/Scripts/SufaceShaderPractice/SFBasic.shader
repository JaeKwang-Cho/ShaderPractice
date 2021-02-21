Shader "SFBasic/NewSurfaceShader"
{
    Properties // Property, 툴에서 인터페이스를 제공하고, 툴에서 변경할 수 있는 부분
    {
        // _Name 이렇게 언더바를 붙이는 것은 일종의 어노테이션 같은 것.
        _Brightness("Brightness",Range(-1,1)) = 0.5
        // Range로 툴에서 범위내의 float 값을 얻을 수 있다.
        _FloatValue("FloatValue",Float) = 0.5
        // 그냥 float 값을 입력받는다
        _Color ("Color", Color) = (1,1,1,1)
        // Color 값을 입력 받는다
        _MainTex ("Albedo (RGB)", 2D) = "white" {}
        // 2D 텍스쳐를 받는다.
        _VectorValue("Vector Value", Vector)=(1,1,1,1)
        //Vector 값을 받는다.
        _Glossiness ("Smoothness", Range(0,1)) = 0.5
        _Metallic ("Metallic", Range(0,1)) = 0.0
        _Red ("Red",Range(0,1))=0
        _Green("Green",Range(0,1))=0
        _Blue("Blue",Range(0,1))=0
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        LOD 200

        CGPROGRAM
        // 1. Physically based Standard lighting model, and enable shadows on all light types
            // 전처리 혹은 스니핏(snippet)이라고 부른다.
            // 쉐이더의 조명계산이나 새부적인 분기를 다루는 부분이다.
        #pragma surface surf Standard fullforwardshadows 
            // 주변에 영향을 받지않는 완전 검정색을 만들고 싶으면, Reflection Source를 Custom으로 하고
            // pragam  맨 뒤에 noambient 키워드를 추가하면 된다.

        // Use shader model 3.0 target, to get nicer looking lighting
        #pragma target 3.0
            // 나중에 자세히 다룬다.

        sampler2D _MainTex;

        // 2.구조체, 엔진으로 부터 받아와야 할 데이터들이 들어간다.
        struct Input
        {
            float2 uv_MainTex;
        };

        half _Glossiness;
        half _Metallic;
        fixed4 _Color;
        float _Red;
        float _Green;
        float _Blue;
        float _Brightness;

        // 3. Add instancing support for this shader. You need to check 'Enable Instancing' on materials that use the shader.
        // See https://docs.unity3d.com/Manual/GPUInstancing.html for more information about instancing.
        // #pragma instancing_options assumeuniformscaling
        UNITY_INSTANCING_BUFFER_START(Props)
            // put more per-instance properties here
        UNITY_INSTANCING_BUFFER_END(Props)
            // GPU Instantiating을 해주는 것이다. 없어도 무방한 코드

            // 4. 함수 영역이다. 색상이나 이미지가 출력되는 부분을 만들 수 있다.
            // 위에있는 Input 구조체와, SurfaceOutputStandard 구조체를 인자로 받는다.
            // Albedo, Normal, Emission, Metallic, Smoothness, Occlusion, Alpha를 맴버로 가진다. 
            // 데이터 크기는 float, half, fixed로 나뉘는데 (점점 작아지고) 잘 모르겠다 싶으면 제일 큰 float으로 박아두어라
        void surf (Input IN, inout SurfaceOutputStandard o)
        {
            // Albedo comes from a texture tinted by color
            fixed4 c = tex2D (_MainTex, IN.uv_MainTex) * _Color; 
            // o.Albedo = _Color.rgb; // property에서 넘어온 Albedo 값을 이용
            o.Albedo = float3(_Red, _Green, _Blue); // Tool에서 rgb 값을 선택할 수 있게 해줌
            // o.Albedo = float3(1, 0, 0); // 조명연산이 들어간다.
            // o.Emission = float3(1, 0, 0); // 조명연산이 들어가지 않는다.
            // o.Emission = float3(0.5,0.5,0.5)*float(0.5,0.5,0.5) // 이렇게 연산이 가능하다.
            // Metallic and smoothness come from slider variables

            // float3 rgbTest = float3(1, 0.5, 0);
            // 앞에부터 r, g, b이다. 즉 rgbTest.r은 1, rgbTest.g = 0.5, rgbTest.b = 0인 것이다.
            // 그렇다면 rgbTest.gbr 는 (0.5,0,1)이 되는것이다.

            o.Albedo += _Brightness; // 밝기조절은 이렇게, float 값을 알베도에 더해주면된다.

            o.Metallic = _Metallic;
            o.Smoothness = _Glossiness;
            o.Alpha = c.a;
        }
        ENDCG
    }
    FallBack "Diffuse"
}
