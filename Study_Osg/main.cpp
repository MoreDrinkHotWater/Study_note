#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osg/Node>
#include <osgUtil/Optimizer>
#include <osg/Switch>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>

#include <osg/ShapeDrawable>
#include <osg/Shape>

#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osgGA/GUIEventHandler>

#include <osgFX/Scribe>
#include <osgUtil/LineSegmentIntersector>


osg::ref_ptr<osg::Node> createQuad();
osg::ref_ptr<osg::Node> createLine();
osg::ref_ptr<osg::Node> createShape();

// 申明一个类　UseEventHandler　它是由　osgGA::GUIEventHandler　派生而来的拥有处理事件的能力,其中这种能力体现在一个虚函数 handle 上。一切的处理都在 handle 当中。
/*class UseEventHandler:public osgGA::GUIEventHandler
{
public:
    //两个极其重要的参数,一个是 const osgGA::GUIEventAdapter,注意有不少人在
    //写时少写了个 const 结果就不灵了,因为是虚函数,参数必须与父类中虚函数相应一致。该参数是用来
    //识别各种事件类型的参数。第二个参数也很重要:osgGA::GUIActionAdapter,它是控制显示的参数,其
    //实最重要的是它是 Viewer 的祖父类,由它可以得到 viewer,在操作中有体现。
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {

        //由参数二 osgGA::GUIActionAdapter 得到了 viewer。以下这两句非常常用
        osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
        //viewer未得到就返回false
        if (!viewer) return false;

        //判断事件的类型　　Child0　是飞机 Child１　是牛
        // setNodeMask(0) 是隐藏　setNodeMask(1)是显示
        switch (ea.getEventType())
        {
            //如果是有键按下
            case osgGA::GUIEventAdapter::KEYDOWN:
            {
                //如果是键盘左键　显示　Child１　隐藏　Child0
                if (ea .getKey()== 0xFF51)
                {
                    viewer->getSceneData()->asGroup()->getChild(0)->setNodeMask(1) ;
                    viewer->getSceneData()->asGroup()->getChild(1)->setNodeMask(0) ;

                }
                //如果是键盘右键　显示　Child0　隐藏　Child１
                if(ea.getKey()== 0xFF53)
                {
                    viewer ->getSceneData() ->asGroup() ->getChild(0) ->setNodeMask(0) ;
                    viewer ->getSceneData() ->asGroup() ->getChild(1) ->setNodeMask(1) ;
                }

            }
                break;
            //如果是鼠标push如果事件发生在鼠标上就需要判断是哪个键,
            //一般视 1 代表左键,2 代表中键,4 代表右键。

            case osgGA::GUIEventAdapter ::PUSH:
                //如果是鼠标右键单击　则隐藏　　Child0 和 Child1
                if(ea.getButton () == 4)
                {
                    viewer ->getSceneData() ->asGroup() ->getChild(0) ->setNodeMask(0) ;
                    viewer ->getSceneData() ->asGroup() ->getChild(1) ->setNodeMask(0) ;
                }
                break;
            //没实现？？？
            case osgGA::GUIEventAdapter::DOUBLECLICK:
                if(ea.getButton() == 1)
                {
                    viewer ->getSceneData() ->asGroup() ->getChild(0) ->setNodeMask(1) ;
                    viewer ->getSceneData() ->asGroup() ->getChild(1) ->setNodeMask(1) ;
                }
                break;

            default:
                break;
        }
        return false;
    }
};*/


class CPickHandler: public osgGA::GUIEventHandler
{
public:
    //构造函数中有参数　osgViewer::Viewer *　,这样就可以手动把viewer传入类中了
    CPickHandler(osgViewer::Viewer *viewer):mViewer(viewer){}
    virtual  bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
    {
        switch (ea.getEventType())
        {
            //判断事件为左键单击
            case osgGA::GUIEventAdapter::PUSH:
                if(ea.getButton()==1)
                {
                    //把点击屏幕时的X，Y坐标，传入到Pick函数中去
                    Pick(ea.getX(),ea.getY());
                }
                return true;
        }
        return false;
    }

protected:
    void Pick(float x,float y)
    {
        //申请一个相交测试的结果集，判断屏幕与场景相交后，把得出的结果集放入其中
        osgUtil::LineSegmentIntersector::Intersections intersections;
        //测试屏幕与场景是否相交，相交则放入
        if(mViewer->computeIntersections(x, y, intersections))
        {
            //遍历该结果集
            for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr=intersections.begin();hitr!=intersections.end();++hitr)
            {
                // 遍历器的结果不为空 而且 nodePath 也不为空
                if (!hitr->nodePath.empty() && !(hitr->nodePath.back()->getName().empty()))
                {
                    //对 NodePath 进行遍历 找到我们需要的结点
                    osg::NodePath np=hitr->nodePath;
                    for(int i=np.size()-1;i>0;--i)
                    {
                        //如果结果集中有所需要的结点,则设置隐藏该结点。其中有一个动态转换,如果可以转换成功则左值不为 NULL,否则为 NULL。
                        osgFX::Scribe *sc= dynamic_cast<osgFX::Scribe *> (np[i]);
                        if(sc!=NULL)
                        {
                            if (sc->getNodeMask()!=0)
                            {
                                sc->setNodeMask(0);
                            }
                        }
                    }
                }
            }
        }
    }

    osgViewer::Viewer *mViewer;
};

int main() {
    /*
     * test_1
     * 添加单个模型
     * /
    /*osgViewer::Viewer viewer;
    viewer.setSceneData(osgDB::readNodeFile("glider.osg"));
    viewer.realize();
    viewer.run();*/


    /*
     * test_2
     * 添加状态和操作器
     */
    /*osgViewer::Viewer viewer;
    viewer.setSceneData(osgDB::readNodeFile("/home/zhihui/OSGlib/OSG/OpenSceneGraph-Data-master/glider.osg"));
    viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));
    viewer.addEventHandler(new osgViewer::WindowSizeHandler);
    viewer.addEventHandler(new osgViewer::StatsHandler);
    //添加操作器
    {
        osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;
        keyswitchManipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator());
        keyswitchManipulator->addMatrixManipulator( '2', "Flight", new osgGA::FlightManipulator());
        keyswitchManipulator->addMatrixManipulator( '3', "Drive", new osgGA::DriveManipulator());
        keyswitchManipulator->addMatrixManipulator( '4', "Terrain", new osgGA::TerrainManipulator());
        viewer.setCameraManipulator(keyswitchManipulator.get() );
    }
    //添加路径记录
    viewer.addEventHandler(new osgViewer::RecordCameraPathHandler);
    viewer.realize();
    viewer.run();*/


    /*
     * test_3
     * 添加多个模型（简化写法）
     */
    /*osgViewer::Viewer viewer;
    //申请一个root结点
    osg::ref_ptr<osg::Group> root=new osg::Group();
    //向结点中加入两个子结点
    root->addChild(osgDB::readNodeFile("/home/zhihui/OSGlib/OSG/OpenSceneGraph-Data-master/glider.osg"));
    root->addChild(osgDB::readNodeFile("/home/zhihui/OSGlib/OSG/OpenSceneGraph-Data-master/osgcool.osgt"));
    //设置场景数据
    viewer.setSceneData(root);
    viewer.realize();
    viewer.run();*/


    /*
     * test_4
     * 添加多个模型
     */
    /*osg::ref_ptr<osgViewer::Viewer> viewer=new  osgViewer::Viewer();
    osg::ref_ptr<osg::Group> root= new osg::Group();
    osg::ref_ptr<osg::Node> node1=osgDB::readNodeFile("/home/zhihui/OSGlib/OSG/OpenSceneGraph-Data-master/glider.osg");
    osg::ref_ptr<osg::Node> node2=osgDB::readNodeFile("/home/zhihui/OSGlib/OSG/OpenSceneGraph-Data-master/osgcool.osgt");
    root->addChild(node1.get());
    root->addChild(node2.get());
    viewer->setSceneData(root.get());
    viewer->realize();
    viewer->run();*/


    /*
     * test_5
     * 隐藏指定模型
     */
    /*osg::ref_ptr<osgViewer::Viewer> viewer=new  osgViewer::Viewer();
    osg::ref_ptr<osg::Group> root= new osg::Group();
    osg::ref_ptr<osg::Node> node1=osgDB::readNodeFile("/home/zhihui/OSGlib/OSG/OpenSceneGraph-Data-master/glider.osg");
    osg::ref_ptr<osg::Node> node2=osgDB::readNodeFile("/home/zhihui/OSGlib/OSG/OpenSceneGraph-Data-master/osgcool.osgt");
    root->addChild(node1.get());
    root->addChild(node2.get());

    node2->setNodeMask(0x0);

    viewer->setSceneData(root.get());
    viewer->realize();
    viewer->run();*/


    /*
     * test_6
     * 结点开关
     */
    /*osgViewer::Viewer viewer;
    osg::Group *root=new osg::Group();
    osg::Switch *sw=new osg::Switch();

    osg::Node *node=osgDB::readNodeFile("/home/zhihui/OSGlib/OSG/OpenSceneGraph-Data-master/osgcool.osgt");
    sw->addChild(node,false);
    sw->addChild(osgDB::readNodeFile("/home/zhihui/OSGlib/OSG/OpenSceneGraph-Data-master/glider.osg"));
    root->addChild(sw);
    viewer.setSceneData(root);
    viewer.realize();
    viewer.run();*/


    /*
     * test_7
     * 移动/旋转/缩放模型
     */
    /*osgViewer::Viewer viewer;
    osg::ref_ptr<osg::Group> root=new osg::Group();
    osg::ref_ptr<osg::Node> osgcool=osgDB::readNodeFile("/home/zhihui/OSGlib/OSG/OpenSceneGraph-Data-master/osgcool.osgt");

    //移动模型（向上移动2个单位）三个参数：x轴、向里的y轴、向上的z轴
    osg::ref_ptr<osg::MatrixTransform> trans=new osg::MatrixTransform;
    trans->setMatrix(osg::Matrix::translate(0,0,2));
    trans->addChild(osgcool.get());

    //缩放模型（向下移动2个单位 并且 缩放0.5倍）三个参数：x轴、向里的y轴、向上的z轴
    osg::ref_ptr<osg::MatrixTransform> scale=new osg::MatrixTransform;
    scale->setMatrix(osg::Matrix::scale(0.5,0.5,0.5)*osg::Matrix::translate(0,0,-2));
    scale->addChild(osgcool.get());

    //旋转模型 （向右移动4个单位、向下移动2个单位。缩放0.5倍。平躺45°）四个参数：？？？？
    osg::ref_ptr<osg::MatrixTransform> rot=new osg::MatrixTransform;
    rot->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(45.0),1,0,0)*osg::Matrix::scale(0.5,0.5,0.5)*osg::Matrix::translate(4,0,-2));
    rot->addChild(osgcool.get());

    root->addChild(osgcool.get());
    root->addChild(trans.get());
    root->addChild(scale.get());
    root->addChild(rot.get());

    //为了方便看，我们在场景中加入了窗口缩放
    viewer.addEventHandler(new osgViewer::WindowSizeHandler);
    viewer.setSceneData(root.get());
    viewer.realize();
    viewer.run();*/


    /*
     * test_8
     * 绘制正方形
     * 子函数：osg::ref_ptr <osg::Node> createQuad()
     */
    /*osgViewer::Viewer viewer;
    osg::Group *root= new osg::Group;
    root->addChild(createQuad().get());
    viewer.addEventHandler(new osgViewer::WindowSizeHandler);
    viewer.setSceneData(root);
    viewer.realize();
    viewer.run();*/


    /*
     * test_9
     * 绘制线宽
     * 子函数：osg::ref_ptr<osg::Node> createLine()
     */
    /*osgViewer::Viewer viewer;
    osg::Group *root= new osg::Group;
    root->addChild(createLine().get());
    viewer.addEventHandler(new osgViewer::WindowSizeHandler);
    viewer.setSceneData(root);
    viewer.realize();
    viewer.run();*/


    /*
     * test_10
     * 绘制基本几何类型
     * 子函数：osg::ref_ptr<osg::Node> createShape()
     */
    /*osgViewer::Viewer viewer;
    osg::Group *root= new osg::Group;
    root->addChild(createShape().get());
    viewer.addEventHandler(new osgViewer::WindowSizeHandler);
    viewer.setSceneData(root);
    viewer.realize();
    viewer.run();*/

    /*
     *test_11
     * 响应键盘和鼠标事件
     * 需要类　class UseEventHandler:public osgGA::GUIEventHandler
     */
    /*osgViewer::Viewer viewer;
    osg::ref_ptr<osg::Group> root = new osg::Group() ;
    root ->addChild(osgDB::readNodeFile("/home/zhihui/OSGlib/OSG/OpenSceneGraph-Data-master/cow.osg")) ;
    root ->addChild(osgDB::readNodeFile("/home/zhihui/OSGlib/OSG/OpenSceneGraph-Data-master/glider.osg"));
    viewer.addEventHandler(new osgViewer::WindowSizeHandler);
    viewer.setSceneData(root.get());
    viewer.addEventHandler(new UseEventHandler);
    viewer.realize();
    viewer.run();*/


    /*
     * test_12
     * pick
     * 需要类　class CPickHandler: public osgGA::GUIEventHandler
     */
    osgViewer::Viewer viewer;
    osg::ref_ptr<osg::Group> root = new osg::Group() ;
    root ->addChild(osgDB::readNodeFile("/home/zhihui/OSGlib/OSG/OpenSceneGraph-Data-master/cessna.osg"));

    osg::ref_ptr<osg::Node> cow=osgDB::readNodeFile("/home/zhihui/OSGlib/OSG/OpenSceneGraph-Data-master/cow.osg");

    //添加一个 scribe 结点,该结点下的模型会被加白描线高亮显示。
    osg::ref_ptr<osgFX::Scribe> sc=new osgFX::Scribe;
    //将 cow 模型添加到 sc 结点下
    sc->addChild(cow.get());
    root->addChild(cow.get());
    root->addChild(sc.get());

    viewer.addEventHandler(new osgViewer::WindowSizeHandler);
    viewer.setSceneData(root.get());
    viewer.addEventHandler(new CPickHandler(&viewer));
    viewer.realize();
    viewer.run();
}

/*
//绘制该正方体的代码都在这个函数内，该函数返回一个osg::Node类型的结点
osg::ref_ptr <osg::Node> createQuad()
{
    //申请一个几何体结点
    osg::ref_ptr<osg::Geometry> geom=new osg::Geometry;

    //定义顶点序列（存放4个顶点）
    //申请一个顶点序列，顶点放在vector容器中
    osg::ref_ptr<osg::Vec3Array> v=new osg::Vec3Array;
    //把顶点序列加入到几何体中
    geom->setVertexArray(v.get());
    //定义4个顶点加入到顶点序列中
    v->push_back(osg::Vec3(-1.f, 0.f, -1.f));
    v->push_back(osg::Vec3(1.f, 0.f, -1.f ));
    v->push_back(osg::Vec3(1.f, 0.f, 1.f ));
    v->push_back(osg::Vec3(-1.f, 0.f, 1.f ));

    //定义颜色数组
    //申请一个颜色序列数组，并且与几何体中进行关联,设置绑定方式为逐点绑定。颜色数组的四个参数分别为 RGBA。
    osg::ref_ptr <osg::Vec4Array> c=new osg::Vec4Array;
    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    c->push_back(osg::Vec4(1.f, 0.f, 0.f, 1.f ));
    c->push_back(osg::Vec4(0.f, 1.f, 0.f, 1.f ));
    c->push_back(osg::Vec4(0.f, 0.f, 1.f, 1.f ));
    c->push_back(osg::Vec4(1.f, 1.f, 1.f, 1.f ));

    //定义法线
    //申请一个法线绑定到该四方体中。法线为指向 Y 轴负半轴。法线不同会影响光照的效果。
    osg::ref_ptr<osg::Vec3Array> n=new osg::Vec3Array;
    geom->setNormalArray(n.get());
    geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
    n->push_back(osg::Vec3(0.f,-1.f,0.f));

    //设置顶点关联方式
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

    //几何组结点
    //申请一个几何组结点，并把几何体结点加入到几何组结点当中
    osg::ref_ptr<osg::Geode> geode=new osg::Geode;
    geode->addDrawable(geom.get());
    //返回该组结点，可以被加入到Group中进行渲染
    return geode.get();
}*/

/*
osg::ref_ptr<osg::Node> createLine()
{
    osg::ref_ptr<osg::Geometry> geom=new osg::Geometry;

    //限制线的宽度线宽是一种属性和状态,在 OSG 当中有专门的属性和状态类来管理这些。
    //所有启动属性和状态的操作使用都与此类似。
    osg::ref_ptr<osg::LineWidth> LineSize=new osg::LineWidth;
    LineSize->setWidth(1.0);
    geom->getOrCreateStateSet()->setAttributeAndModes(LineSize.get(),osg::StateAttribute::ON);

    //首先定义四个点
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
    geom->setVertexArray(v.get() );
    v->push_back(osg::Vec3( -1.f, 0.f, -1.f ) );
    v->push_back(osg::Vec3( 1.f, 0.f, -1.f ) );
    v->push_back(osg::Vec3( 1.f, 0.f, 1.f ) );
    v->push_back(osg::Vec3( -1.f, 0.f, 1.f ) );
    //定义颜色数组
    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array;
    geom->setColorArray(c.get() );
    geom->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    c->push_back(osg::Vec4( 1.f, 0.f, 0.f, 1.f ) );
    c->push_back(osg::Vec4( 0.f, 1.f, 0.f, 1.f ) );
    c->push_back(osg::Vec4( 0.f, 0.f, 1.f, 1.f ) );
    c->push_back(osg::Vec4( 1.f, 1.f, 1.f, 1.f ) );
    //定义法线
    osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array;
    geom->setNormalArray( n.get() );
    geom->setNormalBinding( osg::Geometry::BIND_OVERALL );
    n->push_back(osg::Vec3( 0.f, -1.f, 0.f ) );
    //设置顶点关联方式,这里把顶点关联方式改成了：　LINE_LOOP
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, 4 ) );
    //几何组结点
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( geom.get() );
    return geode.get();
}*/


/*
osg::ref_ptr<osg::Node> createShape()
{
    //申请一个几何组结点　来装载几何体
    osg::ref_ptr<osg::Geode> geode=new osg::Geode;
    //申请两个变量表示几何体要使用的半径和高度
    float radius=0.08f;
    float height=0.1f;

    //表示模型的粗细度，值越大精细度就越大。
    osg::TessellationHints *hints=new osg::TessellationHints;
    hints->setDetailRatio(0.5f);

    //分别是申请球结点:规定圆心、半径、精细度;
    //盒子结点:规定盒心、长、精细度;
    //圆锥结点:规定底面圆心、半径、高度、精细度;
    //圆柱结点:规定底面圆心、半径、高度、精细度;
    //胶囊体结点:规定半径、圆心、高度、精细度。
    geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),radius),hints));
    geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.2f,0.0f,0.0f),2*radius ),hints));
    geode->addDrawable(new osg::ShapeDrawable(new osg::Cone(osg::Vec3(0.4f,0.0f,0.0f),radius ,height),hints));
    geode->addDrawable(new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0.60f,0.0f,0.0f),radius ,height),hints));
    geode->addDrawable(new osg::ShapeDrawable(new osg::Capsule(osg::Vec3(0.80f,0.0f,0.0f),radius ,height),hints));

    return geode.get();
}*/


