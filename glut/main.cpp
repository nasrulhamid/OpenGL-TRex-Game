/*
 * T-REX ROBO KOM300
 * 
 * G64164017	NASRUL HAMID
 * G64164021	MUHAMMAD JAKA UTAMA
 * G64164053	ISMAIL ADIMA
 * 
 * Geany Build Command Parameter: g++ -Wall -o "%e" "%f" -lSOIL -lglut -lGL -lGLU
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <ctime>
#include <cmath>
#include <GL/glut.h>
#include "SOIL.h"
#include "tga.h"
#include "tga.c"

using namespace std;

bool stRun = true, stKika=true; 
double tmTick=0, tckKika=0;

float positionX=0, positionY=0;     // Position of the character
float velocityX=4.0f, velocityY=0.0f;     // Velocity of the character
float gravity = 9.8f;           // How strong is gravity
bool stOnGround = true, stCollision = false, stInitGuide=true, stPause=false
	, stBonus=false, stNunduk=false, nightmode=false, stBintangSet=false
	, stRolling=false, bonusSwitch=true;

int obsI=0, eggId; 
double obsXpos=100, obsXposmut, obsXpos2=-10, obsXposmut2
	, bonusXPos=-10, bonusSize=1.0;
double grndXpos;
double chrXpos=3.0;

const float tSpeedMin =1.0f, tSpeedMax = 2.3f, tStep=0.1f;
float tSpeed = tSpeedMin, latarAlpha=1.0, skytmr=0, chRollDeg=0.0; 
long score=0, scoreNotif=1000, hiscore=0, nextNightMode=1200, tmr=0
	, nextBonus=800;

unsigned int crr,crg,crb;

GLuint tx_obs1, tx_obs2, tx_obs1n, tx_obs2n, tx_awan, egg[3];

struct squares_t{
	float x,y,w,h;
} bintang[50], awan[50];


void generateBintang(){
	for(int i=0; i<50; i++){
		bintang[i].x=rand()%200;
		bintang[i].y=rand()%16+4;
		bintang[i].w=rand()%2;
	}
}
void generateAwan(){
	for(int i=0; i<50; i++){
		awan[i].x=rand()%200;
		awan[i].y=rand()%5+13;
		awan[i].w=rand()%2+1;
	}
}

void latar(){
   glBegin(GL_POLYGON);
		glColor4ub(132,208,228,latarAlpha*255);
		glVertex2f(0, 20); //vertex 1 x,y kebawah - kekiri -
		glColor4ub(118,207,230,latarAlpha*255);
		glVertex2f(80, 20);
		glColor4ub(233, 244, 237,latarAlpha*255);
		glVertex2f(80, 0);
		glColor4ub(218, 236, 241,latarAlpha*255);
		glVertex2f(0, 0);
	glEnd();
}

GLuint LoadTexture( const char * filename )
{

	GLuint texture;

	int width, height;

	unsigned char * data;

	FILE * file;

	file = fopen( filename, "rb" );

	if ( file == NULL ) return 0;
	width = 1024;
	height = 512;
	data = (unsigned char *)malloc( width * height * 3 );
	//int size = fseek(file,);
	fread( data, width * height * 3, 1, file );
	fclose( file );

	for(int i = 0; i < width * height ; ++i)
	{
		int index = i*3;
		unsigned char B,R;
		B = data[index];
		R = data[index+2];

		data[index] = R;
		data[index+2] = B;

	}


	glGenTextures( 1, &texture );
	glBindTexture( GL_TEXTURE_2D, texture );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );


	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );
	gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,GL_RGB, GL_UNSIGNED_BYTE, data );
	free( data );

	return texture;
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

void load_soil_textures(){
	tx_obs1 = SOIL_load_OGL_texture(
		"obs1.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_MULTIPLY_ALPHA
	);
	tx_obs2 = SOIL_load_OGL_texture(
		"obs2.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_MULTIPLY_ALPHA
	);
	tx_obs1n = SOIL_load_OGL_texture(
		"obs1n.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_MULTIPLY_ALPHA
	);
	tx_obs2n = SOIL_load_OGL_texture(
		"obs2n.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_MULTIPLY_ALPHA
	);
	tx_awan = SOIL_load_OGL_texture(
		"awan.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_MULTIPLY_ALPHA
	);
	egg[0] = SOIL_load_OGL_texture(
		"egg1.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_MULTIPLY_ALPHA
	);
	egg[1] = SOIL_load_OGL_texture(
		"egg2.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_MULTIPLY_ALPHA
	);
	egg[2] = SOIL_load_OGL_texture(
		"egg3.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_MULTIPLY_ALPHA
	);
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

	//~ float ratio =  w * 3.15 / h;
	float ratio = w/80.0;
	
    //ratio = width / (float) height;
    glViewport(0, 0, w, h);
    
    glClear(GL_COLOR_BUFFER_BIT);

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, (h-w/4)/2, w, h);
	

	// Set the correct perspective.
	//gluPerspective(90,ratio,1,100);
	//gluPerspective(0,1,1,100);
	
	//glOrtho(-w/20, w/20, -h/20, h/20, 1.f, -1.f);
	//~ glOrtho(0, (float)w/ratio, 0, (float)h/ratio, 1.f, -1.f);
	glOrtho(0, 80.0, 0, h/ratio, 1.f, -1.f);
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
			//~ if(stKika) system("play -q walk2.wav &");
			tckKika=0;
		}else tckKika++;
				
		if(score%50==0 && tSpeed<tSpeedMax) tSpeed+=tStep;		
		if(grndXpos>0) grndXpos-=tSpeed; else grndXpos=80;
		
		//OBSTACLE
		if(obsXpos>=-10) 
			obsXpos-=tSpeed; 
		else {
			if (obsXpos2 < 40) {
				srand (time(NULL));
				obsXposmut = rand() % 40;
				obsXpos=80+obsXposmut;
			}
		}
		
		if(obsXpos2>-10) 
			obsXpos2-=tSpeed; 
		else 
			if (obsXpos < 40) {
				srand (time(NULL));
				obsXposmut2 = rand() % 40;
				obsXpos2=80+obsXposmut2;
			}
		
		//BONUS
		if (stBonus) {
			if(bonusSwitch){
				if (bonusSize<1.3) bonusSize+=0.06; else bonusSwitch=!bonusSwitch;
			}else {
				if (bonusSize>1.0) bonusSize-=0.06; else bonusSwitch=!bonusSwitch;
			}
			if (bonusXPos>-10) bonusXPos-=0.8; 
			else { 
				stBonus=false; nextBonus+=800;
				bonusXPos=80+(rand() % 40);
			}
		}else{
			bonusXPos=-10; 
			eggId=rand()%3+5;
		}
			
		if ((positionX+5<bonusXPos && positionX+8>=bonusXPos) && positionY>6){
			system("play -q coin.ogg &");
			score+=100; stBonus=false; bonusXPos=-10; nextBonus+=800;
		}
		//generate bonus
		crr=255; crg=255; crb=0;
		if (score > nextBonus && !stBonus ){
			srand (time(NULL));
			bonusXPos=80+(rand() % 40);
			stBonus=true;
		}
		
		//T-REX
		if (!stOnGround) sprJump(0.2);
		
		score+=tSpeed;
		if (score>scoreNotif) {
			scoreNotif+=1000; 
			system("play -q score.ogg &");
		}		
		if(stRolling && chRollDeg>-360 && !stOnGround) {
			chRollDeg-=60; 
			score+=5;
		}else{
			chRollDeg=0;
		}
		
		//DETEKSI COLLISION
		if (((positionX+4<obsXpos+2 && positionX+9>=obsXpos)
		|| (positionX+4<obsXpos2+2 && positionX+9>=obsXpos2))
		&& positionY<=5) {
			stCollision = true; tSpeed=0; stRun=false; 
			if (score>hiscore) hiscore=score;
			system("play -q collision4.wav &");
		}
		
		//initGuide
		if (stInitGuide && positionX > obsXpos-15 && positionX < obsXpos) stPause=true;
		
		//switch night-day
		tmr++;
		if(tmr>nextNightMode) {
			nightmode=!nightmode;
			nextNightMode += 1200;
			for (int i=0; i<3;i++){
				string fl = "egg"+to_string(i+1)+(nightmode?"n":"")+".png";
				egg[i]=SOIL_load_OGL_texture
					(
						fl.c_str(),
						SOIL_LOAD_AUTO,
						egg[i],
						SOIL_FLAG_DDS_LOAD_DIRECT | SOIL_FLAG_INVERT_Y
					);
			}
		}
		if (nightmode && latarAlpha>0) latarAlpha-=0.05;
		else if(!nightmode && latarAlpha<1) latarAlpha+=0.05;
		if (skytmr<100) skytmr+=0.05*tSpeed; else skytmr=0;
		
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
		glPushMatrix();
			if(stNunduk){
				glRotatef(-15,0,0,1);
				glTranslatef(-1.6,1.1,0);
			}
			glBegin(GL_POLYGON);
				glVertex2f(4,3.3);	// ekor, kaki
				glVertex2f(7,3.3);	//kaki
				glVertex2f(9.5,5.5);
				glVertex2f(9.5,9);		// kepala
				glVertex2f(7.3,9);		// kepala
				glVertex2f(3.7,6.7);	// ekor
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
		glPopMatrix();
		//ekor
		glPushMatrix();
			if(stRun && stOnGround) glTranslatef((stKika?0.5:0),(stKika?0.5:0),0);
			glBegin(GL_POLYGON);
				glVertex2f(1.7,6);
				glVertex2f(4,3.3);	// perut
				glVertex2f(4,6.7);	// perut
				glVertex2f(2,8);
				glVertex2f(1.3,9);
			glEnd();
		glPopMatrix();
		//kepala
		glPushMatrix();
			if(stNunduk){
				glRotatef(-15,0,0,1);
				glTranslatef(-1.6,1.1,0);
			}else{
				glRotatef(stOnGround?0:10, 0.0, 0.0, 1.0);
				glTranslatef(stOnGround?0:1,stOnGround?0:-2,0);
			}
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
			//mata
			glPushMatrix();
				glBegin(GL_POLYGON);
					glVertex2f(8.5,12);		
					glVertex2f(9,12);		
					glVertex2f(9,12.5);		
					glVertex2f(8.5,12.5);
				glEnd();		
			glPopMatrix();
		glPopMatrix();
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
		break;
	}
}

void processSpecialKeys(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_UP:
			if(stOnGround){
				system("play -q jump.ogg &");
				velocityY = -16.0f;
				stOnGround = false;  
			}
			if(stInitGuide && stPause) stPause=false;
			stInitGuide=false;
			stPause = false;
			break;
		case GLUT_KEY_DOWN:
			positionY = 0.0;
			velocityY = 0.0;
			stOnGround = true;
			stNunduk=true;
			stPause = false;
			break;
		case GLUT_KEY_RIGHT:
			stRolling=true;
		break;
	}
}

void processMouse(int button, int state, int x, int y) {
	switch(button) {
		case GLUT_LEFT_BUTTON:
			if(stOnGround){
				system("play -q jump.ogg &");
				velocityY = -16.0f;
				stOnGround = false;  
			}
			if(stInitGuide && stPause) stPause=false;
			stInitGuide=false;
			stPause = false;
			if (state==GLUT_UP){
				if(velocityY < -10.0f)
				velocityY = -10.0f;
			}
			break;
		case GLUT_RIGHT_BUTTON:
			positionY = 0.0;
			velocityY = 0.0;
			stOnGround = true;
			stNunduk=true;
			stPause = false;
			if (state==GLUT_UP)stNunduk=false;
			break;
		//~ case GLUT_KEY_RIGHT:
			//~ stRolling=true;
		break;
	}
}

void drawObstacle(int obsId=0){
	switch (obsId){
	case 1: // BONUS
		glBegin(GL_POLYGON);
		for(int i=0; i<=20; i++)
			glVertex2f( 0.5*cos(i*M_PI/10), 0.5*sin(i*M_PI/10));
		glEnd();
		break;
	case 2:	case 3: // OBSTACLE
		glEnable (GL_TEXTURE_2D); // enable texture mapping depan
		//~ glEnable (GL_BLEND); glBlendFunc (GL_ONE, GL_ONE);
		//~ glTexEnvf(GL_TEXTURE_2D,GL_TEXTURE_ENV_MODE,GL_DECAL);
		//~ glTexEnvf(GL_TEXTURE_2D,GL_TEXTURE_ENV_MODE,GL_MODULATE);
		if(!nightmode) glBindTexture (GL_TEXTURE_2D, (obsId==2 && latarAlpha>0.5)?tx_obs1:tx_obs2);
		else if(nightmode) glBindTexture (GL_TEXTURE_2D, (obsId==2 && latarAlpha<0.5)?tx_obs1n:tx_obs2n);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glBegin(GL_POLYGON);
			glTexCoord2f( 1.0f, 1.0f);	glVertex2f( 0.5, 0.5);
			glTexCoord2f( 0.0f, 1.0f);	glVertex2f(-0.5, 0.5);
			glTexCoord2f( 0.0f, 0.0f);	glVertex2f(-0.5,-0.5);
			glTexCoord2f( 1.0f, 0.0f);	glVertex2f( 0.5,-0.5);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		break;
	case 4: // AWAN
		glEnable (GL_TEXTURE_2D); 
		glBindTexture (GL_TEXTURE_2D, tx_awan);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glBegin(GL_POLYGON);
			glTexCoord2f( 1.0f, 1.0f);	glVertex2f( 1.0, 0.5);
			glTexCoord2f( 0.0f, 1.0f);	glVertex2f(-1.0, 0.5);
			glTexCoord2f( 0.0f, 0.0f);	glVertex2f(-1.0,-0.5);
			glTexCoord2f( 1.0f, 0.0f);	glVertex2f( 1.0,-0.5);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		break;
	case 5: case 6: case 7: // EGG
		glEnable (GL_TEXTURE_2D); 
		glBindTexture (GL_TEXTURE_2D, egg[obsId-5]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glBegin(GL_POLYGON);
			glTexCoord2f( 1.0f, 1.0f);	glVertex2f( 0.3, 0.4);
			glTexCoord2f( 0.0f, 1.0f);	glVertex2f(-0.3, 0.4);
			glTexCoord2f( 0.0f, 0.0f);	glVertex2f(-0.3,-0.4);
			glTexCoord2f( 1.0f, 0.0f);	glVertex2f( 0.3,-0.4);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		break;
	default:
		glBegin(GL_POLYGON);
			glVertex2f( 0.5, 0.5);
			glVertex2f(-0.5, 0.5);
			glVertex2f(-0.5,-0.5);
			glVertex2f( 0.5,-0.5);
		glEnd();
	}
}

void drawGround(){
	glEnable (GL_TEXTURE_2D); // enable texture mapping depan
   glBindTexture (GL_TEXTURE_2D, 1); //bind ke texture n
	glBegin(GL_POLYGON);
		glTexCoord2f( 0, 0);		glVertex2f(-100, 0);
		glTexCoord2f(30, 0);		glVertex2f( 100, 0);
		glTexCoord2f(30, 1);		glVertex2f( 100, 3);
		glTexCoord2f( 0, 1);		glVertex2f(-100, 3);
	glEnd();
	
	//batu
	glPushMatrix();
	glTranslatef(0,2.8,0);
	glScalef(1.5,1.25,0);
	glBegin(GL_POLYGON);
		for(int i=0; i<=20; i++)
			glVertex2f( 0.5*cos(i*M_PI/10), 0.5*sin(i*M_PI/10));
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void drawStar (int n=5){	
	for (int i=0;i<n;i++){
		glPushMatrix();
			glRotatef(i*360/n,0,0,1);
			glBegin(GL_POLYGON);
			glVertex2f(-0.25,0);
			//glVertex2f(-0.25,0.5);
			glVertex2f(0,1);
			//glVertex2f(0.25,0.5);
			glVertex2f(0.25,0);
			glEnd();
		glPopMatrix();
	}
}

void drawSunMoon(){
	glPushMatrix();
		//~ glTranslatef(0.1*sunX,10,1);
		glRotatef(120,0,0,1.0);
		//~ if(wkt==1){	// SIANG
			//~ glColor3ub(255,255,0);
			//~ glBegin(GL_POLYGON);	// SUN
				//~ for (int i=0,n=30,rad=4;i<=n;i++)
					//~ glVertex2f(rad*cos(2.0*pi*i/n),rad*sin(2.0*pi*i/n));
			//~ glEnd();
		//~ }else if (wkt==2){	// MALAM
			glPushMatrix();
				glColor3ub(0,0,0);
				glScalef(1,0.8,1);
				glBegin(GL_POLYGON);
				for (int i=0,n=30,rad=2;i<=n;i++)
					glVertex2f(rad*cos(2.0*M_PI*i/n),rad*sin(2.0*M_PI*i/n));
				glEnd();
			glPopMatrix();
			glBegin(GL_POLYGON);	// BULAN
				glColor3ub(255,255,150);
				for (int i=0,n=30,rad=2;i<=n/2;i++)
					glVertex2f(rad*cos(2.0*M_PI*i/n),rad*sin(2.0*M_PI*i/n));
			glEnd();
			
		//~ }
	glPopMatrix();
}

void resetValues(){
	stCollision=false;
	tSpeed=tSpeedMin;
	positionX=0; positionY=0;
	obsXpos=100; obsXpos2=-10; grndXpos=0; bonusXPos=-10;
	stRun=true;
	stPause=false;
	stInitGuide=true;
	stBonus=false;
	scoreNotif=1000;
	score=0;
	tmr=0;
	nightmode=false;
	nextNightMode=1200;
	nextBonus=800;
	generateBintang();
	generateAwan();
	
	for (int i=0; i<3;i++){
		string fl = "egg"+to_string(i+1)+(nightmode?"n":"")+".png";
		egg[i]=SOIL_load_OGL_texture
			(
				fl.c_str(),
				SOIL_LOAD_AUTO,
				egg[i],
				SOIL_FLAG_DDS_LOAD_DIRECT | SOIL_FLAG_INVERT_Y
			);
	}
}

void releaseSpecialKeys(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_UP:
			if(velocityY < -10.0f)       // If character is still ascending in the jump
				velocityY = -10.0f;
			break;
		case GLUT_KEY_DOWN:
			stNunduk = false;
			break;
		case GLUT_KEY_RIGHT:
			stRolling=false;
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
				system("play -q jump.ogg &");
				velocityY = -16.0f;
				stOnGround = false;
			}
			if(stInitGuide && stPause) stPause=false;
			stInitGuide=false;
			stPause = false;
			break;
		case '1': 
			if (tSpeed>tSpeedMin) tSpeed-=tStep;
			break;
		case '2': 
			if(tSpeed<tSpeedMax) tSpeed+=tStep;
			break;
		case 'y': case 'Y': case 13:
			if(stCollision) resetValues(); 
			stPause=false;
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
	glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
	//~ strPrint(0,0,255,255,255,GLUT_BITMAP_HELVETICA_18,to_string(eggId));  
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
	
	//BONUS POINT
	glPushMatrix();
		glTranslatef(bonusXPos,15,0);
		glScalef(2.0*bonusSize,2.0*bonusSize,1);
		//~ glColor3ub(crr,crg,crb);
		glColor3ub(255,255,255);
		drawObstacle(eggId);	
	glPopMatrix();	
	
	//TREX
	glPushMatrix();
		if (!stOnGround) {
			//~ sprJump(0.1); 
			glTranslatef(positionX,positionY,0);
		}
		glTranslatef(chrXpos,1.7,0);
		if(stRolling && !stOnGround){
			glTranslatef(3.1,3.1,0);
			glRotatef(chRollDeg,0,0,1);
			glTranslatef(-3.1,-3.1,0);
		}
		glScalef(0.4,0.4,0);
		
		glColor3ub(79,90,47);
		//glLineWidth(2);
		genKarakter(1);	
	glPopMatrix();	
	
	//BAYANG
	glPushMatrix();
		glTranslatef(5.2,2,0);
		if(stOnGround) glScalef(1.5,0.5,1); else glScalef(1.25,0.25,1);
		glColor4ub(100,100,100,100);
		glBegin(GL_POLYGON);
			for (unsigned int i=0; i<20; i++)
				glVertex2f(cos(i*M_PI/10),sin(i*M_PI/10));		
		glEnd();		
	glPopMatrix();
	
	//OBSTACLE
	glPushMatrix();
		glTranslatef(obsXpos,5,0);
		glScalef(5,7,0);
		glColor4ub(255,255,255,255);
		drawObstacle(2);	
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(obsXpos2,5,0);
		glScalef(5,7,0);
		glColor3ub(255,255,255);
		drawObstacle(3);	
	glPopMatrix();	
	
	//GROUND
	glPushMatrix();
	
		glTranslatef(grndXpos,0,0);
		glColor3ub(255,255,255);
		//glLineWidth(2);
		drawGround();
	glPopMatrix();
	
	if(nightmode && latarAlpha<0.5){
		//sunmoon
		glPushMatrix();
			glTranslatef(100-skytmr,15,0);
			glColor3ub(255,255,255);
			drawSunMoon();
		glPopMatrix();
		
		//BINTANG
		for (int i = 0; i<30; i++){
			glPushMatrix();
				glTranslatef(bintang[i].x-skytmr,bintang[i].y,0);
				glScalef(0.25*bintang[i].w,0.25*bintang[i].w,0);
				glColor3ub(255,255,255);
				//glLineWidth(2);
				drawStar();
			glPopMatrix();
		}
	}else if (!nightmode && latarAlpha>0.9){
		//awan
		for (int i = 0; i<30; i++){
			glPushMatrix();
				glTranslatef(awan[i].x-skytmr,awan[i].y,0);
				glScalef(awan[i].w,awan[i].w,0);
				glColor3ub(255,255,255);
				drawObstacle(4);
			glPopMatrix();
		}
	}
	
	latar();
		
	glutSwapBuffers();
}

int main(int argc, char **argv)
{	
	glutInit( &argc, argv );
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1000,1000*9/16); // wide screen ratio
	glutCreateWindow("T-Rex Run");	
	
	//load texture
	glTga();
	resetValues();
	load_soil_textures();
	
	glutDisplayFunc(display);
	glutReshapeFunc(changeSize);
	glutIdleFunc(display);
	
	
	// here are the new entries
	glutKeyboardFunc(processNormalKeys);
	glutKeyboardUpFunc(releaseNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutSpecialUpFunc(releaseSpecialKeys);
	glutMouseFunc(processMouse);
	
	Timer(0);
	glutMainLoop();
	
	return 0;
}

