/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2010 Robert Osfield
 *
 * This application is open source and may be redistributed and/or modified
 * freely and without restriction, both in commercial and non commercial applications,
 * as long as this copyright notice is maintained.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <GL/glew.h>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/CoordinateSystemNode>

#include <osg/Switch>
#include <osg/Texture>
#include <osg/StateSet>
#include <osg/Drawable>
#include <osgText/Text>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/SphericalManipulator>

#include <osgGA/Device>
#include <osgUtil/PrintVisitor>

#include <iostream>
#include <stdint.h>

#include <osg/GLExtensions>
//#include <GL/glx.h>

#ifndef GL_ARB_sync
#define GL_MAX_SERVER_WAIT_TIMEOUT        0x9111
#define GL_OBJECT_TYPE                    0x9112
#define GL_SYNC_CONDITION                 0x9113
#define GL_SYNC_STATUS                    0x9114
#define GL_SYNC_FLAGS                     0x9115
#define GL_SYNC_FENCE                     0x9116
#define GL_SYNC_GPU_COMMANDS_COMPLETE     0x9117
#define GL_UNSIGNALED                     0x9118
#define GL_SIGNALED                       0x9119
#define GL_ALREADY_SIGNALED               0x911A
#define GL_TIMEOUT_EXPIRED                0x911B
#define GL_CONDITION_SATISFIED            0x911C
#define GL_WAIT_FAILED                    0x911D
#define GL_SYNC_FLUSH_COMMANDS_BIT        0x00000001
#define GL_TIMEOUT_IGNORED                0xFFFFFFFFFFFFFFFFull
#endif

namespace osgUtil {
class PrintGeom : public PrintVisitor
{
public:
		PrintGeom(std::ostream& out, int indent=0, int step=2) : PrintVisitor(out, indent, step) {}
		void apply(osg::Geode& geode) 
		{
			osg::Geometry* pGeometry = geode.getDrawable(0)->asGeometry();
			PrintVisitor::output()<<geode.libraryName()<<"::"<<geode.className()<< " Drawables(vertices) " 
				<< pGeometry->getVertexArray()->getNumElements() << std::endl;
		}
};

} // end-o-namespace osgUtil

class DisableDisplaylists : public osg::NodeVisitor 
{
public:
					 DisableDisplaylists() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), statesets(0), geodes(0),
					 lods(0), switches(0), transforms(0), groups(0), nodes(0)
	{}
		  					{}
		  		
				    ~DisableDisplaylists() {
					 		OSG_WARN << "LOD cnt " << lods << " stateset cnt " << statesets << " geode cnt " << geodes
								<< " switch cnt " << switches << " transform cnt " << transforms << " group cnt " 
								<< groups << " node cnt " << nodes << std::endl;
						}
					 void apply(osg::Geode& geode)
					 {
					 	++geodes;
								for(unsigned int i=0; i < geode.getNumDrawables(); ++i)
								{
										  osg::Geometry* geom = geode.getDrawable(i)->asGeometry();
										  geom->setUseDisplayList(false);
										  geom->setUseVertexBufferObjects(true);

										  showStateSet(geode.getDrawable(i)->getStateSet());
								}
					 }

					 void apply(osg::Node& node) {
					   ++nodes;
					 	showStateSet(node.getStateSet());
						traverse(node);
					}

					 void apply(osg::Group& node) {
					   ++groups;
					 	showStateSet(node.getStateSet());
						traverse(node);
					}

					 void apply(osg::LOD& node) {
					   ++lods;
					 	showStateSet(node.getStateSet());
						traverse(node);
					}

					 void apply(osg::Transform& node) {
					   ++transforms;
					 	showStateSet(node.getStateSet());
						traverse(node);
					}

					 void apply(osg::Switch& node) {
					   ++switches;
					 	showStateSet(node.getStateSet());
						traverse(node);
					}


		  protected:
					 void showStateSet(osg::StateSet *state) {
								if(state) {
										  ++statesets;
										  // loop through textures gather texture_handles
										  unsigned int numTextures = state->getTextureAttributeList().size();
										  unsigned int i = 0;
										  for(i = 0; i<numTextures; ++i) {
													 osg::StateAttribute* sa = state->getTextureAttribute(i,osg::StateAttribute::TEXTURE);
													 osg::Texture* texture = dynamic_cast<osg::Texture*>(sa);
													 OSG_WARN << "Texture uniqueID " << texture->getUniqueID() << std::endl;
										  }
										  osg::StateAttribute* sa = state->getAttribute(osg::StateAttribute::TEXTURE);
										  if(sa)
										     OSG_WARN << "Found a single texture" << std::endl;


										  OSG_WARN << "Found StateSet has " << numTextures << " textures." << std::endl;
								}
					 }
		private:
			// Counters
			int geodes;
			int nodes;
			int lods;
			int transforms;
			int switches;
			int groups;
			int statesets;
};

class MySwapBuffersCallback : public osg::GraphicsContext::SwapCallback
{
public:
    MySwapBuffersCallback():
        _extensionInitialized(false),
        _glFenceSync(0),
        _glIsSync(0),
        _glDeleteSync(0),
        _glClientWaitSync(0),
        _glWaitSync(0),
        _glGetInteger64v(0),
        _glGetSynciv(0),
        _previousSync(0)
    {
         OSG_NOTICE<<"Created Swap callback."<<std::endl;
    }

    void setUpExtensions()
	 {
				_extensionInitialized = true;
				osg::setGLExtensionFuncPtr(_glFenceSync, "glFenceSync");
				osg::setGLExtensionFuncPtr(_glIsSync, "glIsSync");
				osg::setGLExtensionFuncPtr(_glDeleteSync, "glDeleteSync");
				osg::setGLExtensionFuncPtr(_glClientWaitSync, "glClientWaitSync");
				osg::setGLExtensionFuncPtr(_glWaitSync, "glWaitSync");
				osg::setGLExtensionFuncPtr(_glGetInteger64v, "glGetInteger64v");
				osg::setGLExtensionFuncPtr(_glGetSynciv, "glGetSynciv");
				osg::setGLExtensionFuncPtr(_glUniform1i, "glUniform1i");
				osg::setGLExtensionFuncPtr(_glUseProgram, "glUseProgram");

				//OSG_WARN << glXQueryExtensionsString(glXGetCurrentDisplay(),0) << std::endl;
			//	if(strstr(glXQueryExtensionsString(glXGetCurrentDisplay(),0), "GLX_EXT_swap_control") 
			//						 && strstr(glXQueryExtensionsString(glXGetCurrentDisplay(),0), "GLX_EXT_swap_control_tear")
			//	  )
#if 0
				{
						  PFNGLXSWAPINTERVALEXTPROC _glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC) 
						  		glXGetProcAddress(reinterpret_cast<const GLubyte *>("glXSwapIntervalEXT"));
						  if(_glXSwapIntervalEXT )  {
									 _glXSwapIntervalEXT(glXGetCurrentDisplay(), glXGetCurrentDrawable(), -1);
									 OSG_WARN << "SWAP_CONTROL is On, Go for Adaptive" << std::endl;
						  }
				}
#endif
	 }


	 virtual void swapBuffersImplementation(osg::GraphicsContext* gc)
    {
        // OSG_NOTICE<<"Before swap - place to do swap ready sync"<<std::endl;
        gc->swapBuffersImplementation();
        //glFinish();

        if (!_extensionInitialized) setUpExtensions();

        if (_glClientWaitSync)
        {
            if (_previousSync)
            {
                unsigned int num_seconds = 1;
                GLuint64 timeout = num_seconds * ((GLuint64)1000 * 1000 * 1000);
                _glClientWaitSync(_previousSync, 0, timeout);

                _glDeleteSync(_previousSync);
            }

            _previousSync = _glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        }
        //gc->getState()->checkGLErrors("after glWaitSync");

        //OSG_NOTICE<<"After swap"<<std::endl;
    }

    #ifdef _WIN32
    typedef __int64 GLint64;
    typedef unsigned __int64 GLuint64;
    #else
    typedef int64_t GLint64;
    typedef uint64_t GLuint64;
    #endif
    
    typedef struct __GLsync *GLsync;

    typedef GLsync (GL_APIENTRY * PFNGLFENCESYNCPROC) (GLenum condition, GLbitfield flags);
    typedef GLboolean (GL_APIENTRY * PFNGLISSYNCPROC) (GLsync sync);
    typedef void (GL_APIENTRY * PFNGLDELETESYNCPROC) (GLsync sync);
    typedef GLenum (GL_APIENTRY * PFNGLCLIENTWAITSYNCPROC) (GLsync sync, GLbitfield flags, GLuint64 timeout);
    typedef void (GL_APIENTRY * PFNGLWAITSYNCPROC) (GLsync sync, GLbitfield flags, GLuint64 timeout);
    typedef void (GL_APIENTRY * PFNGLGETINTEGER64VPROC) (GLenum pname, GLint64 *params);
    typedef void (GL_APIENTRY * PFNGLGETSYNCIVPROC) (GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values);
    typedef void (GL_APIENTRY * PFNGLUNIFORM1IPROC) (GLint location, GLint value);
    typedef void (GL_APIENTRY * PFNGLUSEPROGRAMPROC) (GLint pname);

    bool                        _extensionInitialized;

    PFNGLFENCESYNCPROC          _glFenceSync;
    PFNGLISSYNCPROC             _glIsSync;
    PFNGLDELETESYNCPROC         _glDeleteSync;
    PFNGLCLIENTWAITSYNCPROC     _glClientWaitSync;
    PFNGLWAITSYNCPROC           _glWaitSync;
    PFNGLGETINTEGER64VPROC      _glGetInteger64v;
    PFNGLGETSYNCIVPROC          _glGetSynciv;
    PFNGLUNIFORM1IPROC          _glUniform1i;
    PFNGLUSEPROGRAMPROC          _glUseProgram;

    GLsync _previousSync;
};

// Time the draw and fill segements of the openGL Pipeline. Exclude the monitor sync.
//
class Profiler : public osg::GraphicsContext::SwapCallback
{
		  public:
					 Profiler():  _timer1(0) { _query[0] = 0; };

					 virtual void swapBuffersImplementation(osg::GraphicsContext* gc)
					 {
					         GLint done=0;
					         // ...run through glClear(GL_COLOR_BUFFER_BIT | ...) and all the glDrawArrays() calls.
					 			if(!_query[0]) {
									glGenQueries(2, _query); // Create a query object..
								   glQueryCounter(_query[0], GL_TIMESTAMP); // fake query to init.
								}
								glQueryCounter(_query[1], GL_TIMESTAMP);
								// wait...unsynchronized call all fill operations complete
								while (!done) {
									glGetQueryObjectiv(_query[1], GL_QUERY_RESULT_AVAILABLE, &done);
								}
								glGetQueryObjectui64v(_query[0], GL_QUERY_RESULT, &_timer1);// get the results of the query.
								glGetQueryObjectui64v(_query[1], GL_QUERY_RESULT, &_timer2);// get the results of the query.
								OSG_WARN << "Frame render time " << (_timer2-_timer1) / 1.0e3 << " microseconds" << std::endl;

								// glXSwapBuffers()
								gc->swapBuffersImplementation();

								glQueryCounter(_query[0], GL_TIMESTAMP);
					 }
		 private:
			GLuint64 _timer1;
			GLuint64 _timer2;
			GLuint _query[2];

};

int main(int argc, char** argv)
{
	 // use an ArgumentParser object to manage the program arguments.

    osg::ArgumentParser arguments(&argc,argv);

    arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
    arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" is the standard OpenSceneGraph example which loads and visualises 3d models.");
    arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] filename ...");
    arguments.getApplicationUsage()->addCommandLineOption("--image <filename>","Load an image and render it on a quad");
    arguments.getApplicationUsage()->addCommandLineOption("--dem <filename>","Load an image/DEM and render it on a HeightField");
    arguments.getApplicationUsage()->addCommandLineOption("--login <url> <username> <password>","Provide authentication information for http file access.");
    arguments.getApplicationUsage()->addCommandLineOption("-p <filename>","Play specified camera path animation file, previously saved with 'z' key.");
    arguments.getApplicationUsage()->addCommandLineOption("--speed <factor>","Speed factor for animation playing (1 == normal speed).");
    arguments.getApplicationUsage()->addCommandLineOption("--device <device-name>","add named device to the viewer");
    arguments.getApplicationUsage()->addCommandLineOption("--vbos","no display lists, vbos");

    // XXX blf: May not be necessary once deferred rendering.
#ifdef NEED_ALPHA_VISUAL
	 osg::DisplaySettings::instance()->setMinimumNumAlphaBits(8);
#endif

    osgViewer::Viewer viewer(arguments);

    unsigned int helpType = 0;
    if ((helpType = arguments.readHelpType()))
    {
        arguments.getApplicationUsage()->write(std::cout, helpType);
        return 1;
    }

    // report any errors if they have occurred when parsing the program arguments.
    if (arguments.errors())
    {
        arguments.writeErrorMessages(std::cout);
        return 1;
    }

    if (arguments.argc()<=1)
    {
        arguments.getApplicationUsage()->write(std::cout,osg::ApplicationUsage::COMMAND_LINE_OPTION);
        return 1;
    }

    std::string url, username, password;
    while(arguments.read("--login",url, username, password))
    {
        if (!osgDB::Registry::instance()->getAuthenticationMap())
        {
            osgDB::Registry::instance()->setAuthenticationMap(new osgDB::AuthenticationMap);
            osgDB::Registry::instance()->getAuthenticationMap()->addAuthenticationDetails(
                url,
                new osgDB::AuthenticationDetails(username, password)
            );
        }
    }

    std::string device;
    while(arguments.read("--device", device))
    {
        osg::ref_ptr<osgGA::Device> dev = osgDB::readFile<osgGA::Device>(device);
        if (dev.valid())
        {
            viewer.addDevice(dev.get());
        }
    }

    // set up the camera manipulators.
    {
        osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

        keyswitchManipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator() );
        keyswitchManipulator->addMatrixManipulator( '2', "Flight", new osgGA::FlightManipulator() );
        keyswitchManipulator->addMatrixManipulator( '3', "Drive", new osgGA::DriveManipulator() );
        keyswitchManipulator->addMatrixManipulator( '4', "Terrain", new osgGA::TerrainManipulator() );
        keyswitchManipulator->addMatrixManipulator( '5', "Orbit", new osgGA::OrbitManipulator() );
        keyswitchManipulator->addMatrixManipulator( '6', "FirstPerson", new osgGA::FirstPersonManipulator() );
        keyswitchManipulator->addMatrixManipulator( '7', "Spherical", new osgGA::SphericalManipulator() );

        std::string pathfile;
        double animationSpeed = 1.0;
        while(arguments.read("--speed",animationSpeed) ) {}
        char keyForAnimationPath = '8';
        while (arguments.read("-p",pathfile))
        {
            osgGA::AnimationPathManipulator* apm = new osgGA::AnimationPathManipulator(pathfile);
            if (apm || !apm->valid())
            {
                apm->setTimeScale(animationSpeed);

                unsigned int num = keyswitchManipulator->getNumMatrixManipulators();
                keyswitchManipulator->addMatrixManipulator( keyForAnimationPath, "Path", apm );
                keyswitchManipulator->selectMatrixManipulator(num);
                ++keyForAnimationPath;
            }
        }

        viewer.setCameraManipulator( keyswitchManipulator.get() );
    }

    bool doArbSync = arguments.read("--sync");
	 bool doVBOs =  arguments.read("--vbos");
	 bool doPrints =  arguments.read("--printGraph");

	 viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
	 viewer.getCamera()->setCullingMode(osg::Camera::NO_CULLING);

    // add the state manipulator
    viewer.addEventHandler( new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()) );

    // add the thread model handler
    viewer.addEventHandler(new osgViewer::ThreadingHandler);

    // add the window size toggle handler
    viewer.addEventHandler(new osgViewer::WindowSizeHandler);

    // add the stats handler
    viewer.addEventHandler(new osgViewer::StatsHandler);

    // add the help handler
    viewer.addEventHandler(new osgViewer::HelpHandler(arguments.getApplicationUsage()));

    // add the record camera path handler
    viewer.addEventHandler(new osgViewer::RecordCameraPathHandler);

    // add the LOD Scale handler
    viewer.addEventHandler(new osgViewer::LODScaleHandler);

    // add the screen capture handler
    viewer.addEventHandler(new osgViewer::ScreenCaptureHandler);

    // load the data
    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFiles(arguments);
    if (!loadedModel)
    {
        std::cout << arguments.getApplicationName() <<": No data loaded" << std::endl;
        return 1;
    }

    // any option left unread are converted into errors to write out later.
    arguments.reportRemainingOptionsAsUnrecognized();

    // report any errors if they have occurred when parsing the program arguments.
    if (arguments.errors())
    {
        arguments.writeErrorMessages(std::cout);
        return 1;
    }


    // optimize the scene graph, remove redundant nodes and state etc.
    osgUtil::Optimizer optimizer;
    optimizer.optimize(loadedModel.get());

    viewer.setSceneData( loadedModel.get() );
   if(doPrints) {
	 osgUtil::PrintGeom printGraph(std::cout);
	 loadedModel.get()->accept(printGraph);
	}
	if(doVBOs) {
	 DisableDisplaylists disPlayed;
	 loadedModel.get()->accept(disPlayed);
	}

    viewer.realize();
	 viewer.frame();

	 glewExperimental = GL_TRUE;
	 glewInit();

    // Check for multi-texture support on GPU
    for(unsigned int contextID = 0;
            contextID<osg::DisplaySettings::instance()->getMaxNumberOfGraphicsContexts();
            ++contextID)
        {
            osg::Texture::Extensions* textExt = osg::Texture::getExtensions(contextID,false);
            if (textExt)
            {
                if (!textExt->isMultiTexturingSupported())
                {
                    std::cout<<"Warning: multi-texturing not supported by OpenGL drivers, unable to run application."<<std::endl;
                    return 1;
                }
            }
        }

    if (doArbSync)
    {
        osgViewer::ViewerBase::Contexts contexts;
        viewer.getContexts(contexts);
        for(osgViewer::ViewerBase::Contexts::iterator itr = contexts.begin();
            itr != contexts.end();
            ++itr)
        {
            (*itr)->setSwapCallback(new MySwapBuffersCallback);
        }
    } else { // Profile DRAW_TIME
        osgViewer::ViewerBase::Contexts contexts;
        viewer.getContexts(contexts);
        for(osgViewer::ViewerBase::Contexts::iterator itr = contexts.begin();
            itr != contexts.end();
            ++itr)
        {
            (*itr)->setSwapCallback(new Profiler);
        }
	 }

    return viewer.run();

}
