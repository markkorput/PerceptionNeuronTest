#include "ofApp.h"


float frameTime = 1.0/60.0f;

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    setupParams();
    
    mesh1.setMode(OF_PRIMITIVE_LINE_STRIP); // _LOOP
    mesh2.setMode(OF_PRIMITIVE_LINE_STRIP); // _LOOP
    mesh3.setMode(OF_PRIMITIVE_LINE_STRIP); // _LOOP
    mesh_from = &mesh1;
    mesh_to = &mesh2;
    mesh_current = &mesh3;
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
    RUI_SHARE_PARAM(meshMorphDuration, 0.01f, 10.0f);
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

    if(meshMorphAnim.isAnimating()){
        meshMorphAnim.update(frameTime);
        morphMesh(*mesh_from, *mesh_to, *mesh_current, meshMorphAnim.val());
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
        mesh_current->draw();

    cam.end();
}

void ofApp::keyPressed(int key){
    if(key == 'm'){
        // swap mesh pointers;
        // the current mesh becomes the origin (avoiding jumps and glitches)
        // the from mesh becomes the current mesh
        ofMesh *tmp = mesh_current;
        mesh_current = mesh_from;
        mesh_from = tmp;

        // reload leader mesh
        loadMocapMesh(*mesh_to);

        // first load? Load the "trailing" mesh along with the leading mesh
        if(!mesh_from->hasVertices()){
            loadMocapMesh(*mesh_from);
        }

        meshMorphAnim.setDuration(meshMorphDuration);
        meshMorphAnim.setCurve(QUADRATIC_EASE_OUT);
        meshMorphAnim.animateFromTo(0.0f, 1.0f);
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

void ofApp::loadMocapMesh(ofMesh &mesh){
    mesh.clear();
    vector<ofxPerceptionNeuron::Skeleton> skeletons = neuron.getSkeletons();
    for(auto &skeleton: skeletons){
        const vector<ofxPerceptionNeuron::Joint> joints = skeleton.getJoints();
        for(auto &joint: joints){
            mesh.addVertex(joint.global_transform.getTranslation()+joint.offset);
        }
    }
}

void ofApp::morphMesh(ofMesh &from, ofMesh &to, ofMesh &target, float progress){
    vector<ofVec3f> froms = from.getVertices();
    vector<ofVec3f> tos = to.getVertices();
    
    int total = std::min(tos.size(), froms.size());
    int target_count = target.getVertices().size();
    ofVec3f current;
    for(int i=0; i<total; i++){
        current = froms[i].getInterpolated(tos[i], progress);
        if(i >= target_count){
            target.addVertex(current);
        } else {
            target.setVertex(i, current);
        }
    }
}