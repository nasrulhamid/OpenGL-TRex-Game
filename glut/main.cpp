/*
 * T-REX ROBO KOM300
 * 
 * G64164017	NASRUL HAMID
 * G64164021	MUHAMMAD JAKA UTAMA
 * G64164053	ISMAIL ADIMA
 * 
 */

#include <iostream>
#include <iomanip>
#include <ctime>
#include <GL/glut.h>
#include "tga.h"
#include "tga.c"

using namespace std;

bool stRun = true, stKika=true; 
double tmTick=0, tckKika=0;

float positionX=0, positionY=0;     // Position of the character
float velocityX=4.0f, velocityY=0.0f;     // Velocity of the character
float gravity = 6.0f;           // How strong is gravity
bool stOnGround = true, stCollision = false, stInitGuide=true, stPause=false;

int obsI=0; 
double obsXpos, obsXposmut, obsXpos2, obsXposmut2;
double grndXpos;
double chrXpos=3.0;

const float tSpeedMin =1.0f, tSpeedMax = 3.0f, tStep=0.1f;
double tSpeed = tSpeedMin; 
long score=0, scoreNotif=500, hiscore=0;

void latar(){
   glBegin(GL_POLYGON);
		glColor3ub(132,208,228);
		glVertex2f(0, 20); //vertex 1 x,y kebawah - kekiri -
		glColor3ub(118,207,230);
		glVertex2f(80, 20);
		glColor3ub(233, 244, 237);
		glVertex2f(80, 0);
		glColor3ub(218, 236, 241);
		glVertex2f(0, 0);
	glEnd();
}

void load_BMP_texture(char *filename) {

    FILE *file;
    short int bpp;
    short int planes;
    long size;
    unsigned int texture;

    long imwidth;
    long imheight;
    char *imdata;

    file = fopen(filename, "rb");
    fseek(file, 18, SEEK_CUR);

    fread(&imwidth, 4, 1, file);
    fread(&imheight, 4, 1, file);
    size = imwidth * imheight * 3;

    fread(&bpp, 2, 1, file);
    fread(&planes, 2, 1, file);

    fseek(file, 24, SEEK_CUR);
    imdata = (char *)malloc(size);

    fread(imdata, size, 1, file);

	char temp;
    for(long i = 0; i < size; i+=3){
        temp = imdata[i];
        imdata[i] = imdata[i+2];
        imdata[i+2] = temp;
    }

    fclose(file);

    glGenTextures(1, &texture); // then we need to tell OpenGL that we are generating a texture
    glBindTexture(GL_TEXTURE_2D, texture); // now we bind the texture that we are working with

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imwidth, imheight, 0, GL_RGB, GL_UNSIGNED_BYTE, imdata);

    free(imdata); // free the texture
}

void glTga(void)
{
	glEnable (GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	char f[100];
	strcpy(f,"sand.tga");
	loadTGA(f, 1);//load gambar texture ke bidang
	strcpy(f,"box.tga");
	loadTGA(f, 2);//load gambar texture ke bidang
	//~ strcpy(f,"dskin.tga");
	//~ loadTGA(f, 3);//load gambar texture ke bidang
	//load_BMP_texture("dskin.bmp");
}

void grid(int baris, int kolom) {

    // draw grid on floor (0,0,0)
    int i;

    int kiri  = -kolom / 2;
    int kanan = kolom / 2;
    int bawah = -baris / 2;
    int atas  = baris / 2;

    glBegin(GL_LINES);
    glLineWidth(1);

    for (i = 0; i < baris + 1; i++) {
        glVertex2f(kiri, baris / 2 - i);
        glVertex2f(kanan, baris / 2 - i);
    }

    for (i = 0; i < kolom + 1; i++) {
        glVertex2f(kolom / 2 - i, bawah);
        glVertex2f(kolom / 2 - i, atas);
    }

    glEnd();
}

void changeSize(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)	h = 1;

	float ratio =  w * 3.15 / h;
	//~ float ratio = w/80;
	
    //ratio = width / (float) height;
    glViewport(0, 0, w, h);
    
    glClear(GL_COLOR_BUFFER_BIT);

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);
	

	// Set the correct perspective.
	//gluPerspective(90,ratio,1,100);
	//gluPerspective(0,1,1,100);
	
	//glOrtho(-w/20, w/20, -h/20, h/20, 1.f, -1.f);
	//~ glOrtho(0, (float)w/ratio, 0, (float)h/ratio, 1.f, -1.f);
	glOrtho(0, w/ratio, 0, h/ratio, 1.f, -1.f);
	//glOrtho(-40, 40, -20, 20, 1.f, -1.f);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void strPrint(int x, int y, float r, float g, float b, void* font, string str)
{
	glColor3f( r, g, b );
	glRasterPos2f(x, y);
	int len, i;
	len = (int)str.length();
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(font, str[i]);
	}
}


void sprJump(float time)
{	
	if (!stCollision){
		velocityY += gravity * time;        // Apply gravity to vertical velocity
		positionY -= velocityY * time;      // Apply vertical velocity to X position
		 //positionX += velocityX * time;      // Apply horizontal velocity to X position
		 
		 if(positionY < 0.0){
			  positionY = 0.0;
			  velocityY = 0.0;
			  stOnGround = true;
		 }
		 
		 //if(positionX < 0 || positionX > 10)
		 //    velocityX *= -1;
	 }
}

void Timer(int iUnused)
{	
	
	if(!stCollision && !stPause){ 
		if(stRun && (tckKika>1)){
			stKika=!stKika;
			tckKika=0;
		}else tckKika++;
				
		if(score%50==0 && tSpeed<tSpeedMax) tSpeed+=tStep;		
		if(grndXpos>0) grndXpos-=tSpeed; else grndXpos=80;
		
		//OBSTACLE
		if(obsXpos>0) obsXpos-=tSpeed; else {
			srand (time(NULL));
			obsXposmut = rand() % 10;
			obsXpos=80+obsXposmut*2;
		}
		
		//~ if(obsXpos2>0) obsXpos2-=tSpeed; else {
			//~ srand (time(NULL));
			//~ obsXposmut2 = rand() % 10;
			//~ obsXpos2=100+obsXposmut2*2;
		//~ }
		
		//T-REX
		if (!stOnGround) sprJump(0.2);
		
		score+=tSpeed*0.5;
		if (score>scoreNotif) {
			scoreNotif+=500; 
			system("canberra-gtk-play -f score.ogg &");
		}
		//DETEKSI COLLISION
		if (positionX+3<obsXpos+2 && positionX+10>=obsXpos && positionY<=6) {
			stCollision = true; tSpeed=0; stRun=false; 
			if (score>hiscore) hiscore=score;
			system("canberra-gtk-play -f collision.ogg &");
		}
		
		//initGuide
		if (stInitGuide && positionX > obsXpos-18) stPause=true;
		
	}else{
		if (chrXpos==3) chrXpos=2.5; else chrXpos=3;
	}
		
	glutPostRedisplay();
	glutTimerFunc(28, Timer, 0);
}

void genKarakter(int idKarakter){
	switch (idKarakter){
	case 1:	// T-REX DEFINITION
		//glEnable (GL_TEXTURE_2D); // enable texture mapping depan
		//glBindTexture (GL_TEXTURE_2D, 3); //bind ke texture n
		//perut
		glBegin(GL_POLYGON);
			glVertex2f(4,3.3);	// ekor, kaki
			glVertex2f(7,3.3);	//kaki
			glVertex2f(9.5,5.5);
			glVertex2f(9.5,9);		// kepala
			glVertex2f(7.3,9);		// kepala
			glVertex2f(3.7,6.7);	// ekor
		glEnd();
		//ekor
		glBegin(GL_POLYGON);
			glVertex2f(1.3,5.7);
			glVertex2f(4,3.3);	// perut
			glVertex2f(4,6.7);	// perut
			glVertex2f(2,8);
			glVertex2f(1.3,9);
		glEnd();
		//tangan
		glBegin(GL_TRIANGLE_STRIP);
			glVertex2f(9.5,7.5);	// perut
			glVertex2f(9.5,8);		// perut
			glVertex2f(10.3,7.5);
			glVertex2f(10.7,8);
			glVertex2f(10.3,6.7);
			glVertex2f(10.7,6.7);
		glEnd();
		//kepala
		glBegin(GL_POLYGON);
			glVertex2f(7.3,9);		// perut
			glVertex2f(9.5,9);		// perut, mulut
			glVertex2f(10,9.7);		// mulut	
			glVertex2f(10,10.3);			
			glVertex2f(13,10.3);			
			glVertex2f(13,13);			
			glVertex2f(12.5,13.5);			
			glVertex2f(8,13.5);			
			glVertex2f(7.3,13);			
		glEnd();
		//mulut
		glBegin(GL_POLYGON);			
			glVertex2f(9.5,9);		//kepala
			glVertex2f(12,9);
			glVertex2f(12,9.7);
			glVertex2f(10,9.7);		//kepala
		glEnd();
		//kaki
		glPushMatrix();
			if(stRun && stOnGround)glTranslatef(0,(stKika?1:0),0);
			glBegin(GL_TRIANGLE_STRIP);		//kiri
				glVertex2f(6,3.3);		//perut
				glVertex2f(6.5,2.7);		
				glVertex2f(7,4);		//perut
				glVertex2f(6.5,1);		
				glVertex2f(7,1.5);		
				glVertex2f(7.5,1);		
				glVertex2f(7.5,1.5);		
			glEnd();
		glPopMatrix();
		glPushMatrix();
			if(stRun && stOnGround)glTranslatef(0,(stKika?0:1),0);
			glBegin(GL_TRIANGLE_STRIP);		//kanan
				glVertex2f(5.3,3.3);	// perut	
				glVertex2f(4.3,2);		
				glVertex2f(4,3.3);		//perut
				glVertex2f(4.3,1.5);		
				glVertex2f(4,1);		
				glVertex2f(5,1.5);		
				glVertex2f(5,1);		
			glEnd();
		glPopMatrix();
		//mata
		glPushMatrix();
			glColor3ub(17,17,17);
			glBegin(GL_POLYGON);
				glVertex2f(8.5,12);		
				glVertex2f(9,12);		
				glVertex2f(9,12.5);		
				glVertex2f(8.5,12.5);
			glEnd();		
		glPopMatrix();
		break;
	}
}

void processSpecialKeys(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_UP:
			if(stOnGround){
				system("canberra-gtk-play -f jump.ogg &");
				velocityY = -12.0f;
				stOnGround = false;  
			}
			if(stInitGuide && stPause) stPause=false;
			stInitGuide=false;
			break;
		case GLUT_KEY_DOWN:
			positionY = 0.0;
			velocityY = 0.0;
			stOnGround = true;
			break;
	}
}

void drawObstacle(int obsId=0){
	glEnable (GL_TEXTURE_2D); // enable texture mapping depan
   glBindTexture (GL_TEXTURE_2D, 2); //bind ke texture n
	glBegin(GL_POLYGON);
	switch (obsId){
	case 1:
		break;
	default:
		glTexCoord2f( 1.0f, 2.0f);
		glVertex2f( 0.5, 0.5);
		glTexCoord2f( 0.0f, 2.0f);
		glVertex2f(-0.5, 0.5);
		glTexCoord2f( 0.0f, 0.0f);
		glVertex2f(-0.5,-0.5);
		glTexCoord2f( 1.0f, 0.0f);
		glVertex2f( 0.5,-0.5);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void drawGround(){
	glEnable (GL_TEXTURE_2D); // enable texture mapping depan
   glBindTexture (GL_TEXTURE_2D, 1); //bind ke texture n
	glBegin(GL_POLYGON);
		glTexCoord2f( 0, 0);		glVertex2f(-100, 0);
		glTexCoord2f(30, 0);		glVertex2f( 100, 0);
		glTexCoord2f(30, 1);		glVertex2f( 100, 3);
		glTexCoord2f( 0, 1);		glVertex2f(-100, 3);
		/*for (int i =-80; i<80; i+=3){
			glVertex2f(	 i, 0);
			glVertex2f(i+3, 3);
		}*/
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void resetValues(){
	stCollision=false;
	tSpeed=tSpeedMin;
	positionX=0; positionY=0;
	obsXpos=0; obsXpos2=0; grndXpos=0;
	stRun=true;
	stPause=false;
	stInitGuide=true;
	score=0;
}

void releaseSpecialKeys(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_UP:
			if(velocityY < -10.0f)       // If character is still ascending in the jump
				velocityY = -10.0f;
			break;
	}
}

void processNormalKeys(unsigned char key, int x, int y){
	switch(key) {
		case 27: 		// Kode ASCII tombol ESC
			exit(0); 	// Exit kalau ESC ditekan
			break;
		case 'r': case 'R':		// r or R
			stRun=!stRun;
			break;
		case 'p': case 'P':		// r or R
			stPause=!stPause;
			break;
		case ' ':		// SPACEBAR
			if(stOnGround){
				system("canberra-gtk-play -f jump.ogg &");
				velocityY = -12.0f;
				stOnGround = false;
			}
			if(stInitGuide && stPause) stPause=false;
			stInitGuide=false;
			break;
		case '1': 
			if (tSpeed>tSpeedMin) tSpeed-=tStep;
			break;
		case '2': 
			if(tSpeed<tSpeedMax) tSpeed+=tStep;
			break;
		case 'y': case 'Y': case 13:
			if(stCollision) resetValues();
			break;
		case 'n': case 'N':
			if(stCollision) 
				exit(0); 	// Exit kalau ESC ditekan
			break;
	}
}

void releaseNormalKeys(unsigned char key, int x, int y){
	switch(key) {
		case 27: 		// Kode ASCII tombol ESC
			exit(0); 	// Exit kalau ESC ditekan
			break;
		case 'r': case 'R':		// r or R
			stRun=!stRun;
			break;
		case ' ':		// SPACEBAR
			if(velocityY < -10.0f)       // If character is still ascending in the jump
				velocityY = -10.0f;
			break;
		
	}
}


void display()
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	/*
	glPushMatrix();
	grid(20,80);
	glPopMatrix();
	*/
	
// SCORING	
	glPushMatrix();
		glTranslatef(65,17,0);
		strPrint(0,0,255,255,255,GLUT_BITMAP_HELVETICA_18,"SCORE: "+to_string(score));  
	glPopMatrix();	
	if (hiscore>0){
		glPushMatrix();
			glTranslatef(61,15,0);
			strPrint(0,0,255,255,255,GLUT_BITMAP_HELVETICA_18,"HIGH SCORE: "+to_string(hiscore));  
		glPopMatrix();	
	}
	
	//debug
	//~ glPushMatrix();
	//~ glTranslatef(60,10,-1);
	//~ strPrint(0,0,255,255,255,GLUT_BITMAP_HELVETICA_18,to_string(obsXposmut)+" -- "+to_string(obsXposmut2));  
	//~ glPopMatrix();	
	
//COLLISION DETECTION
	if (stCollision) {
		glPushMatrix();
			glTranslatef(33,15,0);
			strPrint(0,0,255,255,255,GLUT_BITMAP_HELVETICA_18,"RESTART? (Y/N)");  
		glPopMatrix();
	}
	if (stPause) {
		glPushMatrix();
			if (stInitGuide==false){
				glTranslatef(37,15,0);
				strPrint(0,0,255,255,255,GLUT_BITMAP_HELVETICA_18,"PAUSED");
			}else{
				glTranslatef(27,15,0);
				strPrint(0,0,255,255,255,GLUT_BITMAP_HELVETICA_18,"Press SPACE or UP for JUMPING");
			}
		glPopMatrix();
	}
	
	//OBSTACLE
	glPushMatrix();
		glTranslatef(obsXpos,5,-1);
		glScalef(5,7,0);
		glColor3ub(255,255,255);
		drawObstacle();	
	glPopMatrix();
	//~ glPushMatrix();
	
	//~ glTranslatef(obsXpos2,5,-1);
	//~ glScalef(3,6,0);
	//~ glColor3ub(255,255,255);
	//~ drawObstacle();
	
	//~ glPopMatrix();
	
	//TREX
	glPushMatrix();
		if (!stOnGround) {
			//~ sprJump(0.1); 
			glTranslatef(positionX,positionY,0);
		}
		glTranslatef(chrXpos,1.7,-1);
		glScalef(0.4,0.4,0);
		glColor3ub(79,90,47);
		//glLineWidth(2);
		genKarakter(1);	
	glPopMatrix();	
	
	//GROUND
	glPushMatrix();
	
	glTranslatef(grndXpos,0,0);
	glColor3ub(255,255,255);
	//glLineWidth(2);
	drawGround();
	glPopMatrix();
	
	latar();
	glutSwapBuffers();
}

int main(int argc, char **argv)
{
	glutInit( &argc, argv );
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1000,250);
	glutCreateWindow("T-Rex Robo");
	
	//load texture
	glTga();
	
	glutDisplayFunc(display);
	glutReshapeFunc(changeSize);
	glutIdleFunc(display);
	
	// here are the new entries
	glutKeyboardFunc(processNormalKeys);
	glutKeyboardUpFunc(releaseNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutSpecialUpFunc(releaseSpecialKeys);
	//glutMouseFunc(processMouse);
	
	Timer(0);
	glutMainLoop();
	
	return 0;
}

