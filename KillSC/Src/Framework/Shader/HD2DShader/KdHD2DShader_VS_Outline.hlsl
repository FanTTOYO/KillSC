#include "inc_KdHD2DShader.hlsli"
#include "../inc_KdCommon.hlsli"

//==================================
// ���_�V�F�[�_�[
//==================================
VSOutput main(float4 pos : POSITION,
	float2 uv : TEXCOORD0, // UV���W
	float3 normal : NORMAL, // �@��
	float4 color : COLOR, // ���_�F
	float3 tangent : TANGENT // �ڐ�
)
{
    VSOutput Out;
	
	// ���_���W���u�@�������v�ɏ������炷
    pos.xyz = pos.xyz + normal * 0.01;
	
	// �u���W�ϊ��v
    Out.Pos = mul(pos, g_mWorld);
    Out.Pos = mul(Out.Pos, g_mView);
    Out.Pos = mul(Out.Pos, g_mProj);
	
    return Out;
}