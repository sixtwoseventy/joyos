import java.io.*;
import java.net.*;

int fRate = 20;
int windowX = 600;
int windowY = 600;
int boundsX = 500;
int boundsY = 500;

float boardWidth = 8.; // dimensions in feet
float boardLength = 8.; // dimensions in feet

int centerX = windowX/2;
int centerY = windowY/2;

// list of all robots currently on the playing table
volatile LinkedList robotList = new LinkedList();

void setup(){
  size(windowX, windowY);
  rectMode(CENTER);
  ellipseMode(CENTER);
  renderBackground();
  frameRate(fRate);
    
  (new Thread(new KinematicsEngine())).start();
  (new Thread(new RobotListener())).start();
}

void renderBackground(){
  background (127);
  fill(255);
  stroke(100);
  rectMode(CENTER);
  rect(centerX,centerY,boundsX,boundsY);
}

void draw(){
  
  renderBackground();
  
  Robot r;
  Iterator i = robotList.iterator();
  
  while (i.hasNext()){
    r = (Robot)i.next();
    r.plotRobot();
  }

}

// determines if the current robot collides with the walls or
// any other robot.  assumes that the robotList has already been
// locked by the calling thread.
boolean collision(Robot r){
  
  if (r.wallCollision()) return true;
  
  Robot anotherRobot;
  Iterator i = robotList.iterator();
  while (i.hasNext()){
    
    anotherRobot = (Robot)i.next();
    if (r == anotherRobot) continue;
    else if (r.collidesWith(anotherRobot)) return true;
    
  }
  
  return false;
  
}

class KinematicsEngine implements Runnable{
  
  public void run(){
    while(true){
      
        synchronized(robotList){
      
          Robot r = null;
          Iterator i = robotList.iterator();
        
          while(i.hasNext()){
        
            r = (Robot)i.next();
            r.savePosition();
            r.updatePosition();
            
            if (collision(r)){
              r.restorePosition();
              r.stuck=true;
            } else{
              r.stuck=false;
            }
          }
        }
        
        try{
          Thread.sleep(10);
        } catch(Exception e){
        }
    }
  }
  
}

class Robot{
  
  // motor parameters
  public float motorSpeed = 900; // motor speed in rps
  
  // vel=255 (full speed) should correspond to about 1 foot/second with a gear ratio of 75 and 1-in radius wheels

  // robot design parameters

  public float robotWidth=1.; // dimensions in feet
  public float robotLength=1.; // 

  public float axleLength=1./2.; // dimensions in feet
  public float wheelRadius=1./12.; // 
  public float wheelThickness=0.5/12.;
  
  public int gearRatio = 75;
  
  // robot port assignments
  public int leftMotor = 0, rightMotor = 1;
  
  // robot status variables
  public volatile float robotX=0.0, robotY=0.0, robotTheta=PI/2.;
  public float oldX=robotX, oldY=robotY, oldTheta=robotTheta;
  public volatile float velL=0.0, velR=0.0; // left and right motor velocities
  public int lastTime=-1;
  public volatile boolean stuck=false;
  
  public void motor_set_vel(int port, int vel){
    if (port == leftMotor) velL = vel;
    else if (port == rightMotor) velR = vel;
  }
  
  public boolean wallCollision(){
    return  ((robotX-robotWidth/2.)  < -boardWidth/2.)  || 
            ((robotX+robotWidth/2.)  >  boardWidth/2.)  ||
            ((robotY-robotLength/2.) < -boardLength/2.) ||
            ((robotY+robotLength/2.) >  boardLength/2.);
  }
  
  public boolean collidesWith(Robot r){
    float d = sqrt((robotX-r.robotX)*(robotX-r.robotX)+(robotY-r.robotY)*(robotY-r.robotY));
    return d < (robotWidth/2. + r.robotWidth/2.);
  }
  
  public void savePosition(){
    oldX = robotX;
    oldY = robotY;  
    oldTheta = robotTheta;
  }
  
  public void restorePosition(){
    robotX = oldX;
    robotY = oldY;
    robotTheta = oldTheta;
  }
  
  public void updatePosition(){
  
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
  
  public void plotRobot(){
  
    pushMatrix();
    
    translate(centerX+scaleX(robotX),centerY+scaleY(robotY));
    rotate(PI/2.-robotTheta);
    
    fill(255,0,0);
    ellipse(0,0,scaleX(robotWidth),scaleY(robotLength));
    fill(0,0,0);
    rect(scaleX(axleLength/2.0),0,scaleX(wheelThickness),scaleY(2.*wheelRadius));
    rect(scaleX(-axleLength/2.0),0,scaleX(wheelThickness),scaleY(2.*wheelRadius));
    
    popMatrix();
    
  }
  
  // convenience functions for robot math

  int scaleX(float x){
    return (int)(((float)boundsX)/((float)boardWidth)*x);
  }
  
  int scaleY(float y){
    return (int)(-1.*((float)boundsY)/((float)boardLength)*y);
  }
  
  boolean sameSign(float a, float b){
    return (a>=0 && b >=0) || (a<=0 && b<=0);
  }
  
  float normVel(float intVel){
    return ((float)(intVel))/255. * motorSpeed * wheelRadius / ((float)gearRatio);
  }
  
}

class RobotListener implements Runnable{  
  
  public void run(){
    
    Socket clientSocket = null;
    ServerSocket serverSocket = null;
    PrintWriter socketOut = null;
    BufferedReader socketIn = null;
    Robot r = null;
    
    // set up server
    try{
      serverSocket = new ServerSocket(4444);
    } catch (IOException e){
      System.out.println("Could not listen on port: 4444");
      exit();
    }
    
    while(true){
      
          try{
            System.out.println("Waiting for client... ");
            clientSocket = serverSocket.accept();
            System.out.println("Client connected.");
            socketOut = new PrintWriter(clientSocket.getOutputStream(), true);
            socketIn = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            
            (new Thread(new RobotSocket(socketIn, socketOut))).start();
            
          }
          
          catch (IOException e){
           System.out.println("Accept failed: 4444");
           exit();
          }
    }
    
  }
  
}

class RobotSocket implements Runnable{
 
  PrintWriter socketOut = null;
  BufferedReader socketIn = null;
  Robot r = null;
  
  public RobotSocket(BufferedReader socketIn, PrintWriter socketOut){
    this.socketIn = socketIn;
    this.socketOut = socketOut;
  }
  
  public void run(){
    
    boolean stillStuck = true;
    r = new Robot();
    
    do{
      
      synchronized(robotList){
        
        if (!collision(r)){
          robotList.add(r);
          stillStuck = false;
        }
        
      }
      
    } while(stillStuck);    

    
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
            r.motor_set_vel(mot, vel);
        }
        else if (chars[0] == 's'){
            if (r.stuck) socketOut.println("t");
            else socketOut.println("f");
        }
        else if (chars[0] == 'a'){
            socketOut.println("" + r.robotTheta);
        }
        
      } catch (Exception e){
        command = "";
      }
    } while (!command.equals("e"));
    
    System.out.println("exit");
    
  }
  
}




