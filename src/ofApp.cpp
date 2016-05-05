#include "ofApp.h"

#include "ofxXmlSettings.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    loadSettings();
}

void ofApp::loadSettings(){
    ofxXmlSettings xml;
    xml.load("settings.xml");

    if(neuron.isConnected())
        neuron.disconnect();

    neuron.connect(xml.getValue("app:neuron:ip", "127.0.0.1"), xml.getValue("app:neuron:port", 7001));
}

//--------------------------------------------------------------
void ofApp::update(){
    neuron.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofClear(0);
    cam.begin();
    neuron.debugDraw();
    ofDrawAxis(100);
    cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'l'){
        loadSettings();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}