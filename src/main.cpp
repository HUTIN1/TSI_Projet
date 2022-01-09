/*****************************************************************************\
 * TP CPE, 4ETI, TP synthese d'images
 * --------------
 *
 * Programme principal des appels OpenGL
 \*****************************************************************************/

#include "declaration.h"
#include<time.h>

//identifiant des shaders
GLuint shader_program_id;
GLuint gui_program_id;

camera cam;
vec3 cam_vec_directeur = vec3(0.0f, 0.0f,-0.3f);
vec3 cam_vec_cote = vec3( 0.3f, 0.0f, 0.0f);

const int nb_obj = 10;
objet3d obj[nb_obj];

vec3 proj_directeur;

objet_aabb missil_aabb;
objet_aabb mur1_aabb;
objet_aabb mur2_aabb;
objet_aabb mur3_aabb;
objet_aabb portail_aabb;
objet_aabb cam_aabb;
objet_aabb sol_aabb;

BOOLEAN jump = false ;
int jumpframe = 10; //nombre de frame pour le jump



int aff_port = 0;


// varaible deplacement
bool avancer = false;
bool reculer = false;
bool droite = false;
bool gauche = false;



/*****************************************************************************\
* initialisation                                                              *
\*****************************************************************************/
static void init()
{
  shader_program_id = glhelper::create_program_from_file("shaders/shader.vert", "shaders/shader.frag"); CHECK_GL_ERROR();

  cam.projection = matrice_projection(60.0f*M_PI/180.0f,1.0f,0.01f,100.0f);
  cam.tr.translation = vec3(0.0f, 2.0f, 0.0f);
   cam.tr.rotation_center = vec3(0.0f, 1.0f, 0.0f);
   cam.tr.rotation_euler = vec3(0.0f, 0.0f, 0.0f);

  init_model_2();
  init_mur();
  init_missile();
  init_portail();

  cam_aabb.max_aabb = vec3(1.0f, 2.0f, 1.0f);
  cam_aabb.min_aabb = vec3(-1.0f, -2.0f, -1.0f);
 
}

/*****************************************************************************\
* display_callback                                                           *
\*****************************************************************************/
static void display_callback()
{
    glClearColor(0.5f, 0.6f, 0.9f, 1.0f); CHECK_GL_ERROR();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); CHECK_GL_ERROR();
    int i;
    int liste[] = { 0, 1, 2,3, 4, 5, 6 };
    int Lliste = 7;
    int u = 0;
    int lsol[] = { 0,1,2 };
    int liste_portail[] = { 8,9 };
    bool col = false;
    objet_aabb col_aabb;
    deplacement();
    

    if (cam.tr.translation.y < -50) {//pour ne pas tomber trop bas
        cam.tr.translation = vec3(0.0f, 0.0f, 0.0f);
 
    }
    
    cam.tr.rotation_center = cam.tr.translation;
    

    for (i = 0; i < nb_obj; ++i) {

        //collision missil
        if (7 == i & obj[7].visible == true) {
            u = 0;
            col_aabb = sol_aabb;
            obj[7].tr.translation += proj_directeur ;
            while(obj[7].visible == true && u < Lliste) { 
                if (u == 4) {
                    col_aabb = mur1_aabb;
                }
                if (collision(missil_aabb, col_aabb, obj[7].tr.rotation_euler, obj[liste[u]].tr.rotation_euler,obj[7].tr.translation, obj[liste[u]].tr.translation)) {
                    obj[7].visible = false;
                    placement_portail(obj[liste[u]]);
                }
                u++;
            }
        }

        //collision cam portail
        u = 0;
        while (u < 2 && !col) {
            if (collision(cam_aabb, portail_aabb, cam.tr.rotation_euler, obj[liste_portail[u]].tr.rotation_euler, cam.tr.translation, obj[liste_portail[u]].tr.translation)) {
                int objettel = !u;
                téléportation(obj[liste_portail[objettel]]);
                col = !col;
            }
            u++;
        }
        col = false;
        u = 0;
        while (u < 3 && !col) {
            if (collision(cam_aabb, sol_aabb, cam.tr.rotation_euler, obj[lsol[u]].tr.rotation_euler, cam.tr.translation, obj[lsol[u]].tr.translation)) {
                cam.tr.translation.y = obj[lsol[u]].tr.translation.y + 2.0f;
                cam.tr.rotation_center.y = cam.tr.translation.y;
                col = !col;
                jump = false;
                jumpframe = 10;
            }
            u++;
        }
        draw_obj3d(obj + i, cam);
    }

  glutSwapBuffers();
 
}



void keyboard_down(unsigned char key, int, int) {// when a key is down
    switch (key) {

    case 'p':
        glhelper::print_screen();
        break;
    case 27:
        exit(0);
        break;

    case 'a':
       
        obj[7].visible = !obj[7].visible;
        obj[7].tr.translation = cam.tr.translation + matrice_rotation(cam.tr.rotation_euler.x, -1.0f, 0.0f, 0.0f) * matrice_rotation(cam.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f) * vec3(0.0f, -0.5f, -2.0f);
        obj[7].tr.rotation_center = obj[4].tr.translation;
        proj_directeur = matrice_rotation(cam.tr.rotation_euler.x, -1.0f, 0.0f, 0.0f) * matrice_rotation(cam.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f) * cam_vec_directeur;
        if (abs(cam.tr.rotation_euler.y) > M_PI / 2) {
            proj_directeur.y = -1 * proj_directeur.y;
        }

        break;

    case 'z':
        avancer = true;
        break;
    case 's':
        reculer = true;
        break;
    case 'd':
        droite = true;
        break;
    case 'q':
        gauche = true;
        break;
    case 'n':
        if (!jump) {
            jump = !jump;
        }
        printf(" j appuie sur n \n");

    }

}      

void keyboard_up(unsigned char key, int, int) {// when the key goes up
    switch (key) {
    case 'z':
        avancer = false;
        break;
    case 's':
        reculer = false;
        break;
    case 'd':
        droite = false;
        break;
    case 'q':
        gauche = false;
        break;
    }
}       

/****************************************************************************\
* look                                                                       *
\****************************************************************************/
static void look(int x, int y) {
    GLfloat deltaX = 300 - x;
    GLfloat deltaY = 300 - y;
    if (!( - 0.9 * M_PI / 2 < cam.tr.rotation_euler.x - deltaY / (1000) && cam.tr.rotation_euler.x - deltaY / (1000) < 0.9 * M_PI / 2)) {
        deltaY = 0;
    }
    cam.tr.rotation_euler.x -= deltaY / (1000);
    cam.tr.rotation_euler.y -= deltaX / (500);
    
   
    glutWarpPointer(600/ 2, 600 / 2);

}


/*****************************************************************************\
* timer_callback                                                              *
\*****************************************************************************/
static void timer_callback(int)
{
  glutTimerFunc(25, timer_callback, 0);
  glutPostRedisplay();
}

/*****************************************************************************\
* main                                                                         *
\*****************************************************************************/
int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | MACOSX_COMPATIBILITY);
  glutInitWindowSize(600, 600);
  glutCreateWindow("OpenGL");

  glutDisplayFunc(display_callback);
  glutKeyboardFunc(keyboard_down);       // when a key is down
  glutKeyboardUpFunc(keyboard_up);       // when the key goes up
  glutPassiveMotionFunc(look);
  glutSetCursor(GLUT_CURSOR_NONE);  //cache le cursor
  glutTimerFunc(25, timer_callback, 0);

  glewExperimental = true;
  glewInit();

  std::cout << "OpenGL: " << (GLchar *)(glGetString(GL_VERSION)) << std::endl;

  init();
  glutMainLoop();

  return 0;
}



/*****************************************************************************\
* draw_obj3d                                                                  *
\*****************************************************************************/
void draw_obj3d(const objet3d* const obj, camera cam)
{
  if(!obj->visible) return;

  glEnable(GL_DEPTH_TEST);
  glUseProgram(obj->prog);
  
  {
    GLint loc_projection = glGetUniformLocation(shader_program_id, "projection"); CHECK_GL_ERROR();
    if (loc_projection == -1) std::cerr << "Pas de variable uniforme : projection" << std::endl;
    glUniformMatrix4fv(loc_projection,1,false,pointeur(cam.projection));    CHECK_GL_ERROR();

    GLint loc_rotation_view = glGetUniformLocation(shader_program_id, "rotation_view"); CHECK_GL_ERROR();
    if (loc_rotation_view == -1) std::cerr << "Pas de variable uniforme : rotation_view" << std::endl;
    mat4 rotation_x = matrice_rotation(cam.tr.rotation_euler.x, 1.0f, 0.0f, 0.0f);
    mat4 rotation_y = matrice_rotation(cam.tr.rotation_euler.y, 0.0f, 1.0f, 0.0f);
    mat4 rotation_z = matrice_rotation(cam.tr.rotation_euler.z, 0.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(loc_rotation_view,1,false,pointeur(rotation_x*rotation_y*rotation_z));    CHECK_GL_ERROR();

    vec3 cv = cam.tr.rotation_center;
    GLint loc_rotation_center_view = glGetUniformLocation(shader_program_id, "rotation_center_view"); CHECK_GL_ERROR();
    if (loc_rotation_center_view == -1) std::cerr << "Pas de variable uniforme : rotation_center_view" << std::endl;
    glUniform4f(loc_rotation_center_view , cv.x,cv.y,cv.z , 0.0f); CHECK_GL_ERROR();

    vec3 tv = cam.tr.translation;
    GLint loc_translation_view = glGetUniformLocation(shader_program_id, "translation_view"); CHECK_GL_ERROR();
    if (loc_translation_view == -1) std::cerr << "Pas de variable uniforme : translation_view" << std::endl;
    glUniform4f(loc_translation_view , tv.x,tv.y,tv.z , 0.0f); CHECK_GL_ERROR();
  }
  {
    GLint loc_rotation_model = glGetUniformLocation(obj->prog, "rotation_model"); CHECK_GL_ERROR();
    if (loc_rotation_model == -1) std::cerr << "Pas de variable uniforme : rotation_model" << std::endl;
    mat4 rotation_x = matrice_rotation(obj->tr.rotation_euler.x, 1.0f, 0.0f, 0.0f);
    mat4 rotation_y = matrice_rotation(obj->tr.rotation_euler.y, 0.0f, 1.0f, 0.0f);
    mat4 rotation_z = matrice_rotation(obj->tr.rotation_euler.z, 0.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(loc_rotation_model,1,false,pointeur(rotation_x*rotation_y*rotation_z));    CHECK_GL_ERROR();

    vec3 c = obj->tr.rotation_center;
    GLint loc_rotation_center_model = glGetUniformLocation(obj->prog, "rotation_center_model");   CHECK_GL_ERROR();
    if (loc_rotation_center_model == -1) std::cerr << "Pas de variable uniforme : rotation_center_model" << std::endl;
    glUniform4f(loc_rotation_center_model , c.x,c.y,c.z , 0.0f);                                  CHECK_GL_ERROR();

    vec3 t = obj->tr.translation;
    GLint loc_translation_model = glGetUniformLocation(obj->prog, "translation_model"); CHECK_GL_ERROR();
    if (loc_translation_model == -1) std::cerr << "Pas de variable uniforme : translation_model" << std::endl;
    glUniform4f(loc_translation_model , t.x,t.y,t.z , 0.0f);                                     CHECK_GL_ERROR();
  }
  glBindVertexArray(obj->vao);                                              CHECK_GL_ERROR();

  glBindTexture(GL_TEXTURE_2D, obj->texture_id);                            CHECK_GL_ERROR();
  glDrawElements(GL_TRIANGLES, 3*obj->nb_triangle, GL_UNSIGNED_INT, 0);     CHECK_GL_ERROR();
}



GLuint upload_mesh_to_gpu(const mesh& m)
{
  GLuint vao, vbo, vboi;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1,&vbo);                                 CHECK_GL_ERROR();
  glBindBuffer(GL_ARRAY_BUFFER,vbo); CHECK_GL_ERROR();
  glBufferData(GL_ARRAY_BUFFER,m.vertex.size()*sizeof(vertex_opengl),&m.vertex[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  glEnableVertexAttribArray(0); CHECK_GL_ERROR();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), 0); CHECK_GL_ERROR();

  glEnableVertexAttribArray(1); CHECK_GL_ERROR();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_opengl), (void*)sizeof(vec3)); CHECK_GL_ERROR();

  glEnableVertexAttribArray(2); CHECK_GL_ERROR();
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(2*sizeof(vec3))); CHECK_GL_ERROR();

  glEnableVertexAttribArray(3); CHECK_GL_ERROR();
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_opengl), (void*)(3*sizeof(vec3))); CHECK_GL_ERROR();

  glGenBuffers(1,&vboi); CHECK_GL_ERROR();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboi); CHECK_GL_ERROR();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,m.connectivity.size()*sizeof(triangle_index),&m.connectivity[0],GL_STATIC_DRAW); CHECK_GL_ERROR();

  return vao;
}



void init_model_2()
{

  mesh m;

  //coordonnees geometriques des sommets
  vec3 p0=vec3(-25.0f,-25.0f,0.0f);
  vec3 p1=vec3( 25.0f,-25.0f,0.0f);
  vec3 p2=vec3( 25.0f,25.0f, 0.0f);
  vec3 p3=vec3(-25.0f,25.0f, 0.0f);

  //normales pour chaque sommet
  vec3 n0=vec3(0.0f,1.0f,0.0f);
  vec3 n1=n0;
  vec3 n2=n0;
  vec3 n3=n0;

  //couleur pour chaque sommet
  vec3 c0=vec3(1.0f,1.0f,1.0f);
  vec3 c1=c0;
  vec3 c2=c0;
  vec3 c3=c0;

  //texture du sommet
  vec2 t0=vec2(0.0f,0.0f);
  vec2 t1=vec2(1.0f,0.0f);
  vec2 t2=vec2(1.0f,1.0f);
  vec2 t3=vec2(0.0f,1.0f);

  vertex_opengl v0=vertex_opengl(p0,n0,c0,t0);
  vertex_opengl v1=vertex_opengl(p1,n1,c1,t1);
  vertex_opengl v2=vertex_opengl(p2,n2,c2,t2);
  vertex_opengl v3=vertex_opengl(p3,n3,c3,t3);

  m.vertex = {v0, v1, v2, v3};

  //indice des triangles
  triangle_index tri0=triangle_index(0,1,2);
  triangle_index tri1=triangle_index(0,2,3);  
  m.connectivity = {tri0, tri1};

  obj[0].nb_triangle = 2;
  obj[0].vao = upload_mesh_to_gpu(m);

  obj[0].texture_id = glhelper::load_texture("data/Sol.tga");

  obj[0].visible = true;
  obj[0].prog = shader_program_id;
  obj[0].tr.rotation_euler.x = M_PI / 2;

  obj[1] = obj[0];
  obj[1].tr.translation = vec3(0.0, 0.0, -55.0);

  obj[2] = obj[0];
  obj[2].tr.translation = vec3(-80.0, -2.0, -20.0);
  obj[3] = obj[0];
  obj[3].tr.translation.y = 25;

  get_aabb(&m, &sol_aabb.min_aabb, &sol_aabb.max_aabb);
  sol_aabb.max_aabb.x = 25.0f;
  sol_aabb.max_aabb.y = 25.0f;



}




void init_mur() {
    mesh m;
    float i = 25;
    //coordonnees geometriques des sommets
    vec3 p0 = vec3(-i, 0, 0.0f);
    vec3 p1 = vec3(i, 0, 0.0f);
    vec3 p2 = vec3(i, i, 0.0f);
    vec3 p3 = vec3(-i, i, 0.0f);

    //normales pour chaque sommet
    vec3 n0 = vec3(0.0f, 0.0f, 1.0f);
    vec3 n1 = n0;
    vec3 n2 = n0;
    vec3 n3 = n0;

    //couleur pour chaque sommet
    vec3 c0 = vec3(0.0f, 0.0f, 1.0f);
    vec3 c1 = c0;
    vec3 c2 = c0;
    vec3 c3 = c0;


    //texture du sommet
    vec2 t0 = vec2(0.0f, 0.0f);
    vec2 t1 = vec2(1.0f, 0.0f);
    vec2 t2 = vec2(1.0f, 1.0f);
    vec2 t3 = vec2(0.0f, 1.0f);

    vertex_opengl v0 = vertex_opengl(p0, n0, c0, t0);
    vertex_opengl v1 = vertex_opengl(p1, n1, c1, t1);
    vertex_opengl v2 = vertex_opengl(p2, n2, c2, t2);
    vertex_opengl v3 = vertex_opengl(p3, n3, c3, t3);


    m.vertex = { v0, v1, v2 ,v3};

    //indice des triangles
    triangle_index tri0 = triangle_index(0, 1, 2);
    triangle_index tri1 = triangle_index(0,2,3);
    m.connectivity = { tri0, tri1 };

    obj[4].vao = upload_mesh_to_gpu(m);

    obj[4].nb_triangle = m.connectivity.size();
    obj[4].texture_id = glhelper::load_texture("data/Mur.tga");
    obj[4].visible = true;
    obj[4].prog = shader_program_id;
    get_aabb(&m, &mur1_aabb.min_aabb, &mur1_aabb.max_aabb);
   /* obj[3].tr.translation = vec3(0.0f, 0.0f, 0.02f);*/

    obj[5] = obj[4];
    obj[6] = obj[4];
   

    obj[5].tr.translation = vec3(0.0f, 0.0f, -67.0f);
    obj[6].tr.translation = vec3(-90.0, -2.0, -20.0);
   

    obj[6].tr.rotation_euler = vec3(0.0f, 1.2f, 0.02f);

    
    mur1_aabb.max_aabb.x = i;
    mur1_aabb.max_aabb.y = i;

    mur2_aabb = mur1_aabb;
    mur3_aabb = mur1_aabb;


}


void init_missile() {

    // Chargement d'un maillage a partir d'un fichier
    mesh m = load_obj_file("data/stegosaurus.obj");

    // Affecte une transformation sur les sommets du maillage
    float s = 0.2f;
    mat4 transform = mat4(s, 0.0f, 0.0f, 0.0f,
        0.0f, s, 0.0f, 0.0f,
        0.0f, 0.0f, s, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    apply_deformation(&m, transform);

    // Centre la rotation du modele 1 autour de son centre de gravite approximatif
    obj[7].tr.rotation_center = vec3(0.0f, 1.0f, 0.0f);

    update_normals(&m);
    fill_color(&m, vec3(1.0f, 1.0f, 1.0f));

    obj[7].vao = upload_mesh_to_gpu(m);

    obj[7].nb_triangle = m.connectivity.size();
    obj[7].texture_id = glhelper::load_texture("data/stegosaurus.tga");
    obj[7].visible = true;
    obj[7].prog = shader_program_id;

    obj[7].tr.translation = vec3(-2.0, 0.0, -10.0);
    get_aabb(&m,&missil_aabb.min_aabb, &missil_aabb.max_aabb);


}

void init_portail(){
    mesh m;
    float h = 4;
    float l = 2;
    //coordonnees geometriques des sommets
    vec3 p0 = vec3(-l/2, -h/2, 0.0f);
    vec3 p1 = vec3(l/2, -h/2, 0.0f);
    vec3 p2 = vec3(l/2, h/2, 0.0f);
    vec3 p3 = vec3(-l/2, h/2, 0.0f);

    //normales pour chaque sommet
    vec3 n0 = vec3(0.0f, 0.0f, -1.0f);
    vec3 n1 = n0;
    vec3 n2 = n0;
    vec3 n3 = n0;

    //couleur pour chaque sommet
    vec3 c0 = vec3(1.0f, 1.0f, 1.0f);
    vec3 c1 = c0;
    vec3 c2 = c0;
    vec3 c3 = c0;


    //texture du sommet
    vec2 t0 = vec2(0.0f, 0.0f);
    vec2 t1 = vec2(1.0f, 0.0f);
    vec2 t2 = vec2(1.0f, 1.0f);
    vec2 t3 = vec2(0.0f, 1.0f);

    vertex_opengl v0 = vertex_opengl(p0, n0, c0, t0);
    vertex_opengl v1 = vertex_opengl(p1, n1, c1, t1);
    vertex_opengl v2 = vertex_opengl(p2, n2, c2, t2);
    vertex_opengl v3 = vertex_opengl(p3, n3, c3, t3);


    m.vertex = { v0, v1, v2 ,v3 };

    //indice des triangles
    triangle_index tri0 = triangle_index(0, 1, 2);
    triangle_index tri1 = triangle_index(0, 2, 3);
    m.connectivity = { tri0, tri1 };


    obj[8].vao = upload_mesh_to_gpu(m);

    obj[8].nb_triangle = m.connectivity.size();
    obj[8].texture_id = glhelper::load_texture("data/portail.jpg");
    obj[8].visible = true;
    obj[8].prog = shader_program_id;
    obj[8].tr.translation = vec3(0.0f, 0.0f, 0.0f);
    obj[9] = obj[8];
    get_aabb(&m, &portail_aabb.min_aabb, &portail_aabb.max_aabb);
    portail_aabb.max_aabb.x = l / 2;
    portail_aabb.max_aabb.y = h / 2;


}

BOOLEAN collision(objet_aabb obj1, objet_aabb obj2,vec3 objet1rot, vec3 objet2rot,vec3 objet1trans, vec3 objet2trans) {


    obj1.max_aabb =matrice_rotation( objet1rot.x,1.0f,0.0f,0.0f)* matrice_rotation(objet1rot.y, 0.0f, 1.0f, 0.0f)* matrice_rotation(objet1rot.z, 0.0f, 0.0f, 1.0f)* obj1.max_aabb;
    obj1.min_aabb = matrice_rotation(objet1rot.x, 1.0f, 0.0f, 0.0f) * matrice_rotation(objet1rot.y, 0.0f, 1.0f, 0.0f) * matrice_rotation(objet1rot.z, 0.0f, 0.0f, 1.0f) * obj1.min_aabb;

    obj1.max_aabb += objet1trans;
    obj1.min_aabb += objet1trans;

    rangestructaabb(&obj2);
    obj2.max_aabb = matrice_rotation(objet2rot.x, 1.0f, 0.0f, 0.0f)*matrice_rotation(objet2rot.y, 0.0f, 1.0f, 0.0f) * matrice_rotation(objet2rot.z, 0.0f, 0.0f, 1.0f)*obj2.max_aabb;
    obj2.min_aabb = matrice_rotation(objet2rot.x, 1.0f, 0.0f, 0.0f) * matrice_rotation(objet2rot.y, 0.0f, 1.0f, 0.0f) * matrice_rotation(objet2rot.z, 0.0f, 0.0f, 1.0f) * obj2.min_aabb;
    

    obj2.max_aabb += objet2trans;
    obj2.min_aabb += objet2trans;


    rangestructaabb(&obj1);
    rangestructaabb(&obj2);
    

    float distz1 = abs(obj1.max_aabb.z - obj1.min_aabb.z);
    float distz2 = abs(obj2.max_aabb.z - obj2.min_aabb.z);
    if (distz1 < distz2) {
        if (!(obj2.max_aabb.z > obj1.max_aabb.z && obj1.max_aabb.z > obj2.min_aabb.z)) {
            if (!(obj2.max_aabb.z > obj1.min_aabb.z && obj1.min_aabb.z > obj2.min_aabb.z)) {
                return false;
            }
        }
    }
    else {
        if (!(obj1.max_aabb.z > obj2.max_aabb.z && obj2.max_aabb.z > obj1.min_aabb.z)) {
            if (!(obj1.max_aabb.z > obj2.min_aabb.z && obj2.min_aabb.z > obj1.min_aabb.z)) {

                return false;
            }
        }
    }

    float disty1 = abs(obj1.max_aabb.y - obj1.min_aabb.y);
    float disty2 = abs(obj2.max_aabb.y - obj2.min_aabb.y);
    if (disty1 < disty2) {
        if (!(obj2.max_aabb.y > obj1.max_aabb.y && obj1.max_aabb.y > obj2.min_aabb.y)) {
            if (!(obj2.max_aabb.y > obj1.min_aabb.y && obj1.min_aabb.y > obj2.min_aabb.y)) {
                return false;
            }
        }
    }
    else {
        if (!(obj1.max_aabb.y > obj2.max_aabb.y && obj2.max_aabb.y > obj1.min_aabb.y)) {
            if (!(obj1.max_aabb.y > obj2.min_aabb.y && obj2.min_aabb.y > obj1.min_aabb.y)) {
                return false;
            }
        }
    }

    float distx1 = abs(obj1.max_aabb.x - obj1.min_aabb.x);
    float distx2 = abs(obj2.max_aabb.x - obj2.min_aabb.x);
    if (distx1 < distx2) {
        if (!(obj2.max_aabb.x > obj1.max_aabb.x && obj1.max_aabb.x > obj2.min_aabb.x)) {
            if (!(obj2.max_aabb.x > obj1.min_aabb.x && obj1.min_aabb.x > obj2.min_aabb.x)) {
                return false;
            }
        }
    }
    else {
        if (!(obj1.max_aabb.x > obj2.max_aabb.x && obj2.max_aabb.x > obj1.min_aabb.x)) {
            if (!(obj1.max_aabb.x > obj2.min_aabb.x && obj2.min_aabb.x > obj1.min_aabb.x)) {
                return false;
            }
        }
    }
    return true;
}


void rangestructaabb(objet_aabb* obj) {
    float v;
    if (obj->max_aabb.z < obj->min_aabb.z) {
        v = obj->max_aabb.z;
        obj->max_aabb.z = obj->min_aabb.z;
        obj->min_aabb.z = v;
    }

    if (obj->max_aabb.y < obj->min_aabb.y) {
        v = obj->max_aabb.y;
        obj->max_aabb.y = obj->min_aabb.y;
        obj->min_aabb.y = v;
    }

    if (obj->max_aabb.x < obj->min_aabb.x) {
        v = obj->max_aabb.x;
        obj->max_aabb.x = obj->min_aabb.x;
        obj->min_aabb.x = v;
    }
}

void placement_portail(objet3d objet) {
    aff_port++;
    if (aff_port == 2) {
        aff_port = 0;
    }
    vec3 trans = proj_directeur;
    vec3 pos = matrice_rotation(objet.tr.rotation_euler.x, -1.0f, 0.0f, 0.0f) * matrice_rotation(objet.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f) * matrice_rotation(objet.tr.rotation_euler.z, 0.0f, 0.0f, -1.0f) * (obj[7].tr.translation - objet.tr.translation);
    pos.z = 0.0f;
    obj[aff_port + 8].tr.translation = objet.tr.translation + matrice_rotation(objet.tr.rotation_euler.x, 1.0f, 0.0f, 0.0f) * matrice_rotation(objet.tr.rotation_euler.y, 0.0f, 1.0f, 0.0f) * matrice_rotation(objet.tr.rotation_euler.z, 0.0f, 0.0f, 1.0f) * pos;
    obj[aff_port + 8].tr.rotation_euler = objet.tr.rotation_euler;

    trans = matrice_rotation(objet.tr.rotation_euler.x, -1.0f, 0.0f, 0.0f) * matrice_rotation(objet.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f) * matrice_rotation(objet.tr.rotation_euler.z, 0.0f, 0.0f, -1.0f) * trans;
    if (trans.z < 0) {
        obj[aff_port + 8].tr.rotation_euler.y += M_PI;
    }
}

void téléportation(objet3d portail) {
    cam.tr.translation = portail.tr.translation;
    if ((-0.9 * M_PI / 2 < portail.tr.rotation_euler.x  && portail.tr.rotation_euler.x  < 0.9 * M_PI / 2)) {
        cam.tr.rotation_euler = portail.tr.rotation_euler;
    } 
    cam.tr.rotation_euler.z = 0;
   cam.tr.rotation_euler.y += -2*portail.tr.rotation_euler.y;
    cam.tr.translation += matrice_rotation(cam.tr.rotation_euler.x, -1.0f, 0.0f, 0.0f) * matrice_rotation(cam.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f) * matrice_rotation(cam.tr.rotation_euler.z, 0.0f, 0.0f,-1.0f) * vec3(0.0f,0.0f,-3.0f);
    if (cam.tr.translation.y < 2.0f) {
        cam.tr.translation.y = 2.0f;
    }
    cam.tr.rotation_center = cam.tr.translation;
}

void deplacement() {
    float r = cam.tr.translation.y;

    if (jump && !jumpframe == 0) {
        printf("je saute\n");
        r += 0.6f;
        printf("%f\n", r);
        jumpframe--;
    }
    else {
        r -= 0.3f; //gravité
    } 

    if (avancer) {
        cam.tr.translation += matrice_rotation(cam.tr.rotation_euler.x, -1.0f, 0.0f, 0.0f) * matrice_rotation(cam.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f) * cam_vec_directeur;
    }
    if (reculer) {
        cam.tr.translation -= matrice_rotation(cam.tr.rotation_euler.x, -1.0f, 0.0f, 0.0f) * matrice_rotation(cam.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f) * cam_vec_directeur;
    }
    if (droite) {
        cam.tr.translation += matrice_rotation(cam.tr.rotation_euler.x, -1.0f, 0.0f, 0.0f) * matrice_rotation(cam.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f) * cam_vec_cote;
    }
    if (gauche) {
        cam.tr.translation -= matrice_rotation(cam.tr.rotation_euler.x, -1.0f, 0.0f, 0.0f) * matrice_rotation(cam.tr.rotation_euler.y, 0.0f, -1.0f, 0.0f) * cam_vec_cote;
    }
    cam.tr.translation.y = r;
    
    
}