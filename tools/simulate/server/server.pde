import java.io.*;
import java.net.*;

int fRate = 16;
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
LinkedList theWalls = new LinkedList();

void setup(){
  size(windowX, windowY);
  rectMode(CENTER);
  ellipseMode(RADIUS);
  renderBackground();
  frameRate(fRate);
  
  Robot theBoard = new Robot();
  theWalls.add(new Rectangle(0.,0.,boardWidth,boardLength, theBoard));
  
  (new Thread(new KinematicsEngine())).start();
  (new Thread(new RobotListener())).start();
}

void renderBackground(){
  background (127);
  fill(255);
  stroke(100);
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
  
  if (inCollision(r.body, theWalls)) return true;
  
  Robot anotherRobot;
  Iterator i = robotList.iterator();
  while (i.hasNext()){
    
    anotherRobot = (Robot)i.next();
    if (r == anotherRobot) continue;
    else if (inCollision(r.body, anotherRobot.body)) return true;
    
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

boolean inCollision(LinkedList obj1, LinkedList obj2){
  
  synchronized(obj1){
    synchronized(obj2){
      Shape x;
      Iterator i = obj1.iterator();
      while(i.hasNext()){
        Shape y;
        Iterator j = obj2.iterator();
        x = (Shape)i.next();
        while(j.hasNext()){
          y = (Shape)j.next();
          if (y.intersectsWith(x)) return true;
        }
      }
    }
  }
  
  return false;
  
}

boolean circleIntersectsWithCircle(Circle c1, Circle c2){
  float cx1 = c1.x+c1.myRobot.robotX;
  float cy1 = c1.y+c1.myRobot.robotY;
  float cx2 = c2.x+c2.myRobot.robotX;
  float cy2 = c2.y+c2.myRobot.robotY;
  
  // save time by avoiding the use of sqrt in the computation
  return ((cx1-cx2)*(cx1-cx2)+(cy1-cy2)*(cy1-cy2)) < (c1.r+c2.r)*(c1.r+c2.r);
}

boolean circleIntersectsWithLine(Circle cir, Line s){
    
  // shift circle center
  float cx = cir.x+cir.myRobot.robotX;
  float cy = cir.y+cir.myRobot.robotY;
  
  // rotate points 
  // TODO replace with matrix math
  
  float lineCos = cos(s.myRobot.robotTheta);
  float lineSin = sin(s.myRobot.robotTheta);
  
  float x1 = (s.x1*lineCos-s.y1*lineSin)+s.myRobot.robotX;
  float y1 = (s.x1*lineSin+s.y1*lineCos)+s.myRobot.robotY;
  float x2 = (s.x2*lineCos-s.y2*lineSin)+s.myRobot.robotX;
  float y2 = (s.x2*lineSin+s.y2*lineCos)+s.myRobot.robotY;
  
  // code adapted from
  // http://blog.csharphelper.com/2010/03/28/determine-where-a-line-intersects-a-circle-in-c.aspx

  float dx = x2 - x1;
  float dy = y2 - y1;
  float a = dx*dx + dy*dy;
  float b = 2*(dx*(x1-cx)+dy*(y1-cy));
  float c = (x1-cx)*(x1-cx)+(y1-cy)*(y1-cy)-cir.r*cir.r;
  
  float det = b*b - 4*a*c;
  
  if ((a <= 0.0000001) || (det < 0)){
    // no solutions
    return false;
  } else if (det == 0){
    // one solution
    float t = -b / (2 * a);
    return (0. <= t) && (t <= 1.);
  } else{
    // two solutions
    float t1 = (float)((-b + sqrt(det)) / (2 * a));
    float t2 = (float)((-b - sqrt(det)) / (2 * a));
    return (0. <= t1) && (t1 <= 1.) ||
           (0. <= t2) && (t2 <= 1.);
  }
  
}

boolean lineIntersectsWithLine(Line s1, Line s2){
  
  // rotate points
  // TODO replace with matrix math
  
  float lineCos = cos(s1.myRobot.robotTheta);
  float lineSin = sin(s1.myRobot.robotTheta);
  
  float s1x1 = (s1.x1*lineCos-s1.y1*lineSin)+s1.myRobot.robotX;
  float s1y1 = (s1.x1*lineSin+s1.y1*lineCos)+s1.myRobot.robotY;
  float s1x2 = (s1.x2*lineCos-s1.y2*lineSin)+s1.myRobot.robotX;
  float s1y2 = (s1.x2*lineSin+s1.y2*lineCos)+s1.myRobot.robotY;
     
  lineCos = cos(s2.myRobot.robotTheta);
  lineSin = sin(s2.myRobot.robotTheta);
     
  float s2x1 = (s2.x1*lineCos-s2.y1*lineSin)+s2.myRobot.robotX;
  float s2y1 = (s2.x1*lineSin+s2.y1*lineCos)+s2.myRobot.robotY;
  float s2x2 = (s2.x2*lineCos-s2.y2*lineSin)+s2.myRobot.robotX;
  float s2y2 = (s2.x2*lineSin+s2.y2*lineCos)+s2.myRobot.robotY;
  
  // s1: y1(t1) = dy1*t1 + y1, x1(t1) = dx1*t1 + x1
  float dy1 = s1y2 - s1y1;
  float dx1 = s1x2 - s1x1;
  // s2: y2(t2) = dy2*t2 + y2, x2(t2) = dx2*t2 + x2
  float dy2 = s2y2 - s2y1;
  float dx2 = s2x2 - s2x1;
  
  float det = dy1*dx2 - dy2*dx1;
  float num1 = dx2*(s2y1-s1y1)-dy2*(s2x1-s1x1);
  float num2 = dx1*(s2y1-s1y1)-dy1*(s2x1-s1x1);
  
  // intersection point is the pair (t1,t2) such that
  // y1(t1) == y2(t2) and x1(t1) == x2(t2)
  //
  // corresponds to:
  // t1 == num1/det
  // t2 == num2/det
    
  if (det == 0){
    // parallel lines
    if (num1 != 0) return false;
    else{
      // every value of t1, t2 result in common points, so there is
      // an intersection only if some part of the segments 
      // t1 = [0,1] and t2 = [0,1] overlap
      if (dx2 != 0){
        return segmentOverlap(s1x1,s1x2,s2x1,s2x2);
      } else{
        return segmentOverlap(s1y1,s1y2,s2y1,s2y2);
      }
    }
  } else{
    float t1 = num1/det;
    float t2 = num2/det;
    return (0. <= t1) && (t1 <= 1.) && (0. <= t2) && (t2 <= 1.); 
  }
  
}

boolean segmentOverlap(float a, float b, float x, float y){
  // returns true if [a,b] and [x,y] overlap
  if (a < b){
    if (x < y){
      if (x < a) return a <= y;
      else return (x <= b) && (b <= y);
    } else{
      if (y < a) return a <= x;
      else return (y <= b) && (b <= x);
    }
  } else{
    if (x < y){
      if (x < b) return b <= y;
      else return (x <= a) && (a <= y);
    } else{
      if (y < b) return b <= x;
      else return (y <= a) && (a <= x);
    }
  }
}
  

public abstract class Shape{
  
  public Robot myRobot;  
  
  public abstract boolean intersectsWith(Shape x);
  public abstract void plot();
  
}

class Rectangle extends Shape{
  
  public LinkedList myLines;
  public float x, y, width, height;
  
  public Rectangle(float x, float y, float width, float height, Robot myRobot){
    myLines = new LinkedList();
    this.x = x;
    this.y = y;
    this.width = width;
    this.height = height;
    myLines.add(new Line(x - width/2., y - height/2., x - width/2., y + height/2., myRobot));
    myLines.add(new Line(x - width/2., y + height/2., x + width/2., y + height/2., myRobot));
    myLines.add(new Line(x + width/2., y + height/2., x + width/2., y - height/2., myRobot));
    myLines.add(new Line(x + width/2., y - height/2., x - width/2., y - height/2., myRobot));     
  }
  
  public boolean intersectsWith(Shape x){
        
    Iterator i = myLines.iterator();
    while (i.hasNext()){
      if (((Line)i.next()).intersectsWith((Shape)x)) return true;
    }
    
    return false;
    
  }
  
  public void plot(){
    fill(255,0,0);
    rect(scaleX(x),scaleY(y),scaleX(width),scaleY(height));
  }
  
}

class Circle extends Shape{
  
  public float x, y, r;
  
  public Circle(float x, float y, float r, Robot myRobot){
    this.x = x;
    this.y = y;
    this.r = r;
    super.myRobot = myRobot;
  }
  
  public boolean intersectsWith(Shape x){
        
    String type = x.getClass().getName();
    
    if(type.equals("server$Rectangle")){
      return ((Rectangle)x).intersectsWith(this);
    } else if (type.equals("server$Circle")){
      return circleIntersectsWithCircle((Circle)x, this);
    } else if (type.equals("server$Line")){
      return circleIntersectsWithLine(this, (Line)x);
    } else {
      return false;
    }
    
  }
  
  public void plot(){
    fill(255,0,0);
    ellipse(scaleX(x),scaleY(y),scaleX(r),scaleY(r));
  }
  
}

class Line extends Shape{
  
  public float x1, y1, x2, y2;
  
  public Line(float x1, float y1, float x2, float y2, Robot myRobot){
    this.x1 = x1;
    this.y1 = y1;
    this.x2 = x2;
    this.y2 = y2;
    super.myRobot = myRobot;
  }
  
  public boolean intersectsWith(Shape x){
        
    String type = x.getClass().getName();
    
    if(type.equals("server$Rectangle")){
      return ((Rectangle)x).intersectsWith(this);
    } else if (type.equals("server$Circle")){
      return circleIntersectsWithLine((Circle)x, this);
    } else if (type.equals("server$Line")){
      return lineIntersectsWithLine((Line)x, this);
    } else {
      return false;
    }
    
  }
  
  public void plot(){
    line(scaleX(x1),scaleY(y1),scaleX(x2),scaleY(y2));
  }
    
}

class Robot{
  
  // motor parameters
  public float motorSpeed = 900; // motor speed in rps
  public float motorOffset = 0.; // difference in speed between the motors (in the range of -255 -> 255)
  
  // vel=255 (full speed) should correspond to about 1 foot/second with a gear ratio of 75 and 1-in radius wheels

  // robot design parameters

  public float robotRadius=0.5; // dimensions in feet

  public float axleLength=1./2.; // dimensions in feet
  public float wheelRadius=1./12.; // 
  public float wheelThickness=0.5/12.;
  
  public int gearRatio = 75;
  public int encoderRatio = gearRatio;  // encoder is on the wheel axle
  
  // robot port assignments
  public int leftMotor = 0, rightMotor = 1;
  public int leftEncoderPort = 26, rightEncoderPort = 27;
  
  // robot status variables
  public volatile float robotX=0.0, robotY=0.0, robotTheta=PI/2., rightEncoder = 0.0, leftEncoder = 0.0;
  public float oldX=robotX, oldY=robotY, oldTheta=robotTheta, oldRightEncoder = rightEncoder, oldLeftEncoder = leftEncoder;
  public volatile float velL=0.0, velR=0.0; // left and right motor velocities
  public int lastTime=-1;
  public volatile boolean stuck=false;
  
  // list of all shapes comprising the body
  public volatile LinkedList body = new LinkedList();
  
  public Robot(){
    body.add(new Circle(0., 0., robotRadius, this));
    // body.add(new Rectangle(0., 0., robotRadius*2., robotRadius*2., this));
  }
  
  public void motor_set_vel(int port, int vel){
    if (port == leftMotor) velL = vel;
    else if (port == rightMotor) velR = (vel+motorOffset);
  }
  
  public int encoder_read(int port){
    if (port == rightEncoderPort) return (int)rightEncoder;
    else if (port == leftEncoderPort) return (int)leftEncoder;
    else return 0;
  }
    
  public void savePosition(){
    oldX = robotX;
    oldY = robotY;  
    oldTheta = robotTheta;
    oldRightEncoder = rightEncoder;
    oldLeftEncoder = leftEncoder;
  }
  
  public void restorePosition(){
    robotX = oldX;
    robotY = oldY;
    robotTheta = oldTheta;
    oldRightEncoder = rightEncoder;
    oldLeftEncoder = leftEncoder;
  }
  
  public void updatePosition(){
  
    if ((lastTime > 0) && ((velL != 0) || (velR != 0))){
      
        float dt = ((float)(millis()-lastTime)) / 1000;
        rightEncoder += encoderRate(velR) * dt;
        leftEncoder += encoderRate(velL) * dt;
      
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
    
    synchronized(body){
      
      Shape s;
      Iterator i = body.iterator();
       
      while (i.hasNext()){
        
        s = (Shape)i.next();
        s.plot();
        
      } 

    }

    // draw the wheels
    
    fill(0,0,0);
    rect(scaleX(axleLength/2.0),0,scaleX(wheelThickness),scaleY(2.*wheelRadius));
    rect(scaleX(-axleLength/2.0),0,scaleX(wheelThickness),scaleY(2.*wheelRadius));
    
    popMatrix();
    
  }

  float normVel(float intVel){
    return ((float)(intVel))/255. * motorSpeed * wheelRadius / ((float)gearRatio);
  }
  
  float encoderRate(float intVel){
    return 6. * (((float)(intVel))/255. * motorSpeed) / (2*PI*((float)encoderRatio));
  }
  
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
     * configure robot: 'c'
     * --> 'c r' : change shape to rectangle
     * --> 'c c' : change shape to circle
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
        else if (chars[0] == 'd'){
            // TODO implement distance sensor
            int port = Integer.parseInt(command.substring(2));
            socketOut.println("" + 0);
        }
        else if (chars[0] == 'e'){
            // TODO implement encoder
            int port = Integer.parseInt(command.substring(2));
            socketOut.println("" + r.encoder_read(port));
        }
        else if (chars[0] == 'x'){
            // TODO implement x-position sending
            int port = Integer.parseInt(command.substring(2));
            socketOut.println("" + 0);
        }
        else if (chars[0] == 'y'){
            // TODO implement y-position sending
            int port = Integer.parseInt(command.substring(2));
            socketOut.println("" + 0);
        }
        else if (chars[0] == 'c'){
            // configure shape
            if (chars[2] == 'c'){
              synchronized(r.body){
                r.body.clear();
                r.body.add(new Circle(0, 0, r.robotRadius, r));
              }
            } else if (chars[2] == 'r'){
              synchronized(r.body){
                r.body.clear();
                r.body.add(new Rectangle(0, 0, r.robotRadius*2., r.robotRadius*2., r));
              }
            }
        }
        
      } catch (Exception e){
        command = "";
      }
    } while (!command.equals("e"));
    
    System.out.println("exit");
    
  }
  
}




