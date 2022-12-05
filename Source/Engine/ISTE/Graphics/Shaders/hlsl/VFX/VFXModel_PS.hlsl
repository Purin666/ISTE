#include "../Common.hlsli"

cbuffer ObjectBuffer : register(b3)
{
    float4 myColor;
    
    float2 myUvChangeOverTime0;
    float2 myUvChangeOverTime1;
    float2 myUvChangeOverTime2;
    float2 _g0;
    
    float3 mySample0;
    float _g1;
    float3 mySample1;
    float _g2;
    float3 mySample2;
    float _g3;
    
    float myElaspedTime;
    float myDuration;
    float myDistToCamera;
    float _g4;
};

ColorAndEntityOutput main(VFXPixelInputType input)
{    
    float2 uv0 = input.myUvCoordinates.xy;
    float2 uv1 = input.myUvCoordinates.xy;
    float2 uv2 = input.myUvCoordinates.xy;
    
    // sample gradient before uv is tweaked
    float alpha0 = AlbedoTex.Sample(DefaultSampler, uv0).a;
    float alpha1 = NormalTex.Sample(DefaultSampler, uv1).a;
    float alpha2 = MaterialTex.Sample(DefaultSampler, uv2).a;
    
    // fade towards top, use uv before it's tweaked
    //float heightFade = log10(uv.y + 1) * 3.1415f * sin(uv.y);
    
    // pan uv
    uv0.x += myUvChangeOverTime0.x * myElaspedTime;
    uv0.y += myUvChangeOverTime0.y * myElaspedTime;
    uv1.x += myUvChangeOverTime1.x * myElaspedTime;
    uv1.y += myUvChangeOverTime1.y * myElaspedTime;
    uv2.x += myUvChangeOverTime2.x * myElaspedTime;
    uv2.y += myUvChangeOverTime2.y * myElaspedTime;

    // sample channels
    float3 tex0 = AlbedoTex.Sample(DefaultSampler, uv0).rgb * mySample0;
    float3 tex1 = NormalTex.Sample(DefaultSampler, uv1).rgb * mySample1;
    float3 tex2 = MaterialTex.Sample(DefaultSampler, uv2).rgb * mySample2;
        
    // fade towards background:
    float2 uvPos = input.myPosition.xy / myResolution.xy;
    float3 backgroundWorldPosition = GBufferPosition.Sample(DefaultSampler, uvPos).rgb;
    float backgroundDistance = length(backgroundWorldPosition - input.myWorldPosition.xyz);
    float scaledBackgroundDistance = backgroundDistance / 3.f;
    scaledBackgroundDistance = max(0.f, scaledBackgroundDistance);
    float closeToBackgroundFade = scaledBackgroundDistance / (1.f + scaledBackgroundDistance);

    // fade towards camera
    float cameraFade = clamp(length(input.myWorldPosition - myCamPosition) - 2.f, 0, 1);
    
    // calc alpha
    alpha0 = (min(1.f, 7.f * length(tex0.rgb))) * cameraFade * closeToBackgroundFade * alpha0;
    alpha1 = (min(1.f, 7.f * length(tex1.rgb))) * cameraFade * closeToBackgroundFade * alpha1;
    alpha2 = (min(1.f, 7.f * length(tex2.rgb))) * cameraFade * closeToBackgroundFade * alpha2;
    
    float3 color0 = (myColor.rgb * tex0.r) + (myColor.rgb * tex0.g) + (myColor.rgb * tex0.b);
    float3 color1 = (myColor.rgb * tex1.r) + (myColor.rgb * tex1.g) + (myColor.rgb * tex1.b);
    float3 color2 = (myColor.rgb * tex2.r) + (myColor.rgb * tex2.g) + (myColor.rgb * tex2.b);
    
    float3 colorResult = color0 + color1 + color2;
    float alphaResult = alpha0 + alpha1 + alpha2 + myColor.a;
    
    ColorAndEntityOutput outdata;
    outdata.myAlbedo = float4(colorResult, alphaResult);
    outdata.myEntities = 0;
    return outdata;
}