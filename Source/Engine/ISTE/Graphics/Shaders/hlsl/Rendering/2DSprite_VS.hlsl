#include "../Common.hlsli"

cbuffer ObjectBuffer : register(b2)
{
    //float2x2 my2DRotScaleMatris;
    float2 myPosition;
    float2 myScale;
    float1 myRotation;
    float1 garb;
    float2 myPivot;
    
    float2 myUVStart;
    float2 myUVEnd;
    float2 myUVOffset;
    float2 myUVScale; 
    
    float4 myColor;
};  

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    output.myPosition = 0;
    output.myPosition.w = 1;
    
    //should be moved out
    float2x2 spriteScaleRot = 0;
    spriteScaleRot[0].x = myScale.x;
    spriteScaleRot[1].y = myScale.y;
    
    spriteScaleRot[0].x *= cos(myRotation);
    spriteScaleRot[0].y *= -sin(myRotation);
    spriteScaleRot[1].x *= sin(myRotation);
    spriteScaleRot[1].y *= cos(myRotation);
    

    
    float4 pivotOffset = 0;
    pivotOffset.xy = mul(myPivot, spriteScaleRot).xy;
    
    float4 position = 0;
           position = float4(myPosition.xy + pivotOffset.xy,0, 1);
    output.myWorldPosition.xy = 0;
    
    float4x4 spriteTransform = 0;
    spriteTransform[0] = float4(spriteScaleRot[0], 0, 0);
    spriteTransform[1] = float4(spriteScaleRot[1], 0, 0);
    spriteTransform[2] = float4(0, 0, 1, 0);
    spriteTransform[3] = float4(position.xy, 0, 1);
    
    output.myWorldPosition.xy = output.myPosition.xy = mul(float4(input.myPosition.xyz,1), spriteTransform);
    output.myPosition = mul(my2DWorldToClipMatrix, output.myPosition);
    
    //manual 2d transform
 
    output.myNormal = input.myNormal;
    output.myTangent = input.myTangent;
    output.myBiNormal = input.myBiNormal; 
    output.myColor = input.myColor;  
    output.myUvCoordinates = GetUVRect(float4(myUVStart, myUVEnd), input.myVertexIndex);
    output.myUvCoordinates *= myUVScale;
    output.myUvCoordinates += myUVOffset;
    return output;
}