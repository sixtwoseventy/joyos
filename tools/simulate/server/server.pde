import java.io.*;
import java.net.*;

// used to create font
PFont myFont;

int fRate = 20;
int windowX = 600;
int windowY = 600;
int boundsX = 500;
int boundsY = 500;

float boardWidth = 8.; // dimensions in feet
float boardLength = 8.; // dimensions in feet

int centerX = windowX/2;
int centerY = windowY/2;

// motor parameters
float motorSpeed = 900; // motor speed in rps
// vel=255 (full speed) should correspond to about 1 foot/second with a gear ratio of 75 and 1-in radius wheels

// robot design parameters

float robotWidth=1.; // dimensions in feet
float robotLength=1.; // 

float axleLength=1./2.; // dimensions in feet
float wheelRadius=1./12.; // 
float wheelThickness=0.5/12.;

int gearRatio = 75;

// robot port assignments
int leftMotor = 0, rightMotor = 1;

// robot status variables
volatile float robotX=0.0, robotY=0.0, robotTheta=PI/2.;
float oldX=robotX, oldY=robotY, oldTheta=robotTheta;
volatile float velL=0.0, velR=0.0; // left and right motor velocities
int lastTime=-1;
volatile boolean stuck=false;

// network communication variables
ServerSocket serverSocket = null;

void setup(){
  size(windowX, windowY);
  rectMode(CENTER);
  ellipseMode(CENTER);
  renderBackground();
  frameRate(fRate);
  
  // set up server
  try{
    serverSocket = new ServerSocket(4444);
  } catch (IOException e){
    System.out.println("Could not listen on port: 4444");
    exit();
  }
    
  (new Thread(new RobotSocket())).start();
}

void renderBackground(){
  background (127);
  fill(255);
  stroke(100);
  rectMode(CENTER);
  rect(centerX,centerY,boundsX,boundsY);
}

int scaleX(float x){
  return (int)(((float)boundsX)/((float)boardWidth)*x);
}

int scaleY(float y){
  return (int)(-1.*((float)boundsY)/((float)boardLength)*y);
}

void plotRobot(){
  
  translate(centerX+scaleX(robotX),centerY+scaleY(robotY));
  rotate(PI/2.-robotTheta);
  
  fill(255,0,0);
  ellipse(0,0,scaleX(robotWidth),scaleY(robotLength));
  fill(0,0,0);
  rect(scaleX(axleLength/2.0),0,scaleX(wheelThickness),scaleY(2.*wheelRadius));
  rect(scaleX(-axleLength/2.0),0,scaleX(wheelThickness),scaleY(2.*wheelRadius));
}

float normVel(float intVel){
  return ((float)(intVel))/255. * motorSpeed * wheelRadius / ((float)gearRatio);
}

boolean collision(){
  return  ((robotX-robotWidth/2.)  < -boardWidth/2.)  || 
          ((robotX+robotWidth/2.)  >  boardWidth/2.)  ||
          ((robotY-robotLength/2.) < -boardLength/2.) ||
          ((robotY+robotLength/2.) >  boardLength/2.);
}

void savePosition(){
  oldX = robotX;
  oldY = robotY;  
  oldTheta = robotTheta;
}

void restorePosition(){
  robotX = oldX;
  robotY = oldY;
  robotTheta = oldTheta;
}

boolean sameSign(float a, float b){
  return (a>=0 && b >=0) || (a<=0 && b<=0);
}

void updatePosition(){

  if ((lastTime > 0) && ((velL != 0) || (velR != 0))){
    
      float dt = ((float)(millis()-lastTime)) / 1000;
    
      if (velL == velR){
        float delta = normVel(velL) * dt;
        robotX += delta * cos(robotTheta);
        robotY += delta * sin(robotTheta);
      } else if (sameSign(velL,velR)){
        // center of rotation falls outside robot

        float turnR, dTheta;
        
        if (abs(velR) < abs(velL)) {
        
          if (velL != 0) {
            turnR = axleLength/(normVel(velR)/normVel(velL)-1);
            dTheta=normVel(velL)/turnR * dt;       
          } else {
            turnR = 0;
            dTheta=normVel(velR)/axleLength * dt;
          }
          
          // set robot position equal to center of turning circle
          robotX -= (turnR+axleLength/2.)*sin(robotTheta); 
          robotY += (turnR+axleLength/2.)*cos(robotTheta);
                         
          // then move along the circle
          robotX += (turnR+axleLength/2.)*cos(robotTheta-PI/2.+dTheta);
          robotY += (turnR+axleLength/2.)*sin(robotTheta-PI/2.+dTheta);
          
          robotTheta += dTheta;
          
        } else {
          
          if (velR != 0) {
            turnR = axleLength/(normVel(velL)/normVel(velR)-1);
            dTheta=normVel(velR)/turnR * dt;       
          } else {
            turnR = 0;
            dTheta=normVel(velL)/axleLength * dt;
          }
          
          // set robot position equal to center of turning circle
          robotX += (turnR+axleLength/2.)*sin(robotTheta); 
          robotY -= (turnR+axleLength/2.)*cos(robotTheta);
                         
          // then move along the circle
          robotX += (turnR+axleLength/2.)*cos(robotTheta+PI/2.-dTheta);
          robotY += (turnR+axleLength/2.)*sin(robotTheta+PI/2.-dTheta);
          
          robotTheta -= dTheta;
        }
                                 
      } else {
        // center of rotation falls inside the robot
        
        float lengthR, lengthL, dTheta; // distance from center of rotation to right and left wheels
        
        if (velR > 0){
          // turning left
          lengthR = axleLength * normVel(velR) / (normVel(velR)-normVel(velL));
          dTheta = (normVel(velR) / lengthR) * dt;
          
          // set robot position equal to center of turning circle
          robotX -= (lengthR-axleLength/2.)*sin(robotTheta); 
          robotY += (lengthR-axleLength/2.)*cos(robotTheta);
          
          // then move along the circle
          robotX += (lengthR-axleLength/2.)*cos(robotTheta-PI/2.+dTheta);
          robotY += (lengthR-axleLength/2.)*sin(robotTheta-PI/2.+dTheta);
          
          robotTheta += dTheta;
          
        } else {
          // turning right
          lengthL =  axleLength * normVel(velL) / (normVel(velL)-normVel(velR));
          dTheta = (normVel(velL) / lengthL) * dt;
          
          // set robot position equal to center of turning circle
          robotX += (lengthL-axleLength/2.)*sin(robotTheta); 
          robotY -= (lengthL-axleLength/2.)*cos(robotTheta);
          
          // then move along the circle
          robotX += (lengthL-axleLength/2.)*cos(robotTheta+PI/2.-dTheta);
          robotY += (lengthL-axleLength/2.)*sin(robotTheta+PI/2.-dTheta);
          
          robotTheta -= dTheta;
          
        }
        
      }
      
  }
    
  lastTime = millis();
}

void draw(){
  
  float rps = 2.*PI;

  savePosition();
  updatePosition();
  
  if (collision()){
    restorePosition();
    stuck=true;
  } else{
    stuck=false;
  }
  
  renderBackground();
  plotRobot();

}

// Robot user code 

void motor_set_vel(int port, int vel){
  if (port == leftMotor) velL = vel;
  else if (port == rightMotor) velR = vel;
}

int get_time(){
  return millis();
}

class RobotSocket implements Runnable{
 
  Socket clientSocket = null;
  PrintWriter socketOut = null;
  BufferedReader socketIn = null;
  
  public void run(){
    
    try{
      
      System.out.println("Waiting for client... ");
      
      clientSocket = serverSocket.accept();
      
      System.out.println("Client connected.");
      
      socketOut = new PrintWriter(clientSocket.getOutputStream(), true);
      socketIn = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
    } catch (IOException e){
      System.out.println("Accept failed: 4444");
      exit();
    }
    
    String command = "";
    /* supported commands:
     * exit: 'e'
     * set motor velocity: 'm 0 255' (port number followed by signed velocity)
     * get stuck status: 's' (server will respond with 't' if stuck, 'f' otherwise);
     * get gyro angle: 'a' (server will respond with robotTheta)
     */ 
     
    do{
      try{
        
        command = socketIn.readLine();
        char[] chars = command.toCharArray();
        
        if (chars[0] == 'm'){
            int mot = ((int)chars[2])-48;
            int vel = Integer.parseInt(command.substring(4));
            motor_set_vel(mot, vel);
        }
        else if (chars[0] == 's'){
            if (stuck) socketOut.println("t");
            else socketOut.println("f");
        }
        else if (chars[0] == 'a'){
            socketOut.println("" + robotTheta);
        }
        
      } catch (Exception e){
        command = "";
      }
    } while (!command.equals("e"));
    
    System.out.println("exit");
    
  }
  
}




