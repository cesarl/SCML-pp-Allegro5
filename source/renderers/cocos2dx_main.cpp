#include "SCML_cocos2dx.h"
#include <vector>
#include <list>

#include "CCDirector.h"

using namespace std;
using namespace cocos2d;
using namespace SCML_cocos2dx;
USING_NS_CC;




class Test : public cocos2d::CCLayerColor
{
public:
    
    size_t data_file_index;
    SCML::Data data;
    FileSystem fs;
    list<Entity*> entities;
    
    float x;
    float y;
    float angle;
    float scale;

    virtual bool init();  

    static cocos2d::CCScene* scene();
    void menuCloseCallback(CCObject* pSender);
    CREATE_FUNC(Test);

	bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
};


CCScene* Test::scene()
{
    CCScene * scene = NULL;
    do 
    {
        // 'scene' is an autorelease object
        scene = CCScene::create();
        CC_BREAK_IF(! scene);

        // 'layer' is an autorelease object
        Test *layer = Test::create();
        CC_BREAK_IF(! layer);

        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Test::init()
{
    bool bRet = false;
    do 
    {
        // super init first
        CC_BREAK_IF(! CCLayer::init());

        // Set up scene
        initWithColor(ccc4(255,255,255,255));
        
        data_file_index = 0;
        //SCML::Data data(data_files[data_file_index]);
        data.load("samples/monster/Example.SCML");
        data.log();
        
        fs.load(&data);
        printf("Loaded %zu images.\n", fs.images.size());
        
        x = 400.0f;
        y = 300.0f;
        angle = 0.0f;
        scale = 1.0f;
        
        int i = 0;
        for(map<int, SCML::Data::Entity*>::iterator e = data.entities.begin(); e != data.entities.end(); e++)
        {
            Entity* entity = new Entity(&data, e->first);
            entity->setFileSystem(&fs);
            entity->autorelease();
            entity->setPosition(ccp(x, y));
            entities.push_back(entity);
            this->addChild(entity, 0, 100+i);
            i++;
        }
        printf("Loaded %zu entities.\n", entities.size());
                                    
		/*Entity *animator = new Entity;
		CC_BREAK_IF(! animator);
		animator->autorelease();

		animator->setPosition(ccp(400, 300));
		//animator->setScale(0.8f);
		
		this->addChild(animator, 0, 100);*/


		CCDirector* pDirector = CCDirector::sharedDirector();
		pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);

        bRet = true;

    } while (0);

    return bRet;
}

void Test::menuCloseCallback(CCObject* pSender)
{
    // "close" menu item clicked
    CCDirector::sharedDirector()->end();
}


bool Test::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
	/*Entity *animator = (Entity *)this->getChildByTag(100);

	if (animator)
		animator->PlayNext();*/

    return true;
}




class  AppDelegate : private cocos2d::CCApplication
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    virtual bool applicationDidFinishLaunching();

    virtual void applicationDidEnterBackground();

    virtual void applicationWillEnterForeground();
};

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    CCDirector *pDirector = CCDirector::sharedDirector();

    pDirector->setOpenGLView(CCEGLView::sharedOpenGLView());
    
    TargetPlatform target = getTargetPlatform();
    
    if (target == kTargetIpad)
    {
        // ipad
        
        CCFileUtils::sharedFileUtils()->setResourceDirectory("iphonehd");
        
        // don't enable retina because we don't have ipad hd resource
        CCEGLView::sharedOpenGLView()->setDesignResolutionSize(960, 640, kResolutionNoBorder);
    }
    else if (target == kTargetIphone)
    {
        // iphone
        
        // try to enable retina on device (Interface does not even exist on Linux!)
        if (true)//true == CCDirector::sharedDirector()->enableRetinaDisplay(true))
        {
            // iphone hd
            CCFileUtils::sharedFileUtils()->setResourceDirectory("iphonehd");
        }
        else 
        {
            CCFileUtils::sharedFileUtils()->setResourceDirectory("iphone");
        }
    }
    else 
    {
        // android, windows, blackberry, linux or mac
        // use 960*640 resources as design resolution size
        CCFileUtils::sharedFileUtils()->setResourceDirectory("iphonehd");
        CCEGLView::sharedOpenGLView()->setDesignResolutionSize(1600, 1200, kResolutionNoBorder);
    }

    // turn on display FPS
    //pDirector->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    CCScene *pScene = Test::scene();

    // run
    pDirector->runWithScene(pScene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    CCDirector::sharedDirector()->stopAnimation();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    CCDirector::sharedDirector()->startAnimation();
}




void main_loop(vector<string>& data_files)
{
    CCApplication::sharedApplication()->run();
}





bool init(unsigned int w, unsigned int h)
{
    
    // get application path
    /*int length;
    // 500 is enough?
    char fullpath[500];
    length = readlink("/proc/self/exe", fullpath, sizeof(fullpath));
    fullpath[length] = '\0';

    std::string resourcePath = fullpath;
    resourcePath = resourcePath.substr(0, resourcePath.find_last_of("/"));
    resourcePath += "/../../../Resources/";*/
    
    
    (void)new AppDelegate;  // Cocos2d-x stores this secretly...
    //CCApplication::sharedApplication()->setResourceRootPath(resourcePath.c_str());
    CCEGLView* eglView = CCEGLView::sharedOpenGLView();
    eglView->setFrameSize(800, 600);
    
    return true;
}

void quit()
{
    delete CCApplication::sharedApplication();
}


