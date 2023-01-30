/* OpenSceneGraph example, osgpick.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

/* osgpick sample
* demonstrate use of osgUtil/PickVisitor for picking in a HUD or
* in a 3d scene,
*/
#include <osgFX/Scribe>  
#include <osg/PolygonOffset>
#include <osgUtil/Optimizer>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>

#include <osgGA/TerrainManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>

#include <osg/Material>
#include <osg/Geode>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Projection>
#include <osg/MatrixTransform>
#include <osg/Camera>
#include <osg/io_utils>
#include <osg/ShapeDrawable>

#include <osgText/Text>

#include <sstream>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgDB/WriteFile>
#include<string.h>
#include <windows.h>

#include "C:\Users\25121\Desktop\PositionVisitor.h"

#include "C:\Users\25121\Desktop\Utility.h"

using namespace std;

osg::ref_ptr<osg::Node> createGeodes(osg::ref_ptr<osg::Vec3Array> vArr)
{
	//创建一个用于保存几何信息的图像
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	//向Geode类添加几何体（Drawable）
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geom.get());
	//创建3个顶点的数组
	geom->setVertexArray(vArr.get());
	//由保存的数据绘制三角形
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, 3));
	//为颜色创建数组
	osg::ref_ptr<osg::Vec4Array> cArr = new osg::Vec4Array;
	cArr->push_back(osg::Vec4(1.0, 1.0, 1.0, 1.0));
	geom->setColorArray(cArr.get());
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	return geode.get();
}
// class to handle events with a pick
class PickHandler : public osgGA::GUIEventHandler {
public:

	PickHandler(osgText::Text* updateText, osg::Group* root, osg::Vec3Array *vArr) :
		_updateText(updateText), mRoot(root), mvArr(vArr) {}

	~PickHandler() {}

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	virtual void pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);

	void setLabel(const std::string& name)
	{
		if (_updateText.get()) {
			_updateText->setText(name);

		}

	}

protected:

	osg::ref_ptr<osgText::Text>  _updateText;
	osg::ref_ptr<osg::Group> mRoot;
	osg::ref_ptr<osg::Vec3Array> mvArr;
};

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	switch (ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::PUSH):
	{
		osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
		if (view) pick(view, ea);
		return false;
	}
	case(osgGA::GUIEventAdapter::KEYDOWN):
	{
		if (ea.getKey() == 'c')
		{
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			osg::ref_ptr<osgGA::GUIEventAdapter> event = new osgGA::GUIEventAdapter(ea);
			event->setX((ea.getXmin() + ea.getXmax())*0.5);
			event->setY((ea.getYmin() + ea.getYmax())*0.5);
			if (view) pick(view, *event);
		}
		return false;
	}
	default:
		return false;
	}
}

void PickHandler::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea)
{
	osgUtil::LineSegmentIntersector::Intersections intersections;

	std::string gdlist = "";

	osg::ref_ptr<osg::Vec3Array> vArr1 = new osg::Vec3Array;
	if (view->computeIntersections(ea, intersections))
	{
		osgUtil::LineSegmentIntersector::Intersections::iterator hitr1 = intersections.begin();
		std::ostringstream os;
		if (!hitr1->nodePath.empty() && !(hitr1->nodePath.back()->getName().empty()))
		{
			// the geodes are identified by name.
			os << "Object \"" << hitr1->nodePath.back()->getName() << "\"" << std::endl;
		}
		else if (hitr1->drawable.valid())
		{
			const osg::NodePath& np = hitr1->nodePath;
			for (int i = np.size() - 1; i >= 0; --i) {
				osgFX::Scribe *sc1 = dynamic_cast<osgFX::Scribe*>(np[i]);
				if (sc1 != NULL) {
					os << "Object \"" << sc1->getName() << "\"" << std::endl;
				}
			}
			os << "Object \"" << hitr1->drawable->className() << "\"" << std::endl;
		}

		os << "        local coords vertex(" << hitr1->getLocalIntersectPoint() << ")" << "  normal(" << hitr1->getLocalIntersectNormal() << ")" << std::endl;
		os << "        world coords vertex(" << hitr1->getWorldIntersectPoint() << ")" << "  normal(" << hitr1->getWorldIntersectNormal() << ")" << std::endl;
		const osgUtil::LineSegmentIntersector::Intersection::IndexList& vil = hitr1->indexList;

		for (unsigned int i = 0; i < vil.size(); ++i)
		{

			os << "        vertex indices [" << i << "] = " << vil[i] << std::endl;
			osg::Vec3Array::size_type p = 1;
			float x = mvArr->at(p).x();
			float y = mvArr->at(p).y();
			float z = mvArr->at(p).z();
			vArr1->push_back(osg::Vec3d(x, y, z));
		}
		gdlist += os.str();
		mRoot->addChild(createGeodes(vArr1).get());
	}

	setLabel(gdlist);

}

osg::Node* createHUD(osgText::Text* updateText)
{

	// create the hud. derived from osgHud.cpp
	// adds a set of quads, each in a separate Geode - which can be picked individually
	// eg to be used as a menuing/help system!
	// Can pick texts too!

	osg::Camera* hudCamera = new osg::Camera;
	hudCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	hudCamera->setProjectionMatrixAsOrtho2D(0, 1280, 0, 1024);
	hudCamera->setViewMatrix(osg::Matrix::identity());
	hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
	hudCamera->setClearMask(GL_DEPTH_BUFFER_BIT);

	std::string timesFont("fonts/times.ttf");

	// turn lighting off for the text and disable depth test to ensure its always ontop.
	osg::Vec3 position(150.0f, 800.0f, 0.0f);
	osg::Vec3 delta(0.0f, -60.0f, 0.0f);

	{
		osg::Geode* geode = new osg::Geode();
		osg::StateSet* stateset = geode->getOrCreateStateSet();
		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		geode->setName("simple");
		hudCamera->addChild(geode);

		osgText::Text* text = new  osgText::Text;
		geode->addDrawable(text);

		text->setFont(timesFont);
		text->setText("Click on the triangle for information");
		text->setPosition(position);

		position += delta;
	}



	{ // this displays what has been selected
		osg::Geode* geode = new osg::Geode();
		osg::StateSet* stateset = geode->getOrCreateStateSet();
		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		geode->setName("The text label");
		geode->addDrawable(updateText);
		hudCamera->addChild(geode);

		updateText->setCharacterSize(30.0f);
		updateText->setFont(timesFont);
		updateText->setColor(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
		updateText->setText("");
		updateText->setPosition(position);
		updateText->setDataVariance(osg::Object::DYNAMIC);

		position += delta;
	}
	return hudCamera;
}

int main(int argc, char **argv)
{

	// if not loaded assume no arguments passed in, try use default mode instead.
	osg::ref_ptr<osg::Group> root = new osg::Group();
	std::string dataPath = "G:\\演示试点模型";
	dataPath += "\\Data\\";
	osgDB::DirectoryContents dirNames = osgDB::getDirectoryContents(dataPath);
	for (std::string dir : dirNames)
	{
		if (dir.find(".") != std::string::npos)
			continue;
		std::string osgbFile = dataPath + dir + "\\" + dir + ".osgb";
		osg::Node* Node = osgDB::readNodeFile(osgbFile);
		osg::ref_ptr<osg::Node> cow = osgDB::readNodeFile(osgbFile);
		osg::ref_ptr<osgFX::Scribe> sc = new osgFX::Scribe();//添加一个scribe节点，该节点下的模型会被加白描线高亮显示。  
		sc->addChild(cow.get());
		string name = Utility::getFileNameFromPath(osgbFile);
		sc->setName(name);
		root->addChild(sc.get());
		
		//string name = "Tile_+005_+033_L1_0.osgb";
	}
	PositionVisitor visitor = PositionVisitor("666");
	root->accept(visitor);
	osg::ref_ptr<osg::Vec3Array> vArr = new osg::Vec3Array;
	for (int i = 0; i < visitor.allGeom.size(); i++) {
		osg::Vec3 temp1;
		for (int j = 0; j < visitor.allGeom[i]->vertices.size(); j++) {
			temp1[0] = visitor.allGeom[i]->vertices[j]->coor._v[0];
			temp1[1] = visitor.allGeom[i]->vertices[j]->coor._v[1];
			temp1[2] = visitor.allGeom[i]->vertices[j]->coor._v[2];
			vArr->push_back(temp1);
		}

	}

	osg::ref_ptr<osgText::Text> updateText = new osgText::Text;
	// add the HUD subgraph.
	root->addChild(createHUD(updateText.get()));
	osgViewer::Viewer viewer;

	// add the handler for doing the picking
	viewer.addEventHandler(new PickHandler(updateText.get(), root, vArr));
	// set the scene to render
	viewer.setSceneData(root.get());
	//viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));//显示网格
	return viewer.run();
}
