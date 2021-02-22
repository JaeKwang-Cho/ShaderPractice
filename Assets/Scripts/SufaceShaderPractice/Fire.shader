Shader "Custom/Fire"
{
    Properties
    {
        _MainTex ("Albedo (RGB)", 2D) = "white" {}
        // 이미지를 한번 더 받아본다
        _MainTex2 ("Albedo (RGB)", 2D) = "black"{}
    }
    SubShader
    {
        // 제대로 투명한 Alpha를 하기 위해서
        Tags { "RenderType"="Transparent" "Queue"="Transparent" }
        // 이렇게 바꿔주고
        LOD 200

        CGPROGRAM
        // 이렇게 전처리를 추가해준다.
        #pragma surface surf Standard alpha:fade
        // 그러면 alpha 값을 제대로 해서 반투명한 불이 된다.

        sampler2D _MainTex;
        sampler2D _MainTex2;

        struct Input
        {
            float2 uv_MainTex;
            float2 uv_MainTex2;
        };
        void surf (Input IN, inout SurfaceOutputStandard o)
        {
            // Albedo comes from a texture tinted by color
            fixed4 c = tex2D (_MainTex, IN.uv_MainTex);
            fixed4 d = tex2D(_MainTex2, IN.uv_MainTex2 - float2(0,_Time.y));
            // 일단 빛의 영향을 받지 않게 Emission으로
            o.Emission = c.rgb*d.rgb;

            // 원본 tga 파일에 alpha 값이 있는데, 제대로 투명함이 보이지 않는다.
            // 그러기 위해서는 저 위의 Tags 부분을
            o.Alpha = c.a*d.a;
            // 알파값이 살아있는 부분만 살아있고, 나머지 올라가는 연산과 적당히 곱해져서 타오르는 것처럼 보인다.
        }
        ENDCG
    }
    FallBack "Diffuse"
}
