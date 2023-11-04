#include "inc_KdHD2DShader.hlsli"
#include "../inc_KdCommon.hlsli"

//==================================
// ピクセルシェーダー　１つ１つのピクセルがやってくる
//==================================
float4 main(VSOutput In) : SV_Target0
{
    // 単色で返す
    return float4(1, 0, 0, 1);
}