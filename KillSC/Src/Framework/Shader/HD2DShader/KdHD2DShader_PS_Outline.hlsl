#include "inc_KdHD2DShader.hlsli"
#include "../inc_KdCommon.hlsli"

//==================================
// �s�N�Z���V�F�[�_�[�@�P�P�̃s�N�Z��������Ă���
//==================================
float4 main(VSOutput In) : SV_Target0
{
    // �P�F�ŕԂ�
    return float4(g_outLineColor.r, g_outLineColor.g, g_outLineColor.b, 1);
}