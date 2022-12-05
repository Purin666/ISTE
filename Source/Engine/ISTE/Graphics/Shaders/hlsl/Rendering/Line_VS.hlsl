#include "../Common.hlsli"

//maybe smarter to just remap the vertecies instead
struct LineVertexInput
{
    float3 myPosition    : POSITION;
    float3 myColor       : COLOR; 
};

struct LineVertexToPixel 
{
    float4 myPosition    : SV_POSITION;
    float3 myColor       : COLOR;
};

LineVertexToPixel main(LineVertexInput anInput)
{
    LineVertexToPixel output;
    output.myPosition = float4(anInput.myPosition, 1);
    output.myPosition = mul(myWorldToClipMatrix, output.myPosition);
    
    output.myColor = anInput.myColor;
    return output;
}