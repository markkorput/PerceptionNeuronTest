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

    nextMeshIntervalUpdateTime = NEVER_AUTO_UPDATE_MESH;
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
    RUI_SHARE_PARAM(shuffleMesh);
    RUI_SHARE_PARAM(meshMorphDuration, 0.01f, 5.0f);
    RUI_SHARE_PARAM(meshUpdateInterval, 0.0f, 5.0f);
    RUI_SHARE_PARAM(meshMorphMin, -2.0f, 3.0f);
    RUI_SHARE_PARAM(meshMorphMax, 0.0f, 5.0f);
    //load values from XML, as they were last saved (if they were)
    RUI_LOAD_FROM_XML();

    bParamsDirty = true;
}

void ofApp::applyParams(){
    if(neuron.isConnected())
        neuron.disconnect();

    neuron.connect(neuron_ip, neuron_port);
    
    if(meshUpdateInterval > 0.0f){
        if(nextMeshIntervalUpdateTime == NEVER_AUTO_UPDATE_MESH)
            nextMeshIntervalUpdateTime = 0.1f; // start now!
    } else {
        nextMeshIntervalUpdateTime = NEVER_AUTO_UPDATE_MESH; // stop
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    float t = ofGetElapsedTimef();

    if(bParamsDirty){
        applyParams();
        bParamsDirty = false;
    }

    if(t >= nextMeshIntervalUpdateTime){
        nextMeshIntervalUpdateTime = t + meshUpdateInterval;
        meshUpdate();
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

    if(drawMesh)
        mesh_current->draw();

    cam.end();
}

void ofApp::keyPressed(int key){
    if(key == 'm'){
        meshUpdate();
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

void ofApp::meshUpdate(){
    // swap mesh pointers;
    // the current mesh becomes the origin (avoiding jumps and glitches)
    // the from mesh becomes the current mesh
    ofMesh *tmp = mesh_current;
    mesh_current = mesh_from;
    mesh_from = tmp;
    
    // reload leader mesh
    loadMocapMesh(*mesh_to, shuffleMesh);
    
    // first load? Load the "trailing" mesh along with the leading mesh
    if(!mesh_from->hasVertices()){
        loadMocapMesh(*mesh_from);
    }
    
    meshMorphAnim.setDuration(meshMorphDuration);
    meshMorphAnim.setCurve(EASE_OUT_BACK); //QUADRATIC_EASE_OUT);
    meshMorphAnim.animateFromTo(meshMorphMin, meshMorphMax);
}

void ofApp::loadMocapMesh(ofMesh &mesh, bool shuffled){
    int vertex_count = mesh.getNumVertices();
    int joint_index = 0;

    vector<ofxPerceptionNeuron::Skeleton> skeletons = neuron.getSkeletons();
    for(auto &skeleton: skeletons){
        const vector<ofxPerceptionNeuron::Joint> *joints = &skeleton.getJoints();

        for(auto &joint: *joints){
            if(joint_index >= vertex_count){
                mesh.addVertex(joint.global_transform.getTranslation()+joint.offset);
                vertex_count++;
            } else {
                mesh.setVertex(joint_index, joint.global_transform.getTranslation()+joint.offset);
            }
            joint_index++;
        }
    }

    // if there are more vertices than joints; remove last vertices from mesh
    while(vertex_count > joint_index){
        vertex_count--;
        mesh.removeVertex(vertex_count);
    }

    if(shuffled){
        for (int i = mesh.getNumVertices() - 1; i >= 0; i --) {
            int index = (int)ofRandom(i);
            ofVec3f tempVertex = mesh.getVertex(index);
            mesh.setVertex(index, mesh.getVertex(i));
            mesh.setVertex(i, tempVertex);
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