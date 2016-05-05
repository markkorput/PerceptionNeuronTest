#pragma once

#include "ofMain.h"
#include "ofxPerceptionNeuron.h"
#include "ofxRemoteUIServer.h"

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
//        void exit();
		
		void keyPressed(int key);
//		void keyReleased(int key);
//		void mouseMoved(int x, int y);
//		void mouseDragged(int x, int y, int button);
//		void mousePressed(int x, int y, int button);
//		void mouseReleased(int x, int y, int button);
//		void mouseEntered(int x, int y);
//		void mouseExited(int x, int y);
//		void windowResized(int w, int h);
//		void dragEvent(ofDragInfo dragInfo);
//		void gotMessage(ofMessage msg);

    private:
        void setupParams();
        void applyParams();
        void ruiServerCallback(RemoteUIServerCallBackArg &arg);
        bool bParamsDirty;

    private: // params
        int neuron_port;
        string neuron_ip;
        bool neuron_drawDebug;
    bool drawMesh;

    private:
        ofxPerceptionNeuron::DataReader neuron;
        ofEasyCam cam;
    
        ofMesh mesh;
    
};
