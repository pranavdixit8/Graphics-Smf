/****************************************************************************

 //Adapted from example 5 of the Glui setup ---------------Pranav Dixit

****************************************************************************/



#include <string.h>
#include <GL/glui.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//using namespace std;

float xy_aspect;
int   last_x, last_y;
float rotationX = 0.0, rotationY = 0.0;

/** These are the live variables passed into GLUI ***/
int   wireframe = 0;
int   obj_type = 1;
int   segments = 40;
int   light0_enabled = 1;
int   light1_enabled = 1;

int open_file = 1;
int output_file=1;
int load_mesh = 0;
int save_file = 0;

// char open_filetext[sizeOf(GLUI_String)] = "sphere";
// char save_filetext[sizeOf(GLUI_String)] = "";

// char open_filename[] = "mesh/sphere.smf";
// char save_filename[] = "";

float light0_intensity = 1.0;
float light1_intensity = .4;
int   main_window;
float scale = 1.0;
int   show_sphere=1;
int   show_torus=1;
int   show_axes = 1;
int   show_text = 1;
float sphere_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float torus_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float view_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float obj_pos[] = { 0.0, 0.0, 0.0 };
const char *string_list[] = { "flat shaded", "smooth shaded", "wireframe", "shaded with mesh" };
int   curr_string = 0;

/** Pointers to the windows and some of the controls we'll create **/
GLUI *glui, *glui2;
GLUI_Spinner    *light0_spinner, *light1_spinner;
GLUI_RadioGroup *radio;
GLUI_Panel      *obj_panel;

/********** User IDs for callbacks ********/
#define LIGHT0_ENABLED_ID    200
#define LIGHT1_ENABLED_ID    201
#define LIGHT0_INTENSITY_ID  250
#define LIGHT1_INTENSITY_ID  260
#define ENABLE_ID            300
#define DISABLE_ID           301
#define SHOW_ID              302
#define HIDE_ID              303
#define SHADDING_ID          304


/********** Miscellaneous global variables **********/

GLfloat light0_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
GLfloat light0_diffuse[] =  {.6f, .6f, 1.0f, 1.0f};
GLfloat light0_position[] = {.5f, .5f, 1.0f, 0.0f};

GLfloat light1_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
GLfloat light1_diffuse[] =  {.9f, .6f, 0.0f, 1.0f};
GLfloat light1_position[] = {-1.0f, -1.0f, 1.0f, 0.0f};

GLfloat lights_rotation[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };


/* 
SMF file handling and loading
*/

class Smf {

private:

  bool getEdgeList();
  bool getNormalList();


public:

std::vector<std::vector<GLfloat>> vertices,

std::vector<std::vector<vector<size_t>> faces;

std::vector <std::pair<size_t,size_t>> edges;

std::map<size_t, std::vector<GLfloat> > face_normals;

std::map<size_t, std::vector<GLfloat> > vertex_normals;

friend std::ostream& operator<< (std::ostream& os, const Smf& smf);
Smf(std::string &file){
}

bool load(std::string &file);
bool save(std::string &file);

}

std::ostream& operator<< (std::ostream& os, const SmfModel& smf)
{

for(std::vector<std::vector<GLfloat>>::const_iterator i = smf.vertices.begin();i != smf.vertices.end(); i++){

  os << "v ";

  for( std::vector<GLfloat>::const_iterator j  = i.begin(); j != i.end(); j++){

    os << *j << " ";

  }

  os << std::endl;

}

for( std::vector<std::vector<size_t>>::const_iterator i = smf.faces.begin(); i! = smf.faces.end(); i++){

  os << "f ";

  for( std::vector<size_t>::const_iterator j = i.begin(); j!= i.end(); j++){

    os << *j << " ";

  }

  os << std::endl;
}


for( std::set<std::pair<size_t,size_t>>::const_iterator i = smf.edges.begin(); i! = smf.edges.end(); i++){

  os << "e "<< i -> first << " " << i -> second << std::endl;
}


return os;

}

bool Smf::load(std::string &file)

{

  std::ifstream stream( file.c_str());

  if(!stream){

  cout<< " Error occured while opening the file";

    return false;
  }

  else{

    vertices.clear();
    faces.clear();
  }

std::string l;

while(std::getline(stream, l)){

if(l.size()<){
  continue;
}

std::istringstream iss(l.substr(1));

std::vector<GLfloat> vertex(3);
std::vector<size_t> face(3);
std::vector<GLfloat> diff01(3);
std::vector<GLfloat> diff12(3);
std::vector<GLfloat> normal(3);
GLfloat length;

switch(l[0]){

  case "v":

  iss>> vertex[0]>>vertex[1]>>vertex[2];
  vertices.push_back(vertex);
  break;

  case "f":

  iss>> face[0]>>face[1] >>face[2];
  faces.push_back(face);

  for(int j =0; j<3;j++){

    diff01 = vertices[face[1]- 1][j] - vertices[face[0]-1][j];
    diff12 = vertices[face[2]-1][j]- vertices[face[1]-1][j];

  }

  normal[0] = diff01[1]*diff12[2]- diff01[2]*diff12[1];
  normal[1] = diff01[2]*diff12[0]- diff01[0]*diff12[2];
  normal[0] = diff01[0]*diff12[1]- diff01[1]*diff12[0];
  break;

  length = std::sqrt(pow(normal[0],2)+pow(normal[1],2)+pow(normal[2],2));

  for (int i = 0 ; i < 3; i++){
    normal[i]/=length;
  }
  face_normals.insert(std::make_pair(faces.size()-1,normal));

  for(int i = 0; i<3, i++){

    if( vertex_normals.find(face[i]) == vertex_normals.end())

      vertex_normals[face[j]] = normal;
  else{

    for(int j = 0; j<3;j++){

      vertex_normals[face[j]][k] += normal[k];
    }
  }
}
  break;
  case "#":
    break;

  }


}

stream.close();

getEdgeList();

return true;

}


bool Smf::save(std::string &file){


  std::fstream f;

  f.open(file.c_str(), std::ios_base::out | std::ios_base::in);

  if(f.is_open())
  {
    std::cerr<< "File already exists, Please choose a different name";

    f.close();
  }
  else{

    f.clear();

    f.open(file.c_str(), std::ios_base::out);

    for( std::vector<std::vector<GLfloat>>::iterator i = vertices.begin(); i!= vertices.end(); i++){

      f << "v";

      for(std::vector<GLfloat>:: iterator j = i->begin(); j!= i->end(); j++){
        f << " " << *j;

      }
      f <<std::endl; 


    }

    for( std::vector<std::vector<size_t>>::iterator i = faces.begin(); i!= faces.end(); i++){

      f << "f";

      for( std::vector<size_t>::iterator j = i->begin(); j!= i->end(); j++){

        f << " "<<*j;
      }

      f<<std::endl;
    }

  }

  f.close();
}



bool Smf::display(){

glBegin(GL_TRIANGLES);

for(int i = 0 ; i< faces.size(); i++){


  for( int j = 0 ; j < faces[i].size(); j++){

    std::vector<GLfloat> normal;

    switch(curr_string){

      case 0: 
      normal = face_normals[i];
      break;

      default: 
      normal = vertex_normals[i];
      break;
    }

    glNormal3f(normal[0],normal[1],normal[2]);
    glVertex3f(vertices[faces[i][j] - 1][0],vertices[faces[i][j] - 1][1],vertices[faces[i][j] - 1][2]);

  }
}

glEnd();

return true;

}

void Smf:: getEdgeList()
{

  edges.clear();

  for(std::vector<std::vector<size_t>>::iterator i = faces.begin(); i!= faces.end(); i++){

    for(++ std::std::vector<std::vector<size_t>>::iterator j =i.begin(); j!= i.end(); j++){
      

      if( *(j-1) < *j){

        edges.insert(std::make_pair(*(j-1),*j));

      }
    }

      if( i->back()< i-> front())
      {

        edges.insert(std::make_pair(i->back(),i->front()));

      }
    }

    return;
  }










/**************************************** control_cb() *******************/
/* GLUI control callback                                                 */

void control_cb( int control )
{

if(control == SHADDING_ID){
  switch (curr_string){

        //flat shaded
        case 0 : 
                glShadeModel(GL_FLAT);
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                break;
        //smooth shaded
        case 1 : 
                glShadeModel(GL_SMOOTH);
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                break;
        //wireframe
        case 2: 
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                break;
        //shaded with mesh
        case 3:
                glShadeModel(GL_SMOOTH);
                glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                

      }
    }

  if ( control == LIGHT0_ENABLED_ID ) {
    if ( light0_enabled ) {
      glEnable( GL_LIGHT0 );
      light0_spinner->enable();
    }
    else {
      glDisable( GL_LIGHT0 ); 
      light0_spinner->disable();
    }
  }
  else if ( control == LIGHT1_ENABLED_ID ) {
    if ( light1_enabled ) {
      glEnable( GL_LIGHT1 );
      light1_spinner->enable();
    }
    else {
      glDisable( GL_LIGHT1 ); 
      light1_spinner->disable();
    }
  }
  else if ( control == LIGHT0_INTENSITY_ID ) {
    float v[] = { 
      light0_diffuse[0],  light0_diffuse[1],
      light0_diffuse[2],  light0_diffuse[3] };
    
    v[0] *= light0_intensity;
    v[1] *= light0_intensity;
    v[2] *= light0_intensity;

    glLightfv(GL_LIGHT0, GL_DIFFUSE, v );
  }
  else if ( control == LIGHT1_INTENSITY_ID ) {
    float v[] = { 
      light1_diffuse[0],  light1_diffuse[1],
      light1_diffuse[2],  light1_diffuse[3] };
    
    v[0] *= light1_intensity;
    v[1] *= light1_intensity;
    v[2] *= light1_intensity;

    glLightfv(GL_LIGHT1, GL_DIFFUSE, v );
  }
  else if ( control == ENABLE_ID )
  {
    glui2->enable();
  }
  else if ( control == DISABLE_ID )
  {
    glui2->disable();
  }
  else if ( control == SHOW_ID )
  {
    glui2->show();
  }
  else if ( control == HIDE_ID )
  {
    glui2->hide();
  }


//   else if (control == open_file)
//   {
//     strcpy(open_filename,"mesh/");
//     strcat(open_filename,open_filetext);
//     strcat(open_filename,".smf")
//   }
//   else if (control == load_mesh)
//   {
// smf_model.loadFile(open_filename);
// glutPostRedisplay();

//   }
//   else if (control == output_file)
//   {
// strcpy(open_filename,"mesh/");
//     strcat(open_filename,save_filetext);
//     strcat(open_filename,".smf")

//   }

//   else if (control == save_file)

//   {
// smf_model.save(save_filename)
//   }

}

/**************************************** myGlutKeyboard() **********/

void myGlutKeyboard(unsigned char Key, int x, int y)
{
  switch(Key)
  {
  case 27: 
  case 'q':
    exit(0);
    break;
  };
  
  glutPostRedisplay();
}


/***************************************** myGlutMenu() ***********/

void myGlutMenu( int value )
{
  myGlutKeyboard( value, 0, 0 );
}


/***************************************** myGlutIdle() ***********/

void myGlutIdle()
{
  /* According to the GLUT specification, the current window is 
     undefined during an idle callback.  So we need to explicitly change
     it if necessary */
  if ( glutGetWindow() != main_window ) 
    glutSetWindow(main_window);  

  /*  GLUI_Master.sync_live_all();  -- not needed - nothing to sync in this
                                       application  */

  glutPostRedisplay();
}

/***************************************** myGlutMouse() **********/

void myGlutMouse(int button, int button_state, int x, int y )
{
}


/***************************************** myGlutMotion() **********/

void myGlutMotion(int x, int y )
{
  glutPostRedisplay(); 
}

/**************************************** myGlutReshape() *************/

void myGlutReshape( int x, int y )
{
  int tx, ty, tw, th;
  GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
  glViewport( tx, ty, tw, th );

  xy_aspect = (float)tw / (float)th;

  glutPostRedisplay();
}


/************************************************** draw_axes() **********/
/* Disables lighting, then draws RGB axes                                */

void draw_axes( float scale )
{
  glDisable( GL_LIGHTING );

  glPushMatrix();
  glScalef( scale, scale, scale );

  glBegin( GL_LINES );
 
  glColor3f( 1.0, 0.0, 0.0 );
  glVertex3f( .8f, 0.05f, 0.0 );  glVertex3f( 1.0, 0.25f, 0.0 ); /* Letter X */
  glVertex3f( 0.8f, .25f, 0.0 );  glVertex3f( 1.0, 0.05f, 0.0 );
  glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 1.0, 0.0, 0.0 ); /* X axis      */

  glColor3f( 0.0, 1.0, 0.0 );
  glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 0.0, 1.0, 0.0 ); /* Y axis      */

  glColor3f( 0.0, 0.0, 1.0 );
  glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 0.0, 0.0, 1.0 ); /* Z axis    */
  glEnd();

  glPopMatrix();

  glEnable( GL_LIGHTING );
}


/***************************************** myGlutDisplay() *****************/

void myGlutDisplay()
{
  glClearColor( .9f, .9f, .9f, 1.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glFrustum( -xy_aspect*.04, xy_aspect*.04, -.04, .04, .1, 15.0 );

  glMatrixMode( GL_MODELVIEW );

  glLoadIdentity();
  glMultMatrixf( lights_rotation );
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  
  glLoadIdentity();
  glTranslatef( 0.0, 0.0, -2.6f );
  glTranslatef( obj_pos[0], obj_pos[1], -obj_pos[2] ); 
  glMultMatrixf( view_rotate );

  glScalef( scale, scale, scale );

  /***   These are _live_ variables, which are transparently 
    updated by GLUI ***/

  glPushMatrix();
  glTranslatef( 0.0, -0.25, 0.0 );
  glMultMatrixf( torus_rotate );

  if(show_torus){

    glutSolidTorus( .15,.3,16,segments );
    glShadeModel(GL_FLAT);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

     switch (curr_string){

        //flat shaded
        case 0 : 
                glShadeModel(GL_FLAT);
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                break;
        //smooth shaded
        case 1 : 
                glShadeModel(GL_SMOOTH);
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                break;
        //wireframe
        case 2: 
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                break;
        //shaded with mesh
        case 3:
                glShadeModel(GL_SMOOTH);
                glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );


      }

  }
  if ( show_axes )
    draw_axes(.52f);
  glPopMatrix();

  if ( show_text ) 
  {
    glDisable( GL_LIGHTING );  /* Disable lighting while we render text */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0.0, 100.0, 0.0, 100.0  );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glColor3ub( 0, 0, 0 );
    glRasterPos2i( 10, 10 );

    /*  printf( "text: %s\n", text );              */

    /*** Render the live character array 'text' ***/
    int i;
    for( i=0; i<(int)strlen( string_list[curr_string] ); i++ )
      glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, string_list[curr_string][i] );
  }

  glEnable( GL_LIGHTING );


  glutSwapBuffers(); 
}


/**************************************** main() ********************/

int main(int argc, char* argv[])
{
  /****************************************/
  /*   Initialize GLUT and create window  */
  /****************************************/

  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
  glutInitWindowPosition( 50, 50 );
  glutInitWindowSize( 800, 600 );
 
  main_window = glutCreateWindow( "GLUI Example 5" );
  glutDisplayFunc( myGlutDisplay );
  GLUI_Master.set_glutReshapeFunc( myGlutReshape );  
  GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );
  GLUI_Master.set_glutSpecialFunc( NULL );
  GLUI_Master.set_glutMouseFunc( myGlutMouse );
  glutMotionFunc( myGlutMotion );

  /****************************************/
  /*       Set up OpenGL lights           */
  /****************************************/

  glEnable(GL_LIGHTING);
  glEnable( GL_NORMALIZE );

  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
  glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

  /****************************************/
  /*          Enable z-buferring          */
  /****************************************/

  glEnable(GL_DEPTH_TEST);

  /****************************************/
  /*         Here's the GLUI code         */
  /****************************************/

  printf( "GLUI version: %3.2f\n", GLUI_Master.get_version() );

  /*** Create the side subwindow ***/
  glui = GLUI_Master.create_glui_subwindow( main_window, 
              GLUI_SUBWINDOW_RIGHT );

  // obj_panel = new GLUI_Rollout(glui, "Properties", false );

  /***** Control for object params *****/

  // new GLUI_Checkbox( glui, "Wireframe", &wireframe, 1, control_cb );
  GLUI_Spinner *spinner = 
    new GLUI_Spinner( glui, "Segments:", &segments);
  spinner->set_int_limits( 3, 60 );
  spinner->set_alignment( GLUI_ALIGN_RIGHT );

  GLUI_Spinner *scale_spinner = 
    new GLUI_Spinner( glui, "Scale:", &scale);
  scale_spinner->set_float_limits( .2f, 4.0 );
  scale_spinner->set_alignment( GLUI_ALIGN_RIGHT );




  // /*** Add another rollout ***/
  GLUI_Rollout *options = new GLUI_Rollout(glui, "Options", true );
  // new GLUI_Checkbox( options, "Draw sphere", &show_sphere );
  new GLUI_Checkbox( options, "Show torus", &show_torus );
  new GLUI_Checkbox( options, "Draw axes", &show_axes );
  new GLUI_Checkbox( options, "Draw text", &show_text );

  /**** Add listbox ****/
  new GLUI_StaticText( glui, "" );
  GLUI_Listbox *list = new GLUI_Listbox( glui, "Mesh Options", &curr_string,SHADDING_ID,control_cb );
  int i;
  for( i=0; i<4; i++ )
    list->add_item( i, string_list[i] );

  new GLUI_StaticText( glui, "" );


  /*** Disable/Enable buttons ***/
  // new GLUI_Button( glui, "Disable movement", DISABLE_ID, control_cb );
  // new GLUI_Button( glui, "Enable movement", ENABLE_ID, control_cb );
  // new GLUI_Button( glui, "Hide", HIDE_ID, control_cb );
  // new GLUI_Button( glui, "Show", SHOW_ID, control_cb );

  // new GLUI_EditText(glui, "Open File:", GLUI_EDITTEXT_TEXT, open_filetext,open_file,control_cb);
  new GLUI_Button(glui, "Load", load_mesh, control_cb);

  
  // new GLUI_EditText(glui, "output File:", GLUI_EDITTEXT_TEXT, save_filetext,output_file,control_cb);
  new GLUI_Button(glui,"Save", save_file,control_cb);

new GLUI_StaticText( glui, "" );
new GLUI_StaticText( glui, "" );
new GLUI_StaticText( glui, "" );
new GLUI_StaticText( glui, "" );

  /****** A 'quit' button *****/

  
  new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );


  /**** Link windows to GLUI, and register idle callback ******/
  
  glui->set_main_gfx_window( main_window );


  /*** Create the bottom subwindow ***/
  glui2 = GLUI_Master.create_glui_subwindow( main_window, 
                                             GLUI_SUBWINDOW_BOTTOM );
  glui2->set_main_gfx_window( main_window );

  GLUI_Rotation *view_rot = new GLUI_Rotation(glui2, "Objects", view_rotate );
  view_rot->set_spin( 1.0 );
  new GLUI_Column( glui2, false );

  GLUI_Rotation *tor_rot = new GLUI_Rotation(glui2, "Torus", torus_rotate );
  tor_rot->set_spin( .98 );
  new GLUI_Column( glui2, false );


  GLUI_Translation *trans_xy = 
    new GLUI_Translation(glui2, "Objects XY", GLUI_TRANSLATION_XY, obj_pos );
  trans_xy->set_speed( .005 );
  new GLUI_Column( glui2, false );
  GLUI_Translation *trans_x = 
    new GLUI_Translation(glui2, "Objects X", GLUI_TRANSLATION_X, obj_pos );
  trans_x->set_speed( .005 );
  new GLUI_Column( glui2, false );
  GLUI_Translation *trans_y = 
    new GLUI_Translation( glui2, "Objects Y", GLUI_TRANSLATION_Y, &obj_pos[1] );
  trans_y->set_speed( .005 );
  new GLUI_Column( glui2, false );
  GLUI_Translation *trans_z = 
    new GLUI_Translation( glui2, "Objects Z", GLUI_TRANSLATION_Z, &obj_pos[2] );
  trans_z->set_speed( .005 );

  new GLUI_Column( glui2, false );

   new GLUI_Column( glui2, false );


#if 0
  /**** We register the idle callback with GLUI, *not* with GLUT ****/
  GLUI_Master.set_glutIdleFunc( myGlutIdle );
#endif

  /**** Regular GLUT main loop ****/
  
  glutMainLoop();

  return EXIT_SUCCESS;
}

