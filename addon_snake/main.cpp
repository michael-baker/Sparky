/*
 * snake.dll - snake classic game sparky addon v1.2
 * coded by ruiner@konspiracy.org
 *
 * released on April 13, 2003
 *
 * this is provided as an example, if you make your own addon with this please give credits!
 * this is opengl only...
 *
 * Basic Addon Template by Mulchman
 *
 */

#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <gl/gl.h>

#pragma comment (lib, "opengl32.lib") 
#pragma comment (lib, "winmm.lib")	// This is needed for timeGetTime( )
#pragma warning (disable:4244)
#pragma warning (disable:4018)

struct snak_s
{
	bool alive;
	int x;
	int y;
};

struct pill_s
{
	int x;
	int y;
	bool alive;
};

int		width = 375;	int height = 375;
int		direction =0;	int speed = 0; 
int		highscore = 0;	int score = 0;
int		dir_x;			int dir_y;
int		snake_size;
char	level[15];
bool	alive;
GLuint	base;
HDC		hDC = NULL;	
char	FileName[1024];
FILE	*fp;
static	snak_s snake[650];
static	pill_s pill;

static GLint	vp[4];
static unsigned long tLastTime;

void BuildFont( GLvoid );

bool WINAPI DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved )
{
	switch ( dwReason )
	{
		case DLL_PROCESS_ATTACH:
			BuildFont();
			glGetIntegerv(GL_VIEWPORT,vp);
			alive = false;
			speed = 0;
			sprintf(level,"");
			sprintf(FileName, "sparkyutils\\addons\\snake.dat");
			if(fp = fopen(FileName, "r")) {
				char *line;
				if(line = fgets(FileName, 1024, fp)) {
					line[strlen(line)-1] = '\0';
					highscore = atoi(line);
				} else
					highscore = 0;
				fclose(fp);
			} else {
				highscore = 0;
			}

			break;

		case DLL_PROCESS_DETACH:
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;
	}

	return  true;
}

void glDrawRectangle(float x1, float y1, float x2, float y2, float r, float g, float b, float a)
{
		glColor4f(r,g,b,a);
		glBegin(GL_QUADS); 
		glVertex2f(x1, y1);
		glVertex2f(x2, y1);
		glVertex2f(x2, y2);
		glVertex2f(x1, y2);
		glEnd();
}

void glDrawLine(float x1, float y1, float x2, float y2, float r, float g, float b, float a)
{
		glColor4f(r,g,b,a);
		glBegin(GL_LINES); 
		glVertex2f(x1, y1);
		glVertex2f(x2, y2);
		glEnd();
}

void BuildFont( GLvoid )								
{
	hDC = wglGetCurrentDC( );
	HFONT	font;
	HFONT	oldfont;
	base = glGenLists( 96 );
	font = CreateFont(	-10 ,		//size
		0,							//width
		0,							//orientation
		0,							//angle
		FW_NORMAL,					//weight
		FALSE,						//italic
		FALSE,						//underline
		FALSE,						//strikeout
		ANSI_CHARSET,				//character set	
		OUT_TT_PRECIS,				//output precision
		CLIP_DEFAULT_PRECIS,		//clipping precision
		PROOF_QUALITY,				//output quality
		FF_DONTCARE|DEFAULT_PITCH,	//family and pitch
		"Verdana");					//name of font to use		
	oldfont = (HFONT)SelectObject(hDC, font);  
	wglUseFontBitmaps(hDC, 32, 96, base);				
	SelectObject(hDC, oldfont);							
	DeleteObject(font);	
}

GLvoid glPrint(float x, float y, float r, float g, float b, float a, const char *fmt,...)
{
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	    vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	glColor4f(r,g,b,a);
	glRasterPos2f(x,y);

	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base - 32);								// Sets The Base Character to 32
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits
}

void DrawCube(int x, int y, int face)
{
	float r,g,b;

	switch ( face )
	{
	case 1: // body
		r = 0.55f;
		g = 0.70f;
		b = 0.75f;
		glDrawRectangle(x, y, x+15, y+15, 0.0f, 0.0f, 0.0f, 1.0f);
		glDrawRectangle(x+1, y+1, x+14, y+14, r, g, b, 1.0f);
		break;
	case 2: // face
		if(alive) {
			r = 0.70f;
			g = 0.85f;
			b = 0.90f;
		} else {
			r = 1.0f;
			g = 0.25f;
			b = 0.25f;
		}
		glDrawRectangle(x, y, x+15, y+15, 0.0f, 0.0f, 0.0f, 1.0f);
		glDrawRectangle(x+1, y+1, x+14, y+14, r, g, b, 1.0f);
		break;
	case 3: // apple
		r = 1.0f;
		g = 0.5f;
		b = 0.5f;
		glDrawRectangle(x+2, y+2, x+13, y+13, r, g, b, 1.0f);
		break;
	}
}

void NewGame(void)
{
	alive = true;
	pill.alive = false;
	direction = 4;
	for(int a=1;a<580;a++)
	{
		snake[a].alive=false;
		snake[a].x=0;
		snake[a].y=0;
	}
	snake_size = 4;
	snake[0].x = 90; snake[0].y = 90; snake[0].alive = true;
	snake[1].y = 90; snake[1].x = 75; snake[1].alive = true;
	snake[2].y = 90; snake[2].x = 60; snake[2].alive = true;
	snake[3].y = 90; snake[3].x = 45; snake[3].alive = true;
	snake[4].y = 90; snake[4].x = 30; snake[4].alive = true;
	score = 0;
}

void DrawStuff(void)
{
	for(int a=1;a<=snake_size;a++) {
		if(snake[a].alive)
			DrawCube(snake[a].x, snake[a].y, 1);
	}
	DrawCube(pill.x, pill.y, 3);
	DrawCube(snake[0].x, snake[0].y, 2);
}

void CheckApple(void)
{
	if((snake[0].x == pill.x) && (snake[0].y == pill.y)) {
		pill.alive = 0;
		snake_size++;
		snake[snake_size].alive = true;
		snake[snake_size].x = snake[snake_size - 1].x;
		snake[snake_size].y = snake[snake_size - 1].y;
		score = score + 15 - speed * 2;
	}
}

void MakeApple(void)
{
	bool bSafe;
	srand(time(NULL));
	
	do {
		bSafe = true;
		pill.x=(rand() % (width/15))*15;
		pill.y=(rand() % (height/15))*15;
		pill.alive=true;
		for (int a=0;a<=snake_size;a++) {
			if((pill.x == snake[a].x) && (pill.y == snake[a].y) && (snake[a].alive))
				bSafe = false;
		}
	} 
	while (!bSafe);
}

bool CheckCrashes(void)
{
	if(((snake[0].x + dir_x) > (width - 15)) || ((snake[0].x + dir_x) < 0) || ((snake[0].y + dir_y) > (height - 15)) || ((snake[0].y + dir_y) < 0)) {
		return true;
	}
	
	for (int a=1;a<snake_size;a++) { 
		if(((snake[0].x + dir_x) == snake[a].x) && ((snake[0].y + dir_y) == snake[a].y))
			return true;
	} 
	return false;
}

extern "C"{	

	__declspec(dllexport) char *GetAddonTitle( )
	{
		char *pszTemp = "snake";
		return pszTemp;
	}
	__declspec(dllexport) char *GetAddonAuthor( )
	{
		char *pszTemp = "ruiner";
		return pszTemp;
	}

	__declspec(dllexport) char *GetAddonVersion( )
	{
		char *pszTemp = "1.2";
		return pszTemp;
	}

	__declspec(dllexport) char *GetAddonCommand( )
	{
		char *pszTemp = "bsnake";
		return pszTemp;
	}

	__declspec(dllexport) char *GetAddonDescription( )
	{
		char *pszTemp = "the snake game!";
		return pszTemp;
	}

	__declspec(dllexport) bool Pre_HUD_Key_Event( int eventcode, int keynum, const char *pszCurrentBinding )
	{
		if(eventcode) {
			if(!speed) {
				switch ( keynum )
				{
					case 49: speed = 5; sprintf(level, "NEWBIE"); NewGame(); return false; break; // 1
					case 50: speed = 4; sprintf(level, "EASY"); NewGame(); return false; break; // 2
					case 51: speed = 3; sprintf(level, "MEDIUM");NewGame(); return false; break; // 3
					case 52: speed = 2; sprintf(level, "HARD");NewGame(); return false; break; // 4
					case 53: speed = 1; sprintf(level, "EXPERT");NewGame(); return false; break; //5
				}
			}
	
			if((alive)) {
				switch ( keynum )
				{
					case 128: if(snake[0].x != snake[1].x) direction=1; return false; break; // up
					case 129: if(snake[0].x != snake[1].x) direction=2; return false; break; // down
					case 130: if(snake[0].y != snake[1].y) direction=3;	return false; break; // left
					case 131: if(snake[0].y != snake[1].y) direction=4; return false; break; // right
				}	
			}

			if((!alive) && (speed))
			{	
				switch ( keynum )
				{	
					case 32: speed = 0;	return false; break; // space
				}
			}
		}
		return true;
	}

	__declspec(dllexport) void Post_HUD_Redraw( float flTime, int intermission )
	{
		glPushMatrix( );
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glDisable( GL_TEXTURE_2D );
		glTranslatef((vp[2] - width)/2, (vp[3]-height)/2,0.0f);
		glDrawRectangle( 0,0,width+4, height+41,0.6f,0.6f,0.2f,0.2f );
		glDrawRectangle( 1,1,width+3, height+40, 0.4f,0.4f,0.4f,0.8f );
		glDrawRectangle( 2,2,width+2, 22,0.6f,0.6f,0.6f,0.9f );
		glDrawRectangle( 2, height+25, width+2, height+40,0.3f,0.3f,0.3f,0.4f );
		glPrint( 135.0f, 16.0f, 1.0f, 1.0f, 1.0f, 0.9f, "the snake game v%s", GetAddonVersion());
		glTranslatef(2.0f, 23.0f, 0.0f);
		glPrint(5, height+13, 1.0f, 1.0f, 1.0f, 0.5f, "SCORE: %i", score);
		glPrint(width/2-50, height+13, 1.0f, 1.0f, 1.0f, 0.5f, "HIGHSCORE: %i", highscore);
		glPrint(width-100, height+13, 1.0f, 1.0f, 1.0f, 0.5f, "LEVEL: %s", level);

		if(!speed) {
			glPrint( 5, 15, 1.0f, 1.0f, 1.0f, 1.0f, "How to play:");
			glPrint(10, 30, 1.0f, 1.0f, 1.0f, 1.0f, "Use your arrow keys to control the snake.");
			glPrint(10, 45, 1.0f, 1.0f, 1.0f, 1.0f, "You can't go through walls or through");
			glPrint(10, 60, 1.0f, 1.0f, 1.0f, 1.0f, "the snake or you will die.");
			glPrint(10, 75, 1.0f, 1.0f, 1.0f, 1.0f, "Eat the apples to get points!.");

			glPrint(10, 100, 1.0f, 1.0f, 1.0f, 1.0f, "Choose your level:");
			glPrint(10, 115, 1.0f, 1.0f, 1.0f, 1.0f, "(1= newbie, 2 = easy, 3= medium, 4= hard, 5= expert)");
			glPrint(10, 255, 1.0f, 1.0f, 1.0f, 1.0f, "snake was coded by ruiner");
		} else {
			if(!pill.alive) // make a new pill that's not under the snake
				MakeApple();
	
			if((timeGetTime() - tLastTime)>= (speed * 25)) {
				tLastTime = timeGetTime(); dir_x = 0; dir_y = 0;
				
				if(alive) {
					switch(direction){
						case 3: dir_x=-15; break;
						case 4: dir_x= 15; break;
						case 2: dir_y= 15; break;
						case 1:	dir_y=-15; break;
					}
				
					if (CheckCrashes()) {
						alive = false;
						if(score > highscore) {
							highscore = score;
							sprintf(FileName, "sparkyutils\\addons\\snake.dat");
							DeleteFile(FileName);
							if(fp = fopen(FileName, "at")) {
								fprintf(fp, "%i\n", highscore);
								fclose(fp);
							}
						}
					} else {
						for(int a=snake_size;a>0;a--)
						{
							snake[a].x=snake[a-1].x;
							snake[a].y=snake[a-1].y;
						}

						snake[0].x += dir_x;
						snake[0].y += dir_y;
					}

					CheckApple();
				}
			}
			DrawStuff();
			if(!alive && speed) {
				glPrint(158, 180, 1.0f, 0.5f, 0.5f, 1.0f, "GAME OVER :(");
				glPrint(132, 250, 1.0f, 1.0f, 1.0f, 1.0f, "Hit space for new game.");
			}
		}
		glEnable( GL_TEXTURE_2D );
		glDisable(GL_BLEND);
		glPopMatrix();

	}

	__declspec(dllexport) bool NeedsEnginefuncs( )
	{
		return false;
	}
}
