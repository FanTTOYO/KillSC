#include "KdDebugWireFrame.h"

void KdDebugWireFrame::AddDebugLine(const Math::Vector3& start, const Math::Vector3& end, const Math::Color& col)
{
	// デバッグラインの始点
	KdPolygon::Vertex v1;
	v1.color = col.RGBA().v;
	v1.UV = Math::Vector2::Zero;
	v1.pos = start;

	// デバッグラインの終点
	KdPolygon::Vertex v2;
	v2.color = col.RGBA().v;
	v2.UV = Math::Vector2::Zero;
	v2.pos = end;

	m_debugVertices.push_back(v1);
	m_debugVertices.push_back(v2);
}

void KdDebugWireFrame::AddDebugLine(const Math::Vector3& start, const Math::Vector3& dir, float length, const Math::Color& col)
{
	// デバッグラインの始点
	KdPolygon::Vertex v1;
	v1.color = col.RGBA().v;
	v1.UV = Math::Vector2::Zero;
	v1.pos = start;

	// デバッグラインの終点
	KdPolygon::Vertex v2;
	v2.color = col.RGBA().v;
	v2.UV = Math::Vector2::Zero;
	v2.pos = v1.pos + (dir * length);

	m_debugVertices.push_back(v1);
	m_debugVertices.push_back(v2);
}

void KdDebugWireFrame::AddDebugLineFromMatrix(const Math::Matrix& mat, float scale)
{
	Math::Vector3 start = mat.Translation();
	Math::Vector3 vAxis;

	// X軸描画
	vAxis = mat.Right();
	vAxis.Normalize();
	AddDebugLine(start, start + vAxis * scale, kRedColor);

	// Y軸描画
	vAxis = mat.Up();
	vAxis.Normalize();
	AddDebugLine(start, start + vAxis * scale, kGreenColor);

	// Z軸描画
	vAxis = mat.Backward();
	vAxis.Normalize();
	AddDebugLine(start, start + vAxis * scale, kBlueColor);
}

// デバッグスフィアの描画
void KdDebugWireFrame::AddDebugSphere(const Math::Vector3& pos, float radius, const Math::Color& col)
{
	KdPolygon::Vertex v;
	v.UV = Math::Vector2::Zero;
	v.color = col.RGBA().v;

	int detail = 16;
	for (int i = 0; i < detail + 1; ++i)
	{
		// XZ面
		v.pos = pos;
		v.pos.x += cos((float)i * (360 / detail) * KdToRadians) * radius;
		v.pos.z += sin((float)i * (360 / detail) * KdToRadians) * radius;
		m_debugVertices.push_back(v);

		v.pos = pos;
		v.pos.x += cos((float)(i + 1) * (360 / detail) * KdToRadians) * radius;
		v.pos.z += sin((float)(i + 1) * (360 / detail) * KdToRadians) * radius;
		m_debugVertices.push_back(v);

		// XY面
		v.pos = pos;
		v.pos.x += cos((float)i * (360 / detail) * KdToRadians) * radius;
		v.pos.y += sin((float)i * (360 / detail) * KdToRadians) * radius;
		m_debugVertices.push_back(v);

		v.pos = pos;
		v.pos.x += cos((float)(i + 1) * (360 / detail) * KdToRadians) * radius;
		v.pos.y += sin((float)(i + 1) * (360 / detail) * KdToRadians) * radius;
		m_debugVertices.push_back(v);

		// YZ面
		v.pos = pos;
		v.pos.y += cos((float)i * (360 / detail) * KdToRadians) * radius;
		v.pos.z += sin((float)i * (360 / detail) * KdToRadians) * radius;
		m_debugVertices.push_back(v);

		v.pos = pos;
		v.pos.y += cos((float)(i + 1) * (360 / detail) * KdToRadians) * radius;
		v.pos.z += sin((float)(i + 1) * (360 / detail) * KdToRadians) * radius;
		m_debugVertices.push_back(v);
	}
}

void KdDebugWireFrame::Draw()
{
	if (m_debugVertices.size() < 2) { return; }

	KdShaderManager::Instance().m_HD2DShader.DrawVertices(m_debugVertices, Math::Matrix::Identity);

	m_debugVertices.clear();
}

void KdDebugWireFrame::Release()
{
	m_debugVertices.clear();
}
