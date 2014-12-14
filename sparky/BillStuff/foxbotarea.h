/* ----------------------------------------------------------------------
 * FoxBot Area Zones Code - provided by RedFox for NeoTF.
 * ---------------------------------------------------------------------- */
#include "util.h"

#pragma warning (disable:4244) // float to int loss of data

#define MAX_WAYPOINTS 1024

#define W_FL_DELETED     (1<<31)

#define A_FL_1 (1<<0)
#define A_FL_2 (1<<1)
#define A_FL_3 (1<<2)
#define A_FL_4 (1<<3)

typedef struct area {
   Vector a;   // location
   Vector b;   // location
   Vector c;   // location
   Vector d;   // location
   char namea[64]; //team1's name
   char nameb[64]; //team2..
   char namec[64];
   char named[64];
   int    flags;
} AREA;

#define AREA_VERSION 1

// define the waypoint file header structure...
typedef struct {
   char filetype[8];  // should be "FoXBot"
   int  area_file_version;
   int  number_of_areas;
   char mapname[32];  // name of map for these waypoints
} AREA_HDR;

int last_area=-1;
bool area_on_last;

//my externs
extern bool blue_av[8];
extern bool red_av[8];
extern bool green_av[8];
extern bool yellow_av[8];

extern bool attack[4]; //teams attack
extern bool defend[4]; //teams defend

// waypoints with information bits (flags)

// REMOVED TO GET THIS TO COMPILE
//WAYPOINT waypoints[MAX_WAYPOINTS];

// number of waypoints currently in use
int num_waypoints;

// declare the array of head pointers to the path structures...
//PATH *paths[MAX_WAYPOINTS];

// time that this waypoint was displayed (while editing)
float wp_display_time[MAX_WAYPOINTS];

bool g_waypoint_paths = FALSE;  // have any paths been allocated?
bool g_waypoint_on = FALSE;
bool g_auto_waypoint = FALSE;
bool g_path_waypoint = FALSE;
bool g_find_waypoint = FALSE;
long g_find_wp = 0;
bool g_path_connect = TRUE;
Vector last_waypoint;
float f_path_time = 0.0;

float a_display_time[MAX_WAYPOINTS];
bool g_area_def;

//area defs...
AREA areas[MAX_WAYPOINTS];
int num_areas;
bool is_junction[MAX_WAYPOINTS];

void UTIL_BuildFileName(char *filename, char *arg1, char *arg2)
{

	filename[0]=NULL;
#ifndef __linux__
      strcpy(filename, "foxbot\\tfc\\");
#else
      strcpy(filename, "foxbot/tfc/");
#endif

   if ((arg1) && (*arg1) && (arg2) && (*arg2))
   {
      strcat(filename, arg1);

#ifndef __linux__
      strcat(filename, "\\");
#else
      strcat(filename, "/");
#endif

      strcat(filename, arg2);
   }
   else if ((arg1) && (*arg1))
   {
      strcat(filename, arg1);
   }
}

char *NeoTFGetAreaByTeam( int teamId, int index )
{
	static char sbuf[128];
	switch( teamId )
	{
		case 1: 
			strncpy( sbuf, areas[index].namea, 127 );
			break;

		case 2: 
			strncpy( sbuf, areas[index].nameb, 127 );
			break;

		case 3: 
			strncpy( sbuf, areas[index].namec, 127 );
			break;

		case 4: 
			strncpy( sbuf, areas[index].named, 127 );
			break;

		default: 
			strcpy( sbuf, "unknown location" );
	}

	if( strlen( sbuf ) == 0 ) strcpy( sbuf, "unknown location" );

	return( sbuf );
}

bool AreaInside(cl_entity_t *pEntity,int i)
{
    bool inside=false;
   if((areas[i].flags & A_FL_1)==A_FL_1 && (areas[i].flags & A_FL_2)==A_FL_2 &&
       (areas[i].flags & A_FL_3)==A_FL_3 && (areas[i].flags & A_FL_4)==A_FL_4)
   {
       //find highest/lowest (is x,y in bounds of polygon?)
       float x,y,lx,ly,hx,hy;
       x=pEntity->curstate.origin.x;
       y=pEntity->curstate.origin.y;
       lx=areas[i].a.x;
       if(areas[i].b.x<lx) lx=areas[i].b.x;
       if(areas[i].c.x<lx) lx=areas[i].c.x;
       if(areas[i].d.x<lx) lx=areas[i].d.x;
       ly=areas[i].a.y;
       if(areas[i].b.y<ly) ly=areas[i].b.y;
       if(areas[i].c.y<ly) ly=areas[i].c.y;
       if(areas[i].d.y<ly) ly=areas[i].d.y;
       hx=areas[i].a.x;
       if(areas[i].b.x>hx) hx=areas[i].b.x;
       if(areas[i].c.x>hx) hx=areas[i].c.x;
       if(areas[i].d.x>hx) hx=areas[i].d.x;
       hy=areas[i].a.y;
       if(areas[i].b.y>hy) hy=areas[i].b.y;
       if(areas[i].c.y>hy) hy=areas[i].c.y;
       if(areas[i].d.y>hy) hy=areas[i].d.y;

       //make sure theirs no rounding errors (I think vect = float..)
       lx=lx-1;
       hx=hx+1;
       ly=ly-1;
       hy=hy+1;
       //now the in-bounds check..
       if(x>=lx && x<=hx && y>=ly && y<=hy)
       {
            if (areas[i].a.y<y && areas[i].b.y>=y
            ||  areas[i].b.y<y && areas[i].a.y>=y)
            {
                if (areas[i].a.x+(y-areas[i].a.y)/(areas[i].b.y-areas[i].a.y)*(areas[i].b.x-areas[i].a.x)<x)
                    inside=!inside;
            }
            if (areas[i].b.y<y && areas[i].c.y>=y
            ||  areas[i].c.y<y && areas[i].b.y>=y)
            {
                if (areas[i].b.x+(y-areas[i].b.y)/(areas[i].c.y-areas[i].b.y)*(areas[i].c.x-areas[i].b.x)<x)
                    inside=!inside;
            }
            if (areas[i].c.y<y && areas[i].d.y>=y
            ||  areas[i].d.y<y && areas[i].c.y>=y)
            {
                if (areas[i].c.x+(y-areas[i].c.y)/(areas[i].d.y-areas[i].c.y)*(areas[i].d.x-areas[i].c.x)<x)
                    inside=!inside;
            }
            if (areas[i].d.y<y && areas[i].a.y>=y
            ||  areas[i].a.y<y && areas[i].d.y>=y)
            {
                if (areas[i].d.x+(y-areas[i].d.y)/(areas[i].a.y-areas[i].d.y)*(areas[i].a.x-areas[i].d.x)<x)
                    inside=!inside;
            }
       }
   }
   return inside;
}

int AreaInsideClosest(cl_entity_t *pEntity)
{
    int index;
    float distance;
    distance=9999.0;
    index=-1;
    for (int i=0; i < num_areas; i++)
   {
       if(AreaInside(pEntity,i))
       {
           float lz,hz,a;
           lz=areas[i].a.z;
           if(areas[i].b.z<lz) lz=areas[i].b.z;
           if(areas[i].c.z<lz) lz=areas[i].c.z;
           if(areas[i].d.z<lz) lz=areas[i].d.z;
           hz=areas[i].a.z;
           if(areas[i].b.z>hz) hz=areas[i].b.z;
           if(areas[i].c.z>hz) hz=areas[i].c.z;
           if(areas[i].d.z>hz) hz=areas[i].d.z;           
           //we want the mid point between hz and lz.. that will be our distance..
           //nearly forgot, the distance revolves around the player!
           a=abs((((hz-lz)/2)+lz)-pEntity->curstate.origin.z);
           if(a<distance)
           {
               distance=a;
               index=i;
           }
       }
   }
    return index;
}

int AreaDefPointFindNearest(cl_entity_t *pEntity, float range, int flags)
{
   int i, min_index;
   float distance;
   float min_distance;
   //TraceResult tr;
   pmtrace_t tr;
   Vector o;

   if (num_areas < 1)
      return -1;

   // find the nearest area point (not whole area)...

   min_index = -1;
   min_distance = 9999.0;

   for (i=0; i < num_areas; i++)
   {
      if (areas[i].flags & W_FL_DELETED)
         continue;  // skip any deleted waypoints

      distance=9999;

      if(flags==A_FL_1) 
      {
          distance = (areas[i].a - pEntity->curstate.origin).Length();
          o=areas[i].a;
      }
      if(flags==A_FL_2) 
      {
          distance = (areas[i].b - pEntity->curstate.origin).Length();
          o=areas[i].b;
      }
      if(flags==A_FL_3)
      {
          distance = (areas[i].c - pEntity->curstate.origin).Length();
          o=areas[i].c;
      }
      if(flags==A_FL_4)
      {
          distance = (areas[i].d - pEntity->curstate.origin).Length();
          o=areas[i].d;
      }

      if(distance==9999) continue;

      if ((distance < min_distance) && (distance < range))
      {
         // if waypoint is visible from current position (even behind head)...
		tr = *(gEngfuncs.PM_TraceLine( (float *)&pEntity->curstate.origin, (float *)&o, PM_TRACELINE_PHYSENTSONLY, 2, -1 ));
		//UTIL_TraceLine( pEntity->v.origin + pEntity->v.view_ofs, o,ignore_monsters, pEntity->v.pContainingEntity, &tr );

         if (tr.fraction >= 1.0)
         {
            min_index = i;
            min_distance = distance;
         }
      }
   }

   return min_index;
}

void AreaDefInit()
{
	int i;

	for (i=0; i < MAX_WAYPOINTS; i++)
	{
		a_display_time[i] = 0.0;
		areas[i].flags = 0;
		areas[i].a = Vector(0,0,0);
		areas[i].b = Vector(0,0,0);
		areas[i].c = Vector(0,0,0);
		areas[i].d = Vector(0,0,0);
		areas[i].namea[0] = NULL;
		areas[i].nameb[0] = NULL;
		areas[i].namec[0] = NULL;
		areas[i].named[0] = NULL;
	}
}

bool AreaDefLoad(cl_entity_t *pEntity)
{
   char mapname[64];
   char filename[256];
   AREA_HDR header;
   char msg[80];
   int i;
//return false; //test to see if having areas loaded is
//fucking up the server (mem crashes)

   strcpy(mapname, NameFromFileName(gEngfuncs.pfnGetLevelName()));
   strcat(mapname, ".far");

   FILE *bfp;

   UTIL_BuildFileName(filename, "areas", mapname);
    bfp = fopen(filename, "rb");

    if(bfp==NULL)
    {
        UTIL_BuildFileName(filename, "maps", mapname);
        bfp = fopen(filename, "rb");
    }
   
   if (bfp != NULL)
   {
      //if (IS_DEDICATED_SERVER()) printf("loading area file: %s\n", filename);
      fread(&header, sizeof(header), 1, bfp);

      header.filetype[7] = 0;
      if (strcmp(header.filetype, "FoXBot") == 0)
      {
         if (header.area_file_version != AREA_VERSION)
         {
			 /*
            if (pEntity)
               ClientPrint(pEntity, HUD_PRINTNOTIFY, "Incompatible FoXBot area file version!\nAreas not loaded!\n");
			else
               UTIL_LogPrintf( "[FoxBot] Incompatible FoXBot area file version!\nAreas not loaded!\n");
			   */
			 gEngfuncs.pfnConsolePrint("[sparky] incompatible FoXBot area file version! Areas not loaded!\n");

            fclose(bfp);
            return FALSE;
         }

         header.mapname[31] = 0;

         if (strcmp(header.mapname, NameFromFileName(gEngfuncs.pfnGetLevelName())) == 0)
         {
             //works for areas aswell <img src="images/smilies/smile.gif" border="0" alt="">
            //WaypointInit();  // remove any existing waypoints
             //grr, removes loaded waypoints! doh
            num_areas = 0;

           for (i=0; i < MAX_WAYPOINTS; i++)
           {
              a_display_time[i] = 0.0;
              areas[i].flags = 0;
              areas[i].a = Vector(0,0,0);
              areas[i].b = Vector(0,0,0);
              areas[i].c = Vector(0,0,0);
              areas[i].d = Vector(0,0,0);
              areas[i].namea[0] = NULL;
              areas[i].nameb[0] = NULL;
              areas[i].namec[0] = NULL;
              areas[i].named[0] = NULL;
           }

		   /*
            if (pEntity)
                ClientPrint(pEntity, HUD_PRINTNOTIFY, "Loading FoXBot area file\n");
			else
               UTIL_LogPrintf( "[FoxBot] Loading FoXBot area file %s\n", filename);
			*/
		   gEngfuncs.pfnConsolePrint("[sparky] loading FoXBot area file\n");

            for (i=0; i < header.number_of_areas; i++)
            {
               fread(&areas[i], sizeof(areas[0]), 1, bfp);
               num_areas++;
            }
         }
         else
         {
		    sprintf(msg, "[sparky] %s FoXBot areas are not for this map!\n", filename);
			/*
            if (pEntity)
               ClientPrint(pEntity, HUD_PRINTNOTIFY, msg);
			else 
               UTIL_LogPrintf( msg );
		   */
			gEngfuncs.pfnConsolePrint(msg);

            fclose(bfp);
            return FALSE;
         }
      }
      else
      {
		 sprintf(msg, "[sparky] %s is not a FoXBot area file!\n", filename);
		 /*
         if (pEntity)
            ClientPrint(pEntity, HUD_PRINTNOTIFY, msg);
		 else 
		    UTIL_LogPrintf( msg );
			*/
		 gEngfuncs.pfnConsolePrint(msg);

         fclose(bfp);
         return FALSE;
      }

      fclose(bfp);
   }

   return TRUE;
}

void AreaDefPrintInfo(cl_entity_t *pEntity)
{
   char msg[1024];
   int index,i;
   //int flags;


   i=AreaInsideClosest(pEntity);
   if(i!=-1)
   {
#ifndef __linux__
       _snprintf(msg,1020,"Area %d of %d total\n", i, num_areas);
       gEngfuncs.pfnConsolePrint(msg);
       _snprintf(msg,1020,"Name1 = %s\n",areas[i].namea);
       gEngfuncs.pfnConsolePrint(msg);
       _snprintf(msg,1020,"Name2 = %s\n",areas[i].nameb);
       gEngfuncs.pfnConsolePrint(msg);
       _snprintf(msg,1020,"Name3 = %s\n",areas[i].namec);
       gEngfuncs.pfnConsolePrint(msg);
       _snprintf(msg,1020,"Name4 = %s\n",areas[i].named);
       gEngfuncs.pfnConsolePrint(msg);
#else
       snprintf(msg,1020,"Area %d of %d total\n", i, num_areas);
       gEngfuncs.pfnConsolePrint(msg);
       snprintf(msg,1020,"Name1 = %s\n",areas[i].namea);
       gEngfuncs.pfnConsolePrint(msg);
       snprintf(msg,1020,"Name2 = %s\n",areas[i].nameb);
       gEngfuncs.pfnConsolePrint(msg);
       snprintf(msg,1020,"Name3 = %s\n",areas[i].namec);
       gEngfuncs.pfnConsolePrint(msg);
       snprintf(msg,1020,"Name4 = %s\n",areas[i].named);
       gEngfuncs.pfnConsolePrint(msg);
#endif
   }

   index = AreaDefPointFindNearest(pEntity, 50.0,A_FL_1);

   if (index != -1)
   {
       sprintf(msg,"Area %d of %d total\n", index, num_areas);
       gEngfuncs.pfnConsolePrint(msg);
       gEngfuncs.pfnConsolePrint("Area corner 1\n");
       return;
   }

   index = AreaDefPointFindNearest(pEntity, 50.0,A_FL_2);

   if (index != -1)
   {
       sprintf(msg,"Area %d of %d total\n", index, num_areas);
       gEngfuncs.pfnConsolePrint(msg);
       gEngfuncs.pfnConsolePrint("Area corner 2\n");
       return;
   }

   index = AreaDefPointFindNearest(pEntity, 50.0,A_FL_3);

   if (index != -1)
   {
       sprintf(msg,"Area %d of %d total\n", index, num_areas);
       gEngfuncs.pfnConsolePrint(msg);
       gEngfuncs.pfnConsolePrint("Area corner 3\n");
       return;
   }

   index = AreaDefPointFindNearest(pEntity, 50.0,A_FL_4);

   if (index != -1)
   {
       sprintf(msg,"Area %d of %d total\n", index, num_areas);
       gEngfuncs.pfnConsolePrint(msg);
       gEngfuncs.pfnConsolePrint("Area corner 4\n");
       return;
   }
}
