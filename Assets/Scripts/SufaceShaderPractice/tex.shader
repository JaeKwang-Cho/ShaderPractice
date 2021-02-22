Shader "Custom/tex"
{
    // 텍스쳐 1장만으로 albedo를 출력
    Properties
    {
        // Albedo 텍스쳐를 받는데, Alpha를 사용하지 않고 RGB 채널만 이용하겠다는 뜻.
        // 앞의 "white"는 default 값이다. {}에 아무것도 없으면 흰색을 쓰라는 것
        _MainTex ("Albedo (RGB)", 2D) = "white" {}
        // 텍스쳐 한장을 더 받아서, lerp 선형 보간으로 합쳐볼 것이다.
        _MainTex2 ("Albedo (RGB)",2D)="white"{}
        // tool에서 입력을 받아도 될것이다.
        _lerpTest("lerp기능 테스트",Range(0,1))=0
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        CGPROGRAM
        // Physically based Standard lighting model, and enable shadows on all light types
        #pragma surface surf Standard fullforwardshadows

        // 아직 uv 좌표계를 만나지 않았기 때문에,
        // float4 라고 선언하지 않고, sampler2D라고 선언한다.
        sampler2D _MainTex;
        sampler2D _MainTex2;
        float _lerpTest;

        // 이후에 Input 구조체에서 uv로 받아온다.
        struct Input
        {
            // Input 구조체 안에 넣어야, vertex 내부 정보를 엔진으로 부터 얻을 수 있다.
            float2 uv_MainTex;
            float2 uv_MainTex2;
        };


        void surf (Input IN, inout SurfaceOutputStandard o)
        {
            // 텍스쳐를 연산하는 text2D 함수에, 인자로 넣는다.
            fixed4 c = tex2D (_MainTex, IN.uv_MainTex); // 1. sampler 2. UV
            fixed4 d = tex2D (_MainTex2, IN.uv_MainTex2);
            // 흑백으로 만드는 것은
            // RGB 값이 다 똑같고, 그 평균값이 대신한다는 것
            //o.Albedo = (c.r + c.g + c.b) / 3;

            // lerp 기능을 사용하여 두 텍스쳐를 선형보간으로 합친다.
            // 뒤 float 값은, 두 텍스쳐 중 어떤것을 더 많이 보여줄지 선택하는 것이다.
            o.Albedo = lerp(c.rgb, d.rgb, _lerpTest);

            // 이렇게 알파값을 이용해서, 물 자국이나 발자국 같은 마크를 그릴 수 있다.
            //o.Albedo = lerp(c.rgb, d.rgb, c.a);
            o.Alpha = c.a;
        }
        ENDCG
    }
    FallBack "Diffuse"
}
