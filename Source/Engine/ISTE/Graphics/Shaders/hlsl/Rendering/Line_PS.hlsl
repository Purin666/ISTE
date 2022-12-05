struct LineVertexToPixel
{
    float4 myPosition : SV_POSITION;
    float3 myColor : COLOR;
};

float4 main(LineVertexToPixel anInput) : SV_TARGET
{
    return float4(anInput.myColor, 1.0f);
}