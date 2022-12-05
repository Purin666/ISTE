#include "../Common.hlsli"

ColorAndEntityOutput main(PixelInputType aPixelInput)
{
    ColorAndEntityOutput outdata;
    outdata.myAlbedo = aPixelInput.myColor.rgba;
    outdata.myEntities = 0;
    return outdata;  
}