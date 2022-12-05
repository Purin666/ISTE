#include "../Common.hlsli"

cbuffer ObjectBuffer : register(b2)
{
    float4x4 myModelTransform;
    
    float2 myUVStart;
    float2 myUVEnd;
    float2 myUVOffset;
    float2 myUVScale;
    
    float4 myColor;
};

float2 Rotate(float2 aVec, float aRadian)
{
    float2 result = aVec;
    
    result.x = aVec.x * cos(aRadian) - aVec.y * sin(aRadian);
    result.y = aVec.x * sin(aRadian) + aVec.y * cos(aRadian);
    
    return result;
}

ColorAndEntityOutput main(PixelInputType input)
{
    float2 uvPos = input.myPosition.xy / myResolution.xy;
    
    // wiggle around bottom center
    float time = sin(myTimings.y) * 0.1f;
    float2 offset = float2(0.5f, 1.f);
    float2 uv = input.myUvCoordinates.xy - offset.xy;
    float cosine = cos(time);
    float sine = sin(time);
    float2 uvDif = float2(uv.x * cosine + uv.y * sine, uv.x * sine - uv.y * cosine) + offset;

    float4 diffuse = AlbedoTex.Sample(DefaultSampler, input.myUvCoordinates.xy).rgba;
    
    float3 toEye = myCamPosition.xyz - input.myWorldPosition.xyz;
    
    // fade out towards camera:
	float cameraDistance = dot(toEye, normalize(input.myNormal));
    float scaledDistance = cameraDistance / 50.f;
	scaledDistance = max(0.f, scaledDistance);
	float closeToCameraFade = scaledDistance / (1.f + scaledDistance);

    // fade out towards background:
    float3 backgroundWorldPosition = GBufferPosition.Sample(DefaultSampler, uvPos).rgb;
    float backgroundDistance = length(backgroundWorldPosition - input.myWorldPosition.xyz);
	float scaledBackgroundDistance = backgroundDistance / 30.f;
	scaledBackgroundDistance = max(0.f, scaledBackgroundDistance);
	float closeToBackgroundFade = scaledBackgroundDistance / (0.25f + scaledBackgroundDistance) * 10.f;
    
    //return float4(myColor.rgb, 1.f); // debugging 
    float3 color = diffuse.rgb * myColor.rgb;
    float alpha = 0.7f * diffuse.a * closeToCameraFade * closeToBackgroundFade;
    alpha = alpha * myColor.a;
    
    ColorAndEntityOutput outdata;
    outdata.myAlbedo = float4(color, alpha);
    outdata.myEntities = 0;
    return outdata;
}