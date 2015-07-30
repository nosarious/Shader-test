#include "ofApp.h"

// this code uses two cycling openGL shaders which contain rudimentary perlin noise
// based off of the time code sent to each shader.
//they are intended as a test of samples found on shadertoy.com

#define STRINGIFY(A) #A
//--------------------------------------------------------------
void ofApp::setup(){

    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    
    // shader based off of example from shadertoy that uses
    // information from Ashima: https://github.com/ashima/webgl-noise
    string reactionProgram = STRINGIFY(
                                       
                                       uniform float iGlobalTime;
                                       uniform vec3 iResolution;
                                       uniform vec4 iMouse;
                                       
                                       float time = iGlobalTime*0.025;
                                       
                                       vec4 fragColor;
                                       vec2 fragCoord;
                                       
                                       
                                       vec4 mod289(vec4 x)
                                       {
                                           return x - floor(x * (1.0 / 289.0)) * 289.0;
                                       }
                                       
                                       vec4 permute(vec4 x)
                                       {
                                           return mod289(((x*34.0)+1.0)*x);
                                       }
                                       
                                       vec4 taylorInvSqrt(vec4 r)
                                       {
                                           return 1.79284291400159 - 0.85373472095314 * r;
                                       }
                                       
                                       vec2 fade(vec2 t) {
                                           return t*t*t*(t*(t*6.0-15.0)+10.0);
                                       }
                                       
                                       // Classic Perlin noise
                                       float cnoise(vec2 P)
                                       {
                                           vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
                                           vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
                                           Pi = mod289(Pi); // To avoid truncation effects in permutation
                                           vec4 ix = Pi.xzxz;
                                           vec4 iy = Pi.yyww;
                                           vec4 fx = Pf.xzxz;
                                           vec4 fy = Pf.yyww;
                                           
                                           vec4 i = permute(permute(ix) + iy);
                                           
                                           vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
                                           vec4 gy = abs(gx) - 0.5 ;
                                           vec4 tx = floor(gx + 0.5);
                                           gx = gx - tx;
                                           
                                           vec2 g00 = vec2(gx.x,gy.x);
                                           vec2 g10 = vec2(gx.y,gy.y);
                                           vec2 g01 = vec2(gx.z,gy.z);
                                           vec2 g11 = vec2(gx.w,gy.w);
                                           
                                           vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
                                           g00 *= norm.x;
                                           g01 *= norm.y;
                                           g10 *= norm.z;
                                           g11 *= norm.w;
                                           
                                           float n00 = dot(g00, vec2(fx.x, fy.x));
                                           float n10 = dot(g10, vec2(fx.y, fy.y));
                                           float n01 = dot(g01, vec2(fx.z, fy.z));
                                           float n11 = dot(g11, vec2(fx.w, fy.w));
                                           
                                           vec2 fade_xy = fade(Pf.xy);
                                           vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
                                           float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
                                           return 2.3 * n_xy;
                                       }
                                       
                                       // Classic Perlin noise, periodic variant
                                       float pnoise(vec2 P, vec2 rep)
                                       {
                                           vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
                                           vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
                                           Pi = mod(Pi, rep.xyxy); // To create noise with explicit period
                                           Pi = mod289(Pi);        // To avoid truncation effects in permutation
                                           vec4 ix = Pi.xzxz;
                                           vec4 iy = Pi.yyww;
                                           vec4 fx = Pf.xzxz;
                                           vec4 fy = Pf.yyww;
                                           
                                           vec4 i = permute(permute(ix) + iy);
                                           
                                           vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
                                           vec4 gy = abs(gx) - 0.5 ;
                                           vec4 tx = floor(gx + 0.5);
                                           gx = gx - tx;
                                           
                                           vec2 g00 = vec2(gx.x,gy.x);
                                           vec2 g10 = vec2(gx.y,gy.y);
                                           vec2 g01 = vec2(gx.z,gy.z);
                                           vec2 g11 = vec2(gx.w,gy.w);
                                           
                                           vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
                                           g00 *= norm.x;
                                           g01 *= norm.y;
                                           g10 *= norm.z;
                                           g11 *= norm.w;
                                           
                                           float n00 = dot(g00, vec2(fx.x, fy.x));
                                           float n10 = dot(g10, vec2(fx.y, fy.y));
                                           float n01 = dot(g01, vec2(fx.z, fy.z));
                                           float n11 = dot(g11, vec2(fx.w, fy.w));
                                           
                                           vec2 fade_xy = fade(Pf.xy);
                                           vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
                                           float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
                                           return 2.3 * n_xy;
                                       }
                                       
                                       float fbm(vec2 P, int octaves, float lacunarity, float gain)
                                       {
                                           float sum = 0.0;
                                           float amp = 1.0;
                                           vec2 pp = P;
                                           
                                           int i;
                                           
                                           for(i = 0; i < octaves; i+=1)
                                           {
                                               amp *= gain;
                                               sum += amp * cnoise(pp);
                                               pp *= lacunarity;
                                           }
                                           return sum;
                                           
                                       }
                                       
                                       
                                       float pattern(in vec2 p) {
                                           float l = 2.5;
                                           float g = 0.4;
                                           int oc = 10;
                                           
                                           vec2 q = vec2( fbm( p + vec2(0.0,0.0),oc,l,g),fbm( p + vec2(5.2,1.3),oc,l,g));
                                           vec2 r = vec2( fbm( p + 4.0*q + vec2(1.7,9.2),oc,l,g ), fbm( p + 4.0*q + vec2(8.3,2.8) ,oc,l,g));
                                           return fbm( p + 4.0*r ,oc,l,g);    
                                       }
                                       
                                       float pattern2( in vec2 p, out vec2 q, out vec2 r , in float time)
                                       {
                                           float l = 2.3;
                                           float g = 0.4;
                                           int oc = 10; 
                                           
                                           q.x = fbm( p + vec2(time,time),oc,l,g);
                                           q.y = fbm( p + vec2(5.2*time,1.3*time) ,oc,l,g);
                                           
                                           r.x = fbm( p + 4.0*q + vec2(1.7,9.2),oc,l,g );
                                           r.y = fbm( p + 4.0*q + vec2(8.3,2.8) ,oc,l,g);
                                           
                                           return fbm( p + 4.0*r ,oc,l,g);
                                       }
                                       
                                       void main() {
                                           
                                           vec2 q = gl_FragCoord.xy / vec2(640.0,480.0);
                                           vec2 p = -1.0 + 2.0 * q;
                                           vec2 qq;
                                           vec2 r;
                                           float color = pattern2(p,qq,r,time);
                                           
                                           vec4 c = vec4(color,color,color,color);
                                           c *= 3.5;
                                           
                                           gl_FragColor = c;
                                       }
                                       
                                       );
    
    reaction.setupShaderFromSource(GL_FRAGMENT_SHADER, reactionProgram);
    reaction.linkProgram();
    
    // based on Xor-blobs example from https://www.shadertoy.com/view/lls3zs
    
    string xOrBlobsProgram = STRINGIFY(
                                       
                                       uniform float iGlobalTime;
                                       uniform vec3 iResolution;
                                       uniform vec4 iMouse;
                                       
                                       float time = iGlobalTime*0.025;
                                       
                                    float sfract(float n)
                                    {
                                        return smoothstep(0.0,1.0,fract(n));
                                    }
                                    float rand(vec3 n)
                                    {
                                        return fract(abs(sin(dot(n,vec3(4.3357,-5.8464,6.7645))*52.47))*256.75+0.325);
                                    }
                                    float noise(vec3 n)
                                    {
                                        float h1 = mix(rand(vec3(floor(n.x),floor(n.y),floor(n.z))),rand(vec3(ceil(n.x),floor(n.y),floor(n.z))),sfract(n.x));
                                        float h2 = mix(rand(vec3(floor(n.x),ceil(n.y),floor(n.z))),rand(vec3(ceil(n.x),ceil(n.y),floor(n.z))),sfract(n.x));
                                        float s1 = mix(h1,h2,sfract(n.y));
                                        
                                        h1 = mix(rand(vec3(floor(n.x),floor(n.y),ceil(n.z))),rand(vec3(ceil(n.x),floor(n.y),ceil(n.z))),sfract(n.x));
                                        h2 = mix(rand(vec3(floor(n.x),ceil(n.y),ceil(n.z))),rand(vec3(ceil(n.x),ceil(n.y),ceil(n.z))),sfract(n.x));
                                        float s2 = mix(h1,h2,sfract(n.y));
                                        return mix(s1,s2,fract(n.z));
                                    }
                                       
                                       void main() {
                                           
                                           float c = (noise((vec3(gl_FragCoord.xy/32.0,iGlobalTime+0.5)))-noise(vec3(gl_FragCoord.xy/64.0,-iGlobalTime)));
                                           gl_FragColor = vec4(c,abs(c)*2.0,-c*2.0,1.0);
                                           
                                       }
                                       
                                       );
    
    xOrBlobs.setupShaderFromSource(GL_FRAGMENT_SHADER, xOrBlobsProgram);
    xOrBlobs.linkProgram();
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

    
    //pass variables and draw first shader
    reaction.begin();
        reaction.setUniform3f("iResolution", ofGetWidth()/2, ofGetHeight()/2, 0.0);
        reaction.setUniform1f("iGlobalTime", ofGetElapsedTimef());
        reaction.setUniform4f("iMouse", draggedX, draggedY, clickX, clickY);
        ofRect(0,0,ofGetWidth()/2,ofGetHeight());
    reaction.end();
  
    
    //move origin and draw second shader
    ofPushMatrix();
        ofTranslate(ofGetWidth()/2,0);
        //pass variables and draw second shader
        xOrBlobs.begin();
            xOrBlobs.setUniform3f("iResolution", ofGetWidth()/2, ofGetHeight()/2, 0.0);
            xOrBlobs.setUniform1f("iGlobalTime", ofGetElapsedTimef()*10);
            xOrBlobs.setUniform4f("iMouse", draggedX, draggedY, clickX, clickY);
            ofRect(0,0,ofGetWidth()/2,ofGetHeight());
        xOrBlobs.end();
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    draggedX = x;
    draggedY = y;

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    clickX = x;
    clickY = y;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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
