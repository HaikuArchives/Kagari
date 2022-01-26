#include <AppKit.h>
#include <InterfaceKit.h>
#include <StorageKit.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <GL/gl.h>
#include <GL/glu.h>
#include "SpGLView.h"


// load a 256x256 RGB .RAW file as a texture
GLuint LoadTextureRAW( const char * filename, int wrap )
{
  GLuint texture;
  int width, height;
  BFile file;
  size_t size;
  BResources rsc;
  app_info appInfo;
  unsigned char * data;
 
  // open texture data
  be_app->GetAppInfo(&appInfo);
  file.SetTo(&appInfo.ref, B_READ_ONLY);
  if (file.InitCheck() != B_OK)
    return 0;

  // read texture data
  rsc.SetTo(&file);
  width = 128;
  height = 128;
  size = width * height * 3;
  data = (unsigned char*)rsc.LoadResource(B_RAW_TYPE, filename, &size);
  
  // allocate a texture name
  glGenTextures( 1, &texture );
  
  // select our current texture
  glBindTexture( GL_TEXTURE_2D, texture );
  
  // select modulate to mix texture with color for shading
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  
  // when texture area is small, bilinear filter the closest MIP map
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		   GL_LINEAR_MIPMAP_NEAREST );
  // when texture area is large, bilinear filter the first MIP map
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  // if wrap is true, the texture wraps over at the edges (repeat)
  //       ... false, the texture ends at the edges (clamp)
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		   wrap ? GL_REPEAT : GL_CLAMP );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		   wrap ? GL_REPEAT : GL_CLAMP );
  
  // build our texture MIP maps
  gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width,
		     height, GL_RGB, GL_UNSIGNED_BYTE, data);
  
  return texture;
 
}
	   
		   
enum {SP_UPDATE_FRAME = 'Sufr'};

#ifndef _INTEL_
inline double drand48(void)
{
  return (float)(rand()) / (float)(RAND_MAX);
}
#endif

/** 型宣言 **/
  typedef GLshort vertex_index; /* 頂点index型 */
  typedef GLdouble vector[3];   /* ベクトル型 */
  typedef GLdouble point[3];    /* 座標型 */

/* オブジェクトリスト構造体 */
  typedef struct object_t{
    point *vertecis;       /* 頂点座標 */
    vertex_index *facets;  /* 面データ(頂点Index) */
    vector *normals;       /* 各面の法線 */
    int vertex_count;      /* 頂点数 */
    int facet_count;       /* 面数 */
    point pos;             /* 位置 */
    vector vec;            /* 速度 */
    struct object_t *next; /* オブジェクトへのポインタ */
  } object;

/* 稜線の分割点 */
  typedef struct{
    vertex_index vi1, vi2; /* 稜線の両端の頂点Index */
    point pos;             /* 分割位置 */
  } segment;

/* 面を分割する2点のインデックス */
  typedef struct{
    vertex_index begin,end; /* 端点 */
  } vertex_connection;


/* 最初のオブジェクト(立方体)の頂点データ */
   point my_verts[] = {
     {  1,  1,  1},
     { -1,  1,  1},
     { -1, -1,  1},
     {  1, -1,  1},
     {  1,  1, -1},
     { -1,  1, -1},
     { -1, -1, -1},
     {  1, -1, -1}
   };

/* …の面データ */
  vertex_index my_facets[] = {
    0, 1, 2, 3, -1,
    7, 6, 5, 4, -1,
    4, 5, 1, 0, -1,
    5, 6, 2, 1, -1,
    6, 7, 3, 2, -1,
    7, 4, 0, 3, -1,
  };

/* …の法線データ */
  vector my_normals[] = {
    {  0,  0,  1},
    {  0,  0, -1},
    {  0,  1,  0},
    { -1,  0,  0},
    {  0, -1,  0},
    {  1,  0,  0}
  };

/* texture matrix */
GLdouble texture_matrix[6][16] = {
  {0.5, 0, 0, 0,   0, 0.5, 0, 0,  0, 0, 0, 0,  0.5, 0.5, 0, 1},
  {-0.5, 0, 0, 0,   0, 0.5, 0, 0,  0, 0, 0, 0,  0.5, 0.5, 0, 1},
  {0.5, 0, 0, 0,   0, 0, 0, 0,  0, 0.5, 0, 0,  0.5, 0.5, 0, 1},
  {0, 0, 0, 0,   0.5, 0, 0, 0,  0, 0.5, 0, 0,  0.5, 0.5, 0, 1},
  {-0.5, 0, 0, 0,   0, 0, 0, 0,  0, 0.5, 0, 0,  0.5, 0.5, 0, 1},
  {0, 0, 0, 0,   -0.5, 0, 0, 0,  0, 0.5, 0, 0,  0.5, 0.5, 0, 1},
}
 ;
    

/* 変数宣言 */

/*** オブジェクト操作 ***/

class SpBehavior
{
public:
  SpBehavior(BGLView *owner);
  virtual ~SpBehavior(void);

  void Init(void);
  void SetView(void);
  void IdleObjects(void);
  void Draw(void);
  void Reshape(int x, int y);

  void SetV(float v);
  void SetR(float r);
  void Expand(float d);

private:
  object *NewObject(void);
  void FreeObject(object *obj);
  void MoveObjects(void);
  void DescribeObject(object *obj);
  vertex_index connectLineAndPlane(segment *seg, int *max,
				   int dir,
				   vertex_index vi1, vertex_index vi2,
				   object *obj,
				   GLdouble A,
				   GLdouble B,
				   GLdouble C,
				   GLdouble D);
  int DevideObject(object *obj, object **newobj0, object **newobj1,
		   GLdouble A, GLdouble B, GLdouble C, GLdouble D);

  object *first_object;         /* 先頭オジェクトへのポインタ */
  GLdouble spin;   /* オブジェクト回転量 */
  GLdouble aspect; /* ウィンドウアスペクト比 */
  GLdouble rr;       /* 削り出される球の半径 */
  GLdouble vr;      /* 切片の飛散速度(逆数) */

  GLuint mTexture[4];

  BGLView *mOwner;
};

void
SpBehavior::SetV(float v)
{
  /* map 0.0->1.0 to 0.01->0.5 */
  vr = v / 5 + 0.01;
}


void
SpBehavior::SetR(float r)
{
  /* map 0.0->1.0 to 1.4->0.6 */
  rr = 0.6 +  (1.0 - r) * 0.8;
}


/** 新規にオブジェクトを作成する **/
object *
SpBehavior::NewObject(void){
  object *obj;

  obj = new object;
  obj->next = (object *)NULL;
  return obj;
}

/** オブジェクトを解放する **/
void 
SpBehavior::FreeObject(object *obj){
  delete[] obj->vertecis;
  delete[] obj->facets;
  delete[] obj->normals;
  delete obj;
}


/** オブジェクトを移動量に沿って移動させる **/
void 
SpBehavior::MoveObjects(void){
  object *obj;

  obj = first_object;

  while( obj != NULL){
    obj->pos[0] += obj->vec[0];
    obj->pos[1] += obj->vec[1];
    obj->pos[2] += obj->vec[2];
    obj = obj->next;
  }
}


void
SpBehavior::Expand(float d)
{
  point *v;
  int i;

  for (i = 0, v = first_object->vertecis;
       i < first_object->vertex_count;
       i++, v++)
    {
      (*v)[0] *= d;
      (*v)[1] *= d;
      (*v)[2] *= d;
    }
}


/** オブジェクトの内容を表示 **/
void 
SpBehavior::DescribeObject(object *obj){
  int i,j;

  //  printf("vertecis:%d\nfacets:%d\n",obj->vertex_count,obj->facet_count);
#if 0
  for (i=0; i<obj->vertex_count; i++){
        printf("[%f,%f,%f]\n",obj->vertecis[i][0],
    	   obj->vertecis[i][1],
    	   obj->vertecis[i][2]);
  }
#endif
  j=0;
  for (i=0; i<obj->facet_count; i++){
    while(1){
      if (obj->facets[j] == -1){
	j++;
	puts("");
	break;
      }
      //           printf("%d,",obj->facets[j++]);
    }
  }
}


/** devideObjectのためのサブ関数
 ** 線分と面の交点を求める
 **/
vertex_index 
SpBehavior::connectLineAndPlane(segment *seg, int *max,
				int dir,
				vertex_index vi1, vertex_index vi2,
				object *obj,
				GLdouble A,
				GLdouble B,
				GLdouble C,
				GLdouble D){
  int i;
  double t, x1, x2, y1, y2, z1, z2;
  vertex_index vi;

  if (dir == 1)
    vi = vi1, vi1 = vi2, vi2 = vi;

  for (i=0; i<*max; i++){
    if (seg[i].vi1 == vi1 && seg[i].vi2 == vi2){
      return i;
    }
  }
  
  (*max)++;
  x1 = obj->vertecis[vi1][0];
  y1 = obj->vertecis[vi1][1];
  z1 = obj->vertecis[vi1][2];
  x2 = obj->vertecis[vi2][0];
  y2 = obj->vertecis[vi2][1];
  z2 = obj->vertecis[vi2][2];
  t = (A * x1 + B * y1 + C * z1 + D)
    / (A * (x1 - x2) + B * (y1 - y2) + C * (z1 - z2));
  seg[i].pos[0] = ((x2 - x1) * t + x1);
  seg[i].pos[1] = ((y2 - y1) * t + y1);
  seg[i].pos[2] = ((z2 - z1) * t + z1);
  seg[i].vi1 = vi1;
  seg[i].vi2 = vi2;
  return i;
}


/** オブジェクトを2つに分割する **/
/* 略語.fc = facets_count 面数;
   vc = vartex_count 頂点数;
   vic = vertex_index_count 頂点配列(面データ)カウント;
   dir = 判別面の上下
   */
int
SpBehavior::DevideObject(object *obj, object **newobj0, object **newobj1,
			 GLdouble A, GLdouble B, GLdouble C, GLdouble D){
  GLdouble E, F;
  object *newobj[2];
  segment *mask_vertecis;
  int i, j = 0, dir;
  int *old_vertex_dir;
  int new_fc[2];
  int mask_vc, new_vc[2];
  int mask_vic, new_vic[2], old_vic;
  vertex_index begin, end, viprev, vi, vi1;
  vertex_index *vi_old2new, *mask_facet, *new_facets[2];
  vertex_connection *mask_vertex_connection;
  vector *new_normals[2];

  /* 各頂点について、分割面のどちら側にあるかをチェック */
  new_vc[0] = new_vc[1] = 0;
  old_vertex_dir = new int[obj->vertex_count];
  vi_old2new = new vertex_index[obj->vertex_count];
  D += ( A*obj->pos[0] + B*obj->pos[1] + C*obj->pos[2] );
  for (i=0; i<obj->vertex_count; i++){
    F = 
      A*obj->vertecis[i][0]
	+ B*obj->vertecis[i][1]
	  + C*obj->vertecis[i][2] + D;
    if (F > 0.0){
      vi_old2new[i] = new_vc[0]++;
      old_vertex_dir[i] = 0;
    }else if (F < 0.0){
      vi_old2new[i] = new_vc[1]++;
      old_vertex_dir[i] = 1;
    }else{
      fprintf (stderr, "a vertex is to be compirsed of division mask.\n");
      exit(1);
    }
  }
  if (new_vc[0] * new_vc[1] == 0){
    delete [] old_vertex_dir;
    delete [] vi_old2new;
    return 0;
  }


  /* 実際の分割処理 */
  old_vic = 0;
  for (i=0; i<obj->facet_count; i++)
    while(1)
      if (obj->facets[old_vic++] == -1)	break;
  old_vic++;
  new_facets[0] = new vertex_index[old_vic + obj->facet_count];
  new_facets[1] = new vertex_index[old_vic + obj->facet_count];
  new_normals[0] = new vector[obj->facet_count + 1];
  new_normals[1] = new vector[obj->facet_count + 1];
  mask_vertecis = new segment[obj->facet_count];
  mask_vertex_connection = new vertex_connection[obj->facet_count];
  new_fc[0] = new_fc[1] = 0;
  new_vic[0] = new_vic[1] = mask_vic = old_vic = 0;
  mask_vc = 0;

  for(i=0; i < obj->facet_count; i++){
    vi = vi1 = obj->facets[old_vic++];
    dir = old_vertex_dir[vi1];
    new_facets[dir][new_vic[dir]++] = vi_old2new[vi];

    while (1){
      viprev = vi;
      vi = obj->facets[old_vic++];
      if (vi == -1){  /* この面は分割されていない */
	memcpy (new_normals[dir][new_fc[dir]],
		obj->normals[i], sizeof (vector));
	new_facets[dir][new_vic[dir]++] = -1;
	new_fc[dir]++;
	break;
      }
      if (old_vertex_dir[vi] == dir){  /* とりあえずコピ */
	new_facets[dir][new_vic[dir]++] = vi_old2new[vi];
      }
      else{  /* 分割開始 */
	begin = connectLineAndPlane (mask_vertecis, &mask_vic, 
					dir, viprev, vi,
					obj, A, B, C, D);
	new_facets[dir][new_vic[dir]++] = new_vc[dir] + begin;
	dir = abs (dir - 1);
	new_facets[dir][new_vic[dir]++] = new_vc[dir] + begin;
	new_facets[dir][new_vic[dir]++] = vi_old2new[vi];

	while(1){
	  viprev = vi;
	  vi = obj->facets[old_vic++];
	  if (vi == -1){  /*最初点と最終点が分割線だった */
	    end = connectLineAndPlane (mask_vertecis, &mask_vic,
					  dir, viprev, vi1,
					  obj, A, B, C, D);
	    new_facets[dir][new_vic[dir]++] = new_vc[dir] + end;
	    memcpy (new_normals[dir][new_fc[dir]],
		    obj->normals[i], sizeof (vector));
	    new_facets[dir][new_vic[dir]++] = -1;
	    new_fc[dir]++;
	    dir = abs (dir - 1);
	    new_facets[dir][new_vic[dir]++] = new_vc[dir] + end;
	    memcpy (new_normals[dir][new_fc[dir]],
		    obj->normals[i], sizeof (vector));
	    new_facets[dir][new_vic[dir]++] = -1;
	    new_fc[dir]++;
	    break;
	  }
	  if (old_vertex_dir[vi] == dir){
	    new_facets[dir][new_vic[dir]++] = vi_old2new[vi];
	  } else {
	    end = connectLineAndPlane (mask_vertecis, &mask_vic,
					  dir, viprev, vi,
					  obj, A, B, C, D);
	    new_facets[dir][new_vic[dir]++] = new_vc[dir] + end;
	    memcpy (new_normals[dir][new_fc[dir]],
		    obj->normals[i], sizeof (vector));
	    new_facets[dir][new_vic[dir]++] = -1;
	    new_fc[dir]++;
	    dir = abs (dir - 1);
	    new_facets[dir][new_vic[dir]++] = new_vc[dir] + end;
	    new_facets[dir][new_vic[dir]++] = vi_old2new[vi];

	    while(1){
	      viprev = vi;
	      vi = obj->facets[old_vic++];
	      if (vi == -1){
		memcpy (new_normals[dir][new_fc[dir]],
			obj->normals[i], sizeof (vector));
		new_facets[dir][new_vic[dir]++] = -1;
		new_fc[dir]++;
		break;
	      }
	      else {
		new_facets[dir][new_vic[dir]++] = vi_old2new[vi];
	      }
	    }
	    break;
	  }
	}
	if (dir == 1)
	  vi = end, end = begin, begin = vi;
	mask_vertex_connection[mask_vc].begin = begin;
	mask_vertex_connection[mask_vc].end = end;
	mask_vc++;
	break;
      }
    }
  }

  mask_facet = new vertex_index[mask_vic];
  vi = 0;
  for (i=0; i<mask_vic- 1; i++){
    mask_facet[i] = mask_vertex_connection[vi].begin;
    end = mask_vertex_connection[vi].end;
    for (j=0; j<mask_vic; j++){
      if (mask_vertex_connection[j].begin == end){
	vi = j;
	break;
      }
    }
  }
  mask_facet[i] = mask_vertex_connection[j].begin;

  /* 分割終了。後処理 */
  E = sqrt (A*A + B*B + C*C);
  
  for (i=0; i<2; i++){
    newobj[i] = NewObject();
    memcpy (newobj[i], obj, sizeof (object));
    newobj[i]->vertex_count = new_vc[i] + mask_vic;
    newobj[i]->facet_count = new_fc[i] + 1;
    newobj[i]->vertecis = new point[new_vc[i] + mask_vic];
    newobj[i]->facets = new vertex_index[new_vic[i] + mask_vic + 1];
    memcpy(newobj[i]->facets, new_facets[i], new_vic[i] * sizeof(vertex_index));
    delete[] new_facets[i];
#if 0
    newobj[i]->facets =  realloc (new_facets[i],
				  (new_vic[i] + mask_vic + 1) *
				  sizeof (vertex_index));
#endif
    for (j=0; j<obj->vertex_count; j++)
      if (old_vertex_dir[j] == i)
	memcpy(newobj[i]->vertecis[vi_old2new[j]], obj->vertecis[j], sizeof(point));
    for (j=0; j<mask_vic; j++)
      memcpy(newobj[i]->vertecis[new_vc[i]+j], mask_vertecis[j].pos, sizeof(point));
    if (i == 1){
      for (j=0; j<mask_vic; j++)
	newobj[i]->facets[new_vic[i]+j] = new_vc[i] +mask_facet[j];
      new_normals[i][new_fc[i]][0] =  A/E;
      new_normals[i][new_fc[i]][1] =  B/E;
      new_normals[i][new_fc[i]][2] =  C/E;
    }else{
      for (j=0; j<mask_vic; j++)
	newobj[i]->facets[new_vic[i]+j] = new_vc[i] +mask_facet[mask_vic -j-1];
      new_normals[i][new_fc[i]][0] = -A/E;
      new_normals[i][new_fc[i]][1] = -B/E;
      new_normals[i][new_fc[i]][2] = -C/E;
    }
    newobj[i]->facets[new_vic[i]+j] = -1;
    newobj[i]->normals = new vector[new_fc[i] + 1];
    memcpy(newobj[i]->normals, new_normals[i], (new_fc[i] + 1) * sizeof(point));
    delete[] new_normals[i];
#if 0
    newobj[i]->normals = 
      realloc (new_normals[i], (new_fc[i] + 1) * sizeof (vector));
#endif
  }
  *newobj0 = newobj[0];
  *newobj1 = newobj[1];
  delete [] old_vertex_dir;
  delete [] vi_old2new;
  delete [] mask_vertecis;
  delete [] mask_vertex_connection;
  delete [] mask_facet;
  return 1;
}


/*** 環境操作 ***/

/** 視環境の設定 **/
void 
SpBehavior::SetView (void) {
  mOwner->LockGL();

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (40, 1.0, 0.1, 10000.0);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  gluLookAt (sin(spin)*6.0, cos(spin)*6.0, -2.0,
	     0.0, 0.0, 0.0,
	     -sin(spin), -cos(spin), -1.0);

  mOwner->UnlockGL();
}

/** Idle Function **/
void 
SpBehavior::IdleObjects(void) {
  object *obj, *obj1, *obj2, *prev;
  GLdouble A, B, C, D, E;

  if (spin > 5.7){
    prev = obj = first_object;
    while (obj != NULL){
      if (sqrt (pow(obj->pos[0],2) + pow(obj->pos[1],2) + pow(obj->pos[2],2))
	  > 5.0){
	prev->next = obj->next;
	FreeObject(obj);
	obj = prev->next;
	continue;
      }
      prev = obj;
      obj = obj->next;
    }
    A = drand48()*2-1; B = drand48()*2-1; C = drand48()*2-1;
    E = sqrt (pow (A,2) + pow(B,2) + pow (C,2)) * rr;
    A = A/E, B = B/E, C = C/E;
    D = pow (A,2) + pow (B,2) + pow (C,2);
    obj = first_object;
    if (DevideObject (obj, &obj1, &obj2, A, B, C, D) == 1){
      obj1->next = obj2;
      obj2->next = obj->next;
      
      FreeObject(obj);
      first_object = obj1;
      obj2->vec[0] = -A * vr, obj2->vec[1] = -B * vr, obj2->vec[2] = -C * vr;
    }
  }
  MoveObjects ();
  spin += 0.02;
//  printf ("[vertecis:%d  facets:%d]\n",
//	  first_object->vertex_count, first_object->facet_count);
}


/** オブジェクト表示 **/
void
SpBehavior::Draw()
{
  object *obj;
  int i, j, fc;

  mOwner->LockGL();

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  obj = first_object;
  while (obj != NULL){
    glPushMatrix ();
    glTranslated (obj->pos[0],
		  obj->pos[1],
		  obj->pos[2]);
    fc = 0;
    for (i=0; i < obj->facet_count; i++){
      for (j = 2; j < 6; j++)
	if (my_normals[j][0] == obj->normals[i][0] &&
	    my_normals[j][1] == obj->normals[i][1] &&
	    my_normals[j][2] == obj->normals[i][2])
	  break;

      if (j == 6)
	{
	  glBindTexture( GL_TEXTURE_2D, 0 );
  
	  glBegin(GL_POLYGON);
	  glNormal3dv(obj->normals[i]);
	  //	  glDisable(GL_TEXTURE_2D);
	  do {
	    glVertex3dv(obj->vertecis[obj->facets[fc++]]);
	  } while (obj->facets[fc] != -1);

	  glEnd();
	}
      else
	{
	  glBindTexture( GL_TEXTURE_2D, mTexture[j - 2] );
	  //	  glEnable(GL_TEXTURE_2D);
	  glMatrixMode(GL_TEXTURE);
	  glLoadMatrixd(texture_matrix[j]);
	  //	  glLoadIdentity();
	  glMatrixMode(GL_MODELVIEW);

	  glBegin(GL_POLYGON);
	  glNormal3dv(obj->normals[i]);

	  do {
	    glTexCoord3dv(obj->vertecis[obj->facets[fc]]);
	    glVertex3dv(obj->vertecis[obj->facets[fc++]]);
	  } while (obj->facets[fc] != -1);

	  glEnd();
	}

      fc ++;
    }
    glPopMatrix ();
    obj = obj->next;
  }
  glFlush ();

  mOwner->UnlockGL();
}


/** ウィンドウのリサイズ **/
void
SpBehavior::Reshape(int x, int y){
  aspect = (GLdouble)x / (GLdouble)y;
  glViewport (0, 0, x, y);
}


/** プログラム終了 **/
void ExitWindow (){
  exit (0);
}

/** Initialization **/
void
SpBehavior::Init(void)
{
  mOwner->LockGL();

  GLfloat L_ambient[] = {0.2, 0.2, 0.2, 1.0};    /* 環境光 */
  GLfloat L_diffuse[] = {1.0, 1.0, 1.0, 1.0};    /* 拡散光 */
  GLfloat L_specular[] = {1.0, 1.0, 1.0, 1.0};   /* 反射光 */
  GLfloat L_position[] = {-5.0, -7.0, 10.0, 0.0};/* 光源 */
  GLfloat M_ambient[] = {1.0, 1.0, 1.0, 1.0};    /* 材質:環境光 */
  GLfloat M_diffuse[] = {0.5, 0.5, 0.5, 1.0};    /* 材質:拡散光 */
  GLfloat M_specular[] = {0.9, 0.9, 0.9, 1.0};   /* 材質:反射光 */
  GLfloat M_emission[] = {0.0, 0.0, 0.0, 1.0};   /* 反射係数 */


  object *obj; /* オブジェクトへのポインタ */

  spin = 0.0;
  aspect = 1.0;
  rr = 1;
  vr = 0.1;

  /* オブジェクト初期設定 */
  first_object = obj = NewObject();
  obj->pos[0] = 0; obj->pos[1] = 0; obj->pos[2] = 0;
  obj->vec[0] = 0; obj->vec[1] = 0; obj->vec[2] = 0;
  obj->vertecis = new point[8];
    memcpy (obj->vertecis, my_verts, sizeof (point) * 8);
  obj->facets = new vertex_index[5 * 6];
    memcpy (obj->facets, my_facets, sizeof (vertex_index) * 5 * 6);
  obj->normals = new vector[6];
    memcpy (obj->normals, my_normals, sizeof (vector) * 6);
  obj->vertex_count = 8;
  obj->facet_count = 6;

  /** OpenGL初期化 **/
  /* Window初期化 */
  glClearColor (0.0, 0.0, 0.0, 1.0);

  /* texture settings */
  glEnable(GL_TEXTURE_2D);
  mTexture[0] = LoadTextureRAW("texture1", 1);
  mTexture[1] = LoadTextureRAW("texture4", 1);
  mTexture[2] = LoadTextureRAW("texture3", 1);
  mTexture[3] = LoadTextureRAW("texture2", 1);
 
  /* ライティング設定 */
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);
  glLightfv (GL_LIGHT0, GL_AMBIENT, L_ambient);
  glLightfv (GL_LIGHT0, GL_DIFFUSE, L_diffuse);
  glLightfv (GL_LIGHT0, GL_SPECULAR, L_specular);
  glLightfv (GL_LIGHT0, GL_POSITION, L_position);

  /* 材質設定 */
  glMaterialfv (GL_FRONT, GL_AMBIENT, M_ambient);
  glMaterialfv (GL_FRONT, GL_DIFFUSE, M_diffuse);
  glMaterialfv (GL_FRONT, GL_SPECULAR, M_specular);
  glMaterialfv (GL_FRONT, GL_EMISSION, M_emission);
  glMateriali (GL_FRONT, GL_SHININESS, 60);
  
  /*フラット シェーディングに設定 */
  glShadeModel (GL_FLAT);

  /* 法線正規化モードon */
  glEnable (GL_NORMALIZE);
  /*  glEnable (GL_AUTO_NORMAL);*/

  /* ポリゴンモード設定(表面のみ表示) */
  glPolygonMode (GL_FRONT, GL_FILL);
  glCullFace (GL_BACK);
  glEnable (GL_CULL_FACE);

  /* Depth buffer使用宣言 */
  glEnable (GL_DEPTH_TEST);

  mOwner->UnlockGL();
}


SpBehavior::SpBehavior(BGLView *owner)
 : mOwner(owner)
{
  this->Init();
}

SpBehavior::~SpBehavior(void)
{
  object *obj, *next;

  obj = first_object;
  while (obj)
    {
      next = obj->next;
      this->FreeObject(obj);
      obj = next;
    }

  for (int i = 0; i < 4; i++)
    glDeleteTextures(1, &mTexture[i]);
}


SpGLView::SpGLView(BRect frame, uint32 type)
  : BGLView(frame, "SpGLView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS, type)
  , mInitFrame(frame)
{
  mBehavior = new SpBehavior(this);
}

SpGLView::~SpGLView(void)
{
  delete mBehavior;
}

void
SpGLView::Restart(void)
{
  mLocker.Lock();

  delete mBehavior;
  mBehavior = new SpBehavior(this);
  mBehavior->Reshape(this->Bounds().Width(), this->Bounds().Height());

  mLocker.Unlock();
};


void
SpGLView::SetV(float v)
{
  mLocker.Lock();

  mBehavior->SetV(v);

  mLocker.Unlock();
}


void
SpGLView::SetR(float r)
{
  mLocker.Lock();

  mBehavior->SetR(r);

  mLocker.Unlock();
}


void
SpGLView::Expand(float d)
{
  mLocker.Lock();

  mBehavior->Expand(d);

  mLocker.Lock();
}



void
SpGLView::AttachedToWindow(void)
{
  mLocker.Lock();

  BGLView::AttachedToWindow();
  mBehavior->Reshape(this->Bounds().Width(), this->Bounds().Height());
  this->Looper()->PostMessage(SP_UPDATE_FRAME, this);

  mLocker.Unlock();
}


void
SpGLView::FrameResized(float newWidth, float newHeight)
{
  mLocker.Lock();

  BGLView::FrameResized(newWidth, newHeight);
  mBehavior->Reshape(newWidth, newHeight);

  mLocker.Unlock();
}


void
SpGLView::Update(void)
{
  mLocker.Lock();

  bigtime_t time;

  time = real_time_clock_usecs();

  if (!this->Window()->IsHidden())
    {
      mBehavior->IdleObjects();
      this->LockGL();
      mBehavior->SetView();
      mBehavior->Draw();
      this->SwapBuffers();
      this->UnlockGL();
    }

  time = real_time_clock_usecs() - time;

  if (time < 30 * 1000)
    ::snooze(30 * 1000 - time);

  this->Looper()->PostMessage(SP_UPDATE_FRAME, this);

  mLocker.Unlock();
}


void
SpGLView::MessageReceived(BMessage *msg)
{
  switch (msg->what)
    {
    default:
      BGLView::MessageReceived(msg);
      break;
      
    case SP_UPDATE_FRAME:
      if (this->Window())
	this->Update();
      break;
    }
}


void
SpGLView::ErrorCallback(GLenum whichError) {
  fprintf(stderr, "Unexpected error occured (%d):\\n", (int)whichError);
  fprintf(stderr, "    %s\\n", gluErrorString(whichError));
}



void
SpGLView::GetPreferredSize(float *width, float *height)
{
  *width = mInitFrame.Width();
  *height = mInitFrame.Height();
}
