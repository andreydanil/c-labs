////////////////////////////////////////////////////////////////////////
//
// CS411
//
// Assignment 2 : scanfill raster graphics program
//
////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>


//
// type definitions
//

typedef unsigned char Byte;

typedef struct
{
    Byte r,g,b;
} Pixel;

typedef struct
{
    Pixel **data;
    int   rows,cols;
} Image;

typedef struct
{
    Image fbuff;                  // frame buffer
    Pixel fillColor;              // fill color used by setPixel
    Pixel clearColor;             // clear color used by clearFbuff
    void(*displayHandler)(void);  // current display handler pointer
} Context;

//////////////////////////////////////////////////////////////////////////
// data structure of edge
typedef struct Edge{
    int xmax;  // max x
    float y;   // the y coordinate with scanline
    float dy; //  slope
    struct Edge * pNext; // next edge
}Edge, * LPEdge;

//////////////////////////////////////////////////////////////////////////
typedef enum {NOOBJ, RECT, CIRC, PLINE, PGON} objType;

typedef struct{
    float x,y;      // x and y coordinates of the point
} Point;

typedef struct{
    int   type;     // object type
    float tx,ty;    // translation
    float rotAng;   // rotation
    float sx,sy;    // scale
    float r,g,b;    // color
    float fillFlag; // fill flag
    int   ptsNum;   // number of coordinates in the vertex array
    float param;    // shape parameter (e.g. radius of circle)
    Point pts[100]; // array of vertices
} Obj;


//
// global variables
//
Context C={0,0,0,       // fbuff
    0,0,0,       // fillColor
    255,255,255, // clearColor
    0};          // displayHandler

Obj objLst[100];        // object list
int objsNum;            // the total number of objects

//
// local function prototypes
//
void _reshape(int w, int h);
void _display(void);



////////////////////////////////////////////////////////////////////////
// service functions
////////////////////////////////////////////////////////////////////////


//
// Read object list - same as in assignment 1
//

int readObjLst(char *fn, Obj *objLst)
{
    FILE* fin;
    char c,c1,c2;
    int i;
    Obj *o;
    
    int objsNum=0;
    
    // set default values
    Obj curDefault= {NOOBJ, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0, 0.0};
    
    // open input file
    if(! (fin=fopen(fn,"r")) ){
        printf("\nCan't open file %s\n\7",fn);
        return 0;
    }
    
    // read objects
    while(fscanf(fin,"%c%c",&c1,&c2)!=EOF) {
        
        // remove white space
        while(c1== ' ' || c1 =='\n'){
            c1=c2;
            if(fscanf(fin,"%c",&c2)==EOF) return(objsNum);
        }
        switch(c1){
                
                // add object
            case 'a':
                o = &objLst[objsNum];
                *o = curDefault;
                
                switch(c2){
                    case 'l': // polyline
                    case 'p': // polygon
                        o->type=PLINE;
                        if(c2=='p') o->type=PGON;
                        fscanf(fin,"%d",&o->ptsNum);
                        for(i=0;i<o->ptsNum;i++)fscanf(fin,"%f%f",&o->pts[i].x,&o->pts[i].y);
                        objsNum++;
                        break;
                    case 'r': // rectangle
                        o->type=RECT;
                        fscanf(fin,"%f%f%f%f",&o->pts[0].x,&o->pts[0].y,&o->pts[1].x,&o->pts[1].y);
                        o->ptsNum=2;
                        objsNum++;
                        break;
                    case 'c': // circle
                        o->type=CIRC;
                        fscanf(fin,"%f%f%f",&o->pts[0].x,&o->pts[0].y,&o->param);
                        o->ptsNum=1;
                        objsNum++;
                        break;
                    default:
                        printf("Warning: unknown object type (%c)\n\7",c2);
                }
                break;
                
                // set property
            case 's':
                o = &curDefault;
                
                switch(c2){
                    case 't': fscanf(fin,"%f%f",&o->tx,&o->ty);       break; // translation
                    case 'r': fscanf(fin,"%f",&o->rotAng);            break; // rotation
                    case 's': fscanf(fin,"%f%f",&o->sx,&o->sy);       break; // scale
                    case 'c': fscanf(fin,"%f%f%f",&o->r,&o->g,&o->b); break; // color
                    case 'f': fscanf(fin,"%f",&o->fillFlag);          break; // fill
                    default:
                        printf("Warning: unknown object property (%c)\n\7",c2);
                }
                break;
                
        }
        
        // remove trailing characters to end of line
        while(fread(&c,1,1,fin)) if (c== '\n') break;
    }
    
    // return number of objects read
    fclose(fin);
    return(objsNum);
}


//
// Dump object list - same as in assignment 1
//

void dumpObjLst(int objsNum, Obj *objLst)
{
    int i,j;
    Obj *o;
    
    // print the opbect type and its points
    for(i=0;i<objsNum;i++){
        o = &objLst[i];
        
        printf("*** %d:", o->type);
        for(j=0;j<o->ptsNum;j++) printf(" (%3.1f,%3.1f)",o->pts[j].x, o->pts[j].y);
        printf("\n");
    }
}

//
// Set pixel (x,y) in the frame buffer to the current fill color. Assume
// a coordinate system with (0,0) at the lower left corner that extends
// to (FBUFF_WIDTH-1) in x and to (FBUFF_HEIGHT-1) in y. Perform range
// check to avoid illegal memory access.
//
//inline
void setPixel(int x, int y)
{
    // check if the frame buffer was allocated
    if(!C.fbuff.data) {
        fprintf(stderr,"Error: access to non-allocated buffer\n\7");
        return;
    }
    
    // check range
    if(x<0 || x>= C.fbuff.cols || y<0 || y>=C.fbuff.rows) return;
    
    // fill pixel. Since the image is inverted by glDrawPixels y-inversion should be avoided.
    // C.fbuff.data[FBUFF_HEIGHT-y][x]=fillColor;
    C.fbuff.data[y][x]=C.fillColor;
}



//
// Create a rendering window of hXw pixels (w=width, h=height)
//

//inline
void createWindow(int h, int w, void(*func)(void))
{
    int i,j;
    Pixel *data;
    int argc=1;
    char *argv[1];
    
    // set the display handler
    C.displayHandler=func;
    
    // allocate the frame buffer
    C.fbuff.rows=h;
    C.fbuff.cols=w;
    C.fbuff.data=new Pixel*[h];
    data=new Pixel[w*h];
    for(i=0;i<h;i++) C.fbuff.data[i]=data + i*w;
    
    // clear the frame buffer
    for(i=0;i<C.fbuff.rows;i++) for(j=0;j<C.fbuff.cols;j++)
        C.fbuff.data[i][j]=C.clearColor;
    
    // initialize openGL
    argv[0]= new char[200];
    argv[0][0]=0;
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize (w, h);
    glutInitWindowPosition (100, 100);
    glutCreateWindow("Raster Window");
    glutDisplayFunc(_display);
    glutReshapeFunc(_reshape);
    glutMainLoop();
}


void _reshape(int w, int h)
{
    glViewport (0, 0, w, h);
}



void _display(void)
{
    int i;
    
    // initialize modelview and projection transformations
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glClear (GL_COLOR_BUFFER_BIT);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glOrtho(0.0, C.fbuff.cols, 0.0, C.fbuff.rows, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity ();
    
    // call the customized display handler
    if(!C.displayHandler) fprintf(stderr,"Error: call to unregistered display handler\n\7");
    else (*C.displayHandler)();
    
    // display the frame buffer
    glRasterPos2f(0, 0);
    glDrawPixels(C.fbuff.cols, C.fbuff.rows, GL_RGB, GL_UNSIGNED_BYTE, C.fbuff.data[0]);
    glFlush();
}




////////////////////////////////////////////////////////////////////////
// main program YOUR CODE HERE
////////////////////////////////////////////////////////////////////////

#define ROUND(a) ((int)(a+0.5))

void drawLineDDA(Point &s, Point &e) // from Hearn and Baker
{
    int xa=int(s.x), ya=int(s.y);
    int xb=int(e.x), yb=int(e.y);
    printf("drawing line (%d,%d) -> (%d,%d)\n",xa,ya,xb,yb);
    
    int dx = xb - xa, dy = yb - ya, steps, k;
    float xIncrement, yIncrement, x = xa, y = ya;
    
    if (abs (dx) > abs (dy)) steps = abs (dx);
    else steps = abs (dy);
    xIncrement = dx / (float) steps;
    yIncrement = dy / (float) steps;
    
    setPixel (ROUND(x), ROUND(y));
    for (k=0; k<steps; k++) {
        x += xIncrement;
        y += yIncrement;
        setPixel (ROUND(x), ROUND(y));
    }
}



void drawPolygon(Obj *o)
{
    int i;
    
    Pixel colorTab[6]={255,   0,   0,
        0,   255,   0,
        0,     0, 255,
        255, 255,   0,
        0, 255, 255,
        255,   0, 255};
    
    // draw the polygon with alternating edge colors
    for(i=0; i < o->ptsNum -1 ;i++){
        C.fillColor=colorTab[i%6];          // change the fill color
        drawLineDDA(o->pts[i],o->pts[i+1]); // draw an edge
    }
    C.fillColor=colorTab[i%6];            // change the fill color
    drawLineDDA(o->pts[i],o->pts[0]);
}


void drawFilledPolygon(Obj *o)
{
    
    // YOUR CODE HERE
    // (implement a vertical polygon scanfill algorithm)
    //
    
    //drawPolygon(o);// replace this with an actual polygon filling algorithm
    
    
    //////////////////////////////////////////////////////////////////////////
    int i=0,j=0,k=0;
    int x0=0,x1=0;  // xmin,xmax of scanline(polygon)
    LPEdge pAET=NULL; // head pointer of Active Edge Table
    LPEdge * pET=NULL;  // head pointer of Edge Table
    
    pAET=new Edge; // Initial Active Edge Table
    pAET->pNext=NULL;
    
    // get scanline boundaries (polygon's boundaries in the X direction)
    x0=x1=(int)o->pts[0].x;
    for(i=1;i<o->ptsNum;i++)
    {
        if((int)o->pts[i].x<x0)
            x0=(int)o->pts[i].x;
        else if((int)o->pts[i].x>x1)
            x1=(int)o->pts[i].x;
    }
    if(x0>=x1) return;
    
    //Initial Edge Table
    pET=new LPEdge[x1-x0+1];
    for(i=0;i<=x1-x0;i++)
    {
        pET[i]= new Edge;
        pET[i]->pNext=NULL;
    }
    
    for(i=0;i<o->ptsNum;i++)
    {
        j=(i+1)%o->ptsNum; // next point of edge
        if((int)o->pts[i].x != (int)o->pts[j].x)// add to Edge Table, if it's not vertical
        {
            LPEdge peg;   // the pointer pointed to the edge
            LPEdge ppeg;  // the pointer pointed to the pointer pointed to the edge
            
            // make an edge
            peg =new Edge;
            k=((int)o->pts[i].x>(int)o->pts[j].x)?i:j;
            peg->xmax=(int)o->pts[k].x; // xmax of the edge
            k=(k==j)?i:j;
            peg->y=o->pts[k].y; // the y value of intersection of scanline and the edge
            if((int)o->pts[i].x != (int)o->pts[j].x)
                peg->dy=(o->pts[i].y-o->pts[j].y)/(o->pts[i].x-o->pts[j].x);// slope
            peg->pNext=NULL;
            
            //insert edge
            ppeg=pET[(int)o->pts[k].x-x0];
            while(ppeg->pNext)
                ppeg=ppeg->pNext;
            ppeg->pNext=peg;
        }// end if
    }// end for i
    
    ///////////////////////////////////////////////////
    // scan vertically
    ///////////////////////////////////////////////////
    
    for(i=x0;i<=x1;i++)
    {
        LPEdge peg0=pET[i-x0]->pNext;
        LPEdge peg1=pET[i-x0];
        if(peg0)// find new edge
        {
            while(peg1->pNext)
                peg1=peg1->pNext;
            peg1->pNext=pAET->pNext;
            pAET->pNext=peg0;
        }
        
        // sort Active Edge Table by y values
        peg0=pAET;
        while(peg0->pNext)
        {
            LPEdge pegmax=peg0;
            LPEdge peg1=peg0;
            LPEdge pegi=NULL;
            
            while(peg1->pNext)
            {
                if(peg1->pNext->y>pegmax->pNext->y)
                    pegmax=peg1;
                peg1=peg1->pNext;
            }
            pegi=pegmax->pNext;
            pegmax->pNext=pegi->pNext;
            pegi->pNext=pAET->pNext;
            pAET->pNext=pegi;
            if(peg0 == pAET)
                peg0=pegi;
        }
        
        // remove edges ending at this scanline and update intersection points of active edges
        peg0=pAET;
        while(peg0->pNext)
        {
            if(peg0->pNext->xmax < i+2)
            {
                peg1=peg0->pNext;
                peg0->pNext=peg0->pNext->pNext; //delete
                delete peg1;
                continue;
            }
            peg0->pNext->y+=peg0->pNext->dy; //update intersection points by dy
            peg0=peg0->pNext;
        }
        
        
        // traversal Active Edge Table, and draw lines to fill polygon
        peg0=pAET;
        Point s,e;
        while(peg0->pNext)
        {
            if(peg0->pNext->pNext)
            {
                Pixel colorTab[6]={255,   0,   0,
                    0, 255,   0,
                    0,   0, 255,
                    255, 255,   0,
                    0, 255, 255,
                    255,   0, 255};
                
                
                C.fillColor=colorTab[i%6];          // change the fill color
                
                
                
                
                s.y = peg0->pNext->y;
                e.y = peg0->pNext->pNext->y;
                e.x = s.x = (float)i;
                drawLineDDA(s,e);
                peg0=peg0->pNext->pNext;
            }
            else
                break;
        }
    }
    
    ///////////////////////////////////////////////////
    // end  scan vertically
    ///////////////////////////////////////////////////
    
    
    // delete Edge Table
    for(i=0;i<x1-x0;i++)
        if(pET[i])
            delete pET[i];
    if(pAET)
        delete pAET;
    if(pET)
        delete[] pET;
    
    
    
}



//
// display handler that draws filled/unfilled polygons
//
void myDisplay(void)
{
    int i;
    Obj *o;
    
    // scan the list of objects and draw only filled/unfilled polygons
    // IGNORE ALL OBJECT ATTRIBUTES
    for (i=0;i<objsNum;i++)
    {   
        o = &(objLst[i]);   
        if(o->type != PGON) continue;   
        
        if(o->fillFlag == 0) drawPolygon(o);   
        else drawFilledPolygon(o);   
    }   
}   



int main(int argc, char *argv[])   
{   
    // check transferred arguments   
    if(argc<2){   
        printf("\nUsage scanfill <drawFile>\n\n");   
        exit(0);   
    }   
    
    // load, dump, and process the object file   
    objsNum=readObjLst(argv[1],objLst);   
    dumpObjLst(objsNum, objLst);   
    
    
    // create a 400x600 pixels raster display that is handled by myDisplay   
    createWindow(400, 600, myDisplay);   
    
    return 0;   
}   


////////////////////////////////////////////////////////////////////////   
// EOF   
////////////////////////////////////////////////////////////////////////