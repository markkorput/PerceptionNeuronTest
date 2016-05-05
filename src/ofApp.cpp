#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    setupParams();
    
    mesh.setMode(OF_PRIMITIVE_LINE_STRIP); // _LOOP
}

void ofApp::setupParams(){
    RUI_SETUP(); //start RemoteUI server
    ofAddListener(OFX_REMOTEUI_SERVER_GET_CLIENT_OF_EVENT(), this, &ofApp::ruiServerCallback);

    RUI_NEW_GROUP("NEURON");
    RUI_SHARE_PARAM(neuron_ip);
    RUI_SHARE_PARAM(neuron_port, 5001, 65535);
    RUI_SHARE_PARAM(neuron_drawDebug);
    RUI_NEW_GROUP("MESH");
    RUI_SHARE_PARAM(drawMesh);
    //load values from XML, as they were last saved (if they were)
    RUI_LOAD_FROM_XML();
    
    bParamsDirty = true;
}

void ofApp::applyParams(){
    if(neuron.isConnected())
        neuron.disconnect();

    neuron.connect(neuron_ip, neuron_port);
}

//--------------------------------------------------------------
void ofApp::update(){
    if(bParamsDirty){
        applyParams();
        bParamsDirty = false;
    }

    neuron.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofClear(0);
    cam.begin();
    if(neuron_drawDebug){
        neuron.debugDraw();
        ofDrawAxis(100);
    }

    if(drawMesh);
        mesh.draw();
    cam.end();
}

void ofApp::keyPressed(int key){
    if(key == 'm'){
        mesh.clear();
        vector<ofxPerceptionNeuron::Skeleton> skeletons = neuron.getSkeletons();
        for(auto &skeleton: skeletons){
            const vector<ofxPerceptionNeuron::Joint> joints = skeleton.getJoints();
            for(auto &joint: joints){
                mesh.addVertex(joint.global_transform.getTranslation()+joint.offset);
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::ruiServerCallback(RemoteUIServerCallBackArg &arg){
    
    switch (arg.action) {
        case CLIENT_DID_SET_PRESET:
        case CLIENT_UPDATED_PARAM:
            bParamsDirty = true;
            break;
        default:
            break;
    }
}

