#include "Segment_Tree.h"
#include <io.h>


ImU32 Segment_Tree::ContrastingColor(ImU32)
{
	return ImU32();
}

void Segment_Tree::graphUpdate()
{
}

float Segment_Tree::calcDist(float, float, float, float)
{
	return 0.0f;
}

void Segment_Tree::controlsUpdate()
{
}

void Segment_Tree::drawEdge(ImDrawList*, int, int)
{
}
Segment_Tree::Vertex Segment_Tree::Merge(Vertex a, Vertex b)
{
	Vertex res;
	res.minimum = std::min(a.minimum, b.minimum);
	res.maximum = std::max(a.maximum, b.maximum);
	//res.gcd = __gcd(a.gcd, b.gcd);
	res.sum = a.sum + b.sum;
	res.xr = (a.xr ^ b.xr);
	return res;
}
Segment_Tree::Vertex Segment_Tree::Query(int node, int s, int e, int l, int r)
{
	if (l > e || s > r)
		return Neutral;
	if (s >= l && e <= r)
		return Tree[node];
	int md = (s + e) >> 1;
	Vertex shmal = Query(node * 2, s, md, l, r);
	Vertex Yemen = Query(node * 2 + 1, md + 1, e, l, r);
	return Merge(shmal, Yemen);
}

Segment_Tree::Vertex Segment_Tree::Query(int l, int r)
{
	return Query(l - 1, r - 1);
}

Segment_Tree::Vertex Segment_Tree::Query(int index)
{
	return Query(index - 1, index - 1);
}

Segment_Tree::Vertex Segment_Tree::Single(int value)
{
	Vertex ret;
	ret.xr = value;
	ret.gcd = value;
	ret.sum = value;
	ret.minimum = value;
	ret.maximum = value;
	return ret;
}

void Segment_Tree::update(int node, int s, int e, int indx, int val)
{
	if (s == e)
		return Tree[node] = Single(val), void();
	int md = (s + e) / 2;
	if (indx > md)
		update(node * 2 + 1, md + 1, e, indx, val);
	else
		update(node * 2, s, md, indx, val);
	Tree[node] = Merge(Tree[node * 2], Tree[node * 2 + 1]);
}

void Segment_Tree::build(int, int, int)
{
}

void Segment_Tree::clear()
{
}

Segment_Tree::Segment_Tree(std::string name, int& state, float& GuiScale, bool& settingsEnabled, int& colorMode)
	: GrandWindow(name, state, GuiScale, settingsEnabled, colorMode)
{
	//io = &ImGui::GetIO(); (void)io;
}

Segment_Tree::~Segment_Tree()
{
}

void Segment_Tree::update()
{
}