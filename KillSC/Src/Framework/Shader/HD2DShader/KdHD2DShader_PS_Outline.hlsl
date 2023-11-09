#include "inc_KdHD2DShader.hlsli"
#include "../inc_KdCommon.hlsli"

//==================================
// ピクセルシェーダー　１つ１つのピクセルがやってくる
//==================================
float4 main(VSOutput In) : SV_Target0
{
    // 単色で返す
    return float4(g_outLineColor.r, g_outLineColor.g, g_outLineColor.b, 1);
}